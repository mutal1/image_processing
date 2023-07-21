#include "API.h"

// raw2bmp
int Grayraw2bmp(char *rawfile, char *bmpfile, DWORD width, DWORD height)
{
    FILE *file_raw = NULL;
    FILE *file_bmp = NULL;
    BITMAPFILEHEADER file_h;
    BITMAPINFOHEADER info_h;
    RGBQUAD rgb[256];
    BYTE *raw_buffer = NULL;
    BYTE *bmp_buffer = NULL;

    file_raw = fopen(rawfile, "rb");
    if (file_raw == NULL)
    {
        printf("file open error\n");
        return 0;
    }

    file_bmp = fopen(bmpfile, "wb");

    file_h.bfType = 0x4D42; // BM��
    file_h.bfReserved1 = 0;
    file_h.bfReserved2 = 0;
    file_h.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(rgb); // �������� ��ġ
    info_h.biSize = sizeof(BITMAPINFOHEADER);
    info_h.biWidth = (DWORD)width;
    info_h.biHeight = (DWORD)height;
    info_h.biplanes = 1;
    info_h.biBitCount = 8; // ��� �̹��� 8bits
    info_h.biCompression = 0;
    info_h.biXpelsPerMeter = 0;
    info_h.biYpelsPerMeter = 0;
    info_h.biClrUsed = 0;
    info_h.biClrlmportant = 0;

    DWORD rwline = WIDTHBYTES(info_h.biWidth * info_h.biBitCount); // bmp width �� 4�� ���
    DWORD rwbmp_size = rwline * info_h.biHeight;
    DWORD rwraw_size = info_h.biWidth * info_h.biHeight;

    info_h.biSizeImage = rwbmp_size;                           // bmp �������ͺκ� ������
    file_h.bfSize = info_h.biSizeImage + file_h.bfOffBits + 2; // ��ü ������

    raw_buffer = (BYTE *)malloc(rwraw_size * sizeof(BYTE)); // raw���� ������ ����

    if (raw_buffer == NULL)
    {
        printf("NULL!\n");
        goto move;
    }

    fread(raw_buffer, sizeof(BYTE), rwraw_size, file_raw); // raw ���� ����

    bmp_buffer = (BYTE *)malloc(info_h.biSizeImage * sizeof(BYTE)); // bmp�������� ������ ����
    if (bmp_buffer == NULL)
    {
        printf("NULL!\n");
        goto move;
    }

    printf("��� =%d ���� = %d, �ȷ�Ʈ = %d", sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER), sizeof(rgb));
    fwrite((char *)&file_h, sizeof(BYTE), sizeof(BITMAPFILEHEADER), file_bmp);

    fwrite((char *)&info_h, sizeof(BYTE), sizeof(BITMAPINFOHEADER), file_bmp);

    pallet(rgb);

    fwrite((char *)rgb, sizeof(BYTE), sizeof(rgb), file_bmp);

    change(bmp_buffer, raw_buffer, rwline, info_h.biHeight);

    fwrite(bmp_buffer, sizeof(BYTE), info_h.biSizeImage, file_bmp);

move:

    free(raw_buffer);
    free(bmp_buffer);

    fclose(file_raw);
    fclose(file_bmp);

    return 1;
}

// ���Ϲ��� �Լ�
void change(BYTE *bmp_buffer, BYTE *raw_buffer, DWORD width, DWORD height)
{

    for (int i = 0; i < height; i++)
    {

        for (int j = 0; j < width; j++)
        {

            bmp_buffer[i * width + j] = raw_buffer[(height - i - 1) * width + j];
        }
    }
}
void pallet(RGBQUAD *rgb)
{

    for (int i = 0; i < 256; i++)
    {
        (rgb + i)->rgbRed = (BYTE)(i % 256);
        (rgb + i)->rgbGreen = (rgb + i)->rgbRed;
        (rgb + i)->rgbBlue = (rgb + i)->rgbRed;
        (rgb + i)->rgbReserved1 = 0;
    }
}

// histogram ���
void histogramCal(char *rawfile, DWORD wsize, DWORD hsize, int hist_arr[])
{
    DWORD tsize = wsize * hsize;
    BYTE *raw_buf = (BYTE *)malloc(sizeof(BYTE) * tsize);

    FILE *infile = fopen(rawfile, "rb");
    if (infile == NULL)
    {
        printf("error!");
        return 0;
    }
    fseek(infile, headsize, SEEK_SET); // header �κ� �ǳʶ�� �б�
    fread(raw_buf, sizeof(BYTE), sizeof(BYTE) * tsize, (infile));
    fclose(infile);

    // hist_arr �ʱ�ȭ

    for (int i = 0; i < 256; i++)
        hist_arr[i] = 0;

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

void histogram(char *rawfile, char *histo_grpfile, DWORD wsize, DWORD hsize)
{
    int hist_arr[256];
    int bit = 256;
    int ss = hsize / bit;

    histogramCal(rawfile, wsize, hsize, hist_arr);

    // �ִ밪 �ּҰ� ã�� �Լ�
    int min = 1000000;
    int max = 0;
    for (int i = 0; i < bit; i++)
    {
        if (hist_arr[i] <= min)
            min = hist_arr[i];
        if (hist_arr[i] >= max)
            max = hist_arr[i];
    }

    float vd = (float)(max - min);

    for (int i = 0; i < bit; i++)
    {
        hist_arr[i] = (int)(((float)hist_arr[i] - min) * 255.0 * ((float)ss) / vd); // hist_arr�� �ִ밪 = data height ���� set
    }

    int tsize = hsize * wsize;

    BYTE *buf = (BYTE *)malloc(sizeof(BYTE) * tsize);
    // buf �� 255�� �ʱ�ȭ (�׷����� ����� ����)
    for (int i = 0; i < hsize; i++)
        for (int j = 0; j < wsize; j++)
            buf[wsize * i + j] = 255;
    // �ش� ����� �󵵼���ŭ �Ʒ��� ���� 0�� ����(�׷��� ����)
    for (int j = 0; j < wsize; j++)
    {
        for (int i = 0; i < hist_arr[j / (ss)]; i++)
            buf[wsize * (hsize - 1 - i) + j] = 0;
    }

    BYTE *out = (BYTE *)malloc(sizeof(BYTE) * tsize);
    change(out, buf, wsize, hsize);
    free(buf);

    BYTE *headbuf = (BYTE *)malloc(sizeof(BYTE) * headsize);

    FILE *buffp = fopen(rawfile, "rb");
    fread(headbuf, sizeof(BYTE), headsize, buffp);
    fclose(buffp);

    FILE *outfp = fopen(histo_grpfile, "wb");
    fwrite(headbuf, sizeof(BYTE), headsize, outfp);
    fwrite(out, sizeof(BYTE), tsize, outfp);
    fclose(outfp);

    free(headbuf);
    free(out);
}

void histogramEQ(char *rawfile, char *EQrawfile, DWORD wsize, DWORD hsize)
{
    DWORD tsize = wsize * hsize;
    int hist_arr[256];
    DWORD *sum_hist[256];
    int bit = 256;

    histogramCal(rawfile, wsize, hsize, hist_arr);

    int sum = 0;
    float scale_factor = 255.0 / (float)(hsize * wsize); // max pel/h*w

    // ���� ����ȭ ������׷� �˰�����
    for (int i = 0; i < bit; i++)
    {
        sum += hist_arr[i];
        sum_hist[i] = (int)((sum * scale_factor) + 0.5); // rounding
    }

    BYTE *buf = (BYTE *)malloc(sizeof(BYTE) * tsize);
    BYTE *gate = (BYTE *)malloc(sizeof(BYTE) * headsize);

    FILE *in = fopen(rawfile, "rb");
    if (in == NULL)
    {
        printf("error!");
        return 0;
    }
    fseek(in, headsize, SEEK_SET);
    fread(buf, sizeof(BYTE), sizeof(BYTE) * tsize, in);
    fclose(in);

    /// LUT(sum_hist)�� ���� buf�� mapping
    for (int i = 0; i < hsize; i++)
    {
        for (int j = 0; j < wsize; j++)
            buf[(i * wsize) + j] = sum_hist[buf[(i * wsize) + j]];
    }

    FILE *outfile = NULL;
    in = fopen(rawfile, "rb");
    if (in == NULL)
    {
        printf("error!");
        return 0;
    }
    fread(gate, sizeof(char), headsize, in);
    fclose(in);

    outfile = fopen(EQrawfile, "wb");
    fwrite(gate, sizeof(char), headsize, outfile);
    fwrite(buf, sizeof(char), tsize, outfile);

    fclose(outfile);
    free(gate);
    free(buf);
}

void convolution(char *filename, char *convfile, DWORD wsize, DWORD hsize, int mwidth, int mheight)
{

    float rate;

    printf("Input rate:");
    scanf("%f", &rate);

    BYTE headbuf[1078]; // header size buf

    // buf ���� �Ҵ�
    BYTE *buf;
    buf = (BYTE *)malloc(sizeof(BYTE) * (hsize * wsize));

    // out buf ���� �Ҵ�
    BYTE *outbuf;
    outbuf = (BYTE *)malloc(sizeof(BYTE) * (hsize * wsize));

    // input file open
    FILE *fp = fopen(filename, "rb");
    fseek(fp, 1078, SEEK_SET);
    fread(buf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp);

    // convmask �����Ҵ�
    int *convmask;
    convmask = (int *)malloc(sizeof(int) * (mheight * mwidth));

    // input filter
    printf("Input mask filter:\n");
    for (int i = 0; i < mheight; i++)
    {
        for (int j = 0; j < mwidth; j++)
        {
            scanf("%d", &convmask[mheight * i + j]);
        }
    }

    // print filter
    printf("rate=%.2f", rate);
    printf("\n");
    for (int i = 0; i < mheight; i++)
    {
        for (int j = 0; j < mwidth; j++)
        {
            printf("[%d]", convmask[mheight * i + j]);
        }
        printf("\n");
    }

    int convsum = 0; // convolution var

    for (int i = 0; i < wsize; i++)
    {
        for (int j = 0; j < hsize; j++)
            outbuf[wsize * i + j] = 0;
    }

    for (int i = 0; i < hsize; i++)
    {
        for (int j = 0; j < wsize; j++)
        {
            if (i == 0 && j == 0)
            { // zero padding
                for (int k = mheight / 2; k < mheight; k++)
                {
                    for (int p = mwidth / 2; p < mwidth; p++)
                    {
                        convsum += (int)(buf[(wsize * (i - 1 + k)) + (j - 1 + p)]) * convmask[(mwidth * k) + p];
                    }
                }
                outbuf[wsize * i + j] = conv_clam(convsum, rate);
            }
            else if (i == hsize - 1 && j == wsize - 1)
            {
                for (int k = 0; k < (mheight / 2) + 1; k++)
                {
                    for (int p = 0; p < (mwidth / 2) + 1; p++)
                    {
                        convsum += (int)(buf[(wsize * (i - 1 + k)) + (j - 1 + p)]) * convmask[(mwidth * k) + p];
                    }
                }
                outbuf[wsize * i + j] = conv_clam(convsum, rate);
            }
            else if (i == hsize - 1 && j == 0)
            {
                for (int k = 0; k < (mheight / 2) + 1; k++)
                {
                    for (int p = mwidth / 2; p < mwidth; p++)
                    {
                        convsum += (int)(buf[(wsize * (i - 1 + k)) + (j - 1 + p)]) * convmask[(mwidth * k) + p];
                    }
                }
                outbuf[wsize * i + j] = conv_clam(convsum, rate);
            }
            else if (i == 0 && j == mwidth - 1)
            {
                for (int k = (mheight / 2); k < mheight; k++)
                {
                    for (int p = 0; p < (mwidth / 2) + 1; p++)
                    {
                        convsum += (int)(buf[(wsize * (i - 1 + k)) + (j - 1 + p)]) * convmask[(mwidth * k) + p];
                    }
                }
                outbuf[wsize * i + j] = conv_clam(convsum, rate);
            }
            else if (i == 0 && j != mwidth - 1 && j != 0)
            {
                for (int k = mheight / 2; k < mheight; k++)
                {
                    for (int p = 0; p < mwidth; p++)
                    {
                        convsum += (int)(buf[(wsize * (i - 1 + k)) + (j - 1 + p)]) * convmask[(mwidth * k) + p];
                    }
                }
                outbuf[wsize * i + j] = conv_clam(convsum, rate);
            }
            else if (i == mheight - 1 && j != mwidth - 1 && j != 0)
            {
                for (int k = 0; k < (mheight / 2) + 1; k++)
                {
                    for (int p = 0; p < mwidth; p++)
                    {
                        convsum += (int)(buf[(wsize * (i - 1 + k)) + (j - 1 + p)]) * convmask[(mwidth * k) + p];
                    }
                }
                outbuf[wsize * i + j] = conv_clam(convsum, rate);
            }
            else if (j == 0 && i != mheight - 1 && i != 0)
            {
                for (int k = 0; k < mheight; k++)
                {
                    for (int p = mwidth / 2; p < mwidth; p++)
                    {
                        convsum += (int)(buf[(wsize * (i - 1 + k)) + (j - 1 + p)]) * convmask[(mwidth * k) + p];
                    }
                }
                outbuf[wsize * i + j] = conv_clam(convsum, rate);
            }
            else if (j == mwidth - 1 && i != mheight - 1 && i != 0)
            {
                for (int k = 0; k < mheight; k++)
                {
                    for (int p = 0; p < ((mwidth / 2) + 1); p++)
                    {
                        convsum += (int)(buf[(wsize * (i - 1 + k)) + (j - 1 + p)]) * convmask[(mwidth * k) + p];
                    }
                }
                outbuf[wsize * i + j] = conv_clam(convsum, rate);
            }
            else
            {
                for (int k = 0; k < mheight; k++)
                {
                    for (int p = 0; p < mwidth; p++)
                    {
                        convsum += (int)(buf[(wsize * (i - 1 + k)) + (j - 1 + p)]) * convmask[(mwidth * k) + p];
                    }
                }
                outbuf[wsize * i + j] = conv_clam(convsum, rate);
            }
            convsum = 0;
        }
    }

    // print outbuf
    /*
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++)
        printf("[%d]",outbuf[i][j]);
    }
    */

    fp = fopen(filename, "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen(convfile, "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);
    fwrite(outbuf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp2);

    // buf �Ҵ� ����
    free(buf);

    // outbuf �Ҵ� ����
    free(outbuf);

    // convmask �Ҵ� ����
    free(convmask);
}

void edge_detection(char *filename, char *edgefile, int wsize, int hsize)
{
    BYTE headbuf[1078]; // header size buf
    // buf
    BYTE *buf = (BYTE *)malloc(sizeof(char) * (wsize * hsize));
    // out buf
    BYTE *outbuf = (BYTE *)malloc(sizeof(char) * (wsize * hsize));

    int threshold1, threshold2;

    printf("Input threshold1:");
    scanf("%d", &threshold1);
    printf("Input threshold2:");
    scanf("%d", &threshold2);

    FILE *fp = fopen(filename, "rb");
    fseek(fp, 1078, SEEK_SET);
    fread(buf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp);

    int convmask_x[3][3] = {{-1, 0, 1},
                            {-2, 0, 2},
                            {-1, 0, 1}};
    int convmask_y[3][3] = {{-1, -2, -1},
                            {0, 0, 0},
                            {1, 2, 1}};

    int convsum_x = 0;
    int convsum_y = 0;

    for (int i = 0; i < wsize; i++)
    {
        for (int j = 0; j < hsize; j++)
            outbuf[hsize * i + j] = 0;
    }

    for (int i = 1; i < hsize - 1; i++)
    {
        for (int j = 1; j < wsize - 1; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                for (int p = 0; p < 3; p++)
                {
                    convsum_x += buf[wsize * (i - 1 + k) + (j - 1 + p)] * convmask_x[k][p];
                }
            }
            for (int k = 0; k < 3; k++)
            {
                for (int p = 0; p < 3; p++)
                {
                    convsum_y += buf[wsize * (i - 1 + k) + (j - 1 + p)] * convmask_y[k][p];
                }
            }
            outbuf[wsize * i + j] = clampping(convsum_x, convsum_y, threshold1, threshold2);
            convsum_x = 0;
            convsum_y = 0;
        }
    }

    fp = fopen(filename, "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen(edgefile, "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);
    fwrite(outbuf, sizeof(BYTE), (hsize * wsize), fp);
    fclose(fp2);

    free(outbuf);
    free(buf);
}

BYTE clampping(int conv_x, int conv_y, int threshold1, int threshold2)
{
    int result;
    result = (int)sqrt((float)(conv_x * conv_x) + (float)(conv_y * conv_y));

    if (result > threshold1)
        result = 255;
    else if (result < threshold2)
        result = 0;
    // result = (result > 255) ? 255 : result;
    return (BYTE)result;
}

BYTE conv_clam(int convsum, float rate)
{
    int result = 0;
    result = (int)(fabs(rate * (float)convsum) + 0.5);
    result = (result > 255) ? 255 : result;
    return (BYTE)result;
}

void lotation(char *filename, char *lotationfile, int wsize, int hsize)
{
    BYTE headbuf[1078]; // header size buf

    BYTE *buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

    BYTE *angle_90buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

    BYTE *angle_180buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

    BYTE *angle_270buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

    int count;

    printf("Input count:");
    scanf("%d", &count);

    FILE *fp = fopen(filename, "rb");
    fseek(fp, 1078, SEEK_SET);
    fread(buf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp);

    for (int i = 0; i < hsize; i++)
    {
        for (int j = 0; j < wsize; j++)
        {
            angle_90buf[wsize * i + j] = buf[(j * wsize) + (wsize - 1 - i)];
        }
    }

    for (int i = 0; i < hsize; i++)
    {
        for (int j = 0; j < wsize; j++)
        {
            angle_180buf[wsize * i + j] = angle_90buf[(j * wsize) + (wsize - 1 - i)];
        }
    }

    for (int i = 0; i < hsize; i++)
    {
        for (int j = 0; j < wsize; j++)
        {
            angle_270buf[wsize * i + j] = angle_180buf[(j * wsize) + (wsize - 1 - i)];
        }
    }

    fp = fopen(filename, "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen(lotationfile, "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);

    if (count == 1)
    {
        fwrite(angle_90buf, sizeof(BYTE), (hsize * wsize), fp);
    }
    else if (count == 2)
    {
        fwrite(angle_180buf, sizeof(BYTE), (hsize * wsize), fp);
    }
    else if (count == 3)
    {
        fwrite(angle_270buf, sizeof(BYTE), (hsize * wsize), fp);
    }

    fclose(fp2);

    free(angle_90buf);
    free(angle_180buf);
    free(angle_270buf);
    free(buf);
}

// void expend(char *filename, char *lotationfile, int wsize, int hsize)
// {
//     BYTE headbuf[1078]; // header size buf

//     BYTE *buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

//     BYTE *outbuf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

//     int count;

//     printf("Input count:");
//     scanf("%d", &count);

//     FILE *fp = fopen(filename, "rb");
//     fseek(fp, 1078, SEEK_SET);
//     fread(buf, sizeof(BYTE), wsize * hsize, fp);
//     fclose(fp);

//     // 여기서 부터 code
//     for (int i = 0; i < hsize; i++)
//     {
//         for (int j = 0; j < wsize; j++)
//         {
//             outbuf[wsize * i + j] = 0;
//         }
//     }

//     for (int i = 1; i < hsize - 1; i++)
//     {
//         for (int j = 1; j < wsize - 1; j++)
//         {
//             for (int k = 0; k < mheight; k++)
//             {
//                 for (int p = 0; p < mwidth; p++)
//                 {
//                     pixel[p + k] = buf[wsize * (i - 1 + k) + (j - 1 + p)]
//                 }
//             }
//             for (int col = 0; col < mheight; col++)
//             {
//                 for (int row = 0; row < mwidth; row++)
//                 {
//                     BYTE temp = 0;
//                     pixel[mwidth * col + row] = pixel[]
//                 }
//             }
//         }
//     }

//     fp = fopen(filename, "rb");
//     fread(headbuf, sizeof(BYTE), 1078, fp);
//     fclose(fp);

//     FILE *fp2 = fopen(lotationfile, "wb");
//     fwrite(headbuf, sizeof(BYTE), 1078, fp2);
//     fwrite(outbuf, sizeof(BYTE), (hsize * wsize), fp);
//     fclose(fp2);

//     free(outbuf);
//     free(buf);
// }

void median(char *filename, char *lotationfile, int wsize, int hsize)
{
    BYTE headbuf[1078]; // header size buf

    BYTE *buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

    BYTE *outbuf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));
    int row, col;

    printf("Input row & col:");
    scanf("%d %d", &row, &col);

    BYTE *pixel_arr = (BYTE *)malloc(sizeof(char) * (row * col));
    BYTE pixel = 0;

    FILE *fp = fopen(filename, "rb");
    fseek(fp, 1078, SEEK_SET);
    fread(buf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp);

    for (int i = 0; i < hsize; i++)
    {
        for (int j = 0; j < wsize; j++)
        {
            outbuf[wsize * i + j] = 0;
        }
    }

    for (int i = 1; i < hsize - 1; i++)
    {
        for (int j = 1; j < wsize - 1; j++)
        {
            for (int k = 0; k < col; k++)
            {
                for (int p = 0; p < row; p++)
                {
                    pixel_arr[k * row + p] = buf[wsize * (i - 1 + k) + (j - 1 + p)];
                }
            }
            pixel = sort(pixel_arr, col, row);
            outbuf[wsize * i + j] = pixel;
        }
    }

    fp = fopen(filename, "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen(lotationfile, "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);
    fwrite(outbuf, sizeof(BYTE), (hsize * wsize), fp);
    fclose(fp2);

    free(outbuf);
    free(buf);
}

BYTE sort(BYTE sort_arr[], int col, int row)
{

    BYTE temp = 0;

    for (int i = 0; i < col * row - 1; i++)
    {
        for (int j = 0; j < col * row - 1 - i; j++)
        {
            if (sort_arr[j] > sort_arr[j + 1])
            {
                temp = sort_arr[j];
                sort_arr[j] = sort_arr[j + 1];
                sort_arr[j + 1] = temp;
            }
        }
    }

    return sort_arr[col * row / 2];
}

void avg(char *filename, char *lotationfile, int wsize, int hsize)
{
    BYTE headbuf[1078]; // header size buf

    BYTE *buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

    BYTE *outbuf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));
    int row, col;
    int sum = 0;

    printf("Input row & col:");
    scanf("%d %d", &row, &col);

    float rate = 1.0 / (float)((float)row * (float)col);

    BYTE *pixel_arr = (BYTE *)malloc(sizeof(char) * (row * col));
    BYTE pixel = 0;

    FILE *fp = fopen(filename, "rb");
    fseek(fp, 1078, SEEK_SET);
    fread(buf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp);

    for (int i = 0; i < hsize; i++)
    {
        for (int j = 0; j < wsize; j++)
        {
            outbuf[wsize * i + j] = 0;
        }
    }

    for (int i = 1; i < hsize - 1; i++)
    {
        for (int j = 1; j < wsize - 1; j++)
        {
            for (int k = 0; k < col; k++)
            {
                for (int p = 0; p < row; p++)
                {
                    pixel_arr[k * row + p] = buf[wsize * (i - 1 + k) + (j - 1 + p)];
                    sum += pixel_arr[k * row + p];
                }
            }

            sum = conv_clam(sum, rate);
            outbuf[wsize * i + j] = sum;
            sum = 0;
        }
    }

    fp = fopen(filename, "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen(lotationfile, "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);
    fwrite(outbuf, sizeof(BYTE), (hsize * wsize), fp);
    fclose(fp2);

    free(outbuf);
    free(buf);
}

void Binarization(char *filename, char *output, int wsize, int hsize, BYTE threshold)
{
    BYTE headbuf[1078]; // header size buf
    // buf �����Ҵ�
    BYTE *buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));
    // out buf ���� �Ҵ�
    BYTE *outbuf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

    // input file open
    FILE *fp = fopen(filename, "rb");
    fseek(fp, 1078, SEEK_SET);
    fread(buf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp);

    int imgSize = wsize * hsize;
    for (int i = 0; i < imgSize; i++)
    {
        if (buf[i] < threshold)
            outbuf[i] = 0;
        else
            outbuf[i] = 255;
    }

    fp = fopen(filename, "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen(output, "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);
    fwrite(outbuf, sizeof(BYTE), (hsize * wsize), fp);
    fclose(fp2);

    free(outbuf);
    free(buf);
}

void MirrorImg(char *filename, char *output, int wsize, int hsize)
{
    BYTE headbuf[1078]; // header size buf
    // buf �����Ҵ�
    BYTE *buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));
    // out buf ���� �Ҵ�
    BYTE *outbuf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

    // input file open
    FILE *fp = fopen(filename, "rb");
    fseek(fp, 1078, SEEK_SET);
    fread(buf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp);

    int newX = 0;

    for (int i = 0; i < hsize; i++)
    {
        for (int j = 0; j < wsize; j++)
        {
            newX = wsize - j - 1;
            outbuf[i * wsize + newX] = buf[i * wsize + j];
        }
    }

    fp = fopen(filename, "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen(output, "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);
    fwrite(outbuf, sizeof(BYTE), (hsize * wsize), fp);
    fclose(fp2);

    free(outbuf);
    free(buf);
}
// 뒤집는거(상하반전)
void FlipImg(char *filename, char *output, int wsize, int hsize)
{
    int newY = 0;

    BYTE headbuf[1078]; // header size buf
    // buf �����Ҵ�
    BYTE *buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));
    // out buf ���� �Ҵ�
    BYTE *outbuf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

    // input file open
    FILE *fp = fopen(filename, "rb");
    fseek(fp, 1078, SEEK_SET);
    fread(buf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp);

    for (int i = 0; i < hsize; i++)
    {
        newY = hsize - i - 1;
        for (int j = 0; j < wsize; j++)
        {
            outbuf[newY * wsize + j] = buf[i * wsize + j];
        }
    }

    fp = fopen(filename, "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen(output, "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);
    fwrite(outbuf, sizeof(BYTE), (hsize * wsize), fp);
    fclose(fp2);

    free(outbuf);
    free(buf);
}

void MoveImg(char *filename, char *output, int wsize, int hsize, int xd, int yd)
{
    BYTE headbuf[1078]; // header size buf
    // buf �����Ҵ�
    BYTE *buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));
    // out buf ���� �Ҵ�
    BYTE *outbuf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

    // input file open
    FILE *fp = fopen(filename, "rb");
    fseek(fp, 1078, SEEK_SET);
    fread(buf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp);

    int newX = 0, newY = 0;

    for (int i = 0; i < hsize; i++)
    {
        newY = i + yd;
        if (newY < 0 || newY >= hsize)
        {
            continue;
        }
        for (int j = 0; j < wsize; j++)
        {
            newX = j + xd;
            if (newX < 0 || newX >= wsize)
            {
                continue;
            }
            outbuf[newY * wsize + newX] = buf[i * wsize + j];
        }
    }

    fp = fopen(filename, "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen(output, "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);
    fwrite(outbuf, sizeof(BYTE), (hsize * wsize), fp);
    fclose(fp2);

    free(outbuf);
    free(buf);
}

void NearestInterpolation(char *filename, char *output, double alpha, int wsize, int hsize)
{
    int tmpHeight = 0, tmpWidth = 0, addrIndex = 0;
    int newWidth = alpha * wsize;
    int newHeight = alpha * hsize;
    double stepX = (double)newHeight / (double)hsize;
    double stepY = (double)newWidth / (double)wsize;

    BYTE headbuf[1078]; // header size buf
    // buf �����Ҵ�
    BYTE *buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));
    // out buf ���� �Ҵ�
    BYTE *outbuf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));
    BYTE *midbuf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));
    int start_x, start_y;
    printf("start_x & start_y:\n");
    scanf("%d %d", &start_x, &start_y);
    // input file open
    FILE *fp = fopen(filename, "rb");
    fseek(fp, 1078, SEEK_SET);
    fread(buf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp);

    for (int i = 0; i < hsize; i++)
    {
        for (int j = 0; j < wsize; j++)
        {
            midbuf[i * wsize + j] = buf[(hsize - 1 - i) * wsize + j];
        }
    }

    if (alpha < 1)
    {
        for (int i = 0; i < newWidth; i++)
        {
            for (int j = 0; j < newHeight; j++)
            {
                tmpHeight = round(j / stepX);
                tmpWidth = round(i / stepY);
                addrIndex = (tmpWidth * hsize) + tmpHeight;
                outbuf[(i * hsize) + j] = buf[addrIndex];
            }
        }
    }

    else if (alpha > 1)
    {

        for (int i = 0; i < newWidth / alpha; i++)
        {
            for (int j = 0; j < newHeight / alpha; j++)
            {
                tmpHeight = round(j / stepX);
                tmpWidth = round(i / stepY);
                addrIndex = (tmpWidth * wsize) + tmpHeight + start_x + (wsize * start_y);
                outbuf[((wsize - 1 - i) * hsize) + j] = midbuf[addrIndex];
            }
        }
    }

    fp = fopen(filename, "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen(output, "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);
    fwrite(outbuf, sizeof(BYTE), (hsize * wsize), fp);
    fclose(fp2);

    free(outbuf);
    free(buf);
    free(midbuf);
}