//
//																																			
//	Program : BMP library (only 8bpp support)														
//																																			
//	Author	: T.Y.Chung																									
//																																		
//	Date	: 20xx. xx																										
//																																			
//


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "global.h"
#include "bmp.h"

#define	BMP_HEADERSIZE						(2*5+4*11)		// word->2, long->11
#define	BMP_BYTESPERLINE(width, bits)		((((width)*(bits)+31)/32)*4)
#define	BMP_PIXELSIZE(width, height, bits)	(((((width)*(bits)+31)/32)*4)*height)	


//
//																																			
//	Subroutine :  LoadBMP()																							
//	Purpose    :  read and parsing of BMP header												
//																																			
//

void	LoadBMP(FILE *fp, int *Width, int *Height)
{

	int		i;
	ulong	byte_cnt=0;

	bmptype =		GetWord(fp);	byte_cnt+=2;						// bmp type
	filesize=		GetLong(fp);	byte_cnt+=4;						// bmp file size
	res1=			GetWord(fp);	byte_cnt+=2;							// reserved1
	res2=			GetWord(fp);	byte_cnt+=2;							// reserved2
	pixoff=			GetLong(fp);	byte_cnt+=4;						// offset bytes No. to image bits
				 
	//	read BitMapInfoHeader 			
	bmisize=		GetLong(fp);	byte_cnt+=4;						// structure size of this header
	inBM.bmWidth=	GetLong(fp);	byte_cnt+=4;					// pixel no. per line
	inBM.bmHeight=	GetLong(fp);	byte_cnt+=4;				// line no. per frame 
	inBM.bmPlanes=	GetWord(fp);	byte_cnt+=2;				// plane no. of Device	
	inBM.bmBitsPixel=GetWord(fp);	byte_cnt+=2;				// bit no. per pixel	
	compression=	GetLong(fp);	byte_cnt+=4;					// compression type				
	sizeimage=		GetLong(fp);	byte_cnt+=4;					// size of byte in image
	xscale=			GetLong(fp);	byte_cnt+=4;						// pixel per miter - horizontal
	yscale=			GetLong(fp);	byte_cnt+=4;						// pixel per miter - vertical
	colors=			GetLong(fp);	byte_cnt+=4;						// used color no.		
	impcol=			GetLong(fp);	byte_cnt+=4;						// important color no. 
				

	// check BMP file read error
	if(bmptype!=0x4D42) {	//'B->0x42 M'->0x4d
		printf("\n error -> Not a valid BMP File");
		exit(0);
	}

	if(inBM.bmPlanes!=1) {	// This value must be 1. 
		printf("\n error -> Not a valid BMP File->Plane No.");
		exit(0);
	}	        	
			
	if(compression!=0)	{ // BMP type must be not compression mode.  
		printf("\n error->Not a valid BMP File->Compression Type");
		fclose(fp);
		return;
	}		
			
	if(colors==0) {
		colors=1 << inBM.bmBitsPixel;
	}

	// read colormap
	colormap = NULL;
		
	switch(inBM.bmBitsPixel) {
		case 1:
		case 4:
		case 24:
			printf("\n error -> %dbpp file not supported", inBM.bmBitsPixel);  
			exit(0);
			break;
			
		// read pallete 
		case 8:
			if(!(colormap=(RGBQUAD *)malloc(sizeof(RGBQUAD)*256))) {
				printf("\n error -> malloc failed");
				exit(0);
			}
			
			if(colormap==NULL) {
				printf("\n error -> Out of memory");
				exit(0);
			}
				
			// Color Table for 256 colors
			for(i=0;i<(int)colors;i++) {
				b=		GetByte(fp);		byte_cnt++;
				g=		GetByte(fp);		byte_cnt++;
				r=		GetByte(fp);		byte_cnt++;
				dummy=	GetByte(fp);		byte_cnt++;
				
				colormap[i].rgbBlue=b;
				colormap[i].rgbRed=r;
				colormap[i].rgbGreen=g;
					
			}
			
			break;
		}

		if(byte_cnt>pixoff) {
			printf("\n error -> Corrupt palette");
			exit(0);
		}

		while(byte_cnt<pixoff) {
			dummy=GetByte(fp);	
			byte_cnt++;
		}

		*Width= inBM.bmWidth;
		*Height=inBM.bmHeight;

}	


//
//																																			
//	Subroutine :  WriteBMP()																						
//	Purpose    :  write BMP header																			
//																																			
//

void	WriteBMP(FILE *fp, int Width, int Height)
{
	int		i;
	int		datasize, colormapsize;
	ulong	byte_cnt;
	ulong	bmfsize;
	ulong	cmpsize;
	
	BITMAPINFOHEADER header;

	bmptype=0x4d42;		// 'BM'
	res1=0;
    res2=0;
    compression=0;
	byte_cnt=0;

	// 8bit gray level
		
	colormapsize=256*4;
	datasize=BMP_PIXELSIZE(Width, Height, 8);
	
	bmfsize=BMP_HEADERSIZE+colormapsize+datasize;
	pixoff=BMP_HEADERSIZE+colormapsize;
	cmpsize=datasize;
	header.biBitCount=8;
	header.biClrUsed=256;
	header.biClrImportant=256;
	
  	header.biSize=40; 						// header size
	header.biWidth=Width;
	header.biHeight=Height;
	header.biPlanes=1;
	header.biCompression=0;					// no compression
	header.biSizeImage=0;
	header.biXPelsPerMeter=0;
	header.biYPelsPerMeter=0;
	
	// Write BitMapFileHeader
	fwrite((ushort *)&(bmptype),2,1,fp); byte_cnt+=2;
	fwrite((ulong  *)&(bmfsize),4,1,fp); byte_cnt+=4;
	fwrite((int   *)&(res1),2,1,fp);	byte_cnt+=2;
	fwrite((int   *)&(res2),2,1,fp);	byte_cnt+=2;
	fwrite((ulong *)&(pixoff),4,1,fp);	byte_cnt+=4;

	// Write BitMapInfoHeader
	fwrite((ulong *)&(header.biSize),4,1,fp);			byte_cnt+=4;
	fwrite((long  *)&(header.biWidth),4,1,fp);			byte_cnt+=4;
	fwrite((long  *)&(header.biHeight),4,1,fp);			byte_cnt+=4;
	fwrite((ushort *)&(header.biPlanes),2,1,fp);		byte_cnt+=2;
	fwrite((ushort *)&(header.biBitCount),2,1,fp);		byte_cnt+=2;
	fwrite((ulong *)&(compression),4,1,fp);				byte_cnt+=4;
	fwrite((ulong *)&(cmpsize),4,1,fp);					byte_cnt+=4;
	fwrite((long  *)&(header.biXPelsPerMeter),4,1,fp);	byte_cnt+=4;
	fwrite((long  *)&(header.biYPelsPerMeter),4,1,fp);	byte_cnt+=4;
	fwrite((ulong *)&(header.biClrUsed), 4, 1, fp);		byte_cnt+=4;
	fwrite((ulong *)&(header.biClrImportant), 4, 1,fp);	byte_cnt+=4;

	for(i=0; i<256; i++) {
		putc(i, fp);	// b
		putc(i, fp);	// g
		putc(i, fp);	// r
		putc(0, fp);	// dummy
	}

}


//
//
//	Subroutine :  Vertical_Flip()																				
//	Purpose    :  Memory reverse for BMP file save											
//																																			
//

void Vertical_Flip(uchar *src, int hsize, int vsize)
{

	uchar	*tb1;
	uchar	*tb2;
	int		row_cnt;     
	int		off1;
	int		off2;
	int		bufsize;

	bufsize=hsize;

	if(!(tb1=(uchar *)malloc(bufsize))) {
		printf("\n error -> malloc failed");
		exit(1);
	}
	
	if(!(tb2=(uchar *)malloc(bufsize))) {
		printf("\n error -> malloc failed");
		exit(1);
	}
	
	for (row_cnt=0;row_cnt<(vsize+1)/2;row_cnt++) {
		off1=row_cnt*bufsize;
		off2=((vsize-1)-row_cnt)*bufsize;   
		
		memcpy(tb1, src+off1, bufsize);
		memcpy(tb2, src+off2, bufsize);	
		memcpy(src+off1, tb2, bufsize);
		memcpy(src+off2, tb1, bufsize);
	}	
	
}	

//
//																																			
//	Subroutine :  GetByte()																							
//	Purpose    :  read 1 byte																						
//																																			
//

uchar	GetByte(FILE *fp)
{
	uchar	databuf;
	
	fread((uchar *)&(databuf),1,1,fp);	

	return	databuf;
}


//
//																																			
//	Subroutine :  GetWord()																							
//	Purpose    :  read 2 byte																						
//																																			
//

short	GetWord(FILE *fp)
{
	
	short	databuf;

	fread((short *)&(databuf),2,1,fp);
  
	return	databuf;

}


//
//																																			
//	Subroutine :  GetLong()																							
//	Purpose    :  read 4 byte																						
//																																			
//

long	GetLong(FILE *fp)
{

	long	databuf;

	fread((long *)&(databuf),4,1,fp);
  
	return	databuf;
  	
}
