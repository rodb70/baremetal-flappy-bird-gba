#include <string.h>
#include "mylib.h"

extern unsigned short *videoBuffer;

void setPixel(int r, int c, unsigned short color)
{
    videoBuffer[OFFSET( r, c, 240 )] = color;
}

int calcStringWidth(char *string)
{
    return (int) ((strlen( string ) - 1) * 6);
}

