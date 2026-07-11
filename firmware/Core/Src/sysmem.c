/**
 ******************************************************************************
 * @file    sysmem.c
 * @brief   Minimal newlib _sbrk() implementation for heap management.
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#include <errno.h>
#include <stdint.h>

extern uint8_t _end;         /* defined by the linker script */
extern uint8_t _estack;      /* defined by the linker script */
extern uint32_t _Min_Stack_Size; /* defined by the linker script */

static uint8_t *s_heap_end = NULL;

void *_sbrk(ptrdiff_t incr)
{
    extern uint8_t _end;
    static uint8_t *heap_end = NULL;
    uint8_t *prev_heap_end;

    if (heap_end == NULL)
    {
        heap_end = &_end;
    }

    prev_heap_end = heap_end;

    uint8_t *stack_limit = (uint8_t *)((uint32_t)&_estack - (uint32_t)&_Min_Stack_Size);

    if (heap_end + incr > stack_limit)
    {
        errno = ENOMEM;
        return (void *)-1;
    }

    heap_end += incr;
    (void)s_heap_end;
    return (void *)prev_heap_end;
}
