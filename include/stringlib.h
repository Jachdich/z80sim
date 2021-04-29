#ifndef __STRINGLIB_H
#define __STRINGLIB_H
#include <stddef.h>
char **split(const char *s, char delimiter);
char *trim(char *s);
void strrep(char *buf, char src, char dest);
size_t arrlen(char **arr);
#endif