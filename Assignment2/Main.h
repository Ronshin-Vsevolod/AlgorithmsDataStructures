#ifndef MAIN_H
#define MAIN_H

#include <Eigen/Dense>
#include <vector>
#include <string>
#include <random>

struct SystemPair 
{
    Eigen::MatrixXd A;
    Eigen::VectorXd b;
    
    SystemPair(const Eigen::MatrixXd& _A, const Eigen::VectorXd& _b) : A(_A), b(_b) {}
};

Eigen::VectorXd gaussianElimination(const Eigen::MatrixXd& A, const Eigen::VectorXd& b);
SystemPair readSystemFromCSV(const std::string& filename);
void writeVectorToCSV(const std::string& filename, const Eigen::VectorXd& x);
SystemPair generateRandomSystem(int size, unsigned int seed);

#endif