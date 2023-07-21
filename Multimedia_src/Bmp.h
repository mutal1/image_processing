//
//	bmp.h
//

#include "global.h"

typedef struct tagBITMAP {  /* bm */
	int		bmType;
    int     bmWidth;
    int     bmHeight;
    int     bmWidthBytes;
    ushort  bmPlanes;
    ushort  bmBitsPixel;
    long	*bmBits;
} BITMAP;
			
typedef struct tagRGBQUAD { // rgbq 
    uchar   rgbBlue; 
    uchar   rgbGreen; 
    uchar   rgbRed; 
    uchar   rgbReserved; 
} RGBQUAD; 
 

typedef struct tagBITMAPINFOHEADER{ // bmih 
	ulong	biSize; 
	long	biWidth; 
	long	biHeight; 
	ushort	biPlanes; 
	ushort	biBitCount; 
	ulong	biCompression; 
	ulong	biSizeImage; 
	long	biXPelsPerMeter; 
	long	biYPelsPerMeter; 
	ulong	biClrUsed; 
	ulong  biClrImportant; 
} BITMAPINFOHEADER; 
 

BITMAP	inBM;
ushort	bmptype;
ulong	filesize;
ushort	res1,res2;
ulong	pixoff;    
ulong	bmisize;                    
ulong	compression;
ulong	sizeimage;
long	xscale, yscale;
ulong	colors;
ulong	impcol;
uchar	r, g, b, dummy;
RGBQUAD *colormap;

void	LoadBMP(FILE *fp, int *hsize, int *vsize);
void	WriteBMP(FILE *fp, int hsize, int vsize);
long	GetLong(FILE *fp);
short	GetWord(FILE *fp);
uchar	GetByte(FILE *fp);
void	Vertical_Flip(uchar *src, int hsize, int vsize);
