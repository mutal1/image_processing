#include "bmpfile.h"

int main()
{
	Grayraw2bmp("raw_lena.raw", "bmp_lena.bmp", 256, 256);
		return 0;
}
