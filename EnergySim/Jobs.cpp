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

namespace EnergySim
{
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
	}
	void PublishPreReqDoneJob::Execute()
	{
		NotifyJobStarted();
		PreReqs::done(itsPreReqID);
		NotifyJobFinished();
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