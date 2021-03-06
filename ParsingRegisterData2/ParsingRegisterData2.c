#include <io.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

typedef unsigned __int32 uint32_t;

#define CHAR_SIZE 10

class Register {
	uint32_t address;
};

int main()
{
	//get file 
	std::ifstream inFile;
	inFile.open("registerData.txt");

	if (!inFile) {
		std::cout << "Unable to open file";
		exit(1);
	}

	const char spacing[1] = " ";

	std::string line;
	while (std::getline(inFile, line)) {
		char *token;
		token = strtok(line, spacing);

		while (token != NULL) {
			std::cout << token << std::endl;
			token = strtok(NULL, spacing);
		}
	}

	inFile.close();

	//get input
		//offset
		printf("Input offset: ");
		uint32_t offset;
		scanf_s("%x", &offset);
	
		//value
		printf("Input value: ");
		uint32_t val;
		scanf_s("%x", &val);

		//TEMPORARY: check to see if outputs are correct
		printf("Offset: %x \n", offset);
		printf("Value: %x", val);


    return 0;
}

