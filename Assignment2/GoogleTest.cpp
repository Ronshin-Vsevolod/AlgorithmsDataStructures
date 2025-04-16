#include "Main.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <gtest/gtest.h>
#include <Eigen/Dense>
#include <lazycsv.hpp>

void createDummyCSV(const std::string& filename, const std::vector<std::vector<double>>& data) 
{
    std::string fullpath = "../" + filename;
    std::ofstream file(fullpath);
    if (file.is_open()) 
    {
        // Write the headers in the format A, B, C, ...
        for (size_t i = 0; i < data[0].size() - 1; i++) 
        {
            file << (char)('A' + i) << ",";
        }
        file << "b\n";
        
        for (const auto& row : data) 
        {
            for (size_t i = 0; i < row.size() - 1; i++) 
            {
                file << row[i] << ",";
            }
            file << row.back() << "\n";
        }
        file.close();
        
        std::ifstream debug(fullpath);
        if (debug.is_open()) 
        {
            std::cout << "Contents of " << filename << ":" << std::endl;
            std::string line;
            while (std::getline(debug, line)) 
            {
                std::cout << line << std::endl;
            }
            debug.close();
        }
    }
}

// Test Gaussian elimination on a simple 2x2 system
TEST(LinearSolverTest, SolveSimpleSystem) 
{
    Eigen::MatrixXd A(2, 2);
    A << 2, 1,
         1, 3;
    Eigen::VectorXd b(2);
    b << 7, 10;

    Eigen::VectorXd x = gaussianElimination(A, b);
    
    Eigen::VectorXd b_check = A * x;
    
    ASSERT_TRUE(b_check.isApprox(b, 1e-9));
}

// Test reading system from CSV file with correct extraction of matrix A and vector b
TEST(LinearSolverTest, ReadSimpleSystemFromCSV) 
{
    const std::string filename = "test_simple.csv";
    std::vector<std::vector<double>> data = {{2.0, 1.0, 7.0}, {1.0, 3.0, 10.0}};
    createDummyCSV(filename, data);

    SystemPair system = readSystemFromCSV("../" + filename);
    Eigen::MatrixXd A = system.A;
    Eigen::VectorXd b = system.b;

    ASSERT_EQ(A.rows(), 2);
    ASSERT_EQ(A.cols(), 2);
    ASSERT_EQ(b.size(), 2);

    ASSERT_DOUBLE_EQ(A(0, 0), 2.0);
    ASSERT_DOUBLE_EQ(A(0, 1), 1.0);
    ASSERT_DOUBLE_EQ(A(1, 0), 1.0);
    ASSERT_DOUBLE_EQ(A(1, 1), 3.0);

    ASSERT_DOUBLE_EQ(b(0), 7.0);
    ASSERT_DOUBLE_EQ(b(1), 10.0);

    std::remove(("../" + filename).c_str());
}

// Test CSV reader error handling with invalid data format
TEST(LinearSolverTest, ReadInvalidCSVFormat) 
{
    const std::string filename = "test_invalid.csv";
    std::string fullpath = "../" + filename;
    std::ofstream file(fullpath);
    if (file.is_open()) 
    {
        file << "A\n";
        file << "NotANumber\n";
        file.close();
    }

    ASSERT_THROW(readSystemFromCSV(fullpath), std::runtime_error);

    std::remove(fullpath.c_str());
}

// Test random system generation and solution verification
TEST(LinearSolverTest, GenerateRandomSystem) 
{
    int size = 5;
    unsigned int seed = 42;
    
    auto system = generateRandomSystem(size, seed);
    
    ASSERT_EQ(system.A.rows(), size);
    ASSERT_EQ(system.A.cols(), size);
    ASSERT_EQ(system.b.size(), size);
    
    Eigen::VectorXd x = gaussianElimination(system.A, system.b);
    
    Eigen::VectorXd b_check = system.A * x;
    
    ASSERT_TRUE(b_check.isApprox(system.b, 1e-9));
}

// Test writing solution vector to CSV file with correct formatting
TEST(LinearSolverTest, WriteVectorToCSV) 
{
    const std::string filename = "test_solution.csv";
    std::string fullpath = "../" + filename;
    
    Eigen::VectorXd x(3);
    x << 1.5, 2.0, 3.5;
    
    writeVectorToCSV(fullpath, x);
    
    std::ifstream file(fullpath);
    ASSERT_TRUE(file.is_open());
    
    std::string header;
    std::getline(file, header);
    ASSERT_EQ(header, "x");
    
    std::string line;
    double value;
    int i = 0;
    while (std::getline(file, line) && i < 3) 
    {
        std::stringstream ss(line);
        ss >> value;
        ASSERT_DOUBLE_EQ(value, x(i));
        i++;
    }
    
    file.close();
    
    std::remove(fullpath.c_str());
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}