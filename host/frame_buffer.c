#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "mylib.h"
#include <string.h>
#include <assert.h>

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    uint16_t tft_fb[ SCREEN_HEIGHT ][ SCREEN_WIDTH ];

} monitor_t;

uint16_t *videoBuffer;
uint32_t buttons = ~( 0 );


static monitor_t monitor = { 0 };
monitor_t *m = &monitor;

int quit_filter(void *userdata, SDL_Event *event)
{
    (void) userdata;

    if( SDL_WINDOWEVENT == event->type )
    {
        if( SDL_WINDOWEVENT_CLOSE == event->window.event )
        {
            exit( 0 );
        }
    }
    else if( SDL_QUIT == event->type )
    {
        exit( 0 );
    }

    return 1;
}

volatile uint32_t* frame_buffer_init(void)
{
    buttons = ~( 0 );
    /* Initialise the SDL */
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialise! SDL_Error: %s\n", SDL_GetError() );
        exit( -1 );
    }

    SDL_SetEventFilter( quit_filter, NULL );

    m->window = SDL_CreateWindow( "Flappy Bird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  SCREEN_WIDTH*3, SCREEN_HEIGHT*3, 0 );
    assert( m->window );

    m->renderer = SDL_CreateRenderer( m->window, -1, SDL_RENDERER_SOFTWARE );
    assert( m->renderer );

    m->texture = SDL_CreateTexture( m->renderer, SDL_PIXELFORMAT_BGR555, SDL_TEXTUREACCESS_STATIC,
                                    SCREEN_WIDTH, SCREEN_HEIGHT );
    assert( m->texture );

    SDL_SetTextureBlendMode( m->texture, SDL_BLENDMODE_BLEND );

    videoBuffer = &m->tft_fb[0][0];

    return (void*) m->tft_fb;
}

int frame_buffer_switch(int offset)
{
    (void) offset;

    int rslt = SDL_UpdateTexture( m->texture, NULL, m->tft_fb, SCREEN_WIDTH * sizeof( uint16_t ));
    assert( 0 == rslt );
    rslt = SDL_RenderClear( m->renderer );
    assert( 0 == rslt );

    /* Update the renderer with the texture containing the rendered image */
    rslt = SDL_RenderCopy( m->renderer, m->texture, NULL, NULL );
    assert( 0 == rslt );

    SDL_RenderPresent( m->renderer );

    return 0;
}

void drawRect(int row, int col, int height, int width, unsigned short color)
{
    int endw = row + width;
    int endh = col + height;
    if( endw > SCREEN_WIDTH )
    {
        endw = SCREEN_WIDTH;
    }
    if( endh > SCREEN_HEIGHT )
    {
        endh = SCREEN_HEIGHT;
    }

    for( int h = col; h < endh; h++ )
    {
        for( int w = row; w < endw; w++ )
        {
            m->tft_fb[ h ][ w ] = color;
        }
    }
}

void background_color(int height, int width, unsigned short color)
{
    for( int h = 0; h < height; h++ )
    {
        for( int w = 0; w < width; w++ )
        {
            m->tft_fb[ h ][ w ] = color;
        }
    }
}

void drawFullscreenImage(const unsigned short *image)
{
    memcpy( m->tft_fb, image, sizeof( m->tft_fb ));
}

void fillinScreen(volatile unsigned short color)
{
    background_color( SCREEN_HEIGHT, SCREEN_WIDTH, color );
}

void drawImage3(int row, int col, int width, int height, const unsigned short *image)
{
    for( int c = 0; c < height; c++ )
    {
        for( int r = 0; r < width; r++ )
        {
            m->tft_fb[ col + c ][ row + r ] = *image;
            image++;
        }
    }
}
void undrawImage3(int row, int col, int width, int height, const uint16_t *image)
{
    for( int c = 0; c < height; ++c )
    {
        uint16_t *src = (uint16_t*)image;
        src += OFFSET( row, c + col, SCREEN_WIDTH );
        for( int r = 0; r < width; r++ )
        {
            m->tft_fb[ col + c ][ row + r ] = *src;
            src++;
        }
    }
}

bool print = false;
/**
 * draw a partial image
 *
 * @param r - x start position
 * @param c - y start position
 * @param rowOffset - offset in image to draw from
 * @param width - image x length
 * @param height - image column height
 * @param image - image
 */
void drawImage3FromRow(int r, int c, int rowOffset, int width, int height, const uint16_t *image)
{
    for( int cnt = 0; cnt < height; cnt++ )
    {
        for( int row = rowOffset, pos = 0; row < width; ++row, ++pos )
        {
            m->tft_fb[ c ][ r + pos ] = *( image + row );
        }
        image += width;
    }
}

void drawBackground(const uint16_t *image)
{
    drawImage3( 0,0, SCREEN_WIDTH, SCREEN_HEIGHT, image );
}

void drawHorizontal(int row, int col, int width, unsigned int color)
{
    int endw = row + width;
    if( endw > SCREEN_WIDTH )
    {
        endw = SCREEN_WIDTH;
    }
    for( int r = row; r < endw; r++ )
    {
        m->tft_fb[ col ][ r ] = color;
    }
}

void drawVertical(int row, int col, int height, int width, unsigned int color)
{
    int endw = row + width;
    int endh = col + height;
    if( endw > SCREEN_WIDTH )
    {
        endw = SCREEN_WIDTH;
    }
    if( endh > SCREEN_HEIGHT )
    {
        endh = SCREEN_HEIGHT;
    }

    for( int c = col; c < endh; c++ )
    {
        for( int r = row; r < endw; r++ )
        {
            m->tft_fb[ c ][ r ] = color;
        }
    }
}

uint32_t poll_controller(uint32_t delay)
{
    SDL_Event event;
    uint32_t mask = 0;
    int keyPressed = 0;

    while( SDL_PollEvent( &event ) )
    {
        switch( event.key.keysym.sym )
        {
        case SDLK_a :
            mask |= BUTTON_A;
            break;

        case SDLK_b :
            mask |= BUTTON_B;
            break;

        case SDLK_q :
            mask |= BUTTON_L;
            break;

        case SDLK_w :
            mask |= BUTTON_R;
            break;

        case SDLK_SPACE :
            mask |= BUTTON_SELECT;
            break;

        case SDLK_RETURN2 :
        case SDLK_RETURN :
            mask |= BUTTON_START;
            break;

        case SDLK_KP_0 :
        case SDLK_0 :
            break;

        case SDLK_RIGHT:
        case SDLK_KP_PLUS:
            mask |= BUTTON_RIGHT;
            break;

        case SDLK_LEFT:
        case SDLK_KP_MINUS:
            mask |= BUTTON_LEFT;
            break;

        case SDLK_UP:
            mask |= BUTTON_UP;
            break;

        case SDLK_DOWN:
            mask |= BUTTON_DOWN;
            break;

        case SDLK_ESCAPE:
            exit( 1 );
            break;
        }

        switch( event.type )
        {
        case SDL_KEYDOWN:
            keyPressed = 1;
            break;

        case SDL_KEYUP:
            keyPressed = 0;
            break;

        default:
            break;

        }
    }

    if( keyPressed )
    {
        buttons &= ~( mask );
    }
    else
    {
        buttons |= mask;
    }

    SDL_Delay( delay ); /* Sleep for 5 millisecond */

    return buttons;
}

uint32_t button_get( void )
{
    return buttons;
}

void waitForVBlank(void)
{
    poll_controller(16);
    frame_buffer_switch(0);
}

void delay(int n)
{
    SDL_Delay( n );
}
