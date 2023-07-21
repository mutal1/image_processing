#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef unsigned char BYTE;

BYTE clampping(int conv_x, int conv_y)
{
    int result;
    result = (int)sqrt((conv_x * conv_x) + (conv_y * conv_y));
    result = (result > 255) ? 255 : result;
    return (BYTE)result;
}


int main()
{
    //header size
    BYTE headbuf[1078]; // header size buf
    //buf 동적 할당
    BYTE *buf = (BYTE *)malloc(sizeof(char) * (480 * 640));
    // out buf 동적 할당
    BYTE *outbuf = (BYTE *)malloc(sizeof(char) * (480 * 640));
    int wsize = 640;
    int hsize = 480;
    int mheight = 3;
    int mwidth = 3;
    int threshold1 = 78;
    // input file open
    FILE *fp = fopen("city.bmp", "rb");
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
            outbuf[wsize * i + j] = clampping(convsum_x, convsum_y);
            convsum_x = 0;
            convsum_y = 0;
        }
    }

    fp = fopen("city.bmp", "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen("edge_city.bmp", "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);
    fwrite(outbuf, sizeof(BYTE), (hsize * wsize), fp);
    fclose(fp2);

    free(buf);
    free(outbuf);
}
