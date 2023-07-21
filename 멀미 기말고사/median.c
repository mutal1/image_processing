#include <stdio.h>
#include <stdlib.h>

#define hsize 672
#define wsize 1200
#define col 8
#define row 8
typedef unsigned char BYTE;

BYTE sort(BYTE sort_arr[])
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

void median()
{
    BYTE headbuf[1078]; // header size buf

    BYTE out[hsize][wsize];
    BYTE buf[hsize][wsize];
    BYTE mask[col][row];
    BYTE pixel;

    FILE *fp = fopen("Frog.bmp", "rb");
    fseek(fp, 1078, SEEK_SET);
    fread(buf, sizeof(BYTE), wsize * hsize, fp);
    fclose(fp);

    for (int i = 0; i < hsize; i++)
    {
        for (int j = 0; j < wsize; j++)
        {
            out[i][j] = 0;
        }
    }

    for (int i = 0; i < hsize; i += col)
    {
        for (int j = 0; j < wsize; j += row)
        {
            for (int k = 0; k < col; k++)
            {
                for (int p = 0; p < row; p++)
                {
                    mask[k][p] = buf[i + k][j + p];
                }
            }
            pixel = sort(mask);
            for (int k = 0; k < col; k++)
            {
                for (int p = 0; p < row; p++)
                {
                    out[i + k][j + p] = pixel;
                }
            }
        }
    }

    fp = fopen("Frog.bmp", "rb");
    fread(headbuf, sizeof(BYTE), 1078, fp);
    fclose(fp);

    FILE *fp2 = fopen("median.bmp", "wb");
    fwrite(headbuf, sizeof(BYTE), 1078, fp2);
    fwrite(out, sizeof(BYTE), (hsize * wsize), fp);
    fclose(fp2);
}

int main()
{
    median();
}