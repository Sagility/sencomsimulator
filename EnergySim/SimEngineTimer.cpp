#include "stdafx.h"
#include "EnergySim.h"
#include "SimEngine.h"
#include "SimEngineTimer.h"

namespace EnergySim { 
	SimEngineTimer::SimEngineTimer(SimContext* theContext, double delay):_ctx(theContext), _delay(delay)
	{
	};
	void SimEngineTimer::Start()
	{
		if (_ctx != NULL && _ctx->engine()!=NULL)
		{
			ISimEngine* engine =_ctx->engine();
			_startedtime = engine->simulated_time();
			engine->ScheduleJob(new FinishTimerJob(_ctx,this), _delay);
		}
	}
	void SimEngineTimer::Preempt()
	{
		NotifyPreempted();
		if (_ctx != NULL && _ctx->engine()!=NULL)
		{
			ISimEngine* engine =_ctx->engine();
			// TODO
	
		}
	}

	void SimEngineTimer::Elapse()
	{
		NotifyElapsed();
		if (_ctx != NULL && _ctx->engine()!=NULL)
		{
			ISimEngine* engine =_ctx->engine();
			//TODO
		}
	}
	void FinishTimerJob::Preempt()
	{
		NotifyJobPreempted();
		NotifyJobFinished();
	}
	void FinishTimerJob:: Execute()
	{
		NotifyJobStarted();
		_timer->Elapse();
		NotifyJobFinished();
	}
}