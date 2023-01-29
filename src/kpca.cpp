
// Kernel PCA using the Eigen library, by Tim Nugent 2014

#include <fstream>
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

using namespace Eigen;
using namespace std;

class PCA{

public:
	PCA() : components(2), kernel_type(1), normalise(0), gamma(0.001), constant(1.0), order(2.0) {}
	explicit PCA(MatrixXd& d) : components(2), kernel_type(1), normalise(0), gamma(0.001), constant(1.0), order(2.0) {X = d;}
	void load_data(const char* data, char sep = ',');
	void set_components(const int i){components = i;};
	void set_kernel(const int i){kernel_type = i;};	
	void set_normalise(const int i){normalise = i;};
	void set_gamma(const double i){gamma = i;};