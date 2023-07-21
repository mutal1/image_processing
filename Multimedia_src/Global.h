//
//	global.h
//	

// data type define
#include<stdio.h>
typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef unsigned long	ulong;

// Boolean values 
#define TRUE		1
#define FALSE		0
#define VALID		1
#define INVALID		0

#define	FILE_READ	0
#define	FILE_WRITE	1

#define	MAXBITS		8

FILE	*fp_in, *fp_out, *fp_txt;
uchar	*orgImg, *resImg, *copyImg;
char	filename[10][80];

void	Process_Options(int *argcp, char **argvp[]);
void	Stream_Open(char *fname, int flag);
void	InitializeBmpParam(FILE *fp, int *hSize, int *vSize);
void	ImageReadBmpFile(FILE *fp, int *hSize, int *vSize, uchar *imgBuf);
void	ImageWriteBmpFile(FILE *fp, int hSize, int vSize, uchar *imgBuf);

uchar	*Buff_Alloc(int size, int count);
