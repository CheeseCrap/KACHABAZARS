
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
	void set_constant(const double i){constant = i;};
	void set_order(const double i){order = i;};
	MatrixXd& get_transformed(){return transformed;}	
	void run_pca();
	void run_kpca();
	void print();
	void write_transformed(string);
	void write_eigenvectors(string);
private:
	double kernel(const VectorXd& a, const VectorXd& b);
	MatrixXd X, Xcentered, C, K, eigenvectors, transformed;
	VectorXd eigenvalues, cumulative;
	unsigned int components, kernel_type, normalise;
	double gamma, constant, order;

};

void PCA::load_data(const char* data, char sep){

	// Read data
	unsigned int row = 0;
	ifstream file(data);
	if(file.is_open()){
		string line,token;
		while(getline(file, line)){
			stringstream tmp(line);
			unsigned int col = 0;
			while(getline(tmp, token, sep)){
				if(X.rows() < row+1){
					X.conservativeResize(row+1,X.cols());
				}
				if(X.cols() < col+1){
					X.conservativeResize(X.rows(),col+1);
				}
				X(row,col) = atof(token.c_str());
				col++;
			}
			row++;
		}
		file.close();
		Xcentered.resize(X.rows(),X.cols());
	}else{
		cout << "Failed to read file " << data << endl;
	}

}

double PCA::kernel(const VectorXd& a, const VectorXd& b){

	/*
		Kernels
		1 = RBF
		2 = Polynomial
		TODO - add some of these these:
		http://crsouza.blogspot.co.uk/2010/03/kernel-functions-for-machine-learning.html	

	*/
	switch(kernel_type){
	    case 2  :
	    	return(pow(a.dot(b)+constant,order));
	    default : 
	    	return(exp(-gamma*((a-b).squaredNorm())));
	}

}

void PCA::run_kpca(){

	// Fill kernel matrix
	K.resize(X.rows(),X.rows());
	for(unsigned int i = 0; i < X.rows(); i++){
		for(unsigned int j = i; j < X.rows(); j++){
			K(i,j) = K(j,i) = kernel(X.row(i),X.row(j));
			//printf("k(%i,%i) = %f\n",i,j,K(i,j));
		}	
	}	
	//cout << endl << K << endl;

	EigenSolver<MatrixXd> edecomp(K);
	eigenvalues = edecomp.eigenvalues().real();
	eigenvectors = edecomp.eigenvectors().real();
	cumulative.resize(eigenvalues.rows());
	vector<pair<double,VectorXd> > eigen_pairs; 
	double c = 0.0; 
	for(unsigned int i = 0; i < eigenvectors.cols(); i++){
		if(normalise){
			double norm = eigenvectors.col(i).norm();
			eigenvectors.col(i) /= norm;
		}
		eigen_pairs.push_back(make_pair(eigenvalues(i),eigenvectors.col(i)));
	}
	// http://stackoverflow.com/questions/5122804/sorting-with-lambda
	sort(eigen_pairs.begin(),eigen_pairs.end(), [](const pair<double,VectorXd> a, const pair<double,VectorXd> b) -> bool {return (a.first > b.first);} );
	for(unsigned int i = 0; i < eigen_pairs.size(); i++){
		eigenvalues(i) = eigen_pairs[i].first;
		c += eigenvalues(i);
		cumulative(i) = c;
		eigenvectors.col(i) = eigen_pairs[i].second;
	}
	transformed.resize(X.rows(),components);

	for(unsigned int i = 0; i < X.rows(); i++){
		for(unsigned int j = 0; j < components; j++){
			for (int k = 0; k < K.rows(); k++){
                transformed(i,j) += K(i,k) * eigenvectors(k,j);
		 	}