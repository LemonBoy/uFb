#ifndef UFB_H
#define UFB_H

void
init_console (int width, int height,
    void (* draw_fn) (unsigned int, unsigned int, int));

void
print_char (unsigned char ch);

void
print_string (char *str);

#endif
