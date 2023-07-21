// #define _CRT_SECURE_NO_WARNINGS
#ifndef API_H_
#define API_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define WIDTHBYTES(bits) (((bits)+31)/32*4)
#define headsize 1078

#pragma pack(2)

typedef unsigned char BYTE; // 1Byte
typedef unsigned short WORD; // 2Byte
typedef unsigned int DWORD; // 4Byte
typedef unsigned int LONG;

/*��Ʈ�� ���Ͽ� ���� ����(�������)
-������ü�� ������ �����ϰ� �ִ� ����ü*/
typedef struct tagBITMAPFILEHEADER
{
	WORD bfType; //'BM'�̶�� ���� ������
	DWORD bfSize; //����Ʈ ������ ��ü���� ũ��
	WORD bfReserved1; //����� ����
	WORD bfReserved2; //����� ����
	DWORD bfOffBits; //�������� ��ġ������ �Ÿ�
}BITMAPFILEHEADER;

/*���� ��ü�� ���� ����(�������)
-��Ʈ�� ���� ���� ũ�⳪ ���,�÷� ����,�ȷ�Ʈ ũ�� ����
���� �����ϱ� ���Ͽ� ������� �ٷδ����� ��ġ�ϴ�  ����ü*/


typedef struct tagBITMAPINFOHEADER
{
	DWORD biSize; //����ü ũ��
	LONG biWidth; //�ȼ� ������ ������ ��
	LONG biHeight; //������ ����
	WORD  biplanes; //��Ʈ �÷���  ��(�׻� 1)
	WORD biBitCount; //�ȼ��� ��Ʈ��(�÷�,��� ����)
	DWORD biCompression; //��������
	DWORD biSizeImage; //������ ũ��(����Ʈ ����)
	LONG biXpelsPerMeter; //���� �ػ�
	LONG biYpelsPerMeter; //���� �ػ�
	DWORD biClrUsed; //���� ��� ���� ��
	DWORD biClrlmportant; // �߿��� ���� �ε���
}BITMAPINFOHEADER;

/*�ȷ�Ʈ
-�ȷ�Ʈ�� �ε����� ���� �÷����� �����ϱ� ���� ����ü�̴�.
�� ����ü�� ����Ͽ� �ȷ�Ʈ�� �� ��ŭ �迭�� �Ҵ��Ͽ� �����Ѵ�.
-256 �÷������ ������ �ȷ�Ʈ�迭 ũ�Ⱑ 256��,16 ��Ʈ �÷� ������
�ȷ�Ʈ ũ�Ⱑ 2^16���̴�.biClrUsed ������ �����ϸ� �ȴ�.
-��鿵���� ��� �ȷ�Ʈ�� 256���̸�,Ʈ���÷��� ����
�ε��� ������ �ƴ϶� �����Ͱ��� ���� �����ϹǷ� �ȷ�Ʈ�� ����.*/

typedef struct tagRGBQUAD
{
	BYTE rgbBlue; // B ����
	BYTE rgbGreen; // G ����
	BYTE rgbRed; // R ����
	BYTE rgbReserved1; // ����� ����
}RGBQUAD;

int Grayraw2bmp(char* rawfile, char* bmpfile, DWORD width, DWORD height);
void change(BYTE* bmp_buffer, BYTE* raw_buffer, DWORD width, DWORD height);
void pallet(RGBQUAD* rgb);
void histogram(char* rawfile, char* histo_grpfile, DWORD wsize, DWORD hsize);
void histogramEQ(char* rawfile, char* EQ_rawfile, DWORD wsize, DWORD hsize);
void histogramCal(char* rawfile, DWORD wsize, DWORD hsize, int hist_arr[]);
void convolution(char *filename,char *convfile, DWORD wsize, DWORD hsize, int mwidth, int mheight);

#endif