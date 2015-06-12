#pragma once
#ifndef _SIMENGINETIMER_
#define _SIMENGINETIMER_
#include "Job.h"

namespace EnergySim {
	// forward declaration
	class SimContext;
	class ITimer;


	class ENERGYSIM_DLL_PUBLIC TimerElapsedListener
	{
	public:
		virtual void OnElapsed(ITimer *theTimer, EventArgs *theArgs)=0;
	};
	class ENERGYSIM_DLL_PUBLIC TimerPreemptedListener
	{
	public:
		virtual void OnPreempted(ITimer *theTimer, EventArgs *theArgs)=0;
	};

	class ENERGYSIM_DLL_PUBLIC ITimer{
	protected: 
		vector<TimerElapsedListener*> _elapsedlisteners;
		vector<TimerPreemptedListener*> _preemptedlisteners;
		virtual void NotifyElapsed(){
			for(vector<TimerElapsedListener*>::const_iterator iter = _elapsedlisteners.begin(); iter != _elapsedlisteners.end(); ++iter)
			{
				if(*iter != 0)
				{
					(*iter)->OnElapsed(this, NULL);
				}
			}
		}
				virtual void NotifyPreempted(){
			for(vector<TimerPreemptedListener*>::const_iterator iter = _preemptedlisteners.begin(); iter != _preemptedlisteners.end(); ++iter)
			{
				if(*iter != 0)
				{
					(*iter)->OnPreempted(this, NULL);
				}
			}
		}
	public:
		void AddElapsedListener(TimerElapsedListener *theListener)
		{
			_elapsedlisteners.push_back(theListener);
		};
		void AddPreemptedListener(TimerPreemptedListener *theListener)
		{
			_preemptedlisteners.push_back(theListener);
		};
		void RemoveElapsedListener(TimerElapsedListener *theListener)
		{
			_elapsedlisteners.erase(std::remove(_elapsedlisteners.begin(), _elapsedlisteners.end(), theListener), _elapsedlisteners.end());
		};
		void RemovePreemptedListener(TimerPreemptedListener *theListener)
		{
			_preemptedlisteners.erase(std::remove(_preemptedlisteners.begin(), _preemptedlisteners.end(), theListener), _preemptedlisteners.end());

		}
		virtual void Start()=0;
		virtual void Preempt()=0;
		virtual void Elapse()=0;

	};

	class ENERGYSIM_DLL_PUBLIC SimEngineTimer:public ITimer
	{
	private:
		SimContext* _ctx;
		double _delay;
		double _startedtime;
        double _completiontime;
	public:
		SimEngineTimer();
		SimEngineTimer(SimContext* theContext, double delay);
		virtual void Start();
		virtual void Preempt();
		virtual void Elapse();

		double startedtime()const{return _startedtime;}
		double completiontime()const{return _completiontime;}
	};
    class ENERGYSIM_DLL_PUBLIC FinishTimerJob:public IPreemptableJob    
    {

	private:
		 SimEngineTimer* _timer;

	public:
		 FinishTimerJob(SimContext *context, SimEngineTimer *timer):IPreemptableJob(context)
         {
             _timer = timer;
         }
		 virtual void Preempt();
		 virtual void Execute();
    };
}
#endif