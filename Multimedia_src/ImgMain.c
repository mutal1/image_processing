//
//
//	Program : Simple Image Processing (only gray level support)					
//																																			
//	Author	: T.Y.Chung																									
//
//	Date	: 20XX. xx																										
//
//

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include "global.h"
#include "bmp.h"
#include "imglib.h"


//
//
//	Subroutine	:  main()	
//	Purpose		:  main routine for Simple image processing				
//
//


int		main(int argc, char *argv[]) 
{
		
	int		imgBufSize;	
	int		cols, rows;
	int		mode;
	int		no_bits;
	int		outputFlag;
	int		horiResample, vertiResample, samplingControl;	// sampling parameter
	int		edgeDetectionMode, edgeDetectionFlag, directionFlag;
	int		thumbNailMode;
	int		windowSize;
	int		resizeFlag, newCols, newRows, interpolMode;
	double	scaleFactor_X, scaleFactor_Y;

	static int	command=-1;
	
	while(command) {
		printf("\n ** Image Processing Menu - JTY Image Processor V1.0 ** \n\n");
		printf("  1 : Read Image File (BMP Type) \n");
		printf("  2 : Write Image File (BMP Type) \n");
		printf("  3 : Generate Circular Zone Plate \n");
		printf("  4 : Sampling \n");
		printf("  5 : Quantization \n");
		printf("  6 : 2D FIR Filter \n");
		printf("  7 : Edge Detection \n");
		printf("  8 : Median Filter \n");
		printf("  9  :Image Resize \n");
		printf(" 10 : Generate Thumbnail Image \n");

		printf("  0 : End \n");

		printf("\n -> Processing Number? ");
		scanf("%d", &command);

		switch(command) {
			case 1 :
				printf("\n\t Input File Name :  ");
				scanf("%s", filename[0]);
				
				Stream_Open(filename[0], FILE_READ);						// Open Input File 
				InitializeBmpParam(fp_in, &cols, &rows);					// bmp file read to orgImg
				
				resImg=(uchar *)Buff_Alloc(sizeof(uchar), cols*rows);		// memory alloc. for copy image
				ImageCopy(orgImg, resImg, cols, rows);						// Image Copy of original image
				
				break;

			case 2 :
				printf("\n\t Output File Name :  ");
				scanf("%s", filename[1]);
				
				Stream_Open(filename[1], FILE_WRITE);	
				ImageWriteBmpFile(fp_out, cols, rows, resImg);
				break;

			case 3 :
				printf("\n\t Circular Zone Plate shape (1:circle, 2:eclipse) ? ");
				scanf("%d", &mode);
				
				printf("\t -> H size ? ");
				scanf("%d", &cols);
				printf("\t -> V size ? ");
				scanf("%d", &rows);
					
				imgBufSize=cols*rows;
						
				resImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);
				CircularZonePlate(resImg, cols, rows, mode);
				
				printf("\n\t\t -> Want to generate output image file(Yes=1 No=0)? ");
				scanf("%d", &outputFlag);

				if(outputFlag==1) 
					printf("\t\t -> your next step is to choose Menu 2 \n");
				
				else {
					orgImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);
					ImageCopy(resImg, orgImg, cols, rows);				// result image -> original image
				}

				break;
		
			case 4 :
				printf("  Sub-sampling (1/n) Horizontal & Vertical ?   ");
				scanf ("%d %d", &horiResample, &vertiResample);
				printf("  Control 0: filter off & re-sample on\n");
				printf("          1: filter on  & re-sample on\n");
				printf("          2: filter on  & re-sample off   ");
				scanf("%d", &samplingControl);
				
				if ((samplingControl < 0) || (samplingControl > 2)) break;
				
				imgBufSize=cols*rows;
				resImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);

				Sampling(orgImg, resImg, cols, rows, horiResample, vertiResample, samplingControl);
			
				printf("\n\t\t -> Want to generate output image file(Yes=1 No=0)? ");
				scanf("%d", &outputFlag);

				if(outputFlag==1) 
					printf("\t\t -> your next step is to choose Menu 2 \n");
				else 
					ImageCopy(resImg, orgImg, cols, rows);				// result image -> original image
						
				break;
			
						
			case 5 :
				printf("\n\t  Quantization bits ? ");
				scanf("%d", &no_bits);

				if(no_bits<0 || no_bits>MAXBITS)
					break;
				
				imgBufSize=cols*rows;
				resImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);
		
				Quantization(orgImg, resImg, cols, rows, no_bits);

				printf("\n\t\t -> Want to generate output image file(Yes=1 No=0)? ");
				scanf("%d", &outputFlag);

				if(outputFlag==1) 
					printf("\t\t -> your next step is to choose Menu 2 \n");
							
				else 
					ImageCopy(resImg, orgImg, cols, rows);			// result image -> original image
				
				break;
			
			
			case 6 :
				printf("\n\t  2d FIR Filter ");
			
				imgBufSize=cols*rows;
				resImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);	
						
				FIR2dFilter(orgImg, resImg, cols, rows);

				printf("\n\t\t -> Want to generate output image file(Yes=1 No=0)? ");
				scanf("%d", &outputFlag);

				if(outputFlag==1) 
					printf("\t\t -> your next step is to choose Menu 2 \n");
							
				else 
					ImageCopy(resImg, orgImg, cols, rows);				// result image -> original image
				
				break;


			case 7 :
				printf("\n\t Detection Mode (1:Simple,  2:1st Derivative)");
				printf("\n\t                (3:Compass, 4:2nd Derivative) ? ");
				scanf("%d", &edgeDetectionMode);
				
				switch(edgeDetectionMode) {
					case 1 : 
						imgBufSize=cols*rows;
						resImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);	
						
						printf("\n\t -> Which Mode (1:Homogeneity, 2: Difference) ? ");
						scanf("%d", &edgeDetectionFlag);

						SimpleEdgeDetection(orgImg, resImg, cols, rows, edgeDetectionFlag);	// Homogeneity Edge Detection
						
						printf("\n\n\t -> Want to generate output image file(Yes=1 No=0)? ");
						scanf("%d", &outputFlag);

						if(outputFlag==1) 
							printf("\t\t -> your next step is to choose Menu 2 \n");
							
						else 
							ImageCopy(resImg, orgImg, cols, rows);	// result image -> original image
				
						break;
									
					case 2 :
						imgBufSize=cols*rows;
						resImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);	
						
						printf("\n\t -> Which Mode (1:Roberts, 2:Prewitt, 3:Sobel) ? ");
						scanf("%d", &edgeDetectionFlag);

						printf("\n\t -> Which direction (1:Vertical_axis, 2:Horizontal_axis, 3:both) ? ");
						scanf("%d", &directionFlag);

						FirstDerivativeEdgeDetection(orgImg, resImg, cols, rows, 
													 edgeDetectionFlag, directionFlag);
						
						printf("\n\n\t -> Want to generate output image file(Yes=1 No=0)? ");
						scanf("%d", &outputFlag);

						if(outputFlag==1) 
							printf("\t\t -> your next step is to choose Menu 2 \n");
							
						else 
							ImageCopy(resImg, orgImg, cols, rows);	// result image -> original image
				
						break;
			
					
					case 3 :
						imgBufSize=cols*rows;
						resImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);	
						
						printf("\n\t -> Which Mode (1:Prewitt, 2:Kirsch, 3:Robinson) ? ");
						scanf("%d", &edgeDetectionFlag);

						CompassEdgeDetection(orgImg, resImg, cols, rows, edgeDetectionFlag);
						
						printf("\n\n\t -> Want to generate output image file(Yes=1 No=0)? ");
						scanf("%d", &outputFlag);

						if(outputFlag==1) 
							printf("\t\t -> your next step is to choose Menu 2 \n");
							
						else 
							ImageCopy(resImg, orgImg, cols, rows);	// result image -> original image
				
						break;
				

					case 4 :
						imgBufSize=cols*rows;
						resImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);	
						
						printf("\n\t -> Which Mode (1:Laplacian, 2:DoG(7x7), 3:DoG(9x9) ? ");
						scanf("%d", &edgeDetectionFlag);

						SecondDerivativeEdgeDetection(orgImg, resImg, cols, rows, edgeDetectionFlag);
						
						printf("\n\n\t -> Want to generate output image file(Yes=1 No=0)? ");
						scanf("%d", &outputFlag);

						if(outputFlag==1) 
							printf("\t\t -> your next step is to choose Menu 2 \n");
							
						else 
							ImageCopy(resImg, orgImg, cols, rows);	// result image -> original image
				
						break;

					default :
						break;
				}
				
				break;


			case 8 :
				printf("\n\t  Window Size (1 Dim) ? ");
				scanf("%d", &windowSize);

				if(windowSize<0)
					break;
				
				imgBufSize=cols*rows;
				resImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);
		
				MedianFilter(orgImg, resImg, cols, rows, windowSize);

				printf("\n\t\t -> Want to generate output image file(Yes=1 No=0)? ");
				scanf("%d", &outputFlag);

				if(outputFlag==1) 
					printf("\t\t -> your next step is to choose Menu 2 \n");
							
				else 
					ImageCopy(resImg, orgImg, cols, rows);			// result image -> original image
				
				break;


			case 9 :
				printf("\n\t Image Resize Information (1:Percent Size, 2:Pixel Size) ? ");
				scanf("%d", &resizeFlag);

				scaleFactor_X=1.0;									// default scale factor
				scaleFactor_Y=1.0;
				newCols=newRows=0;

				if(resizeFlag==1) {
					printf("\t -> Percent(%) of H ? ");
					scanf("%lf", &scaleFactor_X);
					printf("\t -> Percent(%) of V ? ");
					scanf("%lf", &scaleFactor_Y);
					scaleFactor_X/=100.;							// Percent 환산 
					scaleFactor_Y/=100.;
				}
				
				else {
					printf("\t -> H size ? ");
					scanf("%d", &newCols);
					printf("\t -> V size ? ");
					scanf("%d", &newRows);
				}

				printf("\n\t -> Which Interpolation Method \n");
				printf("        1: Nearest Neighbor,  2: Bilinear \n");
	//			printf("        3: Cubic,             4: B-Spline ? ");
				scanf("%d", &interpolMode);

				GetImageSizeInform(cols, rows, scaleFactor_X, scaleFactor_Y, &newCols, &newRows);

				imgBufSize=newCols*newRows;
				resImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);
			
				Interpolation(orgImg, resImg, cols, rows, newCols, newRows, interpolMode);
	
				cols=newCols;
				rows=newRows;
				
				printf("\n\t\t -> Want to generate output image file(Yes=1 No=0)? ");
				scanf("%d", &outputFlag);

				if(outputFlag==1) 
					printf("\t\t -> your next step is to choose Menu 2 \n");
				
				else 
					ImageCopy(resImg, orgImg, cols, rows);	// result image -> original image
				
				break;

						
			case 10 :
				printf("\n\t Which Mode (1:Median, 2:Average) ? ");
				scanf("%d", &thumbNailMode);

				printf("\t -> Window Size (1 Dim) ? ");
				scanf("%d", &windowSize);

				if(windowSize<0)
					break;

				newCols=newRows=0;
				scaleFactor_X=1./(double)windowSize;		// Percent 환산 
				scaleFactor_Y=1./(double)windowSize;
				
				GetImageSizeInform(cols, rows, scaleFactor_X, scaleFactor_Y, &newCols, &newRows);
			
				imgBufSize=newCols*newRows;
				resImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);
		
				MakeThumbnailImage(orgImg, resImg, cols, rows, newCols, newRows, 
								   windowSize, thumbNailMode);

				cols=newCols;
				rows=newRows;

				printf("\n\t\t -> Want to generate output image file(Yes=1 No=0)? ");
				scanf("%d", &outputFlag);

				if(outputFlag==1) 
					printf("\t\t -> your next step is to choose Menu 2 \n");
							
				else 
					ImageCopy(resImg, orgImg, cols, rows);	// result image -> original image
				
				break;


			case 0 : 
				printf("  Your Processing Stop \n");
				break;

			default :
				printf(" Not defined number \n");
				break;
		}
 
	}

	free(resImg);
	free(copyImg);
	free(orgImg);

	printf("\n Processing is Finished!!\n");

	return 0;

}

		
//
//
//	Subroutine	:  Stream_Open()																				
//	Purpose		:  I/O File Open																					
//																																		
//													

void	Stream_Open(char *fname, int flag)
{													
														
	switch(flag) {
		case	FILE_READ :
			if((fp_in=fopen(fname,"rb"))==NULL) {
				printf("\n Error->%s file can not open\n",fname);
				exit(1);
			}
			break;

		case	FILE_WRITE :
			if((fp_out=fopen(fname,"wb"))==NULL) {
				printf("\n Error->%s file can not open\n",fname);
				exit(1);
			}	
			break;

		default :
			printf("\n Error->Invalid File I/O Flag\n");
			exit(1);
			break;
	}

}


//
//
//	Subroutine	:  InitializeBmpParam()						
//	Purpose		:  Read BMP parameter				
//											
//												

void	InitializeBmpParam(FILE *fp, int *hSize, int *vSize)
{

	int		imgBufSize;
	int		cols, rows;

	LoadBMP(fp, hSize, vSize);			// BMP header analysis
	
	cols=*hSize;
	rows=*vSize;

	imgBufSize=cols*rows;					// set frame buffer
	orgImg=(uchar *)Buff_Alloc(sizeof(uchar), imgBufSize);
	
	fread(orgImg, 1, imgBufSize, fp);		// file read of org picture
	Vertical_Flip(orgImg, cols, rows);		// memory reverse for BMP type data

	fclose(fp);

}


//
//
//	Subroutine	:  ImageWriteBmpFile()															
//	Purpose		:  Write BMP type image																		
//																																			
//													

void	ImageWriteBmpFile(FILE *fp, int hSize, int vSize, uchar *imgBuf)
{

	int		imgBufSize;

	imgBufSize=hSize*vSize;
	
	WriteBMP(fp, hSize, vSize);				// write BMP header
	Vertical_Flip(imgBuf, hSize, vSize);	// memory reverse for BMP type data
	fwrite(imgBuf, 1, imgBufSize, fp);		// file write 

	fclose(fp);
}