#include "stdafx.h"

#include "SimAdmin.h"
#include <iostream>
#include <string>
#include <fstream>
#include "Jobs.h"
//#include "LogJob.h"
#include <regex>

#include "CRResource.h"
#include "SimModel.h"
/*
Be able to save values from routefollower to attributes
ADD WAIT FOR TIME STEP
Order has starttime and ID
*/

namespace EnergySim
{
	class DelayStep : public Step
	{
		long itsDelay;
	public: DelayStep(long theDelay, long theID, SimContext* theContext)
	{
		itsDelay = theDelay;
		itsID = theID;
		itsContext = theContext;
	}
	public:  void generateSpecificJobs(CombinedJobController& theController)
	{
		theController.AddJob(new DelayJob(itsContext, (int)itsDelay));
	}
	};
	class GetStep : public Step
	{
		long itsRes;
	public: GetStep(long theRes, long theID, SimContext* theContext)
	{
		itsRes = theRes;
		itsID = theID;
		itsContext = theContext;
	}
	public: void generateSpecificJobs(CombinedJobController& theController)
	{
		theController.AddJob(new ClaimJob<long>(NULL, itsRes));
	}
	};
	class GetProcessStep : public Step
	{
		long itsRes;
	public: GetProcessStep(Process* theProcess, SimModel* theModel)
	{
		itsProcess = theProcess;
		itsModel = theModel;
	}
	public: void generateSpecificJobs(CombinedJobController& theController);
	private:
		Process* itsProcess;
		SimModel* itsModel;
	};
	class SetupStep : public Step
	{
		long itsRes;
	public: SetupStep(long theRes, long theID, SimContext* theContext)
	{
		itsRes = theRes;
		itsID = theID;
		itsContext = theContext;
	}
	public: void generateSpecificJobs(CombinedJobController theController)
	{
		theController.AddJob(new ClaimJob<long>(NULL, itsRes));
	}
	};
	class FreeStep : public Step
	{
		long itsRes;
	public: FreeStep(long theRes, long theID, SimContext* theContext)
	{
		itsRes = theRes;
		itsID = theID;
		itsContext = theContext;
	}
	public: void generateSpecificJobs(CombinedJobController& theController)
	{
		theController.AddJob(new ReleaseJob<long>(itsContext, itsRes));
	}
	};
	class FreeProcessStep : public Step
	{
		long itsProcessID;
		long itsRouteFollowerID;
		SimModel* model;
	public: FreeProcessStep(long theProcessID, SimModel* theModel)
	{
		itsProcessID = theProcessID;
		itsRouteFollowerID = 0;
		model = theModel;
	}
	public: void generateSpecificJobs(CombinedJobController& theController);

	};
	class SetStep : public Step
	{
		double itsValue;
		string itsAttribute;
		AttributeHandler* itsHandler;
	public: SetStep(string theAttribute, double theValue, long theID, AttributeHandler* theHandler, SimContext* theContext)
	{
		itsValue = theValue;
		itsAttribute = theAttribute;
		itsID = theID;
		itsHandler = theHandler;
		itsContext = theContext;
	}
	public: void generateSpecificJobs(CombinedJobController& theController)
	{
		theController.AddJob(new SetAttributeJob(itsHandler, itsAttribute, itsValue));
	}
	};
	class WaitStep : public Step
	{
		double itsLowValue, itsHighValue;
		string itsAttribute;
		AttributeHandler* itsHandler;
	public: WaitStep(string theAttribute, double theLowValue, double theHighValue, long theID, AttributeHandler* theHandler, SimContext* theContext)
	{
		itsLowValue = theLowValue;
		itsHighValue = theHighValue;
		itsAttribute = theAttribute;
		itsID = theID;
		itsHandler = theHandler;
		itsContext = theContext;
	}
	public: void generateSpecificJobs(CombinedJobController& theController)
	{
		theController.AddJob(new WaitForAttributeJob(itsHandler, itsAttribute, itsLowValue, itsHighValue));
	}
	};
	class LogStep : public Step
	{
		double itsValue;
		string itsMsg;
	public: LogStep(string theMsg, long theID, SimContext* theContext)
	{
		itsMsg = theMsg;
		itsID = theID;
		itsContext = theContext;
	}
	public: void generateSpecificJobs(CombinedJobController& theController)
	{
		theController.AddJob(new LogJob(itsContext, itsMsg));
	}
	};
	class PublishPreReqDoneStep : public Step
	{
		long itsPreReqDone;
		string itsMsg;
	public: PublishPreReqDoneStep(long thePreReq, long theID, SimContext* theContext)
	{
		itsPreReqDone = thePreReq;
		itsID = theID;
		itsContext = theContext;
	}
	public: void generateSpecificJobs(CombinedJobController& theController);
	};
	class WaitForPreReqDoneStep : public Step
	{
		list<long> itsPreReqNeed;
	public: WaitForPreReqDoneStep(list<long> thePreReq, long theID, SimContext* theContext)
	{
		itsPreReqNeed = thePreReq;
		itsID = theID;
		itsContext = theContext;
	}
	public: void generateSpecificJobs(CombinedJobController& theController);
	};
	class WaitForScheduleStep : public Step
	{
		long itsLotID, itsStepID;
		Schedule* itsSchedule;
	public: WaitForScheduleStep(long theLotID, long theStepID, long theID, SimContext* theContext, Schedule* theSchedule)
	{
		itsLotID = theLotID;
		itsStepID = theStepID;
		itsID = theID;
		itsContext = theContext;
		itsSchedule = theSchedule;
	}
	public: void generateSpecificJobs(CombinedJobController& theController);
	};
	class PublishScheduleStep : public Step
	{
		long itsLotID, itsStepID;
		Schedule* itsSchedule;
	public: PublishScheduleStep(long theLotID, long theStepID, long theID, SimContext* theContext, Schedule* theSchedule)
	{
		itsLotID = theLotID;
		itsStepID = theStepID;
		itsID = theID;
		itsContext = theContext;
		itsSchedule = theSchedule;
	}
	public: void generateSpecificJobs(CombinedJobController& theController);
	};
	void GetProcessStep::generateSpecificJobs(CombinedJobController& theController)
	{
		theController.AddJob(new WaitForResourcesJob(itsProcess, itsModel, theController.ID()));
	}
	void PublishPreReqDoneStep::generateSpecificJobs(CombinedJobController& theController)
	{
		theController.AddJob(new PublishPreReqDoneJob(itsPreReqDone));
	}
	void WaitForPreReqDoneStep::generateSpecificJobs(CombinedJobController& theController)
	{
		theController.AddJob(new WaitForPreReqDoneJob(itsPreReqNeed));
	}
	void WaitForScheduleStep::generateSpecificJobs(CombinedJobController& theController)
	{
		IJob* j = new WaitForScheduleJob(itsSchedule, theController.ID(), itsStepID);
		j->set_context(itsContext);
		theController.AddJob(j);
	}
	void PublishScheduleStep::generateSpecificJobs(CombinedJobController& theController)
	{
		IJob* j = new  PublishScheduleJob(itsSchedule, theController.ID(), itsStepID);
		j->set_context(itsContext);
		theController.AddJob(j);
	}
	void FreeProcessStep::generateSpecificJobs(CombinedJobController& theController)
	{
		IJob* j = new FinishProcessJob(itsProcessID, theController.ID(), model);
		theController.AddJob(j);
	}
	Step* StepReader::translateToStep(list<string> arg)
	{
		string aStepName = arg.front();
		arg.pop_front();
		int i, i1,i2 = 0;
		double d1, d2 = 0;
		string str = "";
		if (aStepName == EnergySim_NameConstants::DELAYSTEP)
		{
			if (arg.size() > 0)
				i = stoi(arg.front(), NULL);
			return new DelayStep(i, 0, itsCtx);
		}
		if (aStepName == EnergySim_NameConstants::WAIT_ATTRIBUTE_STEP)
		{
			if (arg.size() > 0)
				str = arg.front();
			arg.pop_front();
			if (arg.size() > 0)
				d1 = stod(arg.front(), NULL);
			arg.pop_front();
			if (arg.size() > 0)
				d2 = stod(arg.front(), NULL);
			return new  WaitStep(str, d1, d2, 0, itsHandler, itsCtx);
		}
		if (aStepName == EnergySim_NameConstants::PUBLISH_ATTRIBUTE_STEP)
		{
			if (arg.size() > 0)
				str = arg.front();
			arg.pop_front();
			if (arg.size() > 0)
				d1 = stod(arg.front(), NULL);
			return new SetStep(str, d1, 0, itsHandler, itsCtx);
		}
		if (aStepName == EnergySim_NameConstants::PUBLISH_ROUTEFOLLOWER_TO_ATTRIBUTE_STEP)
		{
			// 
		}
		if (aStepName == EnergySim_NameConstants::WAIT_SCHEDULE_STEP)
		{
			if (arg.size() > 0)
				i1 = stoi(arg.front(), NULL);
			arg.pop_front();
			//if (arg.size() > 0)
				//i2 = stoi(arg.front(), NULL);
			return new  WaitForScheduleStep(1, i1, 0, itsCtx, itsSchedule);			
		}
		if (aStepName == EnergySim_NameConstants::PUBLISH_SCHEDULE_STEP)
		{
			if (arg.size() > 0)
				i1 = stoi(arg.front(), NULL);
			arg.pop_front();
		//	if (arg.size() > 0)
			//	i2 = stoi(arg.front(), NULL);
			return new  PublishScheduleStep(1, i1, 0, itsCtx, itsSchedule);
		}
		if (aStepName == EnergySim_NameConstants::WAIT_PREREQ_STEP)
		{
			list<long> aList;
			while (arg.size() > 0)
			{
				i = stoi(arg.front(), NULL);
				aList.push_back(i);
				arg.pop_front();
			}
			return new WaitForPreReqDoneStep(aList, 0, itsCtx);
		}
		if (aStepName == EnergySim_NameConstants::PUBLISH_PREREQ_STEP)
		{
			if (arg.size() > 0)
				i = stoi(arg.front(), NULL);
			return new PublishPreReqDoneStep(i, 0, itsCtx);
		}
		
		if (aStepName == EnergySim_NameConstants::GET_STEP)
		{
			if (arg.size() > 0)
				i = stoi(arg.front(), NULL);
			arg.pop_front();
			// i is an identifier used later to tell which resources should be released.
			// i is an ProcessID, 
			// I need one ID more, lot = routefollowerID

			long processID = i; //stoi(arg.front(), NULL);
			//arg.pop_front();

			Process* aP = new Process();
			aP->processID = processID;
			long ConcreteProcessID = 0;
			list<pair<long, Resource*>>* aRList = new list<pair<long, Resource*>>();
			
			ConcreteProcess* aCP = NULL;
			while (arg.size() > 0)
			{
				if (arg.front() == "OR")
				{
					aCP = new ConcreteProcess(aRList, ConcreteProcessID++);
					aP->itsAlternates.push_back(aCP);
					aRList = new list<pair<long, Resource*>>();
				}
				else
				{
					Resource* aRes = NULL;
					IEntity* IE = IEntity::getByName(arg.front());

					aRes = dynamic_cast < Resource* >(IE);

					for (Resource* r : model->resources.itsValue)
						if (r->name() == arg.front())
						{
							aRes = r;
							break;
						}
					arg.pop_front();
					long nb = stoi(arg.front(), NULL);
					pair<long, Resource*> aPair;
					aPair.first = nb;
					aPair.second = aRes;
					if(aRes!=NULL)
						aRList->push_back(aPair);
				}
				arg.pop_front();
			}
			if (!aRList->empty())
				aP->itsAlternates.push_back(new ConcreteProcess(aRList, ConcreteProcessID++));
			return new GetProcessStep(aP, model);

		}
		if (aStepName == EnergySim_NameConstants::FREE_STEP)
		{
			if (arg.size() > 0)
				i = stoi(arg.front(), NULL);
			arg.pop_front();
			//FreeStep(, SimContext* theContext)
			return new FreeProcessStep(i, model);
		}
		if (aStepName == EnergySim_NameConstants::STATE_SET)
		{

		}
		if (aStepName == EnergySim_NameConstants::WAIT_SETUP_STEP)
		{
		}
		if (aStepName == EnergySim_NameConstants::LOGSTEP)
		{
			if (arg.size() > 0)
				str = arg.front();
			return new LogStep(str, 0, itsCtx);
		}
		if (aStepName == EnergySim_NameConstants::EVENTSTEP)
		{
		}
		return NULL;
	}
}