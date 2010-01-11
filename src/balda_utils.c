#include "balda_utils.h"
#include <stdio.h>

const char* balda_itoa(balda_int_converter_t* converter, int value)
{
	sprintf(converter->buffer, "%d", value);
	return converter->buffer;
}

balda_point_t balda_make_point(int x, int y)
{
	balda_point_t p;
	p.x = x;
	p.y = y;
	return p;
}
