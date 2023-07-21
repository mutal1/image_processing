#include <stdio.h>
#include <stdlib.h>

#define hsize 672
#define wsize 1200
typedef unsigned char BYTE;

void mirriung()
{
    BYTE headbuf[1078];

    BYTE *buf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

    BYTE *outbuf = (BYTE *)malloc(sizeof(char) * (hsize * wsize));

    FILE *fp = fopen("Frog.bmp", "rb");
    fseek(fp, 1078, SEEK_SET);
    fread(buf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp);

    for (int i = 0; i < hsize; i++)
    {
        for (int j = 0; j < wsize; j++)
        {
            outbuf[i * wsize + wsize - j - 1] = buf[i * wsize + j];
        }
    }

    fp = fopen("Frog.bmp", "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen("mirruring.bmp", "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);
    fwrite(outbuf, sizeof(BYTE), (hsize * wsize), fp);
    fclose(fp2);

    free(outbuf);
    free(buf);
}

int main()
{
    mirriung();
}