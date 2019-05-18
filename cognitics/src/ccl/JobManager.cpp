/****************************************************************************
Copyright (c) 2016 Cognitics, Inc.
****************************************************************************/

//#pragma optimize("", off)

#include "ccl/JobManager.h"
#include "ccl/Timer.h"
#include <ccl/inspection.h>

//#define VERBOSE_DEBUG

#ifdef VERBOSE_DEBUG
#include <Windows.h>
#include <psapi.h>
#pragma comment(lib,"psapi.lib")
static size_t ccl_JobManager_mem_initial = 0;
static size_t ccl_JobManager_mem_peak = 0;
size_t ccl_JobManager_mem(void)
{
    PROCESS_MEMORY_COUNTERS memCounter;
    BOOL result = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof( memCounter ));
    if(ccl_JobManager_mem_initial == 0)
        ccl_JobManager_mem_initial = memCounter.WorkingSetSize;
    ccl_JobManager_mem_peak = std::max<size_t>(ccl_JobManager_mem_peak, memCounter.WorkingSetSize);
    return memCounter.WorkingSetSize;
}
#define VERBOSE_JOB_LOG log << ccl::LDEBUG << "[" << __FUNCSIG__ << "] OWNER:" << owner << "  ARBITRARY:" << getArbitraryCounter() << "  MEMORY:" << ccl_JobManager_mem() << "  PEAK:" << ccl_JobManager_mem_peak << log.endl;
#define VERBOSE_JOBWORKER_LOG log << ccl::LDEBUG << "[" << __FUNCSIG__ << "] MEMORY:" << ccl_JobManager_mem() << "  PEAK:" << ccl_JobManager_mem_peak << log.endl;
#define VERBOSE_JOBMANAGER_LOG log << ccl::LDEBUG << "[" << __FUNCSIG__ << "] MEMORY:" << ccl_JobManager_mem() << "  PEAK:" << ccl_JobManager_mem_peak << log.endl;
#else
#define VERBOSE_JOB_LOG void(0);
#define VERBOSE_JOBWORKER_LOG void(0);
#define VERBOSE_JOBMANAGER_LOG void(0);
#endif

namespace ccl
{
    JobWorker::~JobWorker(void)
    {
        VERBOSE_JOBWORKER_LOG
    }

    JobWorker::JobWorker(JobManager *manager) : manager(manager)
    {
        log.init("JobWorker", this);
        VERBOSE_JOBWORKER_LOG
    }

    bool JobWorker::stop(void)
    {
        if(state != FINISHED)
            state = PENDING_STOP;
        return true;
    }

    int JobWorker::run(void)
    {
        if(state != PENDING_STOP)
            state = RUNNING;
        if(manager->threadDataManager)
        {
            if(!manager->threadDataManager->onThreadStarted())
                state = PENDING_STOP;
        }
        while(state == RUNNING)
        {
            Job *job = manager->getNextJob();
            if(job)
            {
                job->incrementTaskCount();
                job->execute();
                if(job->decrementTaskCount() <= 0)
                    finishJob(job);
            }
            else
            {
                ccl::sleep(50);
            }
        }
        if(manager->threadDataManager)
            manager->threadDataManager->onThreadFinished();
        state = FINISHED;
        return 1;
    }

    void JobWorker::finishJob(Job *job)
    {
        if(job->owner)
        {
            job->owner->onChildFinished(job, 0);
            if(job->owner->decrementTaskCount() <= 0)
                finishJob(job->owner);
        }
        manager->finishJob(this, job);
    }

    Job::~Job(void)
    {
        VERBOSE_JOB_LOG
    }

    int Job::arbitrary_counter = 0;
    ccl::mutex Job::arbitrary_counter_mutex;

    Job::Job(JobManager *manager, Job *owner) : manager(manager), owner(owner), task_count(0)
    {
        log.init("Job", this);
        VERBOSE_JOB_LOG
    }

    int Job::onChildFinished(Job *job, int result)
    {
        return 0;
    }

    int Job::incrementTaskCount(void)
    {
        task_count_mutex.lock();
        int result = ++task_count;
        task_count_mutex.unlock();
        return result;
    }

    int Job::decrementTaskCount(void)
    {
        task_count_mutex.lock();
        int result = --task_count;
        task_count_mutex.unlock();
        return result;
    }

    int Job::getTaskCount(void)
    {
        task_count_mutex.lock();
        int result = task_count;
        task_count_mutex.unlock();
        return result;
    }

    int Job::incrementArbitraryCounter(void)
    {
        arbitrary_counter_mutex.lock();
        int result = ++arbitrary_counter;
        arbitrary_counter_mutex.unlock();
        return result;
    }

    int Job::decrementArbitraryCounter(void)
    {
        arbitrary_counter_mutex.lock();
        int result = --arbitrary_counter;
        arbitrary_counter_mutex.unlock();
        return result;
    }

    int Job::getArbitraryCounter(void)
    {
        arbitrary_counter_mutex.lock();
        int result = arbitrary_counter;
        arbitrary_counter_mutex.unlock();
        return result;
    }

    ThreadDataManager::ThreadDataManager(void)
    {
    }

    ThreadDataManager::~ThreadDataManager(void)
    {
    }

    bool ThreadDataManager::onThreadStarted(void)
    {
        return true;
    }

    void ThreadDataManager::onThreadFinished(void)
    {
    }

    JobManager::~JobManager(void)
    {
        waitForCompletion();
        for(std::list<JobWorker *>::iterator it = workers.begin(), end = workers.end(); it != end; ++it)
            delete *it;
        VERBOSE_JOBMANAGER_LOG
    }

    JobManager::JobManager(size_t num_threads, ProgressObserver *progressObserver, ThreadDataManager *threadDataManager) : progressObserver(progressObserver), threadDataManager(threadDataManager)
    {
        log.init("JobManager", this);
        VERBOSE_JOBMANAGER_LOG
        for(size_t i = 0; i < num_threads; ++i)
        {
            JobWorker *worker = new JobWorker(this);
            worker->start();
            workers.push_back(worker);
        }
        for(size_t i = 0; i < num_threads * 4; ++i)
            workers_semaphore.signal();
    }

    Job *JobManager::getNextJob(void)
    {
        Job *job = NULL;
        open_mutex.lock();
        if(!pending_jobs.empty())
        {
            job = pending_jobs.front();
            pending_jobs.pop_front();
        }
        open_mutex.unlock();
        return job;
    }

    void JobManager::finishJob(JobWorker *worker, Job *job)
    {
        if(progressObserver)
            progressObserver->update();
        open_mutex.lock();
        open_jobs.erase(job);
        open_mutex.unlock();
        finished_mutex.lock();
        finished_jobs.push_back(job);
        finished_mutex.unlock();
        workers_semaphore.signal();
    }

    void JobManager::submitJob(Job *job, bool front)
    {
        open_mutex.lock();
        if (job->owner)
            job->owner->incrementTaskCount();
        open_jobs.insert(job);
        if(front)
            pending_jobs.push_front(job);
        else
            pending_jobs.push_back(job);
        open_mutex.unlock();
    }

    void JobManager::waitForSemaphore(int32_t msec)
    {
        workers_semaphore.wait(msec);
    }

    void JobManager::finish(void)
    {
        for(std::list<JobWorker *>::iterator it = workers.begin(), end = workers.end(); it != end; ++it)
            (*it)->stop();
    }

    bool JobManager::finished(void)
    {
        for(std::list<JobWorker *>::iterator it = workers.begin(), end = workers.end(); it != end; ++it)
        {
            if((*it)->getState() != ccl::Runnable::FINISHED)
            {
                return false;
                break;
            }
        }
        return true;
    }

    void JobManager::cleanup(void)
    {
        finished_mutex.lock();
        std::list<Job *> delete_list = finished_jobs;
        finished_jobs.clear();
        finished_mutex.unlock();
        while(!delete_list.empty())
        {
            if(delete_list.back()->owner)
                delete delete_list.back();
            delete_list.pop_back();
        }
        for(std::list<Job *>::iterator it = delete_list.begin(), end = delete_list.end(); it != end; ++it)
            if((*it)->owner)
                delete *it;
    }

    float JobManager::waitForCompletion(float seconds)
    {
        ccl::Timer timer;
        timer.startTimer();
        bool jobs_are_open = true;
        while(jobs_are_open)
        {
            cleanup();
            ccl::sleep(50);
            open_mutex.lock();
            jobs_are_open = !open_jobs.empty();
            open_mutex.unlock();
            float elapsed = timer.getElapsedTime();
            if(elapsed > seconds)
                return elapsed;
        }
        finish();
        while(!finished())
        {
            ccl::sleep(50);
            cleanup();
            float elapsed = timer.getElapsedTime();
            if(elapsed > seconds)
                return elapsed;
        }
        cleanup();
        return -FLT_MAX;
    }

}