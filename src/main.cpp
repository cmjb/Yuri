#include <iostream>
//#include <cstdio>
//#include <stdlib.h>
#include "include/yuri.h"

int listen_port = 0;

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		cout << "Usage: yuri -p {listen_port} [-h {hostname}]" << endl;
		return 0;
	}

	if((int)atoi(argv[2]) >= 65535)
	{
		cout << "ERROR: Port range is beyond or is at end of applicable range.";
		return 0;
	}

	listen_port = (int)atoi(argv[2]);

	if(!listen_port)
	{
		cout << "ERROR: Cannot convert PORT to int." << endl;
		return 0;
	}

	cout << "Loading Yuri's Modules..." << endl;
	Yuri yuri = Yuri();
	yuri.listen(listen_port);
	return 0;
}