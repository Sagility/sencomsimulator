#include "Stdafx.h"
#include "Conveyor.h"
#include "SimModel.h"
#include <functional>
#include <sstream>
#include <string>

namespace EnergySim
{
	//class ENERGYSIM_DLL_PUBLIC FunctionDoJob : public IJob
	//{
	//public:
	//	//constructor
	//	FunctionDoJob(SimContext *context, std::function<void()> theFP) :IJob(context)
	//	{
	//		fp = theFP;
	//	}
	//	~FunctionDoJob()
	//	{
	//	}
	//	virtual string ToString()
	//	{
	//		return "FunctionDoJob\n";
	//	}
	//	virtual void Execute()
	//	{
	//		fp();
	//	};
	//	virtual string classname() { return "FunctionJob"; }
	//private:
	//	std::function<void()> fp;
	//};
	//void delayAndDoJobFunction(std::function<void()> theFP, SimModel* theModel, double theWait)
	//{
	//	if (theWait < 0.0001)
	//		theWait = 0.0;
	//	CombinedJobController* aCJC = new CombinedJobController(theModel->context(), "delayAndDoJobFunctionDoer");
	//	DelayJob* aDJob = new DelayJob(theModel->context(), theWait);
	//	aCJC->AddJob(aDJob);
	//	aCJC->AddJob(new FunctionDoJob(theModel->context(), theFP));
	//	theModel->context()->engine()->ScheduleJobNow(aCJC);
	//}

	class ConveyorFollowerConveyorInfo
	{
	public:
		ConveyorFollower* itsFollower;
		Conveyor* itsConveyor;
		double itsLastPos;
		double itsLastTime;
		double nextStopTime;


	};


}