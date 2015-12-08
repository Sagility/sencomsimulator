#pragma once

#include <iostream>       // std::cout
#include <stack>          // std::stack
#include <vector>         // std::vector
#include <deque>          // std::deque
#include "EnergySim.h"
#include "SimEngine.h"
#include "Job.h"
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <type_traits>
#include <functional>
#include "Parsing.h"
#include "Attributes.h"
#include "Attributes_Internal.h"
#include "Routes.h"

using namespace std;

namespace EnergySim
{
	class SimEngineTimer;
	class SimContext;
	class Resource;
	class Process;
	class TimerElapsedListener;
	class TimerPreemptedListener;
	class Schedule;
	class ConcreteProcess;

	class SetAttributeJob : public IJob
	{
	public: 
		SetAttributeJob(AttributeHandler* theHandler, string theAttribute, double theValue, SimContext* theCtx)
		{
			itsAttribute = theAttribute;
			itsValue = theValue;
			itsHandler = theHandler;
			_ctx = theCtx;
		}
		virtual void Execute()
		{
			NotifyJobStarted();
			itsHandler->replace(itsAttribute, new ConstantValue(itsValue));
			NotifyJobFinished();
		}
	    virtual void Dispose() { }
	private:
		string itsAttribute;
		double itsValue;
		AttributeHandler* itsHandler;
	};
	class WaitForAttributeJob : public IJob, public IListenChange<string, double>
	{
		public: 
			WaitForAttributeJob(AttributeHandler* theHandler, string theAttribute, double theLowValue, double theHighValue, SimContext* theContext)
			{
				itsAttribute = theAttribute;
				itsHighValue = theHighValue;
				itsLowValue = theLowValue;
				itsHandler = theHandler;
				_ctx = theContext;
			}
			virtual void valueChanged(string theAttribute, double theValue)
			{
				if (theAttribute == itsAttribute)
					if (theValue<itsHighValue)
						if (theValue>itsLowValue)
						{
							itsHandler->Changing.erase(std::remove(itsHandler->Changing.begin(), itsHandler->Changing.end(), this));
							NotifyJobFinished();
						}
			}
			virtual void Execute()
			{
				NotifyJobStarted();
				itsHandler->Changing.push_back(this);
			}
			virtual void Dispose() { }
		private:
			AttributeHandler* itsHandler;
			double itsLowValue, itsHighValue;
			string itsAttribute;
	};
	class ENERGYSIM_DLL_PUBLIC LogJob : public IJob
	{

	private:
		string _msg;
	public:
		//constructor
		LogJob(SimContext *context, string msg) :IJob(context)
		{
			set_context(context);
			_msg = msg;
		}
		~LogJob()
		{
			// clear all lists
		}

		virtual string ToString()
		{
			return "LogJob logging " + _msg + "\n";
		}

		virtual void Execute();
		virtual string classname() { return "LogJob"; }
	};



	/* */
	class ENERGYSIM_DLL_PUBLIC ManipulatorJob : public IJob
	{

	private:
		string _msg;
		double itsValue;
		double itsTime;
		string itsObject;
		string itsFunction;
	public:
		//constructor
		ManipulatorJob(SimContext *context, string object, string function, double value) :IJob(context)
		{
			set_context(context);
			itsValue = value;
			itsObject = object;
			itsFunction = function;
		}
		~ManipulatorJob()
		{
			// clear all lists
		}

		virtual string ToString()
		{
			return "ManipulatorJob logging " + _msg + "\n";
		}

		virtual void Execute(){};
		virtual string classname() { return "ManipulatorJob"; }
	};
	class ENERGYSIM_DLL_PUBLIC WaitForResourcesJob : public IJob
	{
	public:
		WaitForResourcesJob(Process* theProcess, SimModel* theModel, long theRouteFollowerID, CombinedJobController* theController);
		virtual void finish()
		{
			NotifyJobFinished();
		}
		virtual void Execute()
		{
			NotifyJobStarted();
			claimResources();
		}
		virtual void Dispose() { }

		long itsRouteFollowerID;
		void claimResources();
		Process* itsResReq;
		SimModel* model;
		CombinedJobController* itsController;
	};
	class ENERGYSIM_DLL_PUBLIC FinishProcessJob : public IJob
	{
	private:
		long itsFollowerID;
		long itsProcessID;
		SimModel* model;
	public:
		FinishProcessJob(long ProcessID, long RouteFollowerID, SimModel* theModel);
		virtual void Execute();
	};
	class PublishPreReqDoneJob : public IJob
	{
	public:
		long itsPreReqID;
		PublishPreReqDoneJob(long thePreReqID)
		{
			itsPreReqID = thePreReqID;
		}
		virtual void Execute();
		virtual void Dispose() { }
	};
	class WaitForPreReqDoneJob : public IJob, public IListenChange<string, int>
	{
	public:
		list<long> itsPreReqs;
		WaitForPreReqDoneJob(list<long> thePreReqs)
		{
			itsPreReqs = thePreReqs;
		}
		virtual void Execute();
		virtual void Dispose() { }
	};
	class ENERGYSIM_DLL_PUBLIC DelayJob : public IPreemptableJob, public TimerElapsedListener, public TimerPreemptedListener
	{
	private:
		double delay;
		SimEngineTimer *_timer;
	public:
		//constructor
		DelayJob(SimContext *context, double msdelay) :IPreemptableJob(context)
		{
			if (msdelay < 0)
				msdelay = 0;
			//if (msdelay > 0) // 150804
			{
				set_context(context);
				delay = msdelay;
				_timer = new SimEngineTimer(context, delay);
			}
		}
		~DelayJob()
		{
			// clear all lists
		}
		void Preempt()
		{
			NotifyJobPreempted();
			//   timer.Preempt();
			NotifyJobFinished();
		}
		virtual string ToString()
		{
			std::ostringstream strs;
			strs << delay;
			std::string str = strs.str();
			return "DelayJob waiting for " + str;
			// not supported yet???  return "DelayJob waiting for " + std::to_string(delay);
		}

		virtual void DelayElapsed()
		{
			NotifyJobFinished();
		}
		virtual void Execute();
		virtual string classname() { return "DelayJob"; }
		virtual void OnElapsed(ITimer *theTimer, EventArgs *theArgs);
		virtual void OnPreempted(ITimer *theTimer, EventArgs *theArgs);
	};
	class WaitForSetupJob : public IJob, IJobFinishedListener
	{
	public:
		WaitForSetupJob(IRouteFollower* theFollower, long theResID, long theSetup, SimContext* theCtx);
		virtual void Execute();
		void finish();
		virtual void Dispose() { }
		virtual void OnJobFinished(IJob *theJob, EventArgs *theArgs);
	private:
		SimContext* itsCtx;
		long itsResID;
		long itsProcessID;
		Resource* itsRes;
		long itsSetup;
		IRouteFollower* itsFollower;
	};
	class PublishScheduleJob : public IJob
	{
	public:
		long itsLotID, itsStepID;
		Schedule* itsSchedule;
		PublishScheduleJob(Schedule* theSchedule, long theLotID, long theStepID)
		{
			itsLotID = theLotID;
			itsStepID = theStepID;
			itsSchedule = theSchedule;
		}
		virtual void Execute();
		virtual void Dispose() { }
	};
	class WaitForScheduleJob : public IJob
	{
	public:
		long itsLotID, itsStepID;
		Schedule* itsSchedule;
		WaitForScheduleJob(Schedule* theSchedule, long theLotID, long theStepID)
		{
			itsLotID = theLotID;
			itsStepID = theStepID;
			itsSchedule = theSchedule;
		}
		virtual void Execute();
		void finish()
		{
			NotifyJobFinished();
		}
		virtual void Dispose() { }
	};

	class SetStateJob : public IJob
	{
	public:
		SetStateJob(IRouteFollower* theFollower, long theResID, ResourceState theState);
		virtual void Execute()
		{
			NotifyJobStarted();
			NotifyJobFinished();
		}
		virtual void Dispose() { }
	private:
		long itsResource;
		ResourceState itsState;
	};

	class SetRouteFollower : public IJob
	{
	public:
		SetRouteFollower(IRouteFollower* theFollower, string theKey, int theValue)
		{
			itsFollower = theFollower;
			itsKey = theKey;
			itsValue = theValue;
		}
		virtual void Execute();
		virtual void Dispose() { }
	private:
		IRouteFollower* itsFollower;
		string itsKey;
		int itsValue;
	};

	class IDJob : public IJob
	{
	public:
		IDJob(int theID, SimContext* theCtx)
		{
			jobID = theID; 
			_ctx = theCtx;
		}
		virtual void Execute()
		{
			NotifyJobStarted();
			NotifyJobFinished();
		}
		virtual void Dispose() { }
	};
	class RunControlJob : public IJob
	{
		int itsIntervall;
		int itsHorizon;
		CombinedJobController* itsController;
	public:
		RunControlJob(int theIntervall, int theHorizon, SimContext* theCtx, CombinedJobController& theController)
		{
			itsController = &theController;
			itsIntervall = theIntervall;
			itsHorizon = theHorizon;
			_ctx = theCtx;
		}
		virtual void Execute()
		{
			NotifyJobStarted();
			if (_ctx->engine()->simulated_time() > itsHorizon)
				terminate;
			if (itsIntervall > 0)
			{
				itsController->AddJob(new DelayJob(_ctx, itsIntervall));
				itsController->AddJob(new RunControlJob(itsIntervall, itsHorizon, _ctx, *itsController));
			}
			NotifyJobFinished();
		}
		virtual void Dispose() { }
	};
	class GOTOidJobIFKeyValue : public IJob
	{
	public:
		GOTOidJobIFKeyValue(int theID, CombinedJobController* theController, string theKey, int theValue)
		{
			itsJobID = theID;
			itsController = theController;
			itsKey = theKey;
			itsValue = theValue;
			_ctx = theController->context();
		}
		virtual void Execute();
		virtual void Dispose() { }
	private:
		int itsJobID;
		CombinedJobController* itsController;
		string itsKey;
		int itsValue;
	};

	class ENERGYSIM_DLL_PUBLIC FunctionDoJob : public IJob
	{
	public:
		//constructor
		FunctionDoJob(SimContext *context, std::function<void()> theFP) :IJob(context)
		{
			fp = theFP;
		}
		~FunctionDoJob()
		{
		}
		virtual string ToString()
		{
			return "FunctionDoJob\n";
		}
		virtual void Execute()
		{
			NotifyJobStarted();
			fp();
			NotifyJobFinished();
		};
		virtual string classname() { return "FunctionJob"; }
	private:
		std::function<void()> fp;
	};
	void ENERGYSIM_DLL_PUBLIC delayAndDoJobFunction(std::function<void()> theFP, SimModel* theModel, double theWait);

}