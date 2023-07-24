
#pragma once

const char OPEN_PARENTHESIS = '(';
const char CLOSE_PARENTHESIS = ')';

const short OPERATOR = 1;
const short OPERAND = 2;

const int LEFT_TO_RIGHT = 3;
const int RIGHT_TO_LEFT = 4;

union datatype {
	unsigned char ch[8];
	double dbl;
};

class CItem
{
private:
	char type;
	union datatype data;

	char GetPrecedence(char ch);
	char GetAssociativity(char ch);

public:
	CItem();
	CItem(double dbl);
	~CItem();

	bool operator ==(char ch);

	bool operator ==(short sh);

	bool operator ==(int i);

	bool operator ==(CItem item);
	bool operator <(CItem item);
	bool operator >(CItem item);

	void operator =(char ch);
	void operator =(double dbl);
	void operator =(CItem item);
	CItem operator +(CItem item);
	CItem operator -(CItem item);
	CItem operator *(CItem item);
	CItem operator /(CItem item);
	CItem operator ^(CItem item);

	operator char();
	operator char* ();
	operator double();
};

