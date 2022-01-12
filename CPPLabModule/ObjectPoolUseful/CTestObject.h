#pragma once
#include <string>

class CTestObject
{
private:
	int m_user_count;
	int m_serial;
public:
	CTestObject() = default;
	CTestObject(int count = 0, int serial = 0) : m_user_count(count), m_serial(serial) {}
	virtual ~CTestObject() = default;
};

