#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define Width 256
#define Height 256
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



int main(void)
{
	FILE* file_raw = NULL;
	FILE* file_bmp = NULL;
	BITMAPFILEHEADER   file_h;
	BITMAPINFOHEADER   info_h;
	RGBQUAD         rgb[256];
	BYTE* raw_buffer = NULL;
	BYTE* bmp_buffer = NULL;

	file_raw = fopen("raw_lena.raw", "rb");
	if (file_raw == NULL)
	{
		printf("file open error\n");
		return 0;
	}

	file_bmp = fopen("bmp_lena.bmp", "wb");

	file_h.bfType = 0x4D42; //BM값
	file_h.bfReserved1 = 0;
	file_h.bfReserved2 = 0;
	file_h.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(rgb); //영상데이터 위치
	info_h.biSize = sizeof(BITMAPINFOHEADER);
	info_h.biWidth = (DWORD)Width;
	info_h.biHeight = (DWORD)Height;
	info_h.biplanes = 1; 
	info_h.biBitCount = 8; //흑백 이미지 8bits
	info_h.biCompression = 0;
	info_h.biXpelsPerMeter = 0;
	info_h.biYpelsPerMeter = 0;
	info_h.biClrUsed = 0;
	info_h.biClrlmportant = 0;

	DWORD rwline = WIDTHBYTES(info_h.biWidth * info_h.biBitCount); // bmp width 는 4의 배수
	DWORD rwbmp_size = rwline * info_h.biHeight;
	DWORD rwraw_size = info_h.biWidth * info_h.biHeight;

	info_h.biSizeImage = rwbmp_size; //bmp 영상데이터부분 사이즈
	file_h.bfSize = info_h.biSizeImage + file_h.bfOffBits + 2; //전체 사이즈

	raw_buffer = (BYTE*)malloc(rwraw_size * sizeof(BYTE)); //raw파일 저장할 버퍼

	if (raw_buffer == NULL)
	{
		printf("NULL!\n");
		goto move;
	}
	
	fread(raw_buffer, sizeof(BYTE), rwraw_size, file_raw); // raw 파일 읽음

	//rgb값 0으로 초기화

	for (int i = 0; i < 256; i++) {
		rgb[i].rgbRed = (BYTE)(i % 256);
		rgb[i].rgbGreen = rgb[i].rgbRed;
		rgb[i].rgbBlue = rgb[i].rgbRed;
		rgb[i].rgbReserved1 = 0;
	}
	
	bmp_buffer = (BYTE*)malloc(info_h.biSizeImage * sizeof(BYTE)); //bmp영상데이터 저장할 버퍼
	if (bmp_buffer == NULL)
	{
		printf("NULL!\n");
		goto move;
	}

	//bmp 파일은 raw파일 영상데이터 상하반전
	
	for (int i = 0; i < info_h.biHeight; i++) {

		for (int j = 0; j < rwline; j++) {

			bmp_buffer[i * rwline + j] = raw_buffer[(info_h.biHeight - i - 1) * info_h.biWidth + j];
		}

	}
	
	fwrite((char*)&file_h, sizeof(BYTE), sizeof(BITMAPFILEHEADER), file_bmp);
	
	fwrite((char*)&info_h, sizeof(BYTE), sizeof(BITMAPINFOHEADER), file_bmp);
	
	fwrite((char*)rgb, sizeof(BYTE), sizeof(rgb), file_bmp);
	
	fwrite(bmp_buffer, sizeof(BYTE), info_h.biSizeImage, file_bmp);

move:

	free(raw_buffer);
	free(bmp_buffer);

	fclose(file_raw);
	fclose(file_bmp);

	return 0;
}