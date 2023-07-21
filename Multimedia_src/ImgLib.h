//
// imglib.h
//

// define	State value for image library
#include "Global.h"
#define	FlipX		0
#define	FlipY		1
#define	FlipD		2
#define	PI			4*atan(1.)
#define WINDOW		4
#define MAX_TAP		64

void	Flip_Processing(int FlipType, uchar *org_img, uchar *recon_img, int *cols, int *rows);
void	ImageCopy(uchar *srcImg, uchar *desImg, int hSize, int vSize);
void	CircularZonePlate(uchar *recon_img, int cols, int rows, int shape);
void	Quantization(uchar *srcImg, uchar *desImg, int hSize, int vSize, int bits);
void	Sampling(uchar *srcImg, uchar *desImg, int hSize, int vSize, int hParam, int vParam, int mode);
void	lowPassFilter(uchar *srcImg, uchar *desImg, int hSize, int vSize, double fx, double fy);
void	subSample(uchar *srcImg, uchar *desImg, int hSize, int vSize, int hParam, int vParam);
void	FIR2dFilter(uchar *srcImg, uchar *desImg, int hSize, int vSize);
void	SimpleEdgeDetection(uchar *srcImg, uchar *desImg, int hSize, int vSize, int flag);
int		HomoOperation(uchar *window);
int		DiffOperation(uchar *window);
void	FirstDerivativeEdgeDetection(uchar *srcImg, uchar *desImg, int hSize, int vSize, int modeFlag, int directFlag);
void	CompassEdgeDetection(uchar *srcImg, uchar *desImg, int hSize, int vSize, int modeFlag);
void	MaskDump(int *target, int *window, int htap, int vtap);
void	EdgeMaskFilter(uchar *srcImg, uchar *desImg, int hSize, int vSize, int *window, int htap, int vtap);
void	CompassEdgeMaskFilter(uchar *srcImg, uchar *desImg, int hSize, int vSize, int **maskCoeff, 
							  int noDirect, int htap, int vtap);
void	SecondDerivativeEdgeDetection(uchar *srcImg, uchar *desImg, int hSize, int vSize, int modeFlag);
void	SecondEdgeMaskFilter(uchar *srcImg, uchar *desImg, int hSize, int vSize, int *window, int htap, int vtap);
void	ZeroCrossing1(int *inputImg, uchar *outputImg, int hSize, int vSize, int threshold);
void	ZeroCrossing2(int *inputImg, uchar *outputImg, int hSize, int vSize, int threshold);

void	MedianFilter(uchar *srcImg, uchar *desImg, int hSize, int vSize, int wSize);
int		MedianOperation(int *window, int noCoeff);
void	QuickSort(int *array, int left, int right);
void	Swap(int *array, int i, int j);

void	GetImageSizeInform(int incols, int inrows, double scale_X, double scale_Y, int *outCols, int *outRows);
void	Interpolation(uchar *srcImg, uchar *desImg, int inCols, int inRows, int newCols, int newRows, int mode);
void	NearestNeighborInterpol(uchar *srcImg, uchar *desImg, int inCols, int inRows, int newCols, int newRows);
void	BilinearInterpol(uchar *srcImg, uchar *desImg, int inCols, int inRows, int newCols, int newRows);
void	CubicInterpol(uchar *srcImg, uchar *desImg, int inCols, int inRows, int newCols, int newRows);
void	SplineInterpol(uchar *srcImg, uchar *desImg, int inCols, int inRows, int newCols, int newRows);
void	MakeThumbnailImage(uchar *srcImg, uchar *desImg, int hSize, int vSize, int newCols, int newRows, int wSize, int mode);
int		AverageOperation(int *window, int noCoeff);
