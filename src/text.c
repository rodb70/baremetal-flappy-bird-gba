#include "mylib.h"
#include "text.h"

void drawChar(int row, int col, char ch, uint16_t color)
{
    for( int c = 0; c < 8; c++ )
    {
        for( int r = 0; r < 6; r++ )
        {
            if( fontdata_6x8[ OFFSET(r, c, 6) + ch * 48 ])
            {
                setPixel( row + r, col + c, color );
            }
        }
    }
}

void drawString(int row, int col, char *str, uint16_t color)
{
    while( *str )
    {
        drawChar( row, col, *str++, color );
        row += 6;
    }
}

