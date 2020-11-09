#ifndef HOST_HARDWARE_H_
#define HOST_HARDWARE_H_
#include <stdbool.h>

extern uint16_t *videoBuffer;
uint32_t button_get( void );

#define BUTTONS button_get()

static inline void hardware_video_init( void )
{
    static bool inited = false;
    extern volatile uint32_t* frame_buffer_init(void);

    if( false == inited )
    {
        (void)frame_buffer_init();
        inited = true;
    }
}

void waitForVblank(void);

#endif /* HOST_HARDWARE_H_ */
