#include "Matrix.h"

// Default constructor
BinaryImage::BinaryImage()
{
	bM = 1;
	bN = 1;

	data = new double[bM*bN];

	for (int x = 0; x < bM*bN; x++)
	{
		data[x] = 0;
	}
}

BinaryImage::BinaryImage(int M, int N, double* input_data, double threshold)
{
	data = input_data;
	bM = M;
	bN = N;

	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (data[i * N + j] > threshold)
			{
				data[i * N + j] = 1;
			}
			else
			{
				data[i * N + j] = 0;
			}
		}
	}
}

// Destructor
BinaryImage::~BinaryImage()
{
}

// Copy constructor
BinaryImage::BinaryImage(const BinaryImage& b)
{
	std::cout << "Binary copy constructor" << std::endl;
	bM = b.bM;
	bN = b.bN;

	double* _data = new double[bM *bN];

	for (int i = 0; i < bM*bN; i++)
	{
		_data[i] = b.data[i];
	}
}
