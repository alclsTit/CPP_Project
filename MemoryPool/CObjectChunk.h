#pragma once
#include <limits>
#include <atomic>
#include <mutex>
#include <vector>

class CObjectChunk
{
private:
	std::atomic<size_t> m_cur_pos;
	std::atomic<size_t> m_total_count;
	std::mutex m_mutex_obj;

	uint8_t* m_data = nullptr;
	std::vector<uint8_t*> m_data_ptr;
	
public:
	CObjectChunk(size_t obj_size, size_t count) : m_cur_pos(count), m_total_count(count)
	{
		auto total_size = obj_size * count;
		m_data = new uint8_t[total_size];

		for (auto idx = 0; idx < count; ++idx)
		{
			m_data_ptr[idx] = m_data + idx * obj_size;
		}
	}
	virtual ~CObjectChunk() noexcept
	{
		m_data = nullptr;
	}
	CObjectChunk(const CObjectChunk& lhs) = delete;
	CObjectChunk& operator=(const CObjectChunk& lhs) = delete;
	CObjectChunk(CObjectChunk&& rhs) = delete;
	CObjectChunk& operator=(CObjectChunk&& rhs) = delete;

	virtual void* Allocate();
	virtual void Deallocate(uint8_t*ptr);

	virtual bool IsEmpty() const noexcept;
	virtual bool IsFull() const noexcept;
};
