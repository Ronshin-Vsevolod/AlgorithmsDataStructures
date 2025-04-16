#include "Main.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <iterator>
#include <Eigen/Dense>
#include <lazycsv.hpp>

Eigen::VectorXd gaussianElimination(const Eigen::MatrixXd& A, const Eigen::VectorXd& b) 
{
    int n = A.rows();
    
    Eigen::MatrixXd Ab(n, n + 1);
    Ab.leftCols(n) = A;
    Ab.col(n) = b;
    
    for (int i = 0; i < n; i++) 
    {
        int maxRow = i;
        double maxVal = std::abs(Ab(i, i));
        
        for (int k = i + 1; k < n; k++) 
        {
            if (std::abs(Ab(k, i)) > maxVal) 
            {
                maxVal = std::abs(Ab(k, i));
                maxRow = k;
            }
        }
        
        if (maxRow != i) 
        {
            Ab.row(i).swap(Ab.row(maxRow));
        }
        
        if (std::abs(Ab(i, i)) < 1e-10) // 1e-10 is considered conditionally 0
        {
            throw std::runtime_error("Matrix is singular or nearly singular");
        }
        
        double pivot = Ab(i, i);
        Ab.row(i) /= pivot;
        
        for (int j = i + 1; j < n; j++) 
        {
            double factor = Ab(j, i);
            Ab.row(j) -= factor * Ab.row(i);
        }
    }
    
    Eigen::VectorXd x(n);
    
    for (int i = n - 1; i >= 0; i--) 
    {
        x(i) = Ab(i, n);
        for (int j = i + 1; j < n; j++) 
        {
            x(i) -= Ab(i, j) * x(j);
        }
    }
    
    return x;
}

SystemPair readSystemFromCSV(const std::string& filename) 
{
    try 
    {
        lazycsv::parser parser{filename};
        
        int num_rows = 0;
        int num_cols = 0;
        
        for (const auto& row : parser) 
        {
            num_rows++;
            
            if (num_rows == 1) 
            {
                num_cols = std::distance(row.begin(), row.end());
            }
        }
        
        if (num_rows == 0) 
        {
            throw std::runtime_error("CSV file is empty");
        }
        
        int matrix_rows = num_rows;
        int matrix_cols = num_cols - 1;
        
        if (matrix_rows <= 0 || matrix_cols <= 0) 
        {
            throw std::runtime_error("Invalid CSV format: insufficient data dimensions");
        }
        
        Eigen::MatrixXd A(matrix_rows, matrix_cols);
        Eigen::VectorXd b(matrix_rows);
        
        parser = lazycsv::parser{filename};
        
        int row_idx = 0;
        for (const auto& row : parser) 
        {
            std::vector<std::string> cells_data;
            for (const auto& cell : row) 
            {
                cells_data.push_back(std::string(cell.trimed()));
            }
            
            if (cells_data.size() < static_cast<size_t>(matrix_cols + 1)) 
            {
                throw std::runtime_error("Row " + std::to_string(row_idx) + " has insufficient columns");
            }
            
            for (int col = 0; col < matrix_cols; col++) 
            {
                try 
                {
                    A(row_idx, col) = std::stod(cells_data[col]);
                } 
                catch (const std::exception& e) 
                {
                    throw std::runtime_error("Error parsing A(" + std::to_string(row_idx) + "," 
                                             + std::to_string(col) + "): " + std::string(e.what()));
                }
            }
            
            try 
            {
                b(row_idx) = std::stod(cells_data[matrix_cols]);
            } 
            catch (const std::exception& e) 
            {
                throw std::runtime_error("Error parsing b(" + std::to_string(row_idx) + "): " + std::string(e.what()));
            }
            
            row_idx++;
        }
        
        return SystemPair(A, b);
    } 
    catch (const std::exception& e) 
    {
        throw std::runtime_error("Failed to read CSV file: " + std::string(e.what()));
    }
}

void writeVectorToCSV(const std::string& filename, const Eigen::VectorXd& x) 
{
    std::ofstream file(filename);
    if (!file.is_open()) 
    {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    
    file << "x\n";
    
    file.precision(15);
    
    for (int i = 0; i < x.size(); i++) 
    {
        file << x(i) << "\n";
    }
    
    file.close();
    std::cout << "Solution saved to " << filename << std::endl;
}

SystemPair generateRandomSystem(int size, unsigned int seed) 
{
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist(-10.0, 10.0);
    
    Eigen::MatrixXd A(size, size);
    Eigen::VectorXd b(size);
    
    for (int i = 0; i < size; i++) 
    {
        for (int j = 0; j < size; j++) 
        {
            A(i, j) = dist(gen);
        }
        A(i, i) = A(i, i) + size; // optional. Makes the matrices "better"
        
        b(i) = dist(gen);
    }
    
    return SystemPair(A, b);
}

int main(int argc, char** argv) 
{
    try 
    {
        if (argc > 1) 
        {
            std::string arg = argv[1];
            
            if (arg == "--generate" || arg == "-g") 
            {
                int size = 3;
                unsigned int seed = 42;
                
                if (argc > 2) size = std::stoi(argv[2]);
                if (argc > 3) seed = std::stoi(argv[3]);
                
                std::cout << "Generating random system of size " << size << " with seed " << seed << std::endl;
                auto system = generateRandomSystem(size, seed);
                
                std::string generatedFilename = "../generated.csv";
                std::ofstream genFile(generatedFilename);
                if (genFile.is_open()) 
                {
                    for (int i = 0; i < size; i++) 
                    {
                        genFile << (char)('A' + i) << ",";
                    }
                    genFile << "b\n";
                    
                    genFile.precision(15);
                    
                    for (int i = 0; i < size; i++) 
                    {
                        for (int j = 0; j < size; j++) 
                        {
                            genFile << system.A(i, j) << ",";
                        }
                        genFile << system.b(i) << "\n";
                    }
                    genFile.close();
                    std::cout << "Generated system saved to " << generatedFilename << std::endl;
                }
                
                Eigen::VectorXd x = gaussianElimination(system.A, system.b);
                
                writeVectorToCSV("../solution.csv", x);
                
                std::cout << "Solution x:\n" << x << "\n";
                
                return 0;
            }
            
            SystemPair system = readSystemFromCSV(arg);
            Eigen::MatrixXd A = system.A;
            Eigen::VectorXd b = system.b;
            
            std::cout << "Matrix A:\n" << A << "\n\n";
            std::cout << "Vector b:\n" << b << "\n\n";
            
            Eigen::VectorXd x = gaussianElimination(A, b);
            
            std::cout << "Solution x:\n" << x << "\n";
            
            writeVectorToCSV("../solution.csv", x);
            
            return 0;
        }
        
        SystemPair system = readSystemFromCSV("../default.csv");
        Eigen::MatrixXd A = system.A;
        Eigen::VectorXd b = system.b;

        std::cout << "Matrix A:\n" << A << "\n\n";
        std::cout << "Vector b:\n" << b << "\n\n";

        Eigen::VectorXd x = gaussianElimination(A, b);

        std::cout << "Solution x:\n" << x << "\n";
        
        writeVectorToCSV("../solution.csv", x);

    } 
    catch (const std::runtime_error& error) 
    {
        std::cerr << "Error: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}
