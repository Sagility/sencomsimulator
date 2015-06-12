// Job.h

#pragma once
#include <vector>
#include <algorithm>
#include "EnergySim.h"
using namespace std;
#include "Eventtypes.h"

namespace EnergySim {

	// forward declarations
	class IJob; 
	class EventArgs;
	class SimEngine;
	class SencomModel; 
	class SimEnvironment;
	class IPreemptableJob;

	// interface class used for Observer pattern to get notified when a IJob gets started
	// in that case the OnJobStarted method will be called
	class ENERGYSIM_DLL_PUBLIC IJobStartedListener
	{
	public:
		virtual void OnJobStarted(IJob *theJob, EventArgs *theArgs)=0;
	};
	// interface class used for Observer pattern to get notified when a IJob gets finished
	// in that case the OnJobStarted method will be called
	class ENERGYSIM_DLL_PUBLIC IJobFinishedListener
	{
	public:
		virtual void OnJobFinished(IJob *theJob, EventArgs *theArgs)=0;
	};

	//

	// interface class used for Observer pattern to get notified when a IPreemptableJob gets preempted
	// in that case the OnJobStarted method will be called
	class ENERGYSIM_DLL_PUBLIC IJobPreemptedListener
	{
	public:
		virtual void OnJobPreempted(IPreemptableJob *theJob, EventArgs *theArgs)=0;
	};

	// Interface class for jobs
	// will notify its observers when the job is started and finished
	class ENERGYSIM_DLL_PUBLIC IJob //: public IEntity
	{
	private:
		vector< IJobStartedListener * > _startedlisteners;
		vector< IJobFinishedListener * > _finishedlisteners;
	protected:
		SimContext *_ctx;
		bool _markedfordeletion;
		virtual void NotifyJobStarted()
		{
			IEvent::publishEvent(EventType::ET_JOB_START, 0, vector<long>());
			if(_startedlisteners.size()==0) return;
			for(vector<IJobStartedListener*>::const_iterator iter = _startedlisteners.begin(); iter != _startedlisteners.end(); ++iter)
			{
				if(*iter != 0)
				{
					if(_markedfordeletion==false)(*iter)->OnJobStarted(this, NULL);
				}
			}
		}
		virtual void NotifyJobFinished()
		{
			IEvent::publishEvent(EventType::ET_JOB_END, 0, vector<long>());
			if (_finishedlisteners.size() == 0) return;
			for(vector<IJobFinishedListener*>::const_iterator iter = _finishedlisteners.begin(); iter != _finishedlisteners.end(); ++iter)
			{
				if(*iter != 0)
				{
					(*iter)->OnJobFinished(this, NULL);
				}
			}
		}
	public:
		IJob() { };
		IJob(SimContext* context) {_ctx=context;_markedfordeletion=false;};
		virtual void Execute()=0;
		virtual void Start(){NotifyJobStarted();}
		virtual void Finish(){NotifyJobFinished();}
		virtual ~IJob()
		{
			_startedlisteners.clear();		
			_finishedlisteners.clear();	
		}
		void AddJobStartedListener(IJobStartedListener *theListener)
		{
			_startedlisteners.push_back(theListener);
		};
		void AddJobFinishedListener(IJobFinishedListener *theListener)
		{
			_finishedlisteners.push_back(theListener);
		};
		void RemoveJobStartedListener(IJobStartedListener *theListener)
		{
			_startedlisteners.erase(std::remove(_startedlisteners.begin(), _startedlisteners.end(), theListener), _startedlisteners.end());
		}
		void RemoveJobFinishedListener(IJobFinishedListener *theListener)
		{
			_finishedlisteners.erase(std::remove(_finishedlisteners.begin(), _finishedlisteners.end(), theListener), _finishedlisteners.end());
		}
        virtual SimContext* context() const {return _ctx; }
		virtual void set_context(SimContext* theContext) { _ctx =theContext; }
		virtual bool IsMarkedForDeletion() const{ return _markedfordeletion;}
		virtual void MarkForDeletion(){
			_markedfordeletion=true;
			if(_ctx!=NULL && _ctx->engine() !=NULL){
				_ctx->engine()->MarkJobForDeletion(this);
			}
		}
		 
		virtual string classname() { return "IJob";}
		virtual string ToString(){ return classname() ;} 
	};

	// Interface class for jobs with a priority
	class ENERGYSIM_DLL_PUBLIC IPriorityJob:public IJob
	{
	protected:
		int _priority;
	public:
		IPriorityJob(SimContext *context):IJob(context){};
		int priority() const{return _priority;}
		void set_priority(int thePriority) {_priority=thePriority;}
		virtual string classname() { return "IPriorityJob";}
	};


	// Interface class for jobs that can be preempted during execution
	// it will notify its listeners that it is preempted
	class ENERGYSIM_DLL_PUBLIC IPreemptableJob: public IJob
	{
	private:
		vector< IJobPreemptedListener *> _jobpreemptedlisteners;
	protected:
		virtual void NotifyJobPreempted()
		{
			for(vector<IJobPreemptedListener*>::const_iterator iter = _jobpreemptedlisteners.begin(); iter != _jobpreemptedlisteners.end(); ++iter)
			{
				if(*iter != 0)
				{
					if(_markedfordeletion==false)(*iter)->OnJobPreempted(this, nullptr);
				}
			}
		}
	public: 
		IPreemptableJob(SimContext *context):IJob(context){};
		virtual void Preempt()=0; 
		void AddJobPreemptedListener(IJobPreemptedListener *theListener)
		{
			_jobpreemptedlisteners.push_back(theListener);
		};
		void RemoveJobPreemptedListener(IJobPreemptedListener *theListener)
		{
			_jobpreemptedlisteners.erase(std::remove(_jobpreemptedlisteners.begin(), _jobpreemptedlisteners.end(), theListener), _jobpreemptedlisteners.end());
		}
		virtual ~IPreemptableJob()
		{			
			_jobpreemptedlisteners.clear();		
		}
		virtual string classname() { return "IPreemptableJob";}
	};
}