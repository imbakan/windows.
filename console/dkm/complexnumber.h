
#pragma once

class CComplexNumber
{
private:

public:
	double x, y;

	CComplexNumber();
	CComplexNumber(double x, double y);
	~CComplexNumber();

	void operator =(CComplexNumber c);

	CComplexNumber operator -(CComplexNumber c);
	CComplexNumber operator +(CComplexNumber c);
	CComplexNumber operator *(CComplexNumber c);
	CComplexNumber operator /(CComplexNumber c);

	void operator +=(CComplexNumber c);
	void operator *=(CComplexNumber c);

	bool operator ==(CComplexNumber c);
	bool operator <(double v);
};
