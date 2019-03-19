#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <ctime>

#define DMSG(x) do { std::cout << x << std::endl; } while(0)

int M = -1;
int N = -1;
int K = -1;
int repeat = 0;

int8_t** src0;
int8_t** src1;
int8_t** dst;

void fillTensor(int8_t** src, int row, int col)
{
	std::srand(time(NULL)/col);

	// allocate & fill memory
	for(int y = 0; y < row; y++)
	{
		src[y] = (int8_t*)malloc(sizeof(int8_t)*col);	
		for(int x = 0; x < col; x++)
		{
			src[y][x] = (int8_t)(((static_cast<float>(rand()) / static_cast<float>(RAND_MAX/2)) - 1) * 10);
		}
	}
}

void freeTensor(int8_t** src, int row, int col)
{
	for(int y = 0; y < row; y++)
	{
		free(src[y]);
	}
	free(src);
}

void printTensor(int8_t** src, const int row, const int col)
{
	for(int y = 0; y < row; y++)
	{
		int8_t* tmp = src[y];
		for(int x = 0; x < col; x++)
		{
			std::cout << static_cast<int16_t>(tmp[x]) << " ";
		}
		std::cout << std::endl;
	}
}

// matmul: src1 x src0 = dst
// return average time
double matmul(int repeat)
{
	double time = 0;
	for(int iter = 0; iter < repeat; iter++)
	{
		double ts1 = clock();
		for(int i = 0; i < N; i++)
		{
			for(int l = 0; l < M; l++)
			{
				int8_t val = 0.0f;
				for(int j = 0; j < K; j++)
				{
					val += src0[i][j] * src1[j][l];
				}
				dst[i][l] = val;
			}
		}
		double ts2 = clock();
		time += (ts2 - ts1);
	}

	// Get average second for executing
	time = time / CLOCKS_PER_SEC / repeat;

	// Return nsec
	return time * 1e+9;
}

/*
 * A x B -> C
 * src1 = A, src0=B, dst=C
 * $ nice -2 ./native_matmul [B's col] [A's row] [A's col]
 */
int main(int argc, char** argv)
{
	if(argc < 5)
	{
		DMSG("Usage: ./native_matmul M N K Repeat");

		return 1;
	}

	N = atoi(argv[1]);
	K = atoi(argv[2]);
	M = atoi(argv[3]);
	repeat = atoi(argv[4]);

	// init & fill the tensor
	src0 = (int8_t**)malloc(sizeof(int8_t*)*N);
	src1 = (int8_t**)malloc(sizeof(int8_t*)*K);
	dst = (int8_t**)malloc(sizeof(int8_t*)*N);

	fillTensor(src0, N, K);
	fillTensor(src1, K, M);
	fillTensor(dst , N, M);

	// matrix multiplication
	double time = matmul(repeat);

	DMSG("nativ C++ (" << N << "x" << K << ") x (" << K << ", " << M << ") time: " << time << "ns");

	freeTensor(src0, N, K);
	freeTensor(src1, K, M);
	freeTensor(dst , N, M);

	return 0;
}
