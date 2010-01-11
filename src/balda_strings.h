#ifndef __BALDA_STRINGS__H__
#define __BALDA_STRINGS__H__

typedef enum
{
	#define BALDA_STRING(name, value) name,
	#include "balda_strings.inl"
	#undef BALDA_STRING
} BALDA_STRING_ID;

const char* balda_string(BALDA_STRING_ID);

#define BALDA_STRING_DEFINE_MACROS
#define BALDA_STRING(name, value)
#include "balda_strings.inl"
#undef BALDA_STRING_DEFINE_MACROS
#undef BALDA_STRING

#endif
