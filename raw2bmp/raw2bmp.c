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

	file_h.bfType = 0x4D42; //BM��
	file_h.bfReserved1 = 0;
	file_h.bfReserved2 = 0;
	file_h.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(rgb); //�������� ��ġ
	info_h.biSize = sizeof(BITMAPINFOHEADER);
	info_h.biWidth = (DWORD)Width;
	info_h.biHeight = (DWORD)Height;
	info_h.biplanes = 1; 
	info_h.biBitCount = 8; //��� �̹��� 8bits
	info_h.biCompression = 0;
	info_h.biXpelsPerMeter = 0;
	info_h.biYpelsPerMeter = 0;
	info_h.biClrUsed = 0;
	info_h.biClrlmportant = 0;

	DWORD rwline = WIDTHBYTES(info_h.biWidth * info_h.biBitCount); // bmp width �� 4�� ���
	DWORD rwbmp_size = rwline * info_h.biHeight;
	DWORD rwraw_size = info_h.biWidth * info_h.biHeight;

	info_h.biSizeImage = rwbmp_size; //bmp �������ͺκ� ������
	file_h.bfSize = info_h.biSizeImage + file_h.bfOffBits + 2; //��ü ������

	raw_buffer = (BYTE*)malloc(rwraw_size * sizeof(BYTE)); //raw���� ������ ����

	if (raw_buffer == NULL)
	{
		printf("NULL!\n");
		goto move;
	}
	
	fread(raw_buffer, sizeof(BYTE), rwraw_size, file_raw); // raw ���� ����

	//rgb�� 0���� �ʱ�ȭ

	for (int i = 0; i < 256; i++) {
		rgb[i].rgbRed = (BYTE)(i % 256);
		rgb[i].rgbGreen = rgb[i].rgbRed;
		rgb[i].rgbBlue = rgb[i].rgbRed;
		rgb[i].rgbReserved1 = 0;
	}
	
	bmp_buffer = (BYTE*)malloc(info_h.biSizeImage * sizeof(BYTE)); //bmp�������� ������ ����
	if (bmp_buffer == NULL)
	{
		printf("NULL!\n");
		goto move;
	}

	//bmp ������ raw���� �������� ���Ϲ���
	
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