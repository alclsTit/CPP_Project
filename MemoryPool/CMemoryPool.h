#pragma once
#include <list>
#include <memory>
#include "CObjectChunk.h"

class CMemoryPool
{
public:
	CMemoryPool() = default;
	virtual ~CMemoryPool() = default;

	template<typename T>
	T* Allocate();

	template<typename T>
	void Deallocate(T* data);

private:
	std::list<CObjectChunk*> m_free_list;
};

