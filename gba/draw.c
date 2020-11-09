/*
 * draw.c
 *
 *  Created on: 9 Nov 2020
 *      Author: rod
 */
#include "mylib.h"
#include "hardware.h"

unsigned short *videoBuffer = (unsigned short*) 0x6000000;

void delay(int n)
{
    volatile int x = 0;
    for( int i = 0; i < n * 100; i++ )
    {
        x++;
    }
}

void drawRect(int row, int col, int height, int width, unsigned short color)
{
    for( int r = 0; r < height; r++ )
    {
        DMA[3].src = &color;
        DMA[3].dst = &videoBuffer[OFFSET( row + r, col, 240 )];
        DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
    }
}

void waitForVBlank()
{
    while( SCANLINECOUNTER > 160 )
        ;
    while( SCANLINECOUNTER < 160 )
        ;
}

void fillScreen(volatile uint16_t color)
{
    DMA[3].src = &color;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = 38400 | DMA_ON | DMA_SOURCE_FIXED;
}

void drawImage3(int r, int c, int width, int height, const uint16_t *image)
{
    for( int row = 0; row < height; ++row )
    {
        DMA[DMA_CHANNEL_3].src = image + OFFSET( row, 0, width );
        DMA[DMA_CHANNEL_3].dst = videoBuffer + OFFSET( r + row, c, SCREEN_WIDTH );
        DMA[DMA_CHANNEL_3].cnt = width |
        DMA_SOURCE_INCREMENT |
        DMA_DESTINATION_INCREMENT |
        DMA_ON;
    }
}

void undrawImage3(int row, int col, int width, int height, const uint16_t *image)
{
    for( int r = 0; r < height; ++r )
    {
        DMA[ DMA_CHANNEL_3 ].src = image + OFFSET( row + r, col, SCREEN_WIDTH );
        DMA[ DMA_CHANNEL_3 ].dst = videoBuffer + OFFSET( row + r, col, SCREEN_WIDTH );
        DMA[ DMA_CHANNEL_3 ].cnt = width | DMA_SOURCE_INCREMENT |
                                   DMA_DESTINATION_INCREMENT |
                                   DMA_ON;
    }
}

void drawBackground(const uint16_t *image)
{
    DMA[DMA_CHANNEL_3].src = image;
    DMA[DMA_CHANNEL_3].dst = videoBuffer;
    DMA[DMA_CHANNEL_3].cnt = 38400 |
    DMA_SOURCE_INCREMENT |
    DMA_DESTINATION_INCREMENT |
    DMA_ON;
}

void drawImage3FromRow(int r, int c, int rowOffset, int width, int height, const uint16_t *image)
{
    for( int row = 0; row < height; ++row )
    {
        DMA[DMA_CHANNEL_3].src = image + OFFSET( row, rowOffset, width );
        DMA[DMA_CHANNEL_3].dst = videoBuffer + OFFSET( r + row, c, 240 );
        DMA[DMA_CHANNEL_3].cnt = (width - rowOffset) |
        DMA_SOURCE_INCREMENT |
        DMA_DESTINATION_INCREMENT |
        DMA_ON;
    }
}
