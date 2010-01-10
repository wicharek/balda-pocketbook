#include "balda_controller.h"
#include "balda.h"
#include <inkview.h>

int main(int argc, char **argv)
{
	balda_t* balda = balda_init();
	balda_controller_init(balda);
	
	InkViewMain(balda_controller_handler);
	
	balda_free(balda);
	return 0;
}
