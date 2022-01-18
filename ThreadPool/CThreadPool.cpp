#include "CThreadPool.h"

namespace ThreadPool
{
	void CThreadPool::ThreadWork()
	{
		while (true)
		{
			std::unique_lock<std::mutex> cv_unique_lock(m_mutex_obj);
			m_cv.wait(cv_unique_lock, [this]() {return !m_jobs.empty() || m_all_stop; });

			if (m_jobs.empty() || m_all_stop)
			{
				return;
			}

			auto cur_job = m_jobs.front();
			m_jobs.pop();
			cv_unique_lock.unlock();

			cur_job();
		}
	}
}

