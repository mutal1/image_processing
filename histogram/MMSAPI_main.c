#include "API.h"

int main()
{
    char rawfile[30];
    char filename[30];
    int mwidth, mheight;
    DWORD wsize;
    DWORD hsize;

    printf("Input filename:");
    scanf("%s", rawfile);
    printf("input size (width):");
    scanf("%u", &wsize);
    printf("input size (height):");
    scanf("%u", &hsize);

    system("cls");
    char input;
    printf("---------------------------------------------------------\n");
    printf("1.raw2bmp\n");
    printf("\n");
    printf("2.file histogram\n");
    printf("\n");
    printf("3.histogram EQ\n");
    printf("\n");
    printf("4.convolution\n");
    printf("---------------------------------------------------------\n");
    printf("\n");
    printf("Select number:");
    scanf(" %c",&input);
    printf("\n");

    switch (input) 
    {
    case '1':
        printf("Input bmp file.bmp:");
        scanf("%s", filename);
        Grayraw2bmp(rawfile, filename, wsize, hsize);
        break;
    case '2':
        printf("Input histogram graph file:");
        scanf("%s", filename);

        histogram(rawfile, filename, wsize, hsize);
        
        break;
    case '3':
        printf("Input EQ file:");
        scanf("%s", filename);
        
        histogramEQ(rawfile, filename, wsize, hsize);
        
        break;
    case '4':
        printf("Input convolution file:");
        scanf("%s", filename);
        printf("Input filter size(width):");
        scanf("%d",&mwidth);
        printf("Input filter size(height):");
        scanf("%d",&mheight);
        
        convolution(rawfile,filename,wsize,hsize,mwidth,mheight);
        
        break;
    }

    return 0;
}