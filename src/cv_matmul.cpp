#include <iostream>
#include "opencv2/opencv.hpp"

#define DMSGN(x) do { std::cout << x << std::endl; } while(0)
#define DMSG(x) do { std::cout << x << " "; } while(0)

using namespace std;
using namespace cv;

static inline double timestamp()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (ts.tv_sec + ts.tv_nsec * 1e-9) * 1000;
}

void fill_tensor(Mat& src)
{
	float low = -1.0f;
	float high = 1.0f;

	randu(src, Scalar(low), Scalar(high));
}

void print_tensor(Mat src)
{
	for(int y = 0; y < src.rows; y++)
	{
		for(int x = 0; x < src.cols; x++)
		{
			DMSG(src.at<float>(y,x));
		}
		DMSGN("");
	}
}

void matmul(Mat src0, Mat src1, Mat& dst)
{
	dst = src1 * src0;
}

/*
 * A x B -> C
 * src1 = A, src0=B, dst=C
 * $ nice -2 ./native_matmul [B's col] [A's row] [A's col]
 */
int main(int argc, char** argv)
{
	if(argc < 3)
	{
		DMSGN("Usage: $./test M N K");
		return 1;
	}

	int M = atoi(argv[1]);
	int N = atoi(argv[2]);
	int K = atoi(argv[3]);

	Mat src0 = Mat (K, M, CV_32FC1);
	Mat src1 = Mat (N, K, CV_32FC1);
	Mat dst  = Mat (N, M, CV_32FC1);

	fill_tensor(src0);
	fill_tensor(src1);

	double ts1 = timestamp();
	matmul(src0, src1, dst);
	double ts2 = timestamp();

	DMSGN("Opencv: (" << N << "x" << K << ") x (" << K << ", " << M << ") time: " << ts2-ts1); 

	return 0;
}
