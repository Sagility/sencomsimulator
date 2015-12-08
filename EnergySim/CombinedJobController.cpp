#include "stdafx.h"
#include <sstream>
#include "EnergySim.h"
#include "SimEngine.h"
#include "SimLogEnvironment.h"
#include "CombinedJobController.h"
#include "SimEngineTimer.h"
#include <assert.h>
#include "Routes.h"

namespace EnergySim { 

	long CombinedJobController::_last_id=0;

	CombinedJobController::CombinedJobController(SimContext *ctx):ICombinedJobController(ctx)
	{
		_name= "NO_NAME";
		_id = ++CombinedJobController::_last_id;
		_started = false;
		// initialise 5 minutes as maximum duration before we decide a job is gone wrong and we should preempt it
		_delay_before_forced_preempt=300; 
		if(ctx!=NULL){
			_env=ctx->environment();
		}
	}
	CombinedJobController::CombinedJobController(SimContext *ctx, string name):ICombinedJobController(ctx)
	{
		_name=name;
		_id = ++CombinedJobController::_last_id;
		_started = false;
		_delay_before_forced_preempt=300; 
		if(ctx!=NULL){
			_env=ctx->environment();
		}
	}
	CombinedJobController::CombinedJobController(SimContext *ctx, string name, IRouteFollower * theFollower) :ICombinedJobController(ctx)
	{
		_name = name;
		_id = ++CombinedJobController::_last_id;
		_started = false;
		_delay_before_forced_preempt = 300;
		if (ctx != NULL){
			_env = ctx->environment();
		}
		itsFollower = theFollower;
	}
	void CombinedJobController::InsertJob(IJob *theJob)
	{
		_jobqueue.push_front(theJob);
	}
	void CombinedJobController::AddJob(IJob *theJob)
	{
		_jobqueue.push_back(theJob);
	}
	void CombinedJobController::RemoveJob(IJob* theJob)
	{
		for (std::deque<IJob*>::iterator it = _jobqueue.begin(); it!=_jobqueue.end(); ++it)
		{
			if( *it==theJob){
				_jobqueue.erase(it);
				break;
			}
		}
	}
	void CombinedJobController::RemoveJobsUntilJobID(int theJobID)
	{
		std::deque<IJob*>::iterator it = _jobqueue.begin();
		while (it != _jobqueue.end())
		{
			if ((*it)->jobID == theJobID)
				break;
			IJob* job = *it;
			it = _jobqueue.erase(it);
			delete job;  //TODO Can we really delete it, the current job gets deleted twice
		}
	}	
	void CombinedJobController::Start()
	{
		_started = true;
		NotifyJobStarted();
		ProcessJobs();
	}
	void CombinedJobController::Execute()
	{
		ProcessJobs();
	}
	void CombinedJobController::ProcessJobs()
	{
		if (_jobqueue.size() > 0)
		{
			if(_env!=NULL){
				std::ostringstream strs;
				strs << _ctx->engine()->simulated_time();
				_env->DebugLog("CombinedJobController " + _name + " will execute next job now at " + strs.str());
			}
			_current_job = NextJob();
			if(_env!=NULL){
				std::ostringstream strs;
				strs <<  _jobqueue.size();
				_env->DebugLog("CombinedJobController " + _name + " will execute next job " + _current_job->ToString() + " jobs remaining: " + strs.str());
			}
			_current_job->AddJobStartedListener(this);
			_current_job->AddJobFinishedListener(this);
			_current_job->Execute();
			// delete jobs that were marked for deletion
		}
		else
		{
			if(_env!=NULL)_env->DebugLog("CombinedJobController " + _name + " sends ProcessingCompleted");
			NotifyJobFinished();
		}
	}
	void CombinedJobController::PreemptJob(IPreemptableJob *theJob)
	{
		if(theJob==NULL) return;
		if(theJob == _current_job)
		{
			theJob->Preempt();
		}
		else
		{
			RemoveJob(theJob);
		}
	}
	void CombinedJobController::OnJobFinished(IJob *theJob, EventArgs *theArgs)
	{
		if(theJob==this)
		{
			if(_current_job==theJob)_current_job=NULL;

			// Note: preferably delete theJob here, but this is not possible because deeper in the call stack we are 
			// still iterating over all JobFinishedListeners of theJob
		    MarkForDeletion();		
		}
		else
		{
			if(_current_job==theJob)_current_job=NULL;
			theJob->MarkForDeletion();
			_ctx->engine()->ScheduleJobNow(this);
		}
	}
	void CombinedJobController::OnJobStarted(IJob *theJob, EventArgs *theArgs)
	{
		if(theJob==this)
		{
			theJob->RemoveJobStartedListener(this);
		}
		else
		{
			// 
			std::ostringstream strs;
			strs <<  "CombinedJobController " + _name + " started execute job " + theJob->ToString();
			_env->DebugLog(strs.str());
		}
	}
	IJob* CombinedJobController::GetNthJob(int index)
	{
		assert(index >=0);
		return _jobqueue[index];
	}
	IJob* CombinedJobController::NextJob()
	{
		IJob* nextjob = _jobqueue.front();
		_jobqueue.pop_front();
		return nextjob;
	}
	// 
	// returns diganostics on current job and remaining jobs in queue
	string CombinedJobController::Diagnostics()
	{
		string res = "";
		string diagname = _name;
		if (diagname == "") diagname = ToString();
		if (_current_job != NULL)
		{
			res = "\nController " + diagname + " current job is:" + _current_job->ToString();
			ICombinedJobController* combinedJobCtrl = dynamic_cast<ICombinedJobController*>(_current_job);
			if (combinedJobCtrl!=NULL)
			{
				res += "\n\t" + combinedJobCtrl->Diagnostics();
			}
			if (_jobqueue.size() > 0)
			{
				res += "\n jobs in queue for " + diagname + " :";
				for(deque<IJob*>::const_iterator iter = _jobqueue.begin(); iter != _jobqueue.end(); ++iter)
				{
					if(*iter != 0)
					{
						res += "\n" + (*iter)->ToString();
					}
				}
				FinishCurrentJob();
			}
			else
			{
				res = "\nController " + diagname + " has no current job ";
			}

			return res;
		}
		return res;
	}
	CombinedJobController::~CombinedJobController()
	{
		_jobqueue.clear();
	}
	void CombinedJobController::FinishCurrentJob()
	{
		// for diagnostics reasons it is useful to force a current job to finish after a given delay
		if (  delay_before_forced_preempt()  <= 0)
		{
			if (_current_job != NULL)
			{
				IPreemptableJob* preemptableJob = dynamic_cast< IPreemptableJob* >( _current_job );
				if (preemptableJob!=NULL)
				{
					preemptableJob->Preempt();
				}
			}
		}
		else
		{
			if (_current_job != NULL)
			{
				IPreemptableJob* preemptableJob = dynamic_cast< IPreemptableJob* >( _current_job );
				{
					_job2Preempt = preemptableJob;
					// start timer
					SimEngineTimer* preempttimer = new SimEngineTimer(_ctx, delay_before_forced_preempt());
					preempttimer->AddElapsedListener(this);
					preempttimer->Start();
				}
			}
		}
	}
	void CombinedJobController::OnElapsed(ITimer *theTimer, EventArgs *theArgs)
	{
		if (_current_job == _job2Preempt)
		{
			IPreemptableJob* preemptableJob = dynamic_cast< IPreemptableJob* >( _current_job );
			if (preemptableJob!=NULL)
			{
				preemptableJob->Preempt();
			}
		}
	}
	void CombinedJobController::Reset()
	{
		// delete all jobs 
		while (!_jobqueue.empty())
		{
			IJob* tobedeleted= _jobqueue.front();
			_jobqueue.pop_front();
			delete tobedeleted;
		}
		if (_current_job != NULL)
		{
			delete _current_job;
			_current_job = NULL;
		}
	}
}
