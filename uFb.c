#include "uFb.h"

#if !defined CONFIG_FONT_HEIGHT || !defined CONFIG_FONT_WIDTH
#error "You must define the font size"
#endif

#if !defined CONFIG_FONT_ARRAY
#error "You must define the font array"
#endif

#ifndef CONFIG_FONT_RANGE_START
#define CONFIG_FONT_RANGE_START 0
#endif

#ifndef CONFIG_FONT_RANGE_END
#define CONFIG_FONT_RANGE_END   0xFF
#endif

/* Version string */
const static char * ufb_version = "0.4";

typedef struct Console
{
    int      width, height;
    int      x, y;
    void   (*draw_fn) (unsigned int, unsigned int, int);   
} Console;

static Console console_ctx;

/* Checks if the letter is included into the font range */
inline char
get_printable (char ch)
{
    if (ch >= CONFIG_FONT_RANGE_START &&
        ch <= CONFIG_FONT_RANGE_END)
    {
        return ch - CONFIG_FONT_RANGE_START;
    }
    
    return ' ';
}

/* Prints a char on the console */
void
print_char (unsigned char ch)
{
    int l, c;
    
    if (console_ctx.x >= console_ctx.width / CONFIG_FONT_WIDTH)
    {
        console_ctx.y++;
        console_ctx.x = 0;
    }
    
    if (console_ctx.y >= console_ctx.height / CONFIG_FONT_HEIGHT)
    {
        console_ctx.y = 0;
    }

    switch (ch)
    {
        case '\n':
            console_ctx.x  = 0;
            console_ctx.y += 1;
        break;

        case '\t':
            console_ctx.x += 2;
        break;

        default:
            ch = get_printable(ch);
            
            for (l = 0; l < CONFIG_FONT_HEIGHT; l++)
            {
                for (c = 0; c < CONFIG_FONT_WIDTH; c++)
                {
                    if ((CONFIG_FONT_ARRAY[ch][l] >> (7 - c)) & 1)
                    {
                        console_ctx.draw_fn(console_ctx.x * CONFIG_FONT_WIDTH + c,
                            console_ctx.y * CONFIG_FONT_HEIGHT + l, 1);
                    } else {
                        console_ctx.draw_fn(console_ctx.x * CONFIG_FONT_WIDTH + c,
                            console_ctx.y * CONFIG_FONT_HEIGHT + l, 0);
                    }
                }
            }
            
            console_ctx.x++;
        break;
    }
}

/* Prints a string on the console. Just a wrapper :| */
void
print_string (char *str)
{
    while (*str)
        print_char(*str++);
}

/* Initialize the console with given params */
void
init_console (int width, int height,
    void (* draw_fn) (unsigned int, unsigned int, int))
{
    console_ctx.width = width;
    console_ctx.height = height;
    console_ctx.x = 0;
    console_ctx.y = 0;
    console_ctx.draw_fn = draw_fn;
}
