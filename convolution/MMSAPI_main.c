#include "API.h"

int main()
{
    char rawfile[30];
    char filename[30];
    int mwidth, mheight;
    int xd, yd;
    double alpha;

    BYTE threshold;
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
    printf("\n");
    printf("5.Edge Detection\n");
    printf("\n");
    printf("6.lotation\n");
    printf("\n");
    printf("7.median\n");
    printf("\n");
    printf("8.avg\n");
    printf("\n");
     printf("9.Move\n");
    printf("\n");
    printf("a.Interpolation\n");
    printf("\n");
    printf("b.Binarization\n");
    printf("\n");
    printf("c.MirrorImg\n");
    printf("\n");
    printf("d.FlipImg\n");
    printf("\n");
    printf("---------------------------------------------------------\n");
    printf("\n");
    printf("Select number:");
    scanf(" %c", &input);
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
        scanf("%d", &mwidth);
        printf("Input filter size(height):");
        scanf("%d", &mheight);

        convolution(rawfile, filename, wsize, hsize, mwidth, mheight);

        break;
    case '5':
        printf("Input edge file:");
        scanf("%s", filename);

        edge_detection(rawfile, filename, wsize, hsize);

        break;
    case '6':
        printf("Input lotation file:");
        scanf("%s", filename);

        lotation(rawfile, filename, wsize, hsize);

        break;

    case '7':
        printf("Input median file:");
        scanf("%s", filename);

        median(rawfile, filename, wsize, hsize);

        break;

    case '8':
        printf("Input avg file:");
        scanf("%s", filename);

        avg(rawfile, filename, wsize, hsize);

        break;

        case '9':
        printf("Input Move file:");
        scanf("%s", filename);
        printf("Move size(width):");
        scanf("%d", &xd);
        printf("Move size(height):");
        scanf("%d", &yd);


        MoveImg(rawfile, filename, wsize, hsize, xd, yd);

        break;
    case 'a':
        printf("Input scale file:");
        scanf("%s", filename);
        printf("alpha size:");
        scanf("%lf", &alpha);

        NearestInterpolation(rawfile, filename, alpha, wsize, hsize);

        break;

    case 'b':
        printf("Input Move file:");
        scanf("%s", filename);
        printf("threshold:");
        scanf("%hhd", &threshold);

        Binarization(rawfile, filename, wsize, hsize, threshold);

        break;

    case 'c':
        printf("Input mirror file:");
        scanf("%s", filename);

        MirrorImg(rawfile, filename, wsize, hsize);

        break;

    case 'd':
        printf("Input flipimg file:");
        scanf("%s", filename);       

        FlipImg(rawfile, filename, wsize, hsize);

        break;

    }

    return 0;
}