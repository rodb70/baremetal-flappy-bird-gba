#ifndef HW09_MYLIB_H
#define HW09_MYLIB_H
#include <stdint.h>
#include "hardware.h"

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160

#define COLOR(r, g, b)  ((r) | (g)<<5 | (b)<<10)
#define BLACK   COLOR(0 , 0 , 0 )
#define WHITE   COLOR(31, 31, 31)
#define RED     COLOR(31, 0 , 0 )
#define GREEN   COLOR(0 , 31, 0 )
#define BLUE    COLOR(0 , 0 , 31)
#define YELLOW  COLOR(31, 31, 0 )
#define MAGENTA COLOR(31, 0 , 31)
#define CYAN    COLOR(0 , 31, 31)
#define ORANGE  COLOR(31, 15, 0 )
#define BROWN   COLOR(18, 9 , 0 )
#define PURPLE  COLOR(15, 0 , 15)
#define TEAL    COLOR(0 , 15, 15)
#define MAROON  COLOR(15, 0 , 0 )
#define GREY    COLOR(15, 15, 15)
#define PINK    COLOR(31, 18, 19)

#define OFFSET(r, c, rowlen) ((c)*(rowlen) + (r))

// Buttons

#define BUTTON_A      (1<<0)
#define BUTTON_B      (1<<1)
#define BUTTON_SELECT (1<<2)
#define BUTTON_START  (1<<3)
#define BUTTON_RIGHT  (1<<4)
#define BUTTON_LEFT   (1<<5)
#define BUTTON_UP     (1<<6)
#define BUTTON_DOWN   (1<<7)
#define BUTTON_R      (1<<8)
#define BUTTON_L      (1<<9)

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)

// Prototypes
void setPixel(int r, int c, unsigned short color);

void drawRect(int row, int col, int height, int width, unsigned short color);

void delay(int n);

void waitForVBlank(void);

void fillScreen(uint16_t color);

/* drawimage3
   * A function that will draw an arbitrary sized image
   * onto the screen (with DMA).
   * @param r row to draw the image
   * @param c column to draw the image
   * @param width width of the image
   * @param height height of the image
   * @param image Pointer to the first element of the image.
   */
void drawImage3(int r, int c, int width, int height, const uint16_t *image);

void undrawImage3(int row, int col, int width, int height, const uint16_t *image);

void drawBackground(const uint16_t *image);

void drawImage3FromRow(int r, int c, int colOffset, int width, int height, const uint16_t *image);

int calcStringWidth(char *string);

#endif //HW09_MYLIB_H
