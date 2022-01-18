#include "CThreadPool.h"
#include <iostream>

int TestFunc(int a, int b)
{
	static int init_num = 1000;
	init_num += a;

	std::cout << "TestFunc = " << init_num << " , " << b << std::endl;

	return init_num + b;
}

int main()
{
	ThreadPool::CThreadPool myThreadPool(2);

	std::vector<std::future<int>> futures;
	for (auto idx = 0; idx < 10; ++idx)
	{
		//futures.emplace_back(myThreadPool.Enqueue(TestFunc, idx, idx * 10));
	}

}