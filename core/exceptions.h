#pragma once
class Exceptions 
{
public:
	static const int RESET = 0;
	static const int BUS_ERROR = 2;
	static const int ADDRESS_ERROR = 3;
	static const int ILLEGAL_INSTRUCTION = 4;
	static const int DIVISION_BY_ZERO = 5;
	static const int CHK = 6;
	static const int TRAPV = 7;
	static const int PRIVILEGE_VIOLATION = 8;
	static const int TRACE = 9;
	static const int LINE_1010 = 10;
	static const int LINE_1111 = 11;
	static const int TRAP = 32;
};