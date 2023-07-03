#ifndef _FLAGPROCS_D
#define _FLAGPROCS_D

#include "markdown.h"

void mkd_set_flag_num(mkd_flag_t *p, unsigned long bit);
void mkd_clr_flag_num(mkd_flag_t *p, unsigned long bit);
void mkd_set_flag_bitmap(mkd_flag_t *p, long bits);

#endif