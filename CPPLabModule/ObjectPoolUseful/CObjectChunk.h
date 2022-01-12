#pragma once
#include <limits>
#include <atomic>
#include <mutex>

/// <summary>
/// ObjSize = 메모리풀에서 관리할 객체 사이즈 / T = 메모리풀 Chunk 하나당 연관된 배열 타입
/// [[Chunk][Chunk].....[Chunk] - MemoryPool
///   array  array ..... array
/// </summary>
template <size_t ObjSize>
class CObjectChunk
{
private:
	static constexpr uint8_t ms_max_alloc_size = std::numeric_limits<uint8_t>::max();

private:
	uint8_t* m_data = nullptr;
	std::atomic<uint8_t> m_current_pos{ 0 };
	std::atomic<uint8_t> m_count{ ms_max_alloc_size };
	std::mutex m_mutex_obj;

public:
	CObjectChunk() : m_count(ms_max_alloc_size), m_current_pos(0)
	{
		m_data = new uint8_t[ObjSize * ms_max_alloc_size];

		for (auto idx = 0; idx < ms_max_alloc_size; ++idx)
			m_data[idx] = idx + 1;
	}
	virtual ~CObjectChunk() noexcept
	{
		std::scoped_lock range_lock(m_mutex_obj);
		delete[] m_data;
	}
	CObjectChunk(const CObjectChunk& lhs) = delete;
	CObjectChunk& operator=(const CObjectChunk& lhs) = delete;
	CObjectChunk(CObjectChunk&& rhs) noexcept : m_data(rhs.m_data), m_count(rhs.m_count), m_current_pos(rhs.m_current_pos) 
	{
		rhs.m_data = nullptr;	// 안해주면 memory leak
	} 
	CObjectChunk& operator=(CObjectChunk&& rhs) noexcept
	{
		this->~CObjectChunk();
		new(this) CObjectChunk(std::move(rhs));
		return *this;
	}

	virtual void* Allocate();
	virtual void Deallocate(void *ptr);
	virtual bool Contains(void* ptr) noexcept;

	virtual bool IsEmpty() noexcept;
	virtual bool IsFree() noexcept;
};


// 처음 메모리블록 생성 or 모든 메모리블록을 다 사용하여 재할당이 필요한 경우 사용
template<size_t ObjSize>
inline void* CObjectChunk<ObjSize>::Allocate()
{
	if (this->IsEmpty())
		return nullptr;

	--m_count;

	std::scoped_lock range_lock(m_mutex_obj);
	void* ptr = m_data + m_current_pos * ObjSize;
	m_current_pos = m_data[m_current_pos * ObjSize];

	return ptr;
}

template<size_t ObjSize>
inline void CObjectChunk<ObjSize>::Deallocate(void* ptr)
{
	if (!Contains(ptr))
		return;

	++m_count;

	std::scoped_lock range_lock(m_mutex_obj);
	auto target_ptr = reinterpret_cast<uint8_t*>(ptr);
	*target_ptr = m_current_pos;
	m_current_pos = (target_ptr - m_data) / ObjSize;
}

template<size_t ObjSize>
bool CObjectChunk<ObjSize>::Contains(void* ptr) noexcept
{
	std::scoped_lock range_lock(m_mutex_obj);
	auto target_ptr = reinterpret_cast<uint8_t*>(ptr);
	return m_data <= target_ptr && target_ptr < m_data + ObjSize * ms_max_alloc_size;
}


template<size_t ObjSize>
bool CObjectChunk<ObjSize>::IsEmpty() noexcept
{
	uint8_t comparer = 0;
	uint8_t desire = 0;
	return m_count.compare_exchange_weak(comparer, desire);
}

template<size_t ObjSize>
bool CObjectChunk<ObjSize>::IsFree() noexcept
{
	uint8_t max_alloc_size = ms_max_alloc_size;
	return m_count.compare_exchange_weak(max_alloc_size, max_alloc_size);
}
