//
//																										
//	Program : Image Processing Library														
//		
//	Author	: T.Y.Chung				
//																																			
//	Date	: 20XX. 11																										
//																																			
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"
#include "imglib.h"

static int	DeciFilter[7]={-29, 0, 88, 138, 88, 0, -29};
static int	InterpolFilter[4] = {-12, 140, 140,-12};	


//
//
//	Subroutine	:  ImageCopy()			
//	Purpose		:  copy source image to destination image	
//		

void	ImageCopy(uchar *srcImg, uchar *desImg, int hSize, int vSize)
{

	int		x, y;

	for(y=0; y<vSize; y++) {
		for(x=0; x<hSize; x++) {
			desImg[y*hSize+x]=srcImg[y*hSize+x];
			
//			if(tmp>235)			desImg[y*hSize+x]=235;
//			else if(tmp<16)		desImg[y*hSize+x]=16;
//			else				desImg[y*hSize+x]=(uchar)tmp;
				
		}
	}
}


//
//																		
//	Subroutine	:  Flip_Processing()									
//	Purpose		:  Flipping of image - x, y, d axis						
//																		
//

void	Flip_Processing(int FlipType, uchar *org_img, uchar *recon_img, int *cols, int *rows)
{

	int		in_rows, in_cols;
	int		x, y;
	uchar	temp_data;
	int		temp;

	in_rows=*rows;
	in_cols=*cols;

	 
	if(FlipType==FlipX) {				// Flip along X direction
		for(y=0; y<in_rows; y++) {
			for(x=0; x<(in_cols/2); x++) {
				temp_data=org_img[y*in_cols+x];
				recon_img[y*in_cols+x]=org_img[y*in_cols+in_cols-1-x];
				recon_img[y*in_cols+in_cols-1-x]=temp_data;
			}
		}     
	}
	
	else if(FlipType==FlipY) {
		for(y=0; y<(in_rows/2); y++) {	// Flip along Y direction 
			for(x=0; x<in_cols; x++) {
	       		temp_data=org_img[y*in_cols+x];
				recon_img[y*in_cols+x]=org_img[(in_rows-1-y)*in_cols+x];
				recon_img[(in_rows-1-y)*in_cols+x]=temp_data;
			}
		}
	}

	else if(FlipType==FlipD) {			// Flip along X-Y diagonal 
		for(y=0; y<in_rows; y++) {
			for(x= 0; x<in_cols; x++) {
				recon_img[x*in_rows+y]=org_img[y*in_cols+x];
			}
		}	    

		temp=in_cols;
		in_cols=in_rows;
		in_rows=temp;
	
	}

	else {
		printf("\n no support Flip mode");
		exit(0);
	}

	*cols=in_cols;
	*rows=in_rows;

}

//
//	
//	Subroutine	:  czp()																					
//	Purpose		:  Zone Plate Circle																			
//																										
//

void	CircularZonePlate(uchar *imgBuf, int hSize, int vSize, int shape)
{
	int		i, j;
	double	Pixel=1.0;
	double	x, y, cx, cy, d;

	if(shape!=1) {
		cx=(double)PI/(double)hSize;
		cy=(double)PI/(double)vSize;
	}

	else {
		if(hSize>vSize) {
			cx=(double)PI/((double)vSize/(double)Pixel);
			cy=(double)PI/(double)vSize;
		}
		else {
			cx=(double)PI/((double)hSize/(double)Pixel);
			cy=(double)PI/(double)hSize;
		}
	}

	for(i=0; i<vSize; i++) {
		for(j=0; j<hSize; j++) {
			x=j-(hSize/2);
			y=i-(vSize/2);
			d=cx*x*x+cy*y*y;
			d=112.0*cos(d)+128.0;
			imgBuf[i*hSize+j]=(uchar)d;
		}
	}	

}


//
//	
//	Subroutine	:  Quantization()																				
//	Purpose		:  Quaitization of Image																	
//																																			*/
//

void	Quantization(uchar *srcImg, uchar *desImg, int hSize, int vSize, int bits)
{

		int x, y;

		bits=MAXBITS-bits;

		for(y=0; y<vSize; y++) 
			for(x=0; x<hSize; x++) 
				desImg[y*hSize+x]=(srcImg[y*hSize+x]>>bits)<<bits;

}				


//
//																													
//	Subroutine	:  Sampling()																						
//	Purpose		:  Sampling Theory Test																		
//																											
//				mode 0: 필터 없음, 재표본화 있음								
//				mode 1: 필터 있음, 재표본화 있음																				
//				mode 2: 필터 있음, 재표본화 없음																						
//																																			
//

void	Sampling(uchar *srcImg, uchar *desImg, int hSize, int vSize, int hParam, int vParam, int mode)
{
	
//	int		i, j;
	double	fx, fy;

	switch (mode) {
		case 0:
			subSample(srcImg, desImg, hSize, vSize, hParam, vParam);
			break;

		case 1:
			fx=1.0/(double)hParam;
			fy=1.0/(double)vParam;

			lowPassFilter(srcImg, desImg, hSize, vSize, fx, fy);
			subSample(desImg, desImg, hSize, vSize, hParam, vParam);
			break;
		
		case 2:
			fx=1.0/(double)hParam;
			fy=1.0/(double)vParam;
			lowPassFilter(srcImg, desImg, hSize, vSize, fx, fy);
			break;

	}

}


//
//																											
//	Subroutine	:  lowPassFilter()																			
//	Purpose		:  LPF of Image																						
//																																		
//

void	lowPassFilter(uchar *srcImg, uchar *desImg, int hSize, int vSize, double fx, double fy)
{

	uchar	*image_buf;
	int		imgBufSize;
	int		i, j, ii, jj;
	int		m, n;
	int		i1, i2, j1, j2;
	float	sinc, hanning;
	float	coefh[MAX_TAP], coefv[MAX_TAP];
	float	aa, bb, cc;
	double	x, y;


	imgBufSize=hSize*vSize;
	image_buf=(uchar *)Buff_Alloc(sizeof(ulong), imgBufSize);
	
	m=(int)(WINDOW/fx);
	
	if(m>MAX_TAP-1) 
		m=MAX_TAP-1;

	coefh[0]=1.0;
	
	for(i=1; i<=m; i++) {
	    x=(double)((float)i*PI*fx);
		sinc=(float)(sin(x)/x);
		x=x/WINDOW;
		hanning=(float)(0.5+0.5*cos(x));
		coefh[i]=sinc*hanning;
	}
	
	n=(int)(WINDOW/fy);
	if(n>MAX_TAP-1) 
		n=MAX_TAP-1;
	
	coefv[0]=1.0;
	
	for(i=1; i<=n; i++){
		y=(double)((float)i*PI*fy);
	    sinc=(float)(sin(y)/y);
		y=y/WINDOW;
	    hanning=(float)(0.5+0.5*cos(y));
		coefv[i]=sinc*hanning;
	}
	
	for(i=0; i<vSize; i++) {
	//	printf("   horizontal filtering: %d\r", i);
		for (j=0; j<hSize; j++) {
			aa=1;
			bb=srcImg[i*hSize+j];

			for(jj=1; jj<=m; jj++) {
				j1=j-jj;
				j2=j+jj;
				
				if(j1<0)			j1=0;
				if(j1>hSize-1)		j1=hSize-1;
				if(j2<0)			j2=0;
				if(j2>hSize-1)		j2=hSize-1;
				
				cc=(float)(srcImg[i*hSize+j1]+srcImg[i*hSize+j2]);
				bb=bb+cc*coefh[jj];
				aa=aa+coefh[jj]*2;
			}
			
			bb=bb/aa;
			
			if(bb<0.)	bb=0.;
			if(bb>255.) bb=255.;
			*(image_buf+(ulong)i*hSize+j)=(uchar)bb;
	    }
	}



	for(i=0; i<vSize; i++) {
	    for(j=0; j<hSize; j++){
			aa=1;
			bb=*(image_buf+(ulong)i*hSize+j);
			
			for(ii=1; ii<=n; ii++) {
				i1=i-ii;
				i2=i+ii;
			    
				if(i1<0)		i1=0;
			    if(i1>vSize-1)	i1=vSize-1;
				if(i2<0)		i2=0;
				if(i2>vSize-1) i2=vSize-1;
				
				cc=(float)(*(image_buf+(ulong)i1*hSize+j)+*(image_buf+(ulong)i2*hSize+j));
				bb=bb+cc*coefv[ii];
				aa=aa+coefv[ii]*2;
			}

			bb=bb/aa;
			if(bb<0.)	bb=0.;
			if(bb>255)	bb=255.;
			
			desImg[i*hSize+j]=(uchar)bb;
		}
	}

	free(image_buf);

}


//
//																							
//	Subroutine	:  subSample()																					
//	Purpose		:  Subsampling																						
//																																			
//

void	subSample(uchar *srcImg, uchar *desImg, int hSize, int vSize, int hParam, int vParam)
{

	int		i, j, ii, jj;

	for(i=0; i<vSize; i++) {
	    for(j=0;j<hSize; j++) {
			ii=(i/vParam)*vParam;
	        jj=(j/hParam)*hParam;
			desImg[i*hSize+j]=srcImg[ii*hSize+jj];
	    }
	}

}



//
//																												
//	Subroutine	:  FIR2dFilter()																				
//	Purpose		:  2D FIR Filter Filter Kernel														
//																																			
//													

void	FIR2dFilter(uchar *srcImg, uchar *desImg, int hSize, int vSize)
{
//	int  	coef[3][3]={-1, -1, -1,
//						-1, 8, -1,
//						-1, -1, -1};		/*  필터 계수 행렬	  */
	double	a=1.5;
	double	coef[3][3]={1., 2., 1.,
						2., 4., 2.,
						1., 2., 1.};
	
	int  		htap=3;				/* 수평 탭 수 */
	int  		vtap=3;				/* 수직 탭 수 */
	
	int			i, j, k, l;
	double		sum;
	int			bias=0;

	for (i=0; i<vSize; i++) {
		for (j=0; j<hSize; j++) {
			sum=0.0;
			
			for(k=-vtap/2; k<=vtap/2; k++) {
				for(l=-htap/2; l<=htap/2; l++) {
					if((i+k>=0)&&(i+k<=vSize-1)&&(j+l>=0)&&(j+l<=hSize-1))
						sum+=srcImg[(i+k)*hSize+(j+l)]*coef[k+(vtap/2)][l+(htap/2)];
				}
			}
			
			sum/=16.;

			sum+=(double)bias;

			if(sum<0.0)			desImg[i*hSize+j]=0;
			else if(sum>255.)	desImg[i*hSize+j]=255;
			else desImg[i*hSize+j]=(uchar)sum;
		}

	}

}


//
//																										
//	Subroutine	:  SimpleEdgeDetection()																
//	Purpose		:  Edge Detection using Homogeneity												
//																																			
//													

void	SimpleEdgeDetection(uchar *srcImg, uchar *desImg, int hSize, int vSize, int flag)
{
	
	int  		htap=3;				/* 수평 탭 수 */
	int  		vtap=3;				/* 수직 탭 수 */
	
	int			i, j, k, l;
//	int			index;
	uchar		window[9];
	uchar		tmp;
	
	int			bias=64;


	for(i=0; i<9; i++)
		window[i]=0;

	for(i=0; i<vSize; i++) {
		for(j=0; j<hSize; j++) {
			for(k=-vtap/2; k<=vtap/2; k++) {
				for(l=-htap/2; l<=htap/2; l++) {
					if((i+k>=0)&&(i+k<=vSize-1)&&(j+l>=0)&&(j+l<=hSize-1))
						window[(k+(vtap/2))*htap+(l+(htap/2))]=srcImg[(i+k)*hSize+(j+l)];
				}
			}
			
			if(flag==1)		tmp=(uchar)HomoOperation(window)+bias;
			else			tmp=(uchar)DiffOperation(window)+bias;
		
			if(tmp<0)			desImg[i*hSize+j]=0;
			else if(tmp>255)	desImg[i*hSize+j]=255;
			else desImg[i*hSize+j]=tmp;
		}

	}

}



//
//																																		
//	Subroutine	:  HomoOperation()																			
//	Purpose		:  Edge Detection using Homogeneity												
//																							
//	Param: window - 3x3 window of source image   |0 1 2|								
//                                               |3 4 5|								
//                                               |6 7 8|								
//																																			
//													

int		HomoOperation(uchar *window)
{

	int		max;                 /* current maximum difference */
	int		diff;                /* current difference */

	diff=abs(window[4]-window[0]);  /* upper left pixel */
    max=diff;

	diff=abs(window[4]-window[1]);  /* upper pixel */
	if(diff>max)
	max=diff;

	diff=abs(window[4]-window[2]);  /* upper right pixel */
	if(diff>max)
	max=diff;

	diff=abs(window[4]-window[5]);  /* right pixel */
	if(diff>max)
	max=diff;

	diff=abs(window[4]-window[8]);  /* lower left pixel */
	if(diff>max)
	max=diff;

	diff=abs(window[4]-window[7]);  /* lower pixel */
	if(diff>max)
	max=diff;

	diff=abs(window[4]-window[6]);  /* bottom left pixel */
	if(diff>max)
	max=diff;

	diff=abs(window[4]-window[3]);  /* left pixel */
	if(diff>max)
	max=diff;

	return	max;
  
}

//
//																																			
//	Subroutine	:  DiffOperation()																			
//	Purpose		:  Edge Detection using Difference												
//																																			
//	Param: window - 3x3 window of source image   |0 1 2|								
//                                               |3 4 5|								
//                                               |6 7 8|								
//																																			
//													

int		DiffOperation(uchar *window)
{

	int		max;                 /* current maximum difference */
	int		diff;                /* current difference */

	diff=abs(window[0]-window[8]);  /* upper left - lower right */
	max=diff;

	diff=abs(window[1]-window[7]);  /* upper - lower */
	if(diff>max)
	max=diff;

	diff=abs(window[2]-window[6]);  /* upper right - lower left */
	if(diff>max)
	max=diff;

	diff=abs(window[5]-window[3]);  /* left - right */
	if(diff>max)
	max=diff;

	return max;

}



//
//																																			
//	Subroutine	:  FirstDerivativeEdgeDetection()												
//	Purpose		:  Edge Detection using 1st derivative										
//																																			
//													

void	FirstDerivativeEdgeDetection(uchar *srcImg, uchar *desImg, int hSize, int vSize, 
									 int modeFlag, int directFlag)
{


	int		noDirect=2, noCoeff=9;
	int		Roberts[2][9]=	{ 0,  0,  0,  0,  0,  1,  0, -1,  0,				// Y axis
										  0,  0,  0,  0,  1,  0,  0,  0, -1};	// X axis
	int		Prewitt[2][9]=	{ 1,  0, -1,  1,  0, -1,  1,  0, -1,				// Y axis
										 -1, -1, -1,  0,  0,  0,  1,  1,  1};	// X axis
	int		Sobel[2][9]=	{ 1,  0, -1,  2,  0, -2,  1,  0, -1,				// Y axis
										 -1, -2, -1,  0,  0,  0,  1,  2,  1};	// X axis
	int		*maskCoeff[9];

	int  	htap=3;				// 수평 탭 수 
	int  	vtap=3;				// 수직 탭 수 
	
	uchar	*tmpImg1, *tmpImg2;
	int		i, j;
	int		imgBufSize;
	double	sum;
	int		bias=0;

	for(i=0; i<noDirect; i++)
		maskCoeff[i]=(int *)Buff_Alloc(sizeof(int), noCoeff);

	switch(modeFlag) {
		case 1 :
			for(i=0; i<noDirect; i++)
				MaskDump(Roberts[i], maskCoeff[i], htap, vtap);
			break;

		case 2 : 
			for(i=0; i<noDirect; i++)
				MaskDump(Prewitt[i], maskCoeff[i], htap, vtap);
			break;

		case 3 : 
			for(i=0; i<noDirect; i++)
				MaskDump(Sobel[i], maskCoeff[i], htap, vtap);
			break;

		default :
			break;
	}

	switch(directFlag) {
		case 1 :	// Vertical Edge
			EdgeMaskFilter(srcImg, desImg, hSize, vSize, maskCoeff[0], htap, vtap);
			break;
		
		case 2 :	// horizontal Edge
			EdgeMaskFilter(srcImg, desImg, hSize, vSize, maskCoeff[1], htap, vtap);
			break;

		case 3 :	// both
			imgBufSize=hSize*vSize;
			tmpImg1=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);
			tmpImg2=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);

			EdgeMaskFilter(srcImg, tmpImg1, hSize, vSize, maskCoeff[0], htap, vtap);
			EdgeMaskFilter(srcImg, tmpImg2, hSize, vSize, maskCoeff[1], htap, vtap);

			for(i=0; i<vSize; i++) {
				for(j=0; j<hSize; j++) {
					sum=sqrt((tmpImg1[i*hSize+j]*tmpImg1[i*hSize+j])+(tmpImg2[i*hSize+j]*tmpImg2[i*hSize+j]));
					sum+=bias;

					if(sum<0.0)			desImg[i*hSize+j]=0;
					else if(sum>255.)	desImg[i*hSize+j]=255;
					else				desImg[i*hSize+j]=(uchar)sum;

				}	
			}

			free(tmpImg1);
			free(tmpImg2);
		
		default :
			break;
	
	}

}



//
//																																			
//	Subroutine	:  CompassEdgeDetection()																
//	Purpose		:  Edge Detection using Compass Operator									
//																																			
//													

void	CompassEdgeDetection(uchar *srcImg, uchar *desImg, int hSize, int vSize, int modeFlag)
{

	int		Prewitt[8][9]=	{ 1,  1, -1,  1, -2, -1,  1,  1, -1,
							  1, -1, -1,  1, -2, -1,  1,  1,  1,
							 -1, -1, -1,  1, -2,  1,  1,  1,  1,
							 -1, -1,  1, -1, -2,  1,  1,  1,  1,
							 -1,  1,  1, -1, -2,  1, -1,  1,  1,
							  1,  1,  1, -1, -2,  1, -1, -1,  1,
							  1,  1,  1,  1, -2,  1, -1, -1, -1,
							  1,  1,  1, -1, -2,  1, -1, -1,  1 };

	int		Kirsch[8][9]=	{ 5, -3, -3,  5,  0, -3,  5, -3, -3,
							 -3, -3, -3,  5,  0, -3,  5,  5, -3,
							 -3, -3, -3, -3,  0, -3,  5,  5,  5,
							 -3, -3, -3, -3,  0,  5, -3,  5,  5,
							 -3, -3,  5, -3,  0,  5, -3, -3,  5,
							 -3,  5,  5, -3,  0,  5, -3, -3, -3,
							  5,  5,  5, -3,  0, -3, -3, -3, -3,
							  5,  5, -3,  5,  0, -3, -3, -3, -3 };

	int		Robinson[8][9]=	{ 1,  0, -1,  1,  0, -1,  1,  0, -1,
							  0, -1, -1,  1,  0, -1,  1,  1,  0,
							 -1, -1, -1,  0,  0,  0,  1,  1,  1,
							 -1, -1,  0, -1,  0,  1,  0,  1,  1,
							 -1,  0,  1, -1,  0,  1, -1,  0,  1,
							  0,  1,  1, -1,  0,  1, -1, -1,  0,
							  1,  1,  1,  0,  0,  0, -1, -1, -1,
							  1,  1,  0,  1,  0, -1,  0, -1, -1 };

	
	int		*maskCoeff[9];

	int  	htap=3;				/* 수평 탭 수 */
	int  	vtap=3;				/* 수직 탭 수 */
	
	int		noDirect=8;
	int		i;

	for(i=0; i<noDirect; i++)
		maskCoeff[i]=(int *)Buff_Alloc(sizeof(int), htap*vtap);

	switch(modeFlag) {
		case 1 :
			for(i=0; i<8; i++) 
				MaskDump(Prewitt[i], maskCoeff[i], htap, vtap);
			break;
		
		case 2 :
			for(i=0; i<8; i++) 
				MaskDump(Kirsch[i], maskCoeff[i], htap, vtap);
			break;

		case 3 :
			for(i=0; i<8; i++) 
				MaskDump(Robinson[i], maskCoeff[i], htap, vtap);
			break;

		default :
			break;
	}

	CompassEdgeMaskFilter(srcImg, desImg, hSize, vSize, &maskCoeff[0], noDirect, htap, vtap);

}




//
//																																			
//	Subroutine	:  SecondDerivativeEdgeDetection()											
//	Purpose		:  Edge Detection using 2nd derivative										
//																																			
//													

void	SecondDerivativeEdgeDetection(uchar *srcImg, uchar *desImg, int hSize, int vSize, int modeFlag)
{

	int		Laplace[2][9]= { 0, -1,  0, -1,  4, -1,  0, -1,  0,		// Laplace 1
							-1, -1, -1, -1,  8, -1, -1, -1, -1};	// Laplace 2

	int		DoG_1[49]= { 0,  0, -1, -1, -1,  0,  0,
						 0, -2, -3, -3, -3, -2,  0,
						-1, -3,  5,  5,  5, -3, -1,
						-1, -3,  5, 16,  5, -3, -1,
						-1, -3,  5,  5,  5, -3, -1,
						 0, -2, -3, -3, -3, -2,  0,
						 0,  0, -1, -1, -1,  0,  0 };

	int		DoG_2[81]= { 0,  0,  0, -1, -1, -1,  0,  0,  0,
						 0, -2, -3, -3, -3, -3, -3, -2,  0,
						 0, -3, -2, -1, -1, -1, -2, -3,  0,
						-1, -3, -1,  9,  9,  9, -1, -3, -1,
						-1, -3, -1,  9, 19,  9, -1, -3, -1,
						-1, -3, -1,  9,  9,  9, -1, -3, -1,
						 0, -3, -2, -1, -1, -1, -2, -3,  0,
						 0, -2, -3, -3, -3, -3, -3, -2,  0,
						 0,  0,  0, -1, -1, -1,  0,  0,  0 };



	
	int		maskCoeff_1[9], maskCoeff_2[49], maskCoeff_3[81];
	int  	htap;				/* 수평 탭 수 */
	int  	vtap;				/* 수직 탭 수 */

	switch(modeFlag) {
		case 1 :
			htap=vtap=3;
			MaskDump(Laplace[1], maskCoeff_1, htap, vtap);
			SecondEdgeMaskFilter(srcImg, desImg, hSize, vSize, maskCoeff_1, htap, vtap);
		//	EdgeMaskFilter(srcImg, desImg, hSize, vSize, maskCoeff_1, htap, vtap);
			break;
		
		case 2 :
			htap=vtap=7;
			MaskDump(DoG_1, maskCoeff_2, htap, vtap);
		//	SecondEdgeMaskFilter(srcImg, desImg, hSize, vSize, maskCoeff_2, htap, vtap);
			EdgeMaskFilter(srcImg, desImg, hSize, vSize, maskCoeff_2, htap, vtap);
			break;

	
		case 3 :
			htap=vtap=9;
			MaskDump(DoG_2, maskCoeff_3, htap, vtap);
			SecondEdgeMaskFilter(srcImg, desImg, hSize, vSize, maskCoeff_3, htap, vtap);
		//	EdgeMaskFilter(srcImg, desImg, hSize, vSize, maskCoeff_3, htap, vtap);
			break;
	

		default :
			break;
	}

}



//
//																																			
//	Subroutine	:  MaskDump()																						
//	Purpose		:  Copy Window Mask Function to Common Mask								
//																																			
//													

void	MaskDump(int *target, int *window, int htap, int vtap)
{

	int		i, j;

	for(i=0; i<vtap; i++) {
		for(j=0; j<htap; j++) {
			window[i*htap+j]=target[i*htap+j];
		}
	}

}




//
//																																			
//	Subroutine	:  EdgeMaskFilter()																			
//	Purpose		:  Filter Masking Function																
//																																			
//													

void	EdgeMaskFilter(uchar *targetImg, uchar *resultImg, int hSize, int vSize, int *window, 
						 int htap, int vtap)
{

	int			i, j, k, l;
	int			sum, tmp;

	for (i=0; i<vSize; i++) {
		for (j=0; j<hSize; j++) {
			sum=0;
			
			for(k=-vtap/2; k<=vtap/2; k++) {
				for(l=-htap/2; l<=htap/2; l++) {
					if((i+k>=0)&&(i+k<=vSize-1)&&(j+l>=0)&&(j+l<=hSize-1))
						sum+=targetImg[(i+k)*hSize+(j+l)]*window[(k+(vtap/2))*htap+(l+(htap/2))];
				}
			}
			
			tmp=abs(sum);

			if(tmp<0)			resultImg[i*hSize+j]=0;
			else if(tmp>255)	resultImg[i*hSize+j]=255;
			else				resultImg[i*hSize+j]=(uchar)tmp;
		}

	}

}



//
//																																			
//	Subroutine	:  SecondaskFilter1()																		
//	Purpose		:  Filter Masking Function																
//																																			
//													

void	SecondEdgeMaskFilter(uchar *targetImg, uchar *resultImg, int hSize, int vSize, int *window,
							 int htap, int vtap)
{

	int		i, j, k, l;
	int		sum;
	int		Th=32;
	int		imgBufSize;
	int		*tmpImg;
	uchar	*resImg1, *resImg2;

	imgBufSize=hSize*vSize;
	tmpImg=(int *)Buff_Alloc(sizeof(int), imgBufSize);	
	resImg1=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);
	resImg2=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);

	for(i=0; i<vSize; i++) {
		for(j=0; j<hSize; j++) {
			sum=0;
			
			for(k=-vtap/2; k<=vtap/2; k++) {
				for(l=-htap/2; l<=htap/2; l++) {
					if((i+k>=0)&&(i+k<=vSize-1)&&(j+l>=0)&&(j+l<=hSize-1))
						sum+=targetImg[(i+k)*hSize+(j+l)]*window[(k+(vtap/2))*htap+(l+(htap/2))];
				}
			}
			
			tmpImg[i*hSize+j]=sum;
			
		}
	}

	ZeroCrossing1(tmpImg, resImg1, hSize, vSize, Th);
	ZeroCrossing2(tmpImg, resImg2, hSize, vSize, Th);

	for(i=0; i<vSize; i++) {
		for(j=0; j<hSize; j++) {
			sum=(int)(sqrt((resImg1[i*hSize+j]*resImg1[i*hSize+j])+(resImg2[i*hSize+j]*resImg2[i*hSize+j])));
		//	sum+=bias;

			if(sum<0)			resultImg[i*hSize+j]=0;
			else if(sum>255)	resultImg[i*hSize+j]=255;
			else				resultImg[i*hSize+j]=(uchar)sum;

		}	
	}

	free(tmpImg);
	free(resImg1);
	free(resImg2);


}


//
//																																			
//	Subroutine	:  CompassEdgeMaskFilter()															
//	Purpose		:  Filter Masking Function																
//																																			
//													

void	CompassEdgeMaskFilter(uchar *targetImg, uchar *resultImg, int hSize, int vSize, int **window, int noDirect, int htap, int vtap)
{

	int			i, j, k, l, dd;
	int			sum, tmp, max;


	for(i=0; i<vSize; i++) {
		for(j=0; j<hSize; j++) {
			max=0;
			
			for(dd=0; dd<8; dd++) {
				sum=0;
				
				for(k=-vtap/2; k<=vtap/2; k++) {
					for(l=-htap/2; l<=htap/2; l++) {
						if((i+k>=0)&&(i+k<=vSize-1)&&(j+l>=0)&&(j+l<=hSize-1)) {
							sum+=targetImg[(i+k)*hSize+(j+l)]*window[dd][(k+(vtap/2))*htap+(l+(htap/2))];
						}
					}
				}
			
				tmp=abs(sum);
				max=(tmp>max) ? tmp : max;
			}	
		
			if(max<0)			resultImg[i*hSize+j]=0;
			else if(max>255)	resultImg[i*hSize+j]=255;
			else				resultImg[i*hSize+j]=(uchar)max;
			
		}
	}

}




//
//																																			
//	Subroutine	:  ZeroCrossing1()																			
//	Purpose		:  Zero Crossing Function																	
//																																			
//	

void	ZeroCrossing1(int *inputImg, uchar *outputImg, int hSize, int vSize, int Th)
{

	int		i, j;
	int		tmp;

	for(i=0; i<vSize; i++) {
		for(j=0; j<hSize; j++) {
			if(i==0 || j==0)	inputImg[i*hSize+j]=outputImg[i*hSize+j]=0;
			else {
				if((inputImg[i*hSize+j]<0)&&(inputImg[i*hSize+(j-1)]>=0)) {
					tmp=inputImg[i*hSize+j]-inputImg[i*hSize+(j-1)];
			
			//		fprintf(fp_txt, "\n case 1 : [%d][%d]=> %d %d => %d", i, j, inputImg[i*hSize+j],
			//				inputImg[i*hSize+(j-1)], tmp);
							
				}
				
				else if((inputImg[i*hSize+j]>0)&&(inputImg[i*hSize+(j-1)]<=0)) {
					tmp=inputImg[i*hSize+j]-inputImg[i*hSize+(j-1)];
					
			//		fprintf(fp_txt, "\n case 2 : [%d][%d]=> %d %d => %d", i, j, inputImg[i*hSize+j],
			//				inputImg[i*hSize+(j-1)], tmp);
				
				}
					
				else {
					tmp=0;
				}

			}
		
			tmp=abs(tmp);

			if(tmp>255)		outputImg[i*hSize+j]=255;
			else if(tmp<Th)	outputImg[i*hSize+j]=0;
			else			outputImg[i*hSize+j]=tmp;
	
		}
	}
				

//	fclose(fp_txt);

}


//
//																																			
//	Subroutine	:  ZeroCrossing2()																			
//	Purpose		:  Zero Crossing Function																	
//																																			
//	

void	ZeroCrossing2(int *inputImg, uchar *outputImg, int hSize, int vSize, int Th)
{

	int		i, j;
	int		tmp;

	for(i=0; i<vSize; i++) {
		for(j=0; j<hSize; j++) {
			if(i==0 || j==0)	inputImg[i*hSize+j]=outputImg[i*hSize+j]=0;
			else {
				if((inputImg[i*hSize+j]<0)&&(inputImg[(i-1)*hSize+j]>=0)) {
					tmp=inputImg[i*hSize+j]-inputImg[(i-1)*hSize+j];
				}
				
				else if((inputImg[i*hSize+j]>0)&&(inputImg[(i-1)*hSize+j]<=0)) {
					tmp=inputImg[i*hSize+j]-inputImg[(i-1)*hSize+j];
				}
					
				else {
					tmp=0;
				}

			}
		
			tmp=abs(tmp);

			if(tmp>255)		outputImg[i*hSize+j]=255;
			else if(tmp<Th)	outputImg[i*hSize+j]=0;
			else			outputImg[i*hSize+j]=tmp;
	
		}
	}
				
}



//
//																																			
//	Subroutine	:  MedianFilter()																				
//	Purpose		:  Median Filtering Main																	
//																																			
//													

void	MedianFilter(uchar *srcImg, uchar *desImg, int hSize, int vSize, int wSize)
{

	int  	htap;				/* 수평 탭 수 */
	int  	vtap;				/* 수직 탭 수 */
	
	int		i, j, k, l;
	int		*window;
	uchar	tmp;
	int		noCoeff;
	
	htap=vtap=wSize;
	noCoeff=htap*vtap;

	window=(int *)Buff_Alloc(sizeof(int), noCoeff);

	for(i=0; i<vSize; i++) {
		for(j=0; j<hSize; j++) {
				
			for(k=-vtap/2; k<=vtap/2; k++) {
				for(l=-htap/2; l<=htap/2; l++) {
					if((i+k>=0)&&(i+k<=vSize-1)&&(j+l>=0)&&(j+l<=hSize-1))
						window[(k+(vtap/2))*htap+(l+(htap/2))]=srcImg[(i+k)*hSize+(j+l)];
				
				//		printf("\n sum=%lf  coeff=%d", sum, coef[k+(vtap/2)][l+(htap/2)]);
				}
			}
			
			tmp=(uchar)MedianOperation(window, noCoeff);
					
			if(tmp<0)			desImg[i*hSize+j]=0;
			else if(tmp>255)	desImg[i*hSize+j]=255;
			else desImg[i*hSize+j]=tmp;
		}

	}

}



//
//																																			
//	Subroutine	:  MedianOperation()																		
//	Purpose		:  Median Filtering Operation															
//																																			
//													

int		MedianOperation(int *window, int wsize)
{

	QuickSort(window, 0, wsize);
    
	return window[wsize/2];

}

//
//																																			
//	Subroutine	:  QuickSort()																					
//	Purpose		:  Sort array of integer																	
//																																			
//													

void	QuickSort(int *array, int left, int right)
{
 
	int		i, last;

	if(left>=right) 
		return;
    
	Swap(array, left, (left + right)/2);
    last=left;
    
	for(i=left+1; i<=right; i++)
		if(array[i]<array[left])
			Swap(array, ++last, i);
    
	Swap(array, left, last);
    QuickSort(array, left, last-1);
    QuickSort(array, last+1, right);
 
}


//
//																																			
//	Subroutine	:  Swap()																								
//	Purpose		:  swaps two elements in an array													
//																																			
//													

void	Swap(int *array, int i, int j)
{
    
	int		temp;

    temp=array[i];
    array[i]=array[j];
    array[j]=temp;

}


//
//																																			
//	Subroutine	:  GetImageSizeInform()																	
//	Purpose		:  new image size calculation															
//																																			
//													

void	GetImageSizeInform(int inCols, int inRows, double scale_X, double scale_Y, 
						   int *outCols, int *outRows)
{

	int		tempCols, tempRows;

	tempCols=*outCols;
	tempRows=*outRows;

	// Check new image dimensions 
	if(tempCols==0 && tempRows==0 &&scale_X==1.0 && scale_Y==1.0)
		printf("\n Warning->Must specify new X and Y dimensions\n");
	if(tempCols==0) 
		tempCols=(int)(inCols*scale_X);	
	if(tempRows==0)
		tempRows=(int)(inRows*scale_Y);

	*outCols=(((tempCols*8)+31)/32)*4;		// 8:gray level bit number
	*outRows=(((tempRows*8)+31)/32)*4;	;	

}



//
//																																			
//	Subroutine	:  Interpolation()																			
//	Purpose		:  interpolation main for image resize										
//																																			
//													

void	Interpolation(uchar *srcImg, uchar *desImg, int inCols, int inRows, 
					  int newCols, int newRows, int mode)
{
	
	switch(mode) {
		case 1 :
			NearestNeighborInterpol(srcImg, desImg, inCols, inRows, newCols, newRows);
			break;

		case 2 :
			BilinearInterpol(srcImg, desImg, inCols, inRows, newCols, newRows);
			break;
/*
		case 3 :
			CubicInterpol(srcImg, desImg, inCols, inRows, newCols, newRows);
			break;
		
		case 4 :
			SplineInterpol(srcImg, desImg, inCols, inRows, newCols, newRows);
			break;
*/
		default :
			break;

	}

}



//
//																																			
//	Subroutine	:  NearestNeighborInterpol()														
//	Purpose		:  Interpolation of Nearest Neighbor											
//																																			
//													

void	NearestNeighborInterpol(uchar *srcImg, uchar *desImg, int inCols, int inRows, 
								int newCols, int newRows)
{

	int		i, j;
	int		addrIndex;
	double	stepX, stepY;
	int		tmpCols, tmpRows;

	stepX=(double)newCols/(double)inCols;
	stepY=(double)newRows/(double)inRows;

	for(i=0; i<newRows; i++) {
		for(j=0; j<newCols; j++) {
			tmpCols=(int)((j/stepX)+0.5);
			tmpRows=(int)((i/stepY)+0.5);

			addrIndex=tmpRows*inCols+tmpCols;

			desImg[i*newCols+j]=srcImg[addrIndex];
		}
	}

}


//
//
//	Subroutine	:  BilinearInterpol()																		
//	Purpose		:  Interpolation of Bilinear															
//																																			
//													

void	BilinearInterpol(uchar *srcImg, uchar *desImg, int inCols, int inRows, 
						 int newCols, int newRows)
{

	uchar	*tmpImg;
	int		tempBufSize;
	int		i, j;
	double	realX, realY, stepX, stepY;
	int		intX, intY;
	uchar	low, high;

	tempBufSize=newCols*inRows;								// temporary image buff size
	tmpImg=(uchar *)Buff_Alloc(sizeof(uchar), tempBufSize);	// allocate temporary image buff

	// First pass - expand in X direction 
	stepX=(double)inCols/(double)newCols;
	stepY=(double)inRows/(double)newRows;

	for(i=0; i<inRows; i++) {
		for(j=0; j<newCols; j++) {
			realX = j*stepX;
			intX=(int)realX;
			
			if((intX+1)<inCols) {
				low =srcImg[(i*inCols)+intX];
				high=srcImg[(i*inCols)+(intX+1)];
				tmpImg[(i*newCols)+j]=low+(uchar)((high-low)*(realX-intX));
			}
	
	
			else {
				tmpImg[(i*newCols)+j]=srcImg[(i*inCols)+intX];
			}
		}
	}

	// Second pass - expand in Y direction 
	
	for(i=0; i<newRows; i++) {
		for(j=0; j<newCols; j++) {
			realY=i*stepY;
			intY=(int)realY;
			
			if((intY+1)<inRows) {
				low =tmpImg[(intY*newCols)+j];
				high=tmpImg[((intY+1)*newCols)+j];
				desImg[(i*newCols)+j]=low+(uchar)((high-low)*(realY-intY));
			}
			
			else {
				desImg[(i*newCols)+j]=tmpImg[(intY*newCols)+j];
			}
		}
	}		

}



//
//																																			
//	Subroutine	:  MakeThumbnailImage()																	
//	Purpose		:  Generate Thumbnail Image (Minification)								
//																																			
//													

void	MakeThumbnailImage(uchar *srcImg, uchar *desImg, int hSize, int vSize, int newCols, int newRows, 
						   int wSize, int mode)
{

	int  	htap;				/* 수평 탭 수 */
	int  	vtap;				/* 수직 탭 수 */
	
	int		i, j, k, l;
	int		*window;
	uchar	tmp;
	int		noCoeff;
	int		xp, yp;
	
	htap=vtap=wSize;
	noCoeff=htap*vtap;

	window=(int *)Buff_Alloc(sizeof(int), noCoeff);

	for(i=0; i<newRows; i++) {
		for(j=0; j<newCols; j++) {
			yp=vtap*i;
			xp=htap*j;
			
			for(k=0; k<vtap; k++) {
				for(l=0; l<htap; l++) {
					window[k*htap+l]=srcImg[(yp+k)*hSize+(xp+l)];

				}
			}
			
			if(mode==1)		tmp=(uchar)MedianOperation(window, noCoeff);
			else			tmp=(uchar)AverageOperation(window, noCoeff);
					
			if(tmp<0)			desImg[i*newCols+j]=0;
			else if(tmp>255)	desImg[i*newCols+j]=255;
			else desImg[i*newCols+j]=tmp;
		}

	}

}


//
//																																			
//	Subroutine	:  AverageOperation()																		
//	Purpose		:  Median Filtering Operation															
//																																			
//													

int		AverageOperation(int *window, int wsize)
{

	int		i, sum=0;

	for(i=0; i<wsize; i++)
		sum+=window[i];
	
    
	return (sum/wsize);

}

//
//																																			
//	Subroutine	:  Buff_Alloc()																					
//	Purpose		:  Buffer Allocation for Picture data											
//																																			
//													

uchar	*Buff_Alloc(int size, int count)
{

	int		i;
	uchar	*image;

	if(!(image=(uchar *)malloc(size*count))) {
		printf("\n malloc failed");
		exit(0);
	}

	for(i=0; i<count; i++) {
		image[i]=0;
	}

	return (image);

}


