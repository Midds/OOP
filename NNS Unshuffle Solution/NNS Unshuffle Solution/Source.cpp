/* This program unshuffles a shuffled image of the University of Lincoln logo using a NNS algorithm to compare the unshuffled image to a noisy image of the logo using the Sum of Squared Differences as a similarity criteria.
As such the variables and functions used are tailored the the one specific input image although it could easily be altered to work with other images too.
The algorithm itself uses a linear search to go through the image, as it is the simplest to implement and a faster search isn't needed for such a small image.
Finally, the image is output in .pgm format, so it will require a pgm viewer such as IrfanView to open.*/
#include "Matrix.h"

double* readTXT(char *fileName, int sizeR, int sizeC);
void WritePGM(char *filename, double *data, int sizeR, int sizeC, int Q);

int main()
{
	// M and N represent the number of rows and columns in the image in pixels.
	int M = 512; int N = 512;

	// input_data is a pointer to a 1D array of M*N doubles stored in heap. Memory allocation is performed 
	// inside readTXT. readTXT will read an image (in .pgm format) of size MxN and will store the result in input_data.
	// once you're done with data DO NOT forget to delete the memory as in the end of this main() function
	double* input_data = 0;
	double* input_data_noisy = 0;

	std::cout << std::endl << "Unshuffling image............." << std::endl;

	// The images are held in text files with a number for each pixel in the image, so they need to be read in and then the matrix class is used to store them/manipulate the data.
	char* inputFileShuffled = ("shuffled_logo.txt");
	char* inputFileNoisy = ("unshuffled_logo_noisy.txt");
	input_data = readTXT(inputFileShuffled, M, N);
	input_data_noisy = readTXT(inputFileNoisy, M, N);

	// Creates two marices, one holds the noisy input and one for the shuffled input.
	Matrix noisyImage(M, N, input_data_noisy);
	Matrix shuffledImage(M, N, input_data);

	// Freeing up memory.
	delete[] input_data;
	delete[] input_data_noisy;

	// Calls a function to reduce the noise of the noisy image (this greatly imporves the accuracy of the NNS algorithm), this is further explained inside the function itself in Matrix.cpp
	noisyImage.reduceNoise();

	// Creates a copy of the shuffled image, this is needed as the original shuffledImage will be turned to binary image.
	Matrix shuffledImageCopy = shuffledImage;

	// Turning both input images to binary for a more accurate comparison between them.
	BinaryImage(M, N, shuffledImage.getData(), 90);
	BinaryImage(M, N, noisyImage.getData(), 90);

	// These matrices will be used to calculate the sum of squared differences.
	Matrix subMatrixN;
	Matrix subMatrixS;
	Matrix diff;
	Matrix squared;
	// unshuffled matrix will hold the final unshuffled image.
	Matrix unshuffled(512, 512, shuffledImageCopy.getData());

	int ssdSum = 0;
	// Two sets of variables to keep track of the block being checked for both the shuffled and noisy image.
	int startRow = 0, endRow = 31, startColumn = 0, endColumn = 31;
	int startRow1 = 0, endRow1 = 31, startColumn1 = 0, endColumn1 = 31;
	// rowTemp and colTemp will hold the location of where the matching block needs to be placed.
	int rowTemp = 0; int colTemp = 0;

	for (int i = 0; i < 16 * 16; i++)
	{
		// subMatrixS takes the first block of the shuffled image (converted to binary), to compare against subMatrixN (also converted to binary), each iteration of the loop takes the next block.
		subMatrixS = shuffledImage.getBlock(startRow, endRow, startColumn, endColumn);

		// ssdTemp is a value to compare the first SSD against, must be a large number to garuantee that the SSD is smaller for the first comparison.
		// ssdTemp could be used as a threshold to speed up the run time of the algorithm, instead of searching through every 32*32 pixel block in the image for the best possible SSD score - if a threshhold was used it would stop searching when a 'good enough' match is found instead, but this may make the result less accurate.
		int ssdTemp = 9999999999;
		for (int j = 0; j < 16 * 16; j++)
		{
			// submatrixN takes the first 32*32 block of the noisy image, the sum of squared differences is then created to for both submatrices
			subMatrixN = noisyImage.getBlock(startRow1, endRow1, startColumn1, endColumn1);
			diff = subMatrixS - subMatrixN;
			squared = diff * diff;

			// Getting the sum of squared.
			for (int k = 0; k < 31 * 31; k++)
			{
				ssdSum = ssdSum + squared.getData()[k];
			}
			// If the ssd sum is 0 it means a perfect match has been found.
			if (ssdSum == 0)
			{
				// Assigns rowTemp and colTemp to the position of the matching block, these will be used later.
				rowTemp = startRow1;
				colTemp = startColumn1;
				// Perfect match means no need to keep looking, so we can break out of this loop now.
				break;
			}
			// If the sum of squared differences is smaller than the previous iteration...
			if (ssdSum <= ssdTemp)
			{
				// ...then it will replace the previous matching block with the current block as the correct match.
				ssdTemp = ssdSum;
				// Assigns rowTemp and colTemp to the position of the matching block, if no better match is found then these variables will be used to set the block in the correct place.
				rowTemp = startRow1;
				colTemp = startColumn1;
			}
			// Resets the ssdSum for the next iteration of the loop.
			ssdSum = 0;
			// Make the loop check the next block along in it's next iteration.
			startColumn1 = startColumn1 + 32;
			endColumn1 = endColumn1 + 32;

			// If the startcolumn is >= M it means that it has reached the end of the row...
			if (startColumn1 >= M)
			{
				// ...so the rowcount is moved down 32 pixels to check the next line of blocks and the column count is reset.
				startColumn1 = 0;
				endColumn1 = 31;
				startRow1 = startRow1 + 32;
				endRow1 = endRow1 + 32;
			}
		}
		// Sets the block currently being matched (shuffledImageCopy.getBlock) to its correct position on the x/y axis for the final image using the rowTemp and colTemp variables.
		// This takes the block from shuffledImageCopy, which is the Non-binary version of the shuffled image. The binary version is used only for comparison purposes.
		unshuffled.setBlock(rowTemp, colTemp, shuffledImageCopy.getBlock(startRow, endRow, startColumn, endColumn));

		// Changes the block that has been found to white so it won't be matched up again.
		noisyImage.setBlock(rowTemp, colTemp);

		// Resets the row and column count of the noisyImage for the next iteration of the loop.
		startRow1 = 0;
		startColumn1 = 0;
		endColumn1 = 31;
		endRow1 = 31;

		// Moves the block being checked in the shuffled image along 32 pixels to the next block.
		startColumn = startColumn + 32;
		endColumn = endColumn + 32;

		// If the startcolumn is >= M it means that it has reached the end of the row...
		if (startColumn >= M)
		{
			// ...so the rowcount is moved down 32 pixels to check the next line of blocks and the column count is reset.
			startColumn = 0;
			endColumn = 31;
			startRow = startRow + 32;
			endRow = endRow + 32;
		}
	}

	// Use Q = 255 for greyscale images and Q = 1 for binary images.
	int Q = 255;

	WritePGM("Final unshuffled logo.pgm", unshuffled.getData(), M, N, Q);

	std::cout << "Finished output \"Final unshuffled logo.pgm\" created in solution file folder" << std::endl;
	std::cout << std::endl << "Press enter to exit..." << std::endl;
	getchar();
	return 0;
}

// Read .txt file with image of size RxC, and convert to an array of doubles
double* readTXT(char *fileName, int sizeR, int sizeC)
{
	double* data = new double[sizeR*sizeC];
	int i = 0;
	std::ifstream myfile(fileName);
	if (myfile.is_open())
	{

		while (myfile.good())
		{
			if (i>sizeR*sizeC - 1) break;
			myfile >> *(data + i);
			i++;
		}
		myfile.close();
	}

	else std::cout << std::endl << "Unable to open file";


	return data;
}

// Convert data from double to .pgm stored in filename
void WritePGM(char *filename, double *data, int sizeR, int sizeC, int Q)
{

	int i, j;
	unsigned char *image;
	std::ofstream myfile;

	image = (unsigned char *) new unsigned char[sizeR*sizeC];

	// Converting interger values to unsigned char

	for (i = 0; i<sizeR*sizeC; i++)
		image[i] = (unsigned char)data[i];

	myfile.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);

	if (!myfile) {
		std::cout << "Can't open file: " << filename << std::endl;
		exit(1);
	}

	myfile << "P5" << std::endl;
	myfile << sizeC << " " << sizeR << std::endl;
	myfile << Q << std::endl;

	myfile.write(reinterpret_cast<char *>(image), (sizeR*sizeC)*sizeof(unsigned char));

	if (myfile.fail()) {
		std::cout << "Can't write image " << filename << std::endl;
		exit(0);
	}

	myfile.close();

	delete[] image;
}

