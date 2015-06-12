#include "stdafx.h"
#include <cassert> 
#include <sstream>
#include "SimEngine.h"
#include "EventScheduler.h"

namespace EnergySim { 

	SimEngine::SimEngine(IEnvironment *env)
	{
		_simulated_time=0;
		_env = env;
		_eventscheduler = new EventScheduler(this);
	}
	SimEngine::~SimEngine()
	{
		_env=NULL;
		delete _eventscheduler;
	}
	void SimEngine::DeleteMarkedJobs()
	{
		set<IJob*>::iterator it;
		for(it=_tobedeletedjobs.begin(); it!=_tobedeletedjobs.end(); it++)
		{
			delete (*it);
		}
		_tobedeletedjobs.clear();
	}
	void SimEngine::Run()
	{
		if(_running==true)return;
		_running=true;
		DeleteMarkedJobs();
		NotifyRunning();
		while(_running)
		{
			double nextdelay =_eventscheduler->ProcessCurrentEvents();
			if(nextdelay <=0) 
			{
				_env->DebugLog("No more events to process, will finish" );
				Finish();
			}
			else advance_time(nextdelay);
			DeleteMarkedJobs();
		}

	};
	void SimEngine::Pause()
	{
		if(_running==false)return;
		_running=false;
		NotifyPause();
	};
	void SimEngine::Finish()
	{
		_running=false;
		NotifyFinish();
	};
	void SimEngine::Reset()
	{
		_simulated_time=0;
		_eventscheduler->Reset();
		_running=false;
	};
	void SimEngine::advance_time(double delta_time)
	{
		assert(delta_time>0);

		if (delta_time > 0)
		{
			std::ostringstream strs;
			strs << "Before advancing time " << _simulated_time << " with delta_time " << delta_time << endl;

			_env->DebugLog(strs.str());
			NotifyBeforeNextStep();
			_simulated_time += delta_time;
			strs << "After advancing time by delta " << delta_time << " the simulated time is " << _simulated_time << endl;

			_env->DebugLog(strs.str());
			NotifyNextStep( delta_time);
		}
	}
	void SimEngine::ScheduleJob(IJob *theJob, double theDelay,int priority)
	{
		_eventscheduler->ScheduleJob(theJob,theDelay,priority); 
	}
	void SimEngine::ScheduleJobNow(IJob *theJob)
	{
		_eventscheduler->ScheduleJobNow(theJob);
	}
	void SimEngine::ScheduleJobAt(IJob *theJob, double theTime, int priority)
	{
		_eventscheduler->ScheduleJobAt(theJob, theTime, priority);
	}
}