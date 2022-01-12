#include "ObjectPoolUseful/CMemoryPool.h"
#include "ObjectPoolUseful/CTestObject.h"
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>

void MemoryPoolPerformCheck(int count)
{
	using Pool = CMemoryPool<CTestObject>;

	std::vector<CTestObject*> tmpStore;
	for (auto idx = 0; idx < count; ++idx)
		tmpStore.emplace_back(CMemoryPool<CTestObject>::Allocate());

	for (auto idx = 0; idx < count; ++idx)
		Pool::Deallocate(tmpStore[idx]);
}

int main()
{
	using namespace std;

	int counter = 10000;
	int thread_count = 2;

	auto start_time = chrono::high_resolution_clock::now();

	vector<thread> local_threads;
	for (auto idx = 0; idx < thread_count; ++idx)
		local_threads.emplace_back([&]() {MemoryPoolPerformCheck(counter); });

	for (auto& thread : local_threads)
		thread.join();

	auto diff_time = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start_time);

	cout << "Element Count = " << counter << endl;
	cout << "Thread [" << thread_count << "]  MemoryPool performance [time] = " << diff_time.count() << "mills" << endl;

	//MemoryPoolPerformCheck(counter);
}