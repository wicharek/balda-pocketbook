#include "balda_strings.h"

#define BALDA_STRING(name, value) const char* name##_ru = value;
#include "balda_strings.inl"
#undef BALDA_STRING

const char* balda_empty_string = "";

const char* balda_string(BALDA_STRING_ID id)
{
	switch (id)
	{
		#define BALDA_STRING(name, value) \
			case name: \
			return name##_ru;
		#include "balda_strings.inl"
		#undef BALDA_STRING
	
		default:
			return balda_empty_string;
	}
}
