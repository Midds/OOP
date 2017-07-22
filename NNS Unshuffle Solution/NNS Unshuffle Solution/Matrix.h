#pragma once
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <istream>
#include <iostream>
#include <string>

class Matrix
{
public:
	//constructor
	Matrix();

	//destructor
	~Matrix();

	//Copy constructor
	Matrix(const Matrix&);

	// Overload operators
	int operator()(int i, int j);
	Matrix operator+(const Matrix& existing);
	Matrix operator=(const Matrix& existing);
	Matrix operator-(const Matrix& existing);
	Matrix operator/(const Matrix& existing);
	Matrix operator*(const Matrix& existing);
	Matrix operator++(int);

	// Functions
	int getM();
	int getN();
	Matrix getBlock(int start_row, int end_row, int start_column, int end_column);
	Matrix(int sizeR, int sizeC, double* input_data);
	Matrix add(const Matrix& other);
	void Matrix::setBlock(int startRow, int startColumn, Matrix block); // Sets a block of data to another specific set of data(Matrix block)
	void Matrix::setBlock(int startRow, int startColumn); // Sets a block of data to be white (all 255).
	double* getData();
	void Matrix::reduceNoise();

private:

	// Functions
	double get(int i, int j);
	void set(int i, int j, double val);

	int _M, _N;
	double *_data;
};

class BinaryImage : public Matrix
{
public:
	BinaryImage();
	BinaryImage(int M, int N, double* input_data, double threshold);
	~BinaryImage();
	BinaryImage::BinaryImage(const BinaryImage&);

private:
	double* data;
	int bM, bN;
};