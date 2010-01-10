#ifndef __BALDA_UTILS__H__
#define __BALDA_UTILS__H__

typedef struct
{
	char buffer[33];
} balda_int_converter_t;

const char* balda_itoa(balda_int_converter_t*, int);

typedef struct
{
	int x, y;
} balda_point_t;

#endif
