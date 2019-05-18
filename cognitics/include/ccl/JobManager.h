/****************************************************************************
Copyright (c) 2016 Cognitics, Inc.
****************************************************************************/
#pragma once

#include "ccl/Runnable.h"
#include "ccl/mutex.h"
#include "ccl/sem.h"
#include "ccl/ProgressObserver.h"
#include "ccl/ObjLog.h"
#include <list>
#include <set>
#include <deque>
#include <algorithm>

/*
The JobManager maintains a set of worker threads.
Each worker thread picks up pending jobs in the queue and executes them.

A job has an owner and can create sub-jobs.

A job that has an owner is cleaned up by the JobManager.
A job that does not have an owner is the responsibility of the application.
This design allows the application to gather data from the top-level jobs when they have completed.

The return code from execute() is used to determine what list the job goes into.
If it is 0 or negative, the job is moved to the finished list.
If it is positive, the job remains on the open list.

When in the waiting list, a job may receive onChildFinished(), which has similar return code handling:
If is is 0 or negative, the job is moved to the finished list.
If it is positive, the job remains on the open list.

*/


namespace ccl
{
    class JobManager;
    class Job;

    class JobWorker : public Runnable
    {
        friend class JobManager;

    private:
        JobManager *manager;
        ccl::ObjLog log;

        void finishJob(Job *job);

    public:
        virtual ~JobWorker(void);
        JobWorker(JobManager *manager);
        virtual bool stop(void);
        virtual int run(void);
    };

    class Job
    {
        friend class JobManager;
        friend class JobWorker;

    private:
        int task_count;
        ccl::mutex task_count_mutex;

    protected:
        ccl::ObjLog log;
        JobManager *manager;
        Job *owner;

        // debugging tool
        static int arbitrary_counter;
        static ccl::mutex arbitrary_counter_mutex;
        static int incrementArbitraryCounter(void);
        static int decrementArbitraryCounter(void);
        static int getArbitraryCounter(void);


        int incrementTaskCount(void);
        int decrementTaskCount(void);
        int getTaskCount(void);

        virtual int onChildFinished(Job *job, int result);

    public:
        virtual ~Job(void);
        Job(JobManager *manager, Job *owner = NULL);

        // execute the job; return 0 for complete, negative for errors, positive to wait for child jobs
        virtual int execute(void) = 0;
    };

    // manages thread data and cleans it up when a thread terminates
    class ThreadDataManager
    {
    public:
        ThreadDataManager(void);
        virtual ~ThreadDataManager(void);

        virtual bool onThreadStarted(void);
        virtual void onThreadFinished(void);
    };

    class JobManager
    {
        friend class JobWorker;

    private:
        ccl::ObjLog log;

        ProgressObserver *progressObserver;

        ccl::semaphore workers_semaphore;
        std::list<JobWorker *> workers;

        ccl::mutex open_mutex;
        std::deque<Job *> pending_jobs;        // pending jobs include all jobs that are waiting to be executed
        std::set<Job *> open_jobs;            // open jobs include all jobs that are pending or running
        ccl::mutex finished_mutex;
        std::list<Job *> finished_jobs;        // finished jobs include all jobs that are complete and waiting for cleanup

        ThreadDataManager *threadDataManager;

        // used by workers to fetch a job from pending_jobs in a thread-safe manner
        Job *getNextJob(void);

        // used by workers to set a job to finished status
        void finishJob(JobWorker *worker, Job *job);

    public:
        // destructor ensures that all threads have been completed
        ~JobManager(void);

        JobManager(size_t num_threads, ProgressObserver *progressObserver = NULL, ThreadDataManager *threadDataManager = NULL);

        // submit a job (asynchronously)
        void submitJob(Job *job, bool front = true);

        // wait for an idle worker
        void waitForSemaphore(int32_t msec = 0xFFFFFFFF);

        // stop all worker threads
        void finish(void);

        // returns true when all threads are finished
        bool finished(void);

        // delete finished jobs that are owned
        // unowned jobs are the responsibility of the application (this allows the application to pull the data it needs)
        void cleanup(void);

        // wait for all jobs to finish
        // returns time elapsed; if all jobs are finished, it will return negative
        float waitForCompletion(float seconds = FLT_MAX);



    };



}