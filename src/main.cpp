#include <string>
#include <iostream>
#include <stdio.h>
#include "header_info.h"

using namespace std;

int main(int argc, char** argv)
{
	if(argc < 2){
		cerr << *(argv+0) << " <path_header> [1:overwrite cpp]" << endl;
		return 1;
	}
	const char* path = *(argv+1);

	bool force = false;
	if(argc > 3){
		force = (atoi(*(argv+2)) == 1);
	}

	header_info hi;
	hi.parse(path);
	hi.make_cpp(force);

	return 0;
}
