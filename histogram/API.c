#include "API.h"

//raw2bmp
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

	file_bmp = fopen(bmpfile, "wb");

	file_h.bfType = 0x4D42; //BM��
	file_h.bfReserved1 = 0;
	file_h.bfReserved2 = 0;
	file_h.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(rgb); //�������� ��ġ
	info_h.biSize = sizeof(BITMAPINFOHEADER);
	info_h.biWidth = (DWORD)width;
	info_h.biHeight = (DWORD)height;
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

	bmp_buffer = (BYTE*)malloc(info_h.biSizeImage * sizeof(BYTE)); //bmp�������� ������ ����
	if (bmp_buffer == NULL)
	{
		printf("NULL!\n");
		goto move;
	}

	printf("��� =%d ���� = %d, �ȷ�Ʈ = %d", sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER), sizeof(rgb));
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

//���Ϲ��� �Լ�
void change(BYTE* bmp_buffer, BYTE* raw_buffer, DWORD width, DWORD height)
{

	for (int i = 0; i < height; i++) {

		for (int j = 0; j < width; j++) {

			bmp_buffer[i * width + j] = raw_buffer[(height - i - 1) * width + j];
		}

	}
}
void pallet(RGBQUAD* rgb)
{

	for (int i = 0; i < 256; i++) {
		(rgb + i)->rgbRed = (BYTE)(i % 256);
		(rgb + i)->rgbGreen = (rgb + i)->rgbRed;
		(rgb + i)->rgbBlue = (rgb + i)->rgbRed;
		(rgb + i)->rgbReserved1 = 0;
	}
}

//histogram ���
void histogramCal(char* rawfile, DWORD wsize, DWORD hsize,int hist_arr[])
{
	DWORD tsize = wsize * hsize;
	BYTE* raw_buf = (BYTE*)malloc(sizeof(BYTE) * tsize);
	
	FILE* infile = fopen(rawfile, "rb");
	if (infile == NULL) {printf("error!");return 0; }
	fseek(infile , headsize, SEEK_SET); //header �κ� �ǳʶ�� �б�
	fread(raw_buf, sizeof(BYTE), sizeof(BYTE)*tsize, (infile));
	fclose(infile);

	// hist_arr �ʱ�ȭ 

	for (int i = 0; i < 256; i++) hist_arr[i] = 0;

	// hist_arr �� ��� �� �� ��ŭ ����
	for (int i = 0; i < hsize; i++)
	{
		for (int j = 0; j < wsize; j++)
		{
			hist_arr[raw_buf[(i * wsize) + j]]++;
		}
	}
	free(raw_buf);
}


void histogram(char* rawfile, char* histo_grpfile, DWORD wsize, DWORD hsize)
{
	int hist_arr[256];
	int bit = 256;
	int ss = hsize / bit;

	histogramCal(rawfile, wsize, hsize, hist_arr);
	
	// �ִ밪 �ּҰ� ã�� �Լ�
	int min = 1000000; int max = 0;
	for (int i = 0;i < bit;i++) {
		if (hist_arr[i] <= min) min = hist_arr[i];
		if (hist_arr[i] >= max) max = hist_arr[i];
	}
	
	float vd = (float)(max - min);

	for (int i = 0; i < bit; i++)
	{
		hist_arr[i] = (int)(((float)hist_arr[i] - min) * 255.0 * ((float)ss) / vd); //hist_arr�� �ִ밪 = data height ���� set
	}

	int tsize = hsize * wsize;

	BYTE* buf = (BYTE*)malloc(sizeof(BYTE) * tsize);
	// buf �� 255�� �ʱ�ȭ (�׷����� ����� ����)
	for (int i = 0; i < hsize; i++)
		for (int j = 0; j < wsize; j++) buf[wsize * i + j] = 255;
	// �ش� ����� �󵵼���ŭ �Ʒ��� ���� 0�� ����(�׷��� ����)
	for (int j = 0; j < wsize; j++)
	{
		for (int i = 0; i < hist_arr[j / (ss)]; i++) buf[wsize * (hsize - 1 - i) + j] = 0;
	}
	
	BYTE* out = (BYTE*)malloc(sizeof(BYTE) * tsize);
	change(out,buf,wsize, hsize);
    free(buf);

	BYTE* headbuf = (BYTE*)malloc(sizeof(BYTE) * headsize);
	
	FILE* buffp = fopen(rawfile, "rb");
	fread(headbuf, sizeof(BYTE), headsize, buffp);
	fclose(buffp);

	FILE *outfp = fopen(histo_grpfile, "wb");
	fwrite(headbuf, sizeof(BYTE), headsize, outfp);
	fwrite(out, sizeof(BYTE), tsize, outfp);
	fclose(outfp);
	
	free(headbuf);
	free(out);
}

void histogramEQ(char* rawfile, char* EQrawfile, DWORD wsize, DWORD hsize)
{
	DWORD tsize = wsize * hsize;
	int hist_arr[256];
	DWORD *sum_hist[256];
	int bit = 256;
	
	histogramCal(rawfile, wsize, hsize, hist_arr);

	int sum = 0;
	float scale_factor = 255.0 / (float)(hsize * wsize); // max pel/h*w

	//���� ����ȭ ������׷� �˰���
	for (int i = 0; i < bit; i++)
	{
		sum += hist_arr[i];
		sum_hist[i] = (int)((sum * scale_factor) + 0.5); // rounding
	}

	BYTE* buf = (BYTE*)malloc(sizeof(BYTE) * tsize);
	BYTE* gate = (BYTE*)malloc(sizeof(BYTE) * headsize);

	FILE* in = fopen(rawfile, "rb");
	if (in == NULL) { printf("error!");return 0; }
	fseek(in, headsize, SEEK_SET);
	fread(buf, sizeof(BYTE), sizeof(BYTE) * tsize, in);
	fclose(in);
	
	/// LUT(sum_hist)�� ���� buf�� mapping
	for (int i = 0; i < hsize; i++)
	{
		for (int j = 0; j < wsize; j++) buf[(i*wsize) + j] = sum_hist[buf[(i*wsize) + j]];
	}

	FILE* outfile = NULL;
	in = fopen(rawfile, "rb");
	if (in == NULL) { printf("error!");return 0; }
	fread(gate, sizeof(char), headsize, in);
	fclose(in);

	outfile = fopen(EQrawfile, "wb");
	fwrite(gate, sizeof(char), headsize, outfile);
    fwrite(buf, sizeof(char), tsize , outfile);

	fclose(outfile);
	free(gate);
	free(buf);
}

void convolution(char *filename,char *convfile, DWORD wsize, DWORD hsize, int mwidth, int mheight){

	float rate;

	printf("Input rate:");
    scanf("%f",&rate);

   BYTE headbuf[1078]; // header size buf

	//buf ���� �Ҵ�
    BYTE *buf;
    buf=(BYTE*)malloc(sizeof(BYTE)*(hsize*wsize));
	
	//out buf ���� �Ҵ�
    BYTE *outbuf;
    outbuf=(BYTE*)malloc(sizeof(BYTE)*(hsize*wsize));
    
	//input file open
    FILE *fp = fopen(filename,"rb");
    fseek(fp,1078,SEEK_SET);
    fread(buf,sizeof(BYTE),wsize*hsize,fp);
    fclose(fp);

	//convmask �����Ҵ�
    int *convmask;
    convmask=(int*)malloc(sizeof(int)*(mheight*mwidth));

	// input filter 
    printf("Input mask filter:\n");
    for(int i=0;i<mheight;i++){
		for(int j=0;j<mwidth;j++){
			scanf("%d",&convmask[mheight*i+j]);
		}
	}

	// print filter
	printf("rate=%.2f",rate);
	printf("\n");
	for(int i=0;i<mheight;i++){
		for(int j=0;j<mwidth;j++){
			printf("[%d]",convmask[mheight*i+j]);
		}
		printf("\n");
	}

    int convsum = 0; //convolution var

    for(int i=0;i<wsize;i++){
        for(int j=0;j<hsize;j++)
        outbuf[hsize*i+j]=0;
    }  
    
    for(int i=0;i<hsize;i++){
        for(int j=0;j<wsize;j++){
            if(i == 0 && j==0){ // zero padding 
                 for(int k=mheight/2;k<mheight;k++){
                    for(int p=mwidth/2;p<mwidth;p++){
                       convsum+=(int)(buf[(hsize*(i-1+k))+(j-1+p)]*convmask[(mwidth*k)+p]);
                    }
                }
				convsum = rate*convsum; //test
                if(convsum < 0 && convsum > -256 ){ //unsigned
                     outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){ //white
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){ //general
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(i == hsize-1 && j == wsize-1){
                for(int k=0;k<(mheight/2)+1;k++){
                    for(int p=0;p<(mwidth/2)+1;p++){
                     convsum+=(int)(buf[(hsize*(i-1+k))+(j-1+p)]*convmask[(mwidth*k)+p]);
                    }
                }
				convsum = rate*convsum; //test
             if(convsum < 0 && convsum > -256 ){ //unsigned
                     outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){ //white
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){ //general
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(i == hsize-1 && j == 0){
                for(int k=0;k<(mheight/2)+1;k++){
                    for(int p=mwidth/2;p<mwidth;p++){
                     convsum+=(int)(buf[(hsize*(i-1+k))+(j-1+p)]*convmask[(mwidth*k)+p]);
                    }
                }
				convsum = rate*convsum; //test
             if(convsum < 0 && convsum > -256 ){ //unsigned
                     outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){ //white
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){ //general
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(i == 0 && j == mwidth-1){
                for(int k=(mheight/2);k<mheight;k++){
                    for(int p=0;p<(mwidth/2)+1;p++){
                     convsum+=(int)(buf[(hsize*(i-1+k))+(j-1+p)]*convmask[(mwidth*k)+p]);
                    }
                }
				convsum = rate*convsum; //test
             if(convsum < 0 && convsum > -256 ){ //unsigned
                     outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){ //white
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){ //general
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(i == 0  && j != mwidth-1 && j != 0 ){
                for(int k=mheight/2;k<mheight;k++){
                    for(int p=0;p<mwidth;p++){
                     convsum+=(int)(buf[(hsize*(i-1+k))+(j-1+p)]*convmask[(mwidth*k)+p]);
                    }
                }
				convsum = rate*convsum; //test
             if(convsum < 0 && convsum > -256 ){ //unsigned
                     outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){ //white
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){ //general
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(i == mheight-1 && j != mwidth-1 && j != 0 ){
                for(int k=0;k<(mheight/2)+1;k++){
                    for(int p=0;p<mwidth;p++){
                     convsum+=(int)(buf[(hsize*(i-1+k))+(j-1+p)]*convmask[(mwidth*k)+p]);
                    }
                }
				convsum = rate*convsum; //test
             if(convsum < 0 && convsum > -256 ){ //unsigned
                     outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){ //white
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){ //general
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(j == 0 && i != mheight-1 && i != 0){
                for(int k=0;k<mheight;k++){
                    for(int p=mwidth/2;p<mwidth;p++){
                     convsum+=(int)(buf[(hsize*(i-1+k))+(j-1+p)]*convmask[(mwidth*k)+p]);
                    }
                }
				convsum = rate*convsum; //test
             if(convsum < 0 && convsum > -256 ){ //unsigned
                     outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){ //white
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){ //general
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(j == mwidth-1 && i != mheight-1 && i != 0){
                for(int k=0;k<mheight;k++){
                    for(int p=0;p<((mwidth/2)+1);p++){
                     convsum+=(int)(buf[(hsize*(i-1+k))+(j-1+p)]*convmask[(mwidth*k)+p]);
                    }
                }
				convsum = rate*convsum; //test
             if(convsum < 0 && convsum > -256 ){ //unsigned
                     outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){ //white
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){ //general
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else{
                for(int k=0;k<mheight;k++){
                    for(int p=0;p<mwidth;p++){
                     convsum+=(int)(buf[(hsize*(i-1+k))+(j-1+p)]*convmask[(mwidth*k)+p]);
                    }
                }
				convsum = rate*convsum; //test
				outbuf[hsize*i+j] = convsum;
             if(convsum < 0 && convsum > -256 ){ //unsigned
                     outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){ //white
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){ //general
                    outbuf[hsize*i+j] = convsum;
                }
                
            }
            convsum=0;
        }
    }

	//print outbuf
    /*
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++)
        printf("[%d]",outbuf[i][j]);
    }
    */
    
    
    
    fp = fopen(filename,"rb");
    fread(headbuf,sizeof(BYTE),1078,fp);
    fclose(fp);

    FILE *fp2 = fopen(convfile,"wb");
    fwrite(headbuf,sizeof(BYTE),1078,fp2);
    fwrite(outbuf,sizeof(BYTE),wsize*hsize,fp);
    fclose(fp2);

    // buf �Ҵ� ����
	free(buf);

	//outbuf �Ҵ� ����
	free(outbuf);

	//convmask �Ҵ� ����
	free(convmask);
}
