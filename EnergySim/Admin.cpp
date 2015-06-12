#include "stdafx.h"
#include<iostream>
#include <sstream>
#include <string>
#include "Admin.h"
#include <functional>
#include <fstream>

#include "CombinedJobController.h"
#include "SimModel.h"

using namespace std;
using namespace EnergySim;
using namespace std::placeholders;

namespace EnergySim
{
	ENERGYSIM_DLL_PUBLIC SetterHandler* gSetterHandler = new SetterHandler();

	SetterHandler* SetterHandler::getSetterHandler()
	{
		if (gSetterHandler == nullptr)
			gSetterHandler = new SetterHandler();
		return gSetterHandler;
	}

	class WakeUpSetterHandler : public IJob
	{
	private:
		SimModel* itsModel;
	public:
		WakeUpSetterHandler(SimModel* theModel)
		{
			itsModel = theModel; 
			_ctx = theModel->context();
		}
		virtual void Execute()
		{
			NotifyJobStarted();
			SetterHandler::getSetterHandler()->wakeUp(itsModel->context()->engine()->simulated_time());
			NotifyJobFinished();		
		}
	};


	void SetterHandler::triggerSetting(SimModel* model)
	{
	//	map <pair<string, double>, double*> callMap;

		for(auto a : callMap)
		{
			CombinedJobController* aCJC = new CombinedJobController(model->context(), a.first.first); 
			int time = *(a.second)*1000;
			aCJC->AddJob(new DelayJob(aCJC->context(), time)); 
			aCJC->AddJob(new WakeUpSetterHandler(model));
			model->context()->engine()->ScheduleJobNow(aCJC);
		}
	}

	SetterHandler::SetterHandler()
	{
		std::ifstream infile("M1.txt");
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream iss(line);
			string name;
			double value, time;
			if (!(iss >> name >> value >> time)) { break; } // error
			double* d = new double(time);
			callMap.insert(make_pair(make_pair(name, value), d));
		}
		startValue = 1000;
	}

	void SetterHandler::wakeUp(double time)
	{
		for (auto it1 : callMap)
		{
			// it1.first is the first key
			if (*it1.second < -10)
				continue;
			if (*it1.second < time + 0.1)
			{
				call(it1.first.first, it1.first.second);
				*it1.second=double(-11);
			}
		}
	}

	MyA::MyA(SetterHandler* aSS)
	{
		//aSS->bMap.insert(pair<long, std::function<void(int)>>(1, std::bind(&MyA::setMyInt, this, _1)));
	}
	MyB::MyB(SetterHandler* aSS)
	{
		//aSS->bMap.insert(pair<long, std::function<void(int)>>(2, std::bind(&MyB::setInt, this, _1)));
		aSS->bMap.insert(pair<long, ISetter*>(11, new RealSetter<int>(std::bind(&MyB::setInt, this, _1))));
		aSS->bMap.insert(pair<long, ISetter*>(12, new RealSetter<double>(std::bind(&MyB::setDouble, this, _1))));
		aSS->bMap.insert(pair<long, ISetter*>(13, new RealSetter<long>(std::bind(&MyB::setLong, this, _1))));
		aSS->bMap.insert(pair<long, ISetter*>(14, new RealSetter<bool>(std::bind(&MyB::setBool, this, _1))));
	}
	MyC::MyC(SetterHandler* aSS)
	{
		//aSS->bMap.insert(pair<long, std::function<void(int)>>(3, std::bind(&MyC::setInt, this, _1, 3)));
	}



}