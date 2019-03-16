#include <iostream>
#include <cstdlib>

#define DMSG(x) do { std::cout << x << std::endl; } while(0)

const float lower_bnd = -1.f;
const float upper_bnd = 1.f;

int M = -1;
int N = -1;
int K = -1;

float** src0;
float** src1;
float** dst;

static inline double timestamp()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (ts.tv_sec + ts.tv_nsec * 1e-9) * 1000;
}

void fillTensor(float** src, int row, int col)
{
	std::srand(time(NULL)/col);

	// allocate & fill memory
	for(int y = 0; y < row; y++)
	{
		src[y] = (float*)malloc(sizeof(float)*col);	
		for(int x = 0; x < col; x++)
		{
			src[y][x] = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX/2)) - 1;
		}
	}
}

void freeTensor(float** src, int row, int col)
{
	for(int y = 0; y < row; y++)
	{
		free(src[y]);
	}
	free(src);
}

void printTensor(float** src, const int row, const int col)
{
	for(int y = 0; y < row; y++)
	{
		float* tmp = src[y];
		for(int x = 0; x < col; x++)
		{
			std::cout << tmp[x] << " ";
		}
		std::cout << std::endl;
	}
}

// matmul: src1 x src0 = dst
void matmul()
{
	for(int i = 0; i < N; i++)
	{
		for(int l = 0; l < M; l++)
		{
			float val = 0.0f;
			for(int j = 0; j < K; j++)
			{
				val += src1[i][j] * src0[j][l];
			}
			dst[i][l] = val;
		}
	}
}

/*
 * A x B -> C
 * src1 = A, src0=B, dst=C
 * $ nice -2 ./native_matmul [B's col] [A's row] [A's col]
 */
int main(int argc, char** argv)
{
	if(argc < 4)
	{
		DMSG("Usage: ./native_matmul M N K");

		return 1;
	}

	M = atoi(argv[1]);
	N = atoi(argv[2]);
	K = atoi(argv[3]);

	// init & fill the tensor
	src0 = (float**)malloc(sizeof(float*)*K);
	src1 = (float**)malloc(sizeof(float*)*N);
	dst = (float**)malloc(sizeof(float*)*N);

	fillTensor(src0, K, M);
	fillTensor(src1, N, K);
	fillTensor(dst , N, M);

	// matrix multiplication
	double ts1 = timestamp();
	matmul();
	double ts2 = timestamp();

	DMSG("nativ C++ (" << N << "x" << K << ") x (" << K << ", " << M << ") time: " << ts2-ts1);

	freeTensor(src0, K, M);
	freeTensor(src1, N, K);
	freeTensor(dst , N, M);

	return 0;
}

