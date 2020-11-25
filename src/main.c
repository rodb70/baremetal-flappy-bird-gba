#include "mylib.h"
#include <stdlib.h>
#include <stdio.h>
#include "text.h"
#include "bird.h"
#include "startScreen.h"
#include "pipeNeckBottom.h"
#include "pipeNeckTop.h"
#include "pipeBody.h"
#include "gameoverScreen.h"
#include "background.h"

typedef struct
{
    int showing;
    int row;
    int topHeight;
    int gapHeight;
} PIPE;

typedef struct
{
    int row;
    int col;
} BIRD;

// State enum definition
enum GBAState
{
    START, START_NO_DRAW, PLAY, GAME_OVER, GAME_OVER_NO_DRAW
};

#define PIPE_MARGIN 1

#define DELAY_TIME 1
#define FLY_HEIGHT 5
#define GRAVITY 1
#define PIPE_SPEED 1
#define PIPE_DISTANCE 100

#define NUM_PIPES 5

void reset(void);

void reGeneratePipes(void);

void enablePipe(PIPE *pipe);

void generatePipeHeight(PIPE *pipe);

void drawBird(void);

void undrawBird(const uint16_t *image);

void drawPipe(PIPE *pipe);

void undrawPipeRear(PIPE *pipe, const uint16_t *image);

void drawPipes(void);

void undrawPipes(void);

void applyGravity(void);

void movePipes(void);

void flyLess(void);

int detectCollision(BIRD *bird1, PIPE *pipe);

int checkAlive(void);

void drawBackground(const uint16_t *image);

volatile int score = 0;

BIRD ourBird;


PIPE *pipes;
PIPE *currentPipe;
PIPE *nextPipe;

PIPE* MallocPipe( size_t size )
{
    static PIPE pipeStore[ 3 ][ NUM_PIPES ] = {{{0}}};
    static size_t ps_idx = 0;

    (void)size;
    PIPE *pipe = &pipeStore[ ps_idx ][ 0 ];
    ps_idx++;
    if( ps_idx >= 3 )
    {
        ps_idx = 0;
    }

    return pipe;
}

// main function, containing the main loop
int main(void)
{
    hardware_video_init();

    enum GBAState state = START;

    // tokens to keep track of keys
    int startDownLastFrame = 0;
    int upDownLastFrame = 0;
    int selectDownLastFrame = 0;
    (void)selectDownLastFrame;

    // main loop
    while( 1 )
    {
        waitForVBlank();
        switch( state )
        {
        case START:
            reset();
            state = START_NO_DRAW;
            drawBackground( startScreen );
            drawBird();
            drawPipes();
            drawString(( SCREEN_WIDTH - calcStringWidth( "Press UP to start" )) / 2, 50, "Press UP to start",
                         WHITE );
            break;

        case START_NO_DRAW:
            if( KEY_DOWN_NOW(BUTTON_UP) && !upDownLastFrame )
            {
                drawBackground( background );
                state = PLAY;
            }
            break;

        case PLAY:
            undrawBird( background );
            undrawPipes();
            movePipes();
            applyGravity();
            if( KEY_DOWN_NOW( BUTTON_UP ) )
            {
                if( upDownLastFrame )
                {
                    if( ourBird.col > FLY_HEIGHT - 2 )
                    {
                        ourBird.col -= FLY_HEIGHT - 2;
                    }
                }
                else
                {
                    flyLess();
                }
            }

            if( !checkAlive() && !KEY_DOWN_NOW( BUTTON_A ) )
            {
                state = GAME_OVER;
                break;
            }

            drawBird();
            drawPipes();
            break;

        case GAME_OVER:
            drawBackground( gameoverScreen );
            char string[ 5 ];
            sprintf( string, "%d", score );
            drawString( 150, 68, string, WHITE );
            drawString(( SCREEN_WIDTH - calcStringWidth( "Press START to restart" )) / 2, 150,
                        "Press START to restart",
                        WHITE );
            state = GAME_OVER_NO_DRAW;
            break;

        case GAME_OVER_NO_DRAW:
            if( KEY_DOWN_NOW(BUTTON_START) && !startDownLastFrame )
            {
                state = START;
            }
            break;
        }

        if( KEY_DOWN_NOW( BUTTON_START ) )
        {
            startDownLastFrame = 1;
        }
        else
        {
            startDownLastFrame = 0;
        }
        if( KEY_DOWN_NOW( BUTTON_UP ) )
        {
            upDownLastFrame = 1;
        }
        else
        {
            upDownLastFrame = 0;
        }
        if( KEY_DOWN_NOW( BUTTON_SELECT ) )
        {
            selectDownLastFrame = 1;
        }
        else
        {
            selectDownLastFrame = 0;
        }
    }
}

// reset the status of the game
void reset(void)
{
    pipes = MallocPipe( sizeof(PIPE) * NUM_PIPES );
    currentPipe = pipes;

    reGeneratePipes();

    ourBird.row = SCREEN_WIDTH / 6;
    ourBird.col = ( SCREEN_HEIGHT - BIRD_HEIGHT ) / 2;

    score = 0;
}

// initiate pipe data
void reGeneratePipes(void)
{
    enablePipe( pipes );
    for( int i = 1; i < NUM_PIPES; ++i )
    {
        pipes[ i ].showing = 0;
        generatePipeHeight( pipes + i );
        pipes[ i ].row = pipes[ 0 ].row + i * PIPE_DISTANCE;
    }
}

// make a pipe visible
void enablePipe(PIPE *pipe)
{
    pipe->showing = 1;
    pipe->row = SCREEN_WIDTH - PIPENECKBOTTOM_WIDTH - 1;
    generatePipeHeight( pipe );
}

// detect collision between bird and pipe
int detectCollision(BIRD *bird1, PIPE *pipe)
{
    if( bird1->col > (pipe->topHeight - 1) && (bird1->col + BIRD_HEIGHT) < pipe->topHeight + pipe->gapHeight )
    {
        return 0;
    }
    else
    {
        if( (bird1->row + BIRD_WIDTH) < pipe->row || bird1->row > pipe->row + PIPENECKBOTTOM_WIDTH )
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}

// check if we survived
int checkAlive(void)
{
    return !detectCollision( &ourBird, currentPipe ) && ourBird.row < SCREEN_HEIGHT;
}

// randomize a pipe
void generatePipeHeight(PIPE *pipe)
{
    int baseHeight = PIPENECKBOTTOM_HEIGHT + PIPENECKBOTTOM_HEIGHT;
    pipe->gapHeight = (BIRD_HEIGHT * 3) + rand() % (BIRD_HEIGHT * 4);
    pipe->topHeight = ( rand() % (SCREEN_HEIGHT - pipe->gapHeight - baseHeight)) + (baseHeight / 2);
}

// draw the bird
void drawBird(void)
{
    drawImage3( ourBird.row, ourBird.col, BIRD_WIDTH, BIRD_HEIGHT, bird );
}

// undraw the bird
void undrawBird(const uint16_t *image)
{
    undrawImage3( ourBird.row, ourBird.col, BIRD_WIDTH, BIRD_HEIGHT, image );
}

void drawPipes(void)
{
    for( int i = 0; i < NUM_PIPES; ++i )
    {
        drawPipe( pipes + i );
    }
}

// draw one pipe, behaves differently when near edges
void drawPipe(PIPE *pipe)
{
    if( !pipe->showing )
    {
        return;
    }

    if( pipe->row < 0 )
    {
        for( int i = 0; i < pipe->topHeight - PIPENECKBOTTOM_HEIGHT; ++i )
        {
            drawImage3FromRow( 0,
                               i,
                               -pipe->row - PIPE_MARGIN,
                               PIPEBODY_WIDTH,
                               PIPEBODY_HEIGHT,
                               pipeBody );
        }

        drawImage3FromRow( 0,
                           pipe->topHeight - PIPENECKBOTTOM_HEIGHT,
                           -pipe->row,
                           PIPENECKBOTTOM_WIDTH,
                           PIPENECKBOTTOM_HEIGHT,
                           pipeNeckTop );

        drawImage3FromRow( 0,
                           pipe->topHeight + pipe->gapHeight,
                           -pipe->row,
                           PIPENECKBOTTOM_WIDTH,
                           PIPENECKBOTTOM_HEIGHT,
                           pipeNeckBottom );
        for( int i = 0; i < SCREEN_HEIGHT - (pipe->topHeight + pipe->gapHeight + PIPENECKBOTTOM_HEIGHT); ++i )
        {
            drawImage3FromRow( 0,
                               i + pipe->topHeight + pipe->gapHeight + PIPENECKBOTTOM_HEIGHT,
                               -pipe->row - PIPE_MARGIN,
                               PIPEBODY_WIDTH,
                               PIPEBODY_HEIGHT,
                               pipeBody );
        }
    }
    else
    {
        for( int i = 0; i < pipe->topHeight - PIPENECKBOTTOM_HEIGHT; ++i )
        {
            drawImage3( pipe->row + PIPE_MARGIN, i, PIPEBODY_WIDTH, PIPEBODY_HEIGHT, pipeBody );
        }
        drawImage3( pipe->row, pipe->topHeight - PIPENECKBOTTOM_HEIGHT, PIPENECKBOTTOM_WIDTH, PIPENECKBOTTOM_HEIGHT, pipeNeckTop );
        drawImage3( pipe->row, pipe->topHeight + pipe->gapHeight, PIPENECKBOTTOM_WIDTH, PIPENECKBOTTOM_HEIGHT,
                    pipeNeckBottom );
        for( int i = 0; i < SCREEN_HEIGHT - (pipe->topHeight + pipe->gapHeight + PIPENECKBOTTOM_HEIGHT); ++i )
        {
            drawImage3( pipe->row + PIPE_MARGIN,
                        i + pipe->topHeight + pipe->gapHeight + PIPENECKBOTTOM_HEIGHT,
                        PIPEBODY_WIDTH, PIPEBODY_HEIGHT, pipeBody );
        }
    }

}

// undraw all pipes
void undrawPipes(void)
{
    for( int i = 0; i < NUM_PIPES; ++i )
    {
        undrawPipeRear( pipes + i, background );
    }
}

// undraw the rear part of a pipe
void undrawPipeRear(PIPE *pipe, const uint16_t *image)
{
    if( !pipe->showing )
    {
        return;
    }

    for( int i = 0; i < pipe->topHeight - PIPENECKBOTTOM_HEIGHT; ++i )
    {
        undrawImage3( pipe->row + PIPE_MARGIN + PIPEBODY_WIDTH - 1,
                      i,
                      PIPE_SPEED,
                      PIPEBODY_HEIGHT,
                      image );
    }

    undrawImage3( pipe->row + PIPENECKBOTTOM_WIDTH - 1,
                  pipe->topHeight - PIPENECKBOTTOM_HEIGHT,
                  PIPE_SPEED,
                  PIPENECKBOTTOM_HEIGHT,
                  image );

    undrawImage3( pipe->row + PIPENECKBOTTOM_WIDTH - 1,
                  pipe->topHeight + pipe->gapHeight,
                  PIPE_SPEED,
                  PIPENECKBOTTOM_HEIGHT,
                  image );

    for( int i = 0; i < SCREEN_HEIGHT - (pipe->topHeight + pipe->gapHeight + PIPENECKBOTTOM_HEIGHT); ++i )
    {
        undrawImage3( pipe->row + PIPE_MARGIN + PIPEBODY_WIDTH - 1,
                      i + pipe->topHeight + pipe->gapHeight + PIPENECKBOTTOM_HEIGHT,
                      PIPE_SPEED,
                      PIPEBODY_HEIGHT,
                      image );
    }
}

void applyGravity(void)
{
    if(ourBird.col < SCREEN_HEIGHT - BIRD_HEIGHT )
    {
        ourBird.col += GRAVITY;
    }
}

void flyLess(void)
{
    if(ourBird.col < SCREEN_HEIGHT - BIRD_HEIGHT )
    {
        ourBird.col -= FLY_HEIGHT - 3;
    }
}

// moves the pipes as game proceeds
void movePipes(void)
{
    if( nextPipe != NULL && currentPipe->row + PIPENECKBOTTOM_WIDTH < 0 )
    {
        pipes = nextPipe;
        currentPipe = nextPipe;
        reGeneratePipes();
        nextPipe = NULL;
    }
    for( int i = 0; i < NUM_PIPES; ++i )
    {
        pipes[ i ].row -= PIPE_SPEED;
        if( pipes[ i ].row < ( SCREEN_WIDTH - PIPENECKBOTTOM_WIDTH ) &&
           ( pipes[ i ].row + PIPENECKBOTTOM_WIDTH ) > 0 )
        {
            pipes[ i ].showing = 1;
        }
        else
        {
            pipes[ i ].showing = 0;
        }
        if( pipes[ i ].row <= ourBird.row + BIRD_WIDTH )
        {
            if( i == NUM_PIPES - 1 )
            {
                nextPipe = MallocPipe( sizeof(PIPE) * NUM_PIPES );
            }
            else
            {
                nextPipe = NULL;
            }
            if( currentPipe != pipes + i )
            {
                score++;
            }
            currentPipe = pipes + i;
        }
    }

}

