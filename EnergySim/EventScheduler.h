// EventScheduler.h

#pragma once
#include "EnergySim.h"
#include "SimEngine.h"
#include "Job.h"
#include <set>
using namespace std;

namespace EnergySim {
	// forward declarations
	class SchedulableEvent;
	class IEnvironment;
	struct SchedulableEventPtrComp;

	class ENERGYSIM_DLL_PUBLIC EventScheduler:IJobFinishedListener
	{
	private:
		ISimEngine *_engine;
		IEnvironment *_env;

		double _lastevent;
		double _nextdelay;
		double _nextexpiration;
		int _updatespersecond;
		double _tolerance;
        bool _calculatedevent;

		multiset<SchedulableEvent*,SchedulableEventPtrComp> *_current_events;
	//	set<SchedulableEvent*,SchedulableEventPtrComp> *_future_events;
		void ExecuteCurrentEvents();
	public :
		EventScheduler(ISimEngine *_engine);
		int updates_per_second(){return _updatespersecond;}
		double simulated_time(){
			if(_engine!=NULL) return _engine->simulated_time();
			return -1;
		}
		void Scene_Step(double deltatime);
		double last_step_time(){ return _lastevent; }
		void Reset();
		double ProcessCurrentEvents();
		void ExecuteJob(IJob *job);
		void ScheduleJob(IJob *theJob, double theDelay,int priority=0);
		void EventScheduler::ScheduleJobNow(IJob *theJob);
		void EventScheduler::ScheduleJobAt(IJob *theJob, double theTime, int priority=0);
		// IJobFinishedListener
		virtual void OnJobFinished(IJob *theJob, EventArgs *theArgs);
	};
}