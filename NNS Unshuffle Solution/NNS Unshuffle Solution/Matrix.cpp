#include "Matrix.h"

// Constructor
Matrix::Matrix()
{
	_M = 1;
	_N = 1;

	_data = new double[_M*_N];

	for (int i = 0; i < _M*_N; i++)
	{
		_data[i] = 0;
	}
}


Matrix::Matrix(int sizeR, int sizeC, double* input_data)
{
	_M = sizeR;
	_N = sizeC;
	_data = new double[_M *_N];

	for (int i = 0; i < _M*_N; i++)
	{
		_data[i] = input_data[i];
	}
}

//Copy constructor
Matrix::Matrix(const Matrix& m)
{
	_M = m._M;
	_N = m._N;

	_data = new double[_M*_N];

	for (int i = 0; i < _M*_N; i++)
	{
		_data[i] = m._data[i];
	}
}

// Operator overloads
// I think these operators may create a memory leak. Matrix() creates a new _data, and then these operators also create new _data's, but i'm not sure how to fix it, i think these operates should be returning a '*this', but they still work.
int Matrix::operator()(int i, int j)
{
	Matrix temp;

	temp._M = _M;
	temp._N = _N;

	temp._data = new double[temp._M*temp._N];

	int aIJ = temp._data[i * 2 + j];

	return _data[i * 2 + j];
}

Matrix Matrix::operator+(const Matrix& other)
{
	Matrix temp;
	temp._M = other._M;
	temp._N = other._N;

	temp._data = new double[temp._M*temp._N];

	for (int x = 0; x < (temp._M*temp._N); x++)
	{
		temp._data[x] = this->_data[x] + other._data[x];
	}

	return temp;
}



Matrix Matrix::operator=(const Matrix& other)
{
	//delete existing _data information - it will be replaced with 'other._data'
	delete[] _data;
	_M = other._M;
	_N = other._N;

	_data = new double[_M*_N];

	//'this' pointer refers to the current object
	for (int x = 0; x < (_M*_N); x++)
	{
		this->_data[x] = other._data[x];
	}

	return *this;
}

Matrix Matrix::operator-(const Matrix& other)
{
	Matrix temp;
	temp._M = other._M;
	temp._N = other._N;

	temp._data = new double[temp._M*temp._N];

	for (int x = 0; x < (temp._M*temp._N); x++)
	{
		temp._data[x] = this->_data[x] - other._data[x];
	}

	return temp;
}

Matrix Matrix::operator/(const Matrix& other)
{
	Matrix temp;
	temp._M = other._M;
	temp._N = other._N;

	temp._data = new double[temp._M*temp._N];

	for (int x = 0; x < (temp._M*temp._N); x++)
	{
		temp._data[x] = this->_data[x] / other._data[x];
	}

	return temp;
}

Matrix Matrix::operator*(const Matrix& other)
{
	Matrix temp;
	temp._M = other._M;
	temp._N = other._N;

	temp._data = new double[temp._M*temp._N];

	for (int x = 0; x < (temp._M*temp._N); x++)
	{
		temp._data[x] = this->_data[x] * other._data[x];
	}

	return temp;
}

Matrix Matrix::operator++(int)
{
	Matrix temp;
	temp._M = _M;
	temp._N = _N;

	temp._data = new double[temp._M*temp._N];

	for (int x = 0; x < (temp._M*temp._N); x++)
	{
		temp._data[x] = this->_data[x]++;
	}

	return temp;
}

// Functions.
int Matrix::getM()
{
	return _M;
}

int Matrix::getN()
{
	return _N;
}

// Creating a submatrix from a given matrix.
Matrix Matrix::getBlock(int start_row, int end_row, int start_column, int end_column)
{
	// set number of rows and columns in the sub-image
	int rows = (end_row - start_row) + 1;
	int columns = (end_column - start_column) + 1;
	// reserve the memory for data extracted from the main image
	double* tempData = new double[rows*columns];

	int n = 0;

	// iterate through the image data, extracting the values according to the rows/columns
	for (int x = start_row; x < (rows + start_row); x++)
	{
		for (int y = start_column; y < (columns + start_column); y++)
		{
			tempData[n] = get(x, y);
			n++;
		}
	}

	Matrix temp(rows, columns, tempData);

	// delete the heap memory reserved for the extracted data
	delete[] tempData;

	return temp;
}

Matrix Matrix::add(const Matrix& other)
{
	//create temporary array of row*colum size
	double* data = new double[other._M*other._N];

	//fill the array with other._data
	for (int x = 0; x < (other._M*other._N); x++)
	{
		data[x] = this->_data[x] + other._data[x];
	}

	//create a temporary Matrix object with the row/column/data info
	Matrix temp(other._M, other._N, data);
	//delete the data array (which we can do as the array is 'deep copied' when 'temp' is created
	delete[] data;

	return temp;
}


// Sets a block of (32*32) pixels to the contents of the input variable 'Matrix block'
void Matrix::setBlock(int startRow, int startColumn, Matrix block)
{
	int rowTemp = startRow;
	int colTemp = startColumn;

	for (int i = 0; i < 32 * 32; i++)
	{
		set(rowTemp, colTemp, block.getData()[i]);
		colTemp++;
		if (colTemp > startColumn + 31)
		{
			rowTemp++;
			colTemp = startColumn;
		}
	}
}

// Sets a block (32 * 32 pixels) to white so it will not be matched again.
void Matrix::setBlock(int startRow, int startColumn)
{
	int rowTemp = startRow;
	int colTemp = startColumn;

	for (int i = 0; i < 32 * 32; i++)
	{
		set(rowTemp, colTemp, 255);
		colTemp++;
		if (colTemp > startColumn + 31)
		{
			rowTemp++;
			colTemp = startColumn;
		}
	}
}

// This function is tailored to the specific noisy image of the Lincoln logo, it may not work for other images - especially the last IF statement.
void Matrix::reduceNoise()
{
	for (int i = 0; i < _N * _M; i++)
	{
		// If a black pixel is completely surrounded by white pixels then it will make the black pixel white.
		if (i > 512 && i < (_N * _N) - _N && _data[i] == 0 && _data[i + _N] != 0 && _data[i - _N] != 0 && _data[i - 1] != 0 && _data[i + 1] != 0)
		{
			_data[i] = 255;
		}

		// Removes further noise - If a white pixel has a black pixel above and below, it will make the white pixel black.
		if (i > 512 && i < (_N * _N) - 512 && _data[i] == 255 && _data[i + _N] == 0 && _data[i - _N] == 0)
		{
			_data[i] = 0;
		}

		// Sorts out noise that is 2 pixels together instead of just one.
		if (_data[i] == 255 && _data[i + 1] == 255 && _data[i - 1] == 0 && _data[i + 2] == 0)
		{
			_data[i] = 0;
		}

		// If a pixel does not match the pixels to the left and right of it...
		if (_data[i] != _data[i + 1] && _data[i] != _data[i - 2])
		{
			// and that pixel is white...
			if (_data[i] == 255)
			{
				//then it will replace the pixel with the colour of the pixel to the right.
				_data[i] = _data[i + 1];
			}
		}
	}
}

// Returns the value held in position i,j of a matrix.
double Matrix::get(int i, int j)
{
	return _data[(i * _N) + j];
}

void Matrix::set(int i, int j, double val)
{
	_data[i * _N + j] = val;
}

double* Matrix::getData()
{
	return _data;
}

//destructor
Matrix::~Matrix()
{
	delete[] _data;
}