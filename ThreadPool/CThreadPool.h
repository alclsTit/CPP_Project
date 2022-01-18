#pragma once
#include <vector>
#include <condition_variable>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <memory>
#include <mutex>

// https://modoocode.com/285 ÂüÁ¶

namespace ThreadPool
{
	class CThreadPool
	{
	public:
		CThreadPool() = default;
		CThreadPool(size_t thread_num, bool is_all_stop = false) : m_thread_num(thread_num), m_all_stop(is_all_stop)
		{
			m_thread_pool.reserve(thread_num);
			for (auto idx = 0; idx < thread_num; ++idx)
			{
				m_thread_pool.emplace_back(new std::thread([this]() { ThreadWork(); }));
			}
		}
		virtual ~CThreadPool()
		{
			m_all_stop = true;
			m_cv.notify_all();

			for (auto& thread : m_thread_pool)
				thread.join();

		}

		void ThreadWork();

		template <class Callable, class... Args>
		std::function<typename std::invoke_result<Callable(Args...)>::type> Enqueue(Callable&& func, Args&&... args);


	private:
		std::vector<std::thread> m_thread_pool;
		std::queue<std::function<void()>> m_jobs;
		size_t m_thread_num{ 0 };
		bool m_all_stop{ false };

		std::condition_variable m_cv;
		std::mutex m_mutex_obj;
	};

	template<class Callable, class ...Args>
	inline std::function<typename std::invoke_result<Callable(Args...)>::type> CThreadPool::Enqueue(Callable&& func, Args && ...args)
	{
		if (m_all_stop)
			throw new std::runtime_error("All thread stopped");

		using return_type = std::function<typename std::invoke_result<Callable(Args...)>::type>;
		auto job = std::make_shared<std::packaged_task<return_type()>>(std::bind(func, args...));
		std::future<return_type> job_result = job->get_future();
		{
			std::scoped_lock<std::mutex> range_lock(m_mutex_obj);
			m_jobs.push([&]() {(*job_result)(); });

		}

		m_cv.notify_one();

		return job_result;
	}

}