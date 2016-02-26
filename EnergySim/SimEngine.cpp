#include "stdafx.h"
#include <cassert> 
#include <sstream>
#include "SimEngine.h"
#include "EventScheduler.h"

#include "Objective.h"

namespace EnergySim { 

	RealSimEngine::RealSimEngine(IEnvironment *env) : SimEngine(env)
	{
		startTime = clock() / CLOCKS_PER_SEC;
		pasueTime = 0;
		timeInPause = 0;
		itsOR = new ObjectiveReporter();
		itsOR->itsEngine = this;
		itsOR->init();
	}

	void RealSimEngine::advance_time(double delta_time)
	{
		// Wait until time has passed
		if (!started)
		{
			started = true;
			startTime = clock() / CLOCKS_PER_SEC;
		}
		double now = clock() / CLOCKS_PER_SEC;
		double next = this->simulated_time() + delta_time + startTime - timeInPause;

		//itsOR->report(1,5000000,now*1000,1);

		/*
		int randNum = rand() % (3 - 1 + 1) + 1;

		double phase = now;

		int type = rand() % (3 - 0 + 1) + 0;
		int lineID = rand() % (3 - 1 + 1) + 1;
		int value = 0;
		if (type == 0)  // MAIN
		{
			while (phase > 100)
				phase -= 100;
			phase = phase / 100;
			value = 22000 + phase*15000;
		}
		if (type == 1)  // Extruder
		{
			while (phase > 80)
				phase -= 80;
			phase = phase / 80;
			value = 14400 + phase * 200;
		}
		if (type ==2)  // Grinder
		{
			while (phase > 40)
				phase -= 40;
			phase = phase / 40;
			value = 1300 + phase * 200;
		}
		if (type == 3)  // Hydraulic
		{
			phase += 30;
			while (phase > 100)
				phase -= 100;
			phase = phase / 100;
			value = 2000 + phase * 14000;
		}
		*/

		//FIX itsOR->report(type, value, lineID); // type,value, lineID   // Type 5 = new order
		while (next > now)
		{
			_sleep(10);
			now = clock() / CLOCKS_PER_SEC;
			
			//FIX next = this->simulated_time() + delta_time + startTime - timeInPause;

			//checkPausedStatus();
			//if (paused)
			//	pause();
			//else
			//	endPause();
			// break;  // MUSTGO
		}

		SimEngine::advance_time(delta_time);
		itsOR->itsModel->itsParser.itsValue->getAttributeHandler()->updateSimVariabels(simulated_time());
		itsOR->itsModel->itsParser.itsValue->getAttributeHandler()->updateAllAttributes();
		itsOR->itsModel->itsParser.itsValue->getAttributeHandler()->reportAllAttributes(simulated_time());
	}
	void RealSimEngine::checkPausedStatus()
	{
		if (FILE *file = fopen("FRED.KLM", "r")) 
		{
			fclose(file);
			paused = true;
		}
		else 
		{
			paused = false;
		}
	
	}
	void RealSimEngine::pause()
	{
		pasueTime = clock() / CLOCKS_PER_SEC;
		paused = true;
		//timeInPause
	}
	void RealSimEngine::endPause()
	{
		
		paused = false;
	}

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
			//FIX delete (*it);
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
	void SimEngine::ScheduleJobAtFront(IJob *theJob)
	{
		_eventscheduler->ScheduleJobNow(theJob, true );
	}
	void SimEngine::ScheduleJobAt(IJob *theJob, double theTime, int priority)
	{
		_eventscheduler->ScheduleJobAt(theJob, theTime, priority);
	}
}