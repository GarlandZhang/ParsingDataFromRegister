#include <io.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "ParsingRegisterData2.h"
#include <sstream>
#include <vector>

typedef unsigned __int32 uint32_t;

#define CHAR_SIZE 10

class FieldValue {
	public:
		std::string name;
		uint32_t bitValue;
};

class BitField {
	public:
		std::string name;
		std::string fieldRange;
		std::string stateOfMutability;
		std::vector<FieldValue> fieldValues;
};

class Register {
	public:
		std::string name;
		uint32_t address;
		std::string stateOfMutability;
		uint32_t resetValue;
		std::vector<BitField> bitFields;
};

std::string getWordAt(int pos, std::string line) {
	std::stringstream iss(line);
	std::string word;
	while (pos >= 0 && iss >> word) {
		pos--;
	}

	return word;
}

Register getRegisterWithAddress(std::vector<Register>& registers, uint32_t offset) {
	for (Register r : registers) {
		//compare address to offset
		if (r.address == offset) {
			return r;
		}
	}
	exit;
}

uint32_t getBit(uint32_t inputValue, int index) {
	return (inputValue >> index) & 1;
}

uint32_t getBits(uint32_t inputValue, int start, int end) {

	uint32_t shifted = inputValue >> start;

	int length = end - start + 1;

	return ((shifted >> length) << length) ^ shifted;
}

int main()
{
	//get file 
	std::ifstream inFile;
	inFile.open("registerData.txt");

	if (!inFile) {
		std::cout << "Unable to open file";
		exit(1);
	}

	std::vector<Register> registers;
	std::string line;
	int setValue = 0;
	
	//pointer variables
	while (std::getline(inFile, line)) {

		if (line.size() == 0) {
			break;
		}

		std::istringstream iss(line);
		std::string word;
		
		if (getWordAt(1, line).compare("ADDRESS") == 0) {

			//create register
			Register newRegister;
			newRegister.name = getWordAt(0, line);
			newRegister.address = std::stoul(getWordAt(2, line), nullptr, 0); //not getting address
			newRegister.stateOfMutability = getWordAt(3, line);

			//std::cout << "Name: " << newRegister.name << " | Address: " << newRegister.address << std::endl;

			registers.push_back(newRegister);

			//set val to take in reset val for next input
			setValue = 1;
		}
		else if (setValue == 1) {
			Register *RTemp = &registers.back();

			RTemp->resetValue = std::stoul(getWordAt(2, line), nullptr, 0);
			/*
			std::cout << "Name: " << registers.back().name << std::endl;
			std::cout << "RTemp | Reset value: " << RTemp->resetValue << std::endl;
			std::cout << "From registers | Reset value: " << registers.back().resetValue << std::endl;*/

			//set value to take in bitfields for next input
			setValue = 2;
		}
		else if (setValue == 2) {
			
			if (getWordAt(1, line).compare("VALUE") == 0) {
				FieldValue fieldValue;
				fieldValue.name = getWordAt(0, line);
				fieldValue.bitValue = std::stoul(getWordAt(2, line), nullptr, 0);

				Register *RTemp = &registers.back();
				BitField *BTemp = &RTemp->bitFields.back();
				BTemp->fieldValues.push_back(fieldValue);
			}
			else
			{
				BitField bitField;
				bitField.name = getWordAt(0, line);

				std::string fieldRangeStr = getWordAt(1, line);

				bitField.fieldRange = fieldRangeStr.substr(4, fieldRangeStr.find("]") - 4);
				bitField.stateOfMutability = getWordAt(2, line);

				Register *RTemp = &registers.back();
				RTemp->bitFields.push_back(bitField);

			}
		}
	}

	//output data
	/*for (Register r : registers) {
		std::cout << "Name: " << r.name << std::endl;
		std::cout << "Address: " << r.address << std::endl;
		std::cout << "Reset Value: " << r.resetValue << std::endl;
		std::cout << "Bitfield size: " << r.bitFields.size() << std::endl;
		for (BitField b : r.bitFields) {
			std::cout << "Bitfield name: " << b.name << std::endl;
			std::cout << "Value of bitfield: " << b.fieldRange << std::endl;
			std::cout << "Field size: " << b.fieldValues.size() << std::endl;
			for (FieldValue f : b.fieldValues) {
				std::cout << "FieldValue name: " << f.name << std::endl;
				std::cout << "Value of fieldvalue: " << f.bitValue << std::endl;
			}
		}
		std::cout << std::endl;
	}*/

	inFile.close();

	const std::string SEPARATOR = "========================================================";

	//DEMO
	std::cout << SEPARATOR << std::endl;
	std::cout << "Example demonstration on how to use: " << std::endl;
	std::cout << "Input offset : 0x0154" << std::endl;
	std::cout << "Input value : 0x14372456" << std::endl;
	std::cout << SEPARATOR << std::endl;
	std::cout << "Register Name: REGISTERNAME#1" << std::endl;
	std::cout << "\tBitField Name: BITFIELDNAME#1 | Bit Index: 0 | Value: 0x0" << std::endl;
	std::cout << "\tBitField Name: BITFIELDNAME#2 | Bit Index: 1 | Value: 0x1" << std::endl;
	std::cout << SEPARATOR << "\n" << std::endl;
	std::cout << "NOW YOU TRY!!" << std::endl;
	//DEMO

	std::cout << SEPARATOR << std::endl;

	while (1) {
		//get input
		//offset -> this will be address
		std::cout << "Input offset: ";
		uint32_t offset;
		scanf_s("%x", &offset);

		//value
		std::cout << "Input value: ";
		uint32_t inputVal;
		scanf_s("%x", &inputVal);

		std::cout << SEPARATOR << std::endl;

		//Get register corresponding to offset (address)
		Register reg = getRegisterWithAddress(registers, offset);

		std::cout << "Register Name: " << reg.name << std::endl;

		//Get bit fields
		for (int i = reg.bitFields.size() - 1; i >= 0; i--) {
			BitField bitField = reg.bitFields.at(i);
			std::string fieldRange = bitField.fieldRange;
			uint32_t state = 0;

			std::cout << "\tBitField Name: " << bitField.name;

			if (fieldRange.find(":") == -1) {
				//just one bit value
				int index = std::stoi(fieldRange);
				state = getBit(inputVal, index);
				std::cout << " | Bit Index: " << index;
			}
			else {
				int end = std::stoi(fieldRange.substr(0, fieldRange.find(":")));
				int start = std::stoi(fieldRange.substr(fieldRange.find(":") + 1));
				state = getBits(inputVal, start, end);
				std::cout << " | Bit Index: " << start << " to " << end;
			}

			if (bitField.fieldValues.size() == 0) {
				std::cout << " | Value: " << std::hex << "0x" << state << std::endl;
			}
			else {
				bool isFound = false;
				for (FieldValue fieldValue : bitField.fieldValues) {
					if (fieldValue.bitValue == state) {
						isFound = true;
						std::cout << std::endl << "\t\t Field Name: " << fieldValue.name << " | Bit Value: " << std::hex << "0x" << fieldValue.bitValue << std::endl;
						break;
					}
				}
			}
		}

		std::cout << SEPARATOR << std::endl;
	}

	return 0;
}

