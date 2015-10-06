// SchedulableEvent.h

#pragma once
#include "EnergySim.h"
#include <vector>
#include <algorithm>
#include "Job.h"
using namespace std;

namespace EnergySim {

	class IJob;
	class SchedulableEvent;

//BT	bool operator<(const SchedulableEvent & se1, const SchedulableEvent & se2);

	class ENERGYSIM_DLL_PUBLIC SchedulableEvent{
	private:
		int _priority;
		double _time;
		IJob* _job;
	public: 
		/// <summary>
		/// Constructor 
		/// </summary>
		/// <param name="time">Simulated time when the event should be scheduled</param>
		/// <param name="job">pointer to the job that should be excuted when EventScheduler schedules this event</param>
		/// <param name="priority">Priority of the event to indicate to the EventScheduler which event should be executed before another event with the same time</param>
		SchedulableEvent(double time, IJob *job, int priority)
		{
			_time = time;
			_job = job;
			_priority = priority;
		};
		int priority()const {return _priority; }
		void set_priority(int prior) { _priority = prior; }
		double time()const{ return _time;}
		void set_time(double t){_time=t;}
		IJob* job() const{ return _job;}


	};
	struct SchedulableEventPtrComp
	{
		bool operator()(const SchedulableEvent* se1, const SchedulableEvent* se2) const{ 
			if(se1->time() < se2->time()) return true;
			if(se1->time() > se2->time())return false;
			if(se1->priority() < se2->priority())return true;
			if(se1->priority() > se2->priority())return false;
		//	return false; // Change 150920
			return (se1 < se2); // to make sure we have strict ordering, the set checks by switching lhs/rhs 
		}
	};

}