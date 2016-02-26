// EnergySimtest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include "SimLogEnvironment.h"
#include "EnergySim.h"
#include "SimEngine.h"
#include "CombinedJobController.h"
#include "ClaimRelease.h"
#include "Jobs.h"
#include "CRResource.h"
#include "Schedule.h"
#include <sstream>
#include "SimModel.h"

#include "SimEngine.h"

#include "Objective.h"

namespace EnergySim
{
	ControlJob::ControlJob(SimContext *context, double msdelay, CombinedJobController* theController, ObjectiveReporter* theOR) :IJob(context) 
	{
		theOR->itsModel = _ctx->aModel;
		if (msdelay < 0)
			msdelay = 0;
		//if (msdelay > 0) // 150804
		{
			itsController = theController;
			itsOR = theOR;
			set_context(context);
			delay = msdelay;
			_timer = new SimEngineTimer(context, delay);
		}
	}


	string WaitForResourcesJob::ToString(){ return "WaitForResourcesJob" + to_string(itsResReq->processID); }

	void WaitForPreReqDoneJob::Execute()
	{
		NotifyJobStarted();
		PreReqs::wait(this);
	}
	void LogJob::Execute()
	{
		NotifyJobStarted();
		std::ostringstream strs;
		strs << _ctx->engine()->simulated_time() << " :\t" << _msg << endl;
		_ctx->environment()->Log(strs.str());
		NotifyJobFinished();
	}

	void PublishScheduleJob::Execute()
	{
		NotifyJobStarted();
		itsSchedule->markJobAsStarted(itsLotID, itsStepID);
		NotifyJobFinished();
	}
	void WaitForScheduleJob::Execute()
	{
		NotifyJobStarted();
		if (itsSchedule->canIStart(itsLotID, itsStepID))
		{
			finish();
			return;
		}
		itsSchedule->itsWaiters->push_back(this);
		//finish();
	}
	void PublishPreReqDoneJob::Execute()
	{
		NotifyJobStarted();
		PreReqs::done(itsPreReqID);
		NotifyJobFinished();
	}
	void EndOrderJob::Execute()
	{
		((SimEngine*)itsModel->context()->engine())->itsOR->report(5, 0, itsLine);
	}
	void DelayJob::Execute()
	{
		SetterHandler::getSetterHandler()->wakeUp(this->_ctx->engine()->simulated_time());
		NotifyJobStarted();
		if (delay > 0)
		{
			_timer->AddElapsedListener(this);
			_timer->AddPreemptedListener(this);
			_timer->Start();
		}
		else DelayElapsed();
	}
	void ControlJob::Execute()
	{
		NotifyJobStarted();
		if (delay > 0)
		{
			_timer->AddElapsedListener(this);
			_timer->Start();
		}
		else DelayElapsed();
	}
	void ControlJob::OnElapsed(ITimer *theTimer, EventArgs *theArgs)
	{
		DelayElapsed();
	}
	void ControlJob::DelayElapsed()
	{
		itsOR->reportAll();
	//	_ctx->aModel->itsParser.itsValue->getAttributeHandler()->updateAllAttributes();
	//	_ctx->aModel->itsParser.itsValue->getAttributeHandler()->reportAllAttributes(_ctx->engine()->simulated_time());
		//CHANGED 160224 double delay = 2 + ((double)rand() / (double)RAND_MAX) * 2;
		static double delay = 11 + ((double)rand() / (double)RAND_MAX) * 2;
		static bool started = false;
		if (!started)
		{
			string str;
			ifstream infile;
			infile.open("timing.txt");
			while (!infile.eof()) // To get you all the lines.
			{
				getline(infile, str); // Saves the line in STRING.
				delay = stod(str);
				delay += ((double)rand() / (double)RAND_MAX);
				if (delay < 1)
					delay = 1;
				break;
			}
			infile.close();
			started = true;
		}
		itsController->AddJob(new SetAttributeJob(this->_ctx->aModel->itsParser.itsValue->getAttributeHandler(), "dummy", 0, context()));
		itsController->AddJob(new ControlJob(_ctx, delay, itsController, itsOR));
		NotifyJobFinished();
	}
	void BottleDoneJob::Execute()
	{
		NotifyJobStarted();
		((SimEngine*)itsModel->context()->engine())->itsOR->report(4, 2, itsLine);
		NotifyJobFinished();
	}
	void DelayForAttributeJob::Execute()
	{
		SetterHandler::getSetterHandler()->wakeUp(this->_ctx->engine()->simulated_time());
		NotifyJobStarted();
		delay = _ctx->aModel->itsParser.itsValue->getAttributeHandler()->getAttribute(attribute);
		if (delay < 0.0001)
		{
			DelayElapsed();
			return;
		}
		_timer = new SimEngineTimer(_ctx, delay);
		if (delay > 0)
		{
			_timer->AddElapsedListener(this);
			_timer->AddPreemptedListener(this);
			_timer->Start();
		}
		else DelayElapsed();
	}
	void DelayForAttributeJob::OnElapsed(ITimer *theTimer, EventArgs *theArgs)
	{
		DelayElapsed();
	}
	void DelayForAttributeJob::OnPreempted(ITimer *theTimer, EventArgs *theArgs)
	{
		Preempt();
	}
	void DelayJob::OnElapsed(ITimer *theTimer, EventArgs *theArgs)
	{
		DelayElapsed();
	}
	void DelayJob::OnPreempted(ITimer *theTimer, EventArgs *theArgs)
	{
		Preempt();
	}

	void WaitForSetupJob::finish()
	{
		itsRes->setup(itsSetup);
		NotifyJobFinished();
	}

	void WaitForSetupJob::OnJobFinished(IJob *theJob, EventArgs *theArgs)
	{
		finish();
	}

	WaitForSetupJob::WaitForSetupJob(IRouteFollower* theFollower, long theProcessID, long theSetup, SimContext* theCtx)
	{
		itsCtx = theCtx;
		itsFollower = theFollower;
		itsSetup = theSetup;
		itsProcessID = theProcessID;
	}

	void WaitForSetupJob::Execute()
	{
		NotifyJobStarted();
		IEntity* aIE = IEntity::getByID(itsResID);
		Resource* aR = dynamic_cast<Resource*>(aIE);
		aR = itsFollower->primaryRes(itsProcessID);
		if (aR != NULL)
		{
			if (itsSetup != aR->setup())
			{
				itsRes = aR;
				double waitTime = 10;
				DelayJob* aDJ = new DelayJob(itsCtx, waitTime);
				CombinedJobController* aCJC = new CombinedJobController(itsCtx);
				aCJC->AddJob(aDJ);
				aDJ->AddJobFinishedListener(this);
				return;
			}
		}
		NotifyJobFinished();
	}

	void SetRouteFollower::Execute()
	{
		NotifyJobStarted();

		CombinedJobController* aCJC = new CombinedJobController(_ctx);
		itsFollower->addSlot(itsKey, itsValue);
		NotifyJobFinished();
	}

	void GOTOidJobIFKeyValue::Execute()
	{
		NotifyJobStarted();
		if( itsValue == itsController->getSlot(itsKey))
			itsController->RemoveJobsUntilJobID(itsJobID);
		NotifyJobFinished();
	}

	void ENERGYSIM_DLL_PUBLIC delayAndDoJobFunction(std::function<void()> theFP, SimModel* theModel, double theWait)
	{
		if (theWait < 0.0001)
			theWait = 0.0;
		CombinedJobController* aCJC = new CombinedJobController(theModel->context(), "delayAndDoJobFunctionDoer");
		DelayJob* aDJob = new DelayJob(theModel->context(), theWait);
		aCJC->AddJob(aDJob);
		aCJC->AddJob(new FunctionDoJob(theModel->context(), theFP));
		theModel->context()->engine()->ScheduleJobNow(aCJC);
	}

}