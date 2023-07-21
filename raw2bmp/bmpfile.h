#define _CRT_SECURE_NO_WARNINGS
#ifndef Bmp_H_
#define Bmp_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<Windows.h>

#define WIDTHBYTES(bits) (((bits)+31)/32*4)

#pragma pack(2)

typedef unsigned char BYTE; // 1Byte
typedef unsigned short WORD; // 2Byte
typedef unsigned int DWORD; // 4Byte
typedef unsigned int LONG;

/*비트맵 파일에 대한 정보(파일헤드)
-파일자체의 정보를 저장하고 있는 구조체*/
typedef struct tagBITMAPFILEHEADER
{
	WORD bfType; //'BM'이라는 값을 저장함
	DWORD bfSize; //바이트 단위로 전체파일 크기
	WORD bfReserved1; //예약된 변수
	WORD bfReserved2; //예약된 변수
	DWORD bfOffBits; //영상데이터 위치까지의 거리
}BITMAPFILEHEADER;

/*영상 자체에 대한 정보(영상헤드)
-비트맵 영상에 대한 크기나 흑백,컬러 정보,팔레트 크기 정보 
등을 저장하기 위하여 파일헤드 바로다음에 위치하는  구조체*/


typedef struct tagBITMAPINFOHEADER
{
	DWORD biSize; //구조체 크기
	LONG biWidth; //픽셀 단위로 영상의 폭
	LONG biHeight; //영상의 높이
	WORD  biplanes; //비트 플레인  수(항상 1)
	WORD biBitCount; //픽셀당 비트수(컬러,흑백 구별)
	DWORD biCompression; //압축유무
	DWORD biSizeImage; //영상의 크기(바이트 단위)
	LONG biXpelsPerMeter; //가로 해상도
	LONG biYpelsPerMeter; //세로 해상도
	DWORD biClrUsed; //실제 사용 색상 수
	DWORD biClrlmportant; // 중요한 색상 인덱스
}BITMAPINFOHEADER;

/*팔레트
-팔레트는 인덱스에 의한 컬러값을 저장하기 위한 구조체이다.
이 구조체를 사용하여 팔레트의 수 만큼 배열을 할당하여 저장한다.
-256 컬러모드의 영상은 팔레트배열 크기가 256개,16 비트 컬러 영상은
팔레트 크기가 2^16개이다.biClrUsed 변수를 참조하면 된다.
-흑백영상의 경우 팔레트는 256개이며,트루컬러의 경우는 
인덱스 저장이 아니라 데이터값을 직접 저장하므로 팔레트가 없다.*/

typedef struct tagRGBQUAD
{
	BYTE rgbBlue; // B 성분
	BYTE rgbGreen; // G 성분
	BYTE rgbRed; // R 성분
	BYTE rgbReserved1; // 예약된 변수
}RGBQUAD;

int Grayraw2bmp(char* rawfile, char* bmpfile, DWORD width, DWORD height);
void change(BYTE* bmp_buffer, BYTE* raw_buffer, DWORD width, DWORD height);
void pallet(RGBQUAD* rgb);

int histogram(char* rawfile, char* histo_grpfile, DWORD wsize, DWORD hsize);
int histogramEQ(char* rawfile, char* EQ_rawfile, DWORD wsize, DWORD hsize);
void histogramCal(char* rawfile, DWORD wsize, DWORD hsize, DWORD* hist_arr);


#endif