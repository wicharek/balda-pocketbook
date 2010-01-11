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

balda_point_t balda_make_point(int x, int y);

#define BALDA_SQR(x) ((x)*(x))
#define BALDA_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define BALDA_MAX(x, y) (((x) > (y)) ? (x) : (y))

#define balda_points_equal(p1, p2) (((p1).x == (p2).x) && ((p1).y == (p2).y))

#endif
