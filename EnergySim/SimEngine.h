// SimEngine.h
#pragma once
#include "config.h"
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include "EventArgs.h"
#include "SimLogEnvironment.h"

using namespace std;

namespace EnergySim {
	// Forward declarations
	class SimEngineTimer;
	class SimContext;
	class ISimEngine;
	class IEnvironment;
	class EventArgs;
	class EventScheduler;
	class NextStepEventArgs;
	class IJob;


	class ENERGYSIM_DLL_PUBLIC EngineRunningListener
	{
	public:
		virtual void OnRun(ISimEngine *theEngine, EventArgs *theArgs)=0;
	};
	class ENERGYSIM_DLL_PUBLIC EnginePauseListener
	{
	public:
		virtual void OnPause(ISimEngine *theEngine, EventArgs *theArgs)=0;
	};

    class ENERGYSIM_DLL_PUBLIC NextStepEventArgs: EventArgs
	{
	private: 
		double _delta;
	public:
		NextStepEventArgs(double deltatime){
			_delta = deltatime;
		}
		double delta() const {return _delta;}
	};

	class ENERGYSIM_DLL_PUBLIC EngineStepListener
	{
	public:
		virtual void OnNextStep(ISimEngine *theEngine, NextStepEventArgs *theArgs)=0;
	};
	class ENERGYSIM_DLL_PUBLIC EngineBeforeStepListener
	{
	public:
		virtual void OnBeforeNextStep(ISimEngine *theEngine, EventArgs *theArgs)=0;
	};
	class ENERGYSIM_DLL_PUBLIC EngineFinishListener
	{
	public:
		virtual void OnFinish(ISimEngine *theEngine, EventArgs *theArgs)=0;
	};
	
	class ENERGYSIM_DLL_PUBLIC ISimEngine
	{
	protected:
		IEnvironment *_env;
		EventScheduler *_eventscheduler;
		bool _running;
		double _simulated_time;
		set<IJob*> _tobedeletedjobs;
		vector<EngineRunningListener*> _runlisteners;
		vector<EnginePauseListener*> _pauselisteners;
		vector<EngineStepListener*> _steplisteners;
		vector<EngineBeforeStepListener*> _beforesteplisteners;
		vector<EngineFinishListener*> _finishlisteners;
		void NotifyRunning()
		{
			for(vector<EngineRunningListener*>::const_iterator iter = _runlisteners.begin(); iter != _runlisteners.end(); ++iter)
			{
				if(*iter != 0)
				{
					(*iter)->OnRun(this, NULL);
				}
			}
		}
		void NotifyPause()
		{
			for(vector<EnginePauseListener*>::const_iterator iter = _pauselisteners.begin(); iter != _pauselisteners.end(); ++iter)
			{
				if(*iter != 0)
				{
					(*iter)->OnPause(this, NULL);
				}
			}
		}
		void NotifyFinish()
		{
			for(vector<EngineFinishListener*>::const_iterator iter = _finishlisteners.begin(); iter != _finishlisteners.end(); ++iter)
			{
				if(*iter != 0)
				{
					(*iter)->OnFinish(this, NULL);
				}
			}
		}
		void NotifyNextStep(double deltatime)
		{
			for(vector<EngineStepListener*>::const_iterator iter = _steplisteners.begin(); iter != _steplisteners.end(); ++iter)
			{
				if(*iter != 0)
				{
					(*iter)->OnNextStep(this, new NextStepEventArgs(deltatime));
				}
			}
		}
		void NotifyBeforeNextStep()
		{
			for(vector<EngineBeforeStepListener*>::const_iterator iter = _beforesteplisteners.begin(); iter != _beforesteplisteners.end(); ++iter)
			{
				if(*iter != 0)
				{
					(*iter)->OnBeforeNextStep(this, NULL);
				}
			}
		}

	public:
		void MarkJobForDeletion(IJob *tobedeletedjob){_tobedeletedjobs.insert(tobedeletedjob);};
		void AddEngineRunningListener(EngineRunningListener *theListener)
		{
			_runlisteners.push_back(theListener);
		};
		void AddEnginePauseListener(EnginePauseListener *theListener)
		{
			_pauselisteners.push_back(theListener);
		};
		void AddEngineFinishListener(EngineFinishListener *theListener)
		{
			_finishlisteners.push_back(theListener);
		};
		void AddEngineStepListener(EngineStepListener *theListener)
		{
			_steplisteners.push_back(theListener);
		};
		void AddEngineBeforeStepListener(EngineBeforeStepListener *theListener)
		{
			_beforesteplisteners.push_back(theListener);
		};
		void RemoveEngineRunningListener(EngineRunningListener *theListener){
			_runlisteners.erase(std::remove(_runlisteners.begin(), _runlisteners.end(), theListener), _runlisteners.end());
		};
		void RemoveEnginePauseListener(EnginePauseListener *theListener){
			_pauselisteners.erase(std::remove(_pauselisteners.begin(), _pauselisteners.end(), theListener), _pauselisteners.end());
		};
		void RemoveEngineFinishListener(EngineFinishListener *theListener){
			_finishlisteners.erase(std::remove(_finishlisteners.begin(), _finishlisteners.end(), theListener), _finishlisteners.end());
		};
		void RemoveEngineStepListener(EngineStepListener *theListener){
			_steplisteners.erase(std::remove(_steplisteners.begin(), _steplisteners.end(), theListener), _steplisteners.end());
		};
		void RemoveEngineBeforeStepListener(EngineBeforeStepListener *theListener){
			_beforesteplisteners.erase(std::remove(_beforesteplisteners.begin(), _beforesteplisteners.end(), theListener), _beforesteplisteners.end());
		};
		virtual IEnvironment *environment() const{return _env;};
		virtual void Run()=0;
		virtual void Pause()=0;
		virtual void Reset()=0;
		virtual void Finish()=0;
		bool IsRunning() const{return _running;}
		double simulated_time ()const{return _simulated_time;}
		virtual void advance_time(double delta_time)=0;

		virtual void ScheduleJob(IJob *theJob, double theDelay,int priority=0)=0;
		virtual	void ScheduleJobNow(IJob *theJob)=0;
		virtual void ScheduleJobAt(IJob *theJob, double theTime, int priority=0)=0;

	};
	class ENERGYSIM_DLL_PUBLIC SimEngine:public ISimEngine
	{
	protected: 
		void DeleteMarkedJobs();
	public:
		SimEngine(IEnvironment *env);
		virtual SimEngine::~SimEngine();

		// overriding
		virtual void Run();
		virtual void Pause();
		virtual void Reset();
		virtual void Finish();
		virtual void advance_time(double delta_time);

		virtual void ScheduleJob(IJob *theJob, double theDelay,int priority=0);
		virtual void ScheduleJobNow(IJob *theJob);
		virtual void ScheduleJobAt(IJob *theJob, double theTime, int priority=0);
	};
}