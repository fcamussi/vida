
#include "window.h"



int main(int argc, char *argv[])
{
	MainWindow mainwindow;
	
	gtk_init(&argc, &argv);

	Create_Main_Window(&mainwindow);

	gtk_main();

	return 0;
}
