#include "stdafx.h"
#include <sstream>
#include <cassert>
#include "SimEngine.h"
#include "EventScheduler.h"
#include "EnergySim.h"
#include "SchedulableEvent.h"
#include "SimLogEnvironment.h"
#include "Job.h"

namespace EnergySim { 

	bool operator<(const SchedulableEvent & se1, const SchedulableEvent & se2)
	{
		if(se1.time() < se2.time()) return true;
		if(se1.time() > se2.time())return false;
		if(se1.priority() < se2.priority())return true;
		if(se1.priority() > se2.priority())return false;
		return false;
	}

	EventScheduler::EventScheduler(ISimEngine *engine){

		if(engine!=NULL){
			_engine = engine;
			_env= _engine->environment();
		}
		_tolerance = 0.000001;
		_updatespersecond=0;
		_calculatedevent = false;
		_lastevent=0;
		_current_events = new set<SchedulableEvent*,SchedulableEventPtrComp>();
		if (_updatespersecond == 0) _nextdelay = std::numeric_limits<double>::infinity();
		else _nextdelay = 1000.0 / _updatespersecond;
	}

	void EventScheduler::Reset()
	{
		_lastevent = 0;
		_current_events->clear();

		if (_updatespersecond == 0) _nextdelay = std::numeric_limits<double>::infinity();
		else _nextdelay = 1000.0 / _updatespersecond;
	}
	double EventScheduler::ProcessCurrentEvents()
	{
		assert(_engine!=NULL);
		if (_engine->IsRunning() == false) return 0.0;
		// calculate delta
		double delta = _engine->simulated_time() - _lastevent;
		if (_calculatedevent)
		{
			std::ostringstream strs;
			strs << "eventtimer_Elapsed  at calculated event" << _engine->simulated_time() << " previous was at " << _lastevent << endl;
			_env->DebugLog( strs.str() );
			_calculatedevent = false;
		}
		_lastevent = _engine->simulated_time();

		ExecuteCurrentEvents();
		double nextdelta =0;
		if(_current_events->size() >0){
			std::set<SchedulableEvent*,SchedulableEventPtrComp>::iterator it=_current_events->begin();
			SchedulableEvent* se= *it;
			// get next event from list and use it to calculate next delta time increase (in ms)
			nextdelta= se->time()-_engine->simulated_time();
			// take into account updates per second as well (e.g. for graphical rendering)
			// check whether we should correct next delta
			if(_updatespersecond >0)
			{
				if(1000.0 / _updatespersecond < nextdelta) nextdelta = 1000.0 / _updatespersecond;
			}
			std::ostringstream strs;
			strs << "ProcessCurrentEvents at scheduled time" << _lastevent << " determined nextdelta" << nextdelta << " from " << se->job()->ToString() << " with number of events = " << _current_events->size() << endl;
			_env->DebugLog(strs.str());
		}
		return nextdelta;
	}
	void EventScheduler::ExecuteCurrentEvents()
	{
		// iterate over events that should be executed 
		if(_current_events->size()==0)return;
		std::set<SchedulableEvent*,SchedulableEventPtrComp>::iterator it=_current_events->begin();
		SchedulableEvent* se= *it;

		while (se != NULL && se->time() <= _lastevent + _tolerance)
		{
			if (se->job() != NULL)
			{
				std::ostringstream strs;
				strs << "eventtimer_Elapsed at scheduled time" << se->time() << " will Invoke Job " << se->job()->ToString() << " with priority " << se->priority() << endl;

				_env->DebugLog(strs.str());
				ExecuteJob(se->job());
			}
			_current_events->erase(*it);
			if(_current_events->size()>0){
				it=_current_events->begin();
				se =*it;
			}
			else{
				se=NULL;
			}
		}
	}
	void EventScheduler::ExecuteJob(IJob *job){
		//
		job->AddJobFinishedListener(this);

		// Note: do not use ScheduleJobNow to prevent endless loop
		if(job!=NULL) job->Execute();
	}

	//TODO
	// To check whether this is needed here or at engine
	void EventScheduler::Scene_Step(double deltatime)
	{
		// move this to subscription of NextStep or maybe _before next step
		//  itsParser.itsHandler.reportAndUpdateAllAttributes(_engine->simulated_time());
		if (deltatime > 0)
		{
			//SimEnvironment.CurrentEnvironment.DebugLog("At " + engine.SimulatedTime + " we perform Step with delta  " + deltatime);
			// if (NextStep != NULL) NextStep(deltatime);
		}
	}
	void EventScheduler::ScheduleJob(IJob *theJob, double theDelay,int priority)
	{
		if(theDelay>=0 )
			_current_events->insert(new SchedulableEvent(this->simulated_time() + theDelay, theJob, priority));
	}
	void EventScheduler::ScheduleJobNow(IJob *theJob)
	{
		_current_events->insert(new SchedulableEvent(this->simulated_time(), theJob, 0));
	}
	void EventScheduler::ScheduleJobAt(IJob *theJob, double theTime, int priority)
	{
		if(theTime> this->simulated_time())
			_current_events->insert(new SchedulableEvent(theTime, theJob, priority));
	}

	void EventScheduler::OnJobFinished(IJob *theJob, EventArgs *theArgs){

		if(theJob!=NULL){
			theJob->MarkForDeletion();
		}
	}
}