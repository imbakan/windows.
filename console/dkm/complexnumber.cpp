
#include "complexnumber.h"

CComplexNumber::CComplexNumber()
{
	x = y = 0.0;
}

CComplexNumber::CComplexNumber(double x, double y)
{
	this->x = x;
	this->y = y;
}

CComplexNumber::~CComplexNumber()
{

}

void CComplexNumber::operator =(CComplexNumber c)
{
	x = c.x;
	y = c.y;
}

CComplexNumber CComplexNumber::operator -(CComplexNumber c)
{
	return CComplexNumber(x - c.x, y - c.y);
}

CComplexNumber CComplexNumber::operator +(CComplexNumber c)
{
	return CComplexNumber(x + c.x, y + c.y);
}

// (a + bi)(c + di)   = (ac - bd) + (bc + ad)i
CComplexNumber CComplexNumber::operator *(CComplexNumber c)
{
	double sx, sy;

	sx = x * c.x - y * c.y;
	sy = y * c.x + x * c.y;

	return CComplexNumber(sx, sy);
}

CComplexNumber CComplexNumber::operator /(CComplexNumber c)
{
	CComplexNumber a(x, y), b(c.x, c.y), d(c.x, -c.y);
	CComplexNumber e, f, g;

	e = a * d;
	f = b * d;

	return CComplexNumber(e.x / f.x, e.y / f.x);
}

void CComplexNumber::operator +=(CComplexNumber c)
{
	x += c.x ;
	y += c.y;
}

void CComplexNumber::operator *=(CComplexNumber c)
{
	CComplexNumber a, b;

	b = CComplexNumber(x, y);
	a = b * c;

	x = a.x;
	y = a.y;
}

bool CComplexNumber::operator ==(CComplexNumber c)
{
	return (x == c.x && y == c.y);
}

bool CComplexNumber::operator <(double v)
{
	return (x < v && y < v);
}
