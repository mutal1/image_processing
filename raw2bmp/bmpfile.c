#include "bmpfile.h"

int Grayraw2bmp(char* rawfile, char* bmpfile, DWORD width, DWORD height)
{
	FILE* file_raw = NULL;
	FILE* file_bmp = NULL;
	BITMAPFILEHEADER   file_h;
	BITMAPINFOHEADER   info_h;
	RGBQUAD         rgb[256];
	BYTE* raw_buffer = NULL;
	BYTE* bmp_buffer = NULL;

	file_raw = fopen(rawfile, "rb");
	if (file_raw == NULL)
	{
		printf("file open error\n");
		return 0;
	}

	file_bmp = fopen(bmpfile,"wb");

	file_h.bfType = 0x4D42; //BM값
	file_h.bfReserved1 = 0;
	file_h.bfReserved2 = 0;
	file_h.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(rgb); //영상데이터 위치
	info_h.biSize = sizeof(BITMAPINFOHEADER);
	info_h.biWidth = (DWORD)width;
	info_h.biHeight = (DWORD)height;
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

	bmp_buffer = (BYTE*)malloc(info_h.biSizeImage * sizeof(BYTE)); //bmp영상데이터 저장할 버퍼
	if (bmp_buffer == NULL)
	{
		printf("NULL!\n");
		goto move;
	}



	fwrite((char*)&file_h, sizeof(BYTE), sizeof(BITMAPFILEHEADER), file_bmp);

	fwrite((char*)&info_h, sizeof(BYTE), sizeof(BITMAPINFOHEADER), file_bmp);

	pallet(rgb);

	fwrite((char*)rgb, sizeof(BYTE), sizeof(rgb), file_bmp);

	change(bmp_buffer, raw_buffer, rwline, info_h.biHeight);

	fwrite(bmp_buffer, sizeof(BYTE), info_h.biSizeImage, file_bmp);

move:

	free(raw_buffer);
	free(bmp_buffer);

	fclose(file_raw);
	fclose(file_bmp);

	return 1;

}

void change(BYTE* bmp_buffer, BYTE* raw_buffer ,DWORD width, DWORD height)
{

	for (int i = 0; i < height; i++) {

		for (int j = 0; j < width; j++) {

			bmp_buffer[i * width + j] = raw_buffer[(height - i - 1)* width + j];
		}

	}
}
void pallet(RGBQUAD* rgb)
{
	
	for (int i = 0; i < 256; i++) {
		(rgb+i)->rgbRed = (BYTE)(i % 256);
		(rgb+i)->rgbGreen = (rgb + i)->rgbRed;
		(rgb + i)->rgbBlue = (rgb + i)->rgbRed;
		(rgb + i)->rgbReserved1 = 0;
	}
}

void histogramCal(char* rawfile, DWORD wsize, DWORD hsize, DWORD* hist_arr)
{
	int i, j;
	int pel = 256;
	BYTE* raw_buf;

	FILE* infile = fopen(rawfile, "rb");
	if (infile == NULL) { printf("error!");return 0; }
	raw_buf = (BYTE*)malloc(wsize * hsize);
	fread(raw_buf, sizeof(BYTE), hsize * wsize, infile);

	/// histogram배열을 초기화
	for (i = 0; i < pel; i++) hist_arr[i] = 0;

	/// 영상의 histogram을 계산
	for (i = 0; i < hsize; i++)
	{
		for (j = 0; j < wsize; j++)
			hist_arr[raw_buf[i + j]]++;
	}

	free(raw_buf);
	fclose(infile);
}

int histogram(char* rawfile, char* histo_grpfile, DWORD wsize, DWORD hsize)
{
	DWORD hist_arr[256];

	histogramCal(rawfile, wsize, hsize, hist_arr);

	int hmin = 1000000;
	int hmax = 0;

	for (int i = 0; i < 256; i++)
	{
		if (hist_arr[i] <= hmin) hmin = hist_arr[i];
		if (hist_arr[i] >= hmax) hmax = hist_arr[i];
	}
	//if(vmax==vmin) return;

	float vd = (float)(hmax - hmin);
	for (int i = 0; i < 256; i++)
	{
		hist_arr[i] = (DWORD)(((float)hist_arr[i] - hmin) * 255.0 / vd);
	}

	BYTE* hist_grp = (BYTE*)malloc(wsize * hsize);

	// 히스토그램의 출력
	for (int i = 0; i < hsize; i++)
		for (int j = 0; j < wsize; j++) hist_grp[i + j] = 255;

	for (int j = 0; j < wsize; j++)
	{
		for (int i = 0; i < hist_arr[j]; i++) hist_grp[256 * (255 - i) + j] = 0;
	}

	FILE* outfile = fopen(histo_grpfile, "wb");
	fwrite(hist_grp, sizeof(BYTE), wsize * hsize, outfile);
	free(hist_grp);
	fclose(outfile);

	return 1;

}

int histogramEQ(char* rawfile, char* EQrawfile, DWORD wsize, DWORD hsize)
{
	DWORD hist_arr[256];
	DWORD sum_hist[256];

	histogramCal(rawfile, wsize, hsize, hist_arr);

	int sum = 0;
	float scale_factor = 255.0f / (float)(hsize * wsize);

	for (int i = 0; i < 256; i++)
	{
		sum += hist_arr[i];
		sum_hist[i] = (int)((sum * scale_factor) + 0.5);
	}

	BYTE* buf = (BYTE*)malloc(wsize * hsize);

	/// LUT로써 sum_hist배열을 사용하여 영상을 변환
	for (int i = 0; i < hsize; i++)
	{
		for (int j = 0; j < wsize; j++) buf[i + j] = sum_hist[buf[i + j]];
	}

	// 메모리를 해제


	FILE* outfile = fopen(EQrawfile, "wb");
	fwrite(buf, sizeof(char), 256 * 256, outfile);
	free(buf);
	fclose(outfile);

	return 1;
}