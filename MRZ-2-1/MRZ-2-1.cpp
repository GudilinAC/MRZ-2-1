//@author CImg team, version 2.4.1
#include "CImg-2.4.1\CImg.h"
#include <iostream>
#include <fstream>

//@author Гудилин Андрей, гр.621702
#define COLOR_CONST 3
#define STEP_INIT_CONST 500
#define INPUT_FILE_NAME "input.bmp"

int n = 8;
int m = 8;
int p = 30;
double e = 620;

cimg_library::CImg<unsigned char> image(INPUT_FILE_NAME);
int width = image.width();
int height = image.height();
int w = width / m;
int h = height / n;
int N = n * m * COLOR_CONST;
int L = h * w;
double E = 0;
double Eq = 0;
double* a1 = 0;
double a2 = 0;
float** X;
float* Y;
float** W[2];
float* Xresult;

void saveImage();

//@author Гудилин Андрей, гр.621702
int main()
{	
	char* str = new char[(N + L)* p * sizeof(float) + 3];
	std::ofstream file("commpressed", std::ios_base::binary | std::ios_base::trunc);
	file.write(str, (N + L) * p * sizeof(float) + 3);
	file.close();
	delete[]str;
	
	std::ifstream in(INPUT_FILE_NAME, std::ifstream::ate | std::ifstream::binary);
	double inSize = in.tellg();
	in.close();

	std::ifstream out("commpressed", std::ifstream::ate | std::ifstream::binary);
	double outSize = out.tellg();
	in.close();

	std::cout << "Z: " << inSize / outSize << std::endl;

	W[0] = new float*[N];
	W[1] = new float*[p];
	for (int i = 0; i < N; i++) {
		W[0][i] = new float[p];
		for (int j = 0; j < p; j++) {
			W[0][i][j] = (((float)(rand()) / RAND_MAX * 2) - 1) * 0.1;
		}
	}
	for (int i = 0; i < p; i++) {
		W[1][i] = new float[N];
		for (int j = 0; j < N; j++) {
			W[1][i][j] = W[0][j][i];
		}
	}

	Xresult = new float[N];
	Y = new float[p];

	X = new float*[L];
	for (int i = 0; i < L; i++) {
		X[i] = new float[N];
	}

	int Li = -1;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int num = 0;
			Li++;
			for (int i1 = 0; i1 < n; i1++) {
				for (int j1 = 0; j1 < m; j1++) {
					for (int k = 0; k < COLOR_CONST; k++) {
						X[Li][num++] = (2.0 * image(j*m + j1, i*n + i1, 0, k) / 255) - 1;
					}
				}
			}
		}
	}

	float* ww = new float[p];

	a1 = new double[L];
	for (int q = 0; q < L; q++) {
		a1[q] = STEP_INIT_CONST;
		for (int i = 0; i < N; i++) {
			a1[q] += (X[q][i] * X[q][i]);
		}
		a1[q] = 1 / a1[q];
	}

	double oX;
	int it = 0;
	do
	{
		for (int q = 0; q < L; q++) {
			a2 = 0;
			for (int i = 0; i < p; i++) {
				Y[i] = 0;
				for (int j = 0; j < N; j++) {
					Y[i] += W[0][j][i] * X[q][j];
				}
			}
			for (int i = 0; i < N; i++) {
				Xresult[i] = 0;
				for (int j = 0; j < p; j++) {
					Xresult[i] += W[1][j][i] * Y[j];
				}
			}
			a2 = STEP_INIT_CONST;
			for (int i = 0; i < p; i++) {
				a2 += (Y[i] * Y[i]);
			}
			a2 = 1 / a2;
			for (int i = 0; i < p; i++) {
				ww[i] = 0;
				for (int j = 0; j < N; j++) {
					ww[i] += (Xresult[j] - X[q][j]) * W[1][i][j];
				}
			}
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < p; j++) {
					W[0][i][j] -= (a1[q] * X[q][i] * ww[j]);
				}
			}
			for (int i = 0; i < p; i++) {
				for (int j = 0; j < N; j++) {
					W[1][i][j] -= a2 * Y[i] * (Xresult[j] - X[q][j]);
				}
			}
		}
		E = 0;
		for (int q = 0; q < L; q++) {
			{
				Eq = 0;
				for (int i = 0; i < p; i++) {
					Y[i] = 0;
					for (int j = 0; j < N; j++) {
						Y[i] += W[0][j][i] * X[q][j];
					}
				}
				for (int i = 0; i < N; i++) {
					Xresult[i] = 0;
					for (int j = 0; j < p; j++) {
						Xresult[i] += W[1][j][i] * Y[j];
					}
				}
				for (int i = 0; i < N; i++) {
					oX = (Xresult[i] - X[q][i]);
					Eq += (oX * oX);
				}
				E += Eq;
			}
		}
		std::cout << E << "\t";
		it++;
	} while (E > (e * 2));
	saveImage();
	std::cout << std::endl << std::endl;
	std::cout << "E: " << E << std::endl;
	std::cout << "Iterations: " << it << std::endl;

	//std::cout << std::endl << "W: " << std::endl;
	//for (int i = 0; i < N; i++) {
	//	for (int j = 0; j < p; j++) {
	//		std::cout << W[0][i][j] << "\t";
	//	}
	//	std::cout << std::endl;
	//}

	//std::cout << std::endl << "W': " << std::endl;
	//for (int i = 0; i < p; i++) {
	//	for (int j = 0; j < N; j++) {
	//		std::cout << W[1][i][j] << "\t\t";
	//	}
	//	std::cout << std::endl;
	//}
	system("pause");
}

//@author Гудилин Андрей, гр.621702
void saveImage() {
	cimg_library::CImg<unsigned char> img(width, height, 1, 3);
	img.fill(0);
	for (int q = 0; q < L; q++) {
		for (int i = 0; i < p; i++) {
			Y[i] = 0;
			for (int j = 0; j < N; j++) {
				Y[i] += W[0][j][i] * X[q][j];
			}
		}
		double b = 0;
		for (int i = 0; i < N; i++) {
			Xresult[i] = 0;
			for (int j = 0; j < p; j++) {
				Xresult[i] += W[1][j][i] * Y[j];
			}
		}
		int pos = 0;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				for (int k = 0; k < COLOR_CONST; k++) {
					int pixel = (int)(255 * ((Xresult[pos++] + 1) / 2));
					if (pixel > 255) {
						pixel = 255;
					}
					if (pixel < 0) {
						pixel = 0;
					}
					img(q % w * m + j, q / h * n + i, 0, k) = pixel;
				}
			}
		}
	}
	img.save_bmp("result.bmp");
}