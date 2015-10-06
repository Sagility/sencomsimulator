// CombinedJobController.h

#pragma once
#include "config.h"
#include <deque>
#include <algorithm>
#include "Job.h"
#include "SimEngineTimer.h"

using namespace std;

namespace EnergySim {
    class ENERGYSIM_DLL_PUBLIC ICombinedJobController:public IJob
    {
	protected:
		long _id;
		IJob* _current_job;
		int _num_remaining_jobs;
		string _name;
	public:
		ICombinedJobController(SimContext *context):IJob(context){};
        long ID() const {return _id; }
		void ID(long theID) { _id = theID; }

        /// <summary>
        ///  Adds a job to end of the queue of jobs to be processed by the ICombinedJobController
        /// </summary>
        /// <param name="job">the job</param>
        virtual void AddJob(IJob* job)=0;

        /// <summary>
        ///  Adds a job to beginning of the queue of jobs to be processed by the ICombinedJobController
        /// </summary>
        /// <param name="job">the job</param>
        virtual void InsertJob(IJob* job)=0;

        /// <summary>
        ///  Removes a job from the queue of jobs so that it is no longer processed by the ICombinedJobController
        /// </summary>
        /// <param name="job">the job</param>
        virtual void RemoveJob(IJob* job)=0;

        /// <summary>
        /// Removes the next job from the queue of jobs and returns it
        /// </summary>
        /// <returns>the first job on the queue</returns>
        virtual IJob* NextJob()=0;

        /// <summary>
        /// Clears all jobs
        /// </summary>
        virtual void Reset()=0;


        /// <summary>
        /// If the given job is on the queue it will be removed, if the job is already busy it will be preempted
        /// </summary>
        /// <param name="job">the given job, if null then the current job will be preeempted</param>
        virtual void PreemptJob(IPreemptableJob* job)=0;

        /// <summary>
        /// returns the currently active job (job being processed)
        /// </summary>
		IJob* CurrentJob() const { return _current_job;};


        /// <summary>
        /// returns the number of remaining jobs on the queue (does not include the job currenly being processed)
        /// </summary>
		int num_remaining_jobs()const{ return _num_remaining_jobs;}

        /// <summary>
        /// returns the nth job on the queue
        /// </summary>
        /// <param name="index">n should be between 0 and  number of remaining jobs -1</param>
        /// <returns>the nth job</returns>
        virtual IJob* GetNthJob(int index)=0;
        // 
        // returns diganostics on current job and remaining jobs in queue
        virtual string Diagnostics()=0;

    };
	class IRouteFollower;
	class ENERGYSIM_DLL_PUBLIC CombinedJobController : public ICombinedJobController, public IJobStartedListener, public IJobFinishedListener, public TimerElapsedListener
	{
	protected:
		static long _last_id;	
		deque<IJob*> _jobqueue;
		bool _started;
		bool _keepalive;
		double _delay_before_forced_preempt;
		IJob* _job2Preempt;
		IEnvironment *_env;
		IRouteFollower* itsFollower;

	public:
		
		/// constructor
		CombinedJobController(SimContext *ctx);
		/// constructor with name for diagnostic pusrposes
		CombinedJobController(SimContext *ctx,string name);

		CombinedJobController(SimContext *ctx, string name, IRouteFollower * theFollower);
		///destructor
		virtual ~CombinedJobController();

		IRouteFollower* follower() { return itsFollower; }

		virtual void Execute();
		virtual void Start();
		virtual void ProcessJobs();
        /// <summary>
        ///  Adds a job to end of the queue of jobs to be processed by the ICombinedJobController
        /// </summary>
        /// <param name="job">the job</param>
        virtual void AddJob(IJob* job);

        /// <summary>
        ///  Adds a job to beginning of the queue of jobs to be processed by the ICombinedJobController
        /// </summary>
        /// <param name="job">the job</param>
        virtual void InsertJob(IJob* job);

        /// <summary>
        ///  Removes a job from the queue of jobs so that it is no longer processed by the ICombinedJobController
        /// </summary>
        /// <param name="job">the job</param>
        virtual void RemoveJob(IJob* job);

        /// <summary>
        /// Removes the next job from the queue of jobs and returns it
        /// </summary>
        /// <returns>the first job on the queue</returns>
        virtual IJob* NextJob();

        /// <summary>
        /// Clears all jobs
        /// </summary>
        virtual void Reset();


        /// <summary>
        /// If the given job is on the queue it will be removed, if the job is already busy it will be preempted
        /// </summary>
        /// <param name="job">the given job, if null then the current job will be preeempted</param>
        virtual void PreemptJob(IPreemptableJob* job);
        /// <summary>
        /// returns the nth job on the queue
        /// </summary>
        /// <param name="index">n should be between 0 and  number of remaining jobs -1</param>
        /// <returns>the nth job</returns>
        virtual IJob* GetNthJob(int index);
        // 
        // returns diganostics on current job and remaining jobs in queue
        virtual string Diagnostics();
		virtual void FinishCurrentJob();

		virtual void OnJobFinished(IJob *theJob, EventArgs *theArgs);
		virtual void OnJobStarted(IJob *theJob, EventArgs *theArgs);
		virtual void OnElapsed(ITimer *theTimer, EventArgs *theArgs);
		
		bool keep_alive() const { return _keepalive;}
		void keep_alive(bool keepalive)  {  _keepalive=keepalive;}
	    double delay_before_forced_preempt() const { return _delay_before_forced_preempt;}
		void delay_before_forced_preempt(bool delay_before_forced_preempt)  {  _delay_before_forced_preempt=delay_before_forced_preempt;}
		bool started() const {return _started;};
		virtual string classname() { return "CombinedJobController";}
	};
}