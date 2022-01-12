#pragma once
#include "CObjectChunk.h"
#include <list>
#include <memory>
#include <mutex>

template <typename T>
class CMemoryPool
{
private:
	static auto& GetChunks()
	{
		static std::list<CObjectChunk<sizeof(T)>> chunks;
		return chunks;
	}

private:
	static inline std::mutex m_mutex_obj;

public:
	CMemoryPool() = default;
	virtual ~CMemoryPool() = default;

	static T* Allocate();
	static void Deallocate(T* data);
};

template<typename T>
inline T* CMemoryPool<T>::Allocate()
{
	std::scoped_lock<std::mutex> range_lock(m_mutex_obj);
	auto& local_chunks = CMemoryPool::GetChunks();

	for (auto& target : local_chunks)
	{
		if (!target.IsEmpty())
			return reinterpret_cast<T*>(target.Allocate());
	}

	local_chunks.emplace_back();
	return reinterpret_cast<T*>(local_chunks.back().Allocate());

	//ToDo:할당된 Chunk 공간을 모두 사용하였을 때 처리 
	//std::cout << "Chunk size full..." << std::endl;
	//return nullptr;
}

template<typename T>
inline void CMemoryPool<T>::Deallocate(T* data)
{
	std::scoped_lock<std::mutex> range_lock(m_mutex_obj);
	auto& local_chunks = GetChunks();

	for (auto iter = local_chunks.begin(); iter != local_chunks.end(); ++iter)
	{
		auto& chunk = *iter;
		if (chunk.Contains(data))
		{
			chunk.Deallocate(data);
			if (chunk.IsFree())
				local_chunks.erase(iter);

			return;
		}
	}
}
