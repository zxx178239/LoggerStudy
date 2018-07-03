#ifndef INOCOPY_H
#define INOCOPY_H

class INoCopy
{
private:
	INoCopy(const INoCopy &rhs);
	INoCopy& operator = (const INoCopy& rhs);

public:
	INoCopy(){}
	~INoCopy(){}
};

#endif