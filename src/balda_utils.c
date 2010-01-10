#include "balda_utils.h"
#include <stdio.h>

const char* balda_itoa(balda_int_converter* converter, int value)
{
	sprintf(converter->buffer, "%d", value);
	return converter->buffer;
}
