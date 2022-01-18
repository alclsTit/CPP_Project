#include "CObjectChunk.h"

void* CObjectChunk::Allocate()
{
	if (IsEmpty())
		return nullptr;

	std::scoped_lock range_lock(m_mutex_obj);
	uint8_t* cur_memory_slot = m_data_ptr[m_cur_pos];
	--m_cur_pos;

	return cur_memory_slot + sizeof(void*);
}

void CObjectChunk::Deallocate(uint8_t* ptr)
{
	if (IsFull())
		return;

	++m_cur_pos;

	std::scoped_lock range_lock(m_mutex_obj);
	m_data_ptr[m_cur_pos] = ptr;
}

bool CObjectChunk::IsEmpty() const noexcept
{
	return m_cur_pos <= 0;
}

bool CObjectChunk::IsFull() const noexcept
{
	return m_cur_pos >= m_total_count;
}