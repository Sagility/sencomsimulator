
#include "stdafx.h"
#include<iostream>
#include "SimLogEnvironment.h"
#include "EnergySim.h"
#include "SimEngine.h"
#include "CombinedJobController.h"
#include "ClaimRelease.h"
#include "Schedule.h"
#include "SimModel.h"


namespace EnergySim
{
	SchedElement::SchedElement(long a, long b, long c, long d, list<long> e)
	{
		lotID = a;
		processID = b;
		time = c;
		processModeID = d;
		resID = e;
	}
	void Schedule::markJobAsStarted(long lotID, long stepID)
	{
		pair<long, long> *aJob = new pair<long, long>(lotID, stepID);
		pair<double, pair<long, long>> *aPP = new pair<double, pair<long, long>>(300.0, *aJob);

		for (SchedElement *aSE : *itsInSchedule)
		{
			if (aJob->first == aSE->lotID)
				if (aJob->second == aSE->processID)
			{
				itsInSchedule->remove(aSE);
				break;
			}
		}

		bool keepGoing = true;
		while (keepGoing)
		{
			keepGoing = false;
			for (WaitForScheduleJob* j : *itsWaiters)
			{
				if (canIStart(j->itsLotID, j->itsStepID))
				{
					j->finish();
					itsWaiters->remove(j);
					keepGoing = true;
					break;
				}
			}
		}
	}


	void Schedule::printOutSchedule(string s1, string s2)
	{
		static bool started = false;
		string myFileName = "fileNameout.txt";
		myFileName = "outSchedule.txt";
		ofstream pFile;
		if (!started)
			pFile.open(myFileName.c_str(), std::ios_base::trunc);
		else
			pFile.open(myFileName.c_str(), std::ios_base::app);
		started = true;

		pFile << s1 << s2 << endl;
		
		pFile.close();

	}

	void Schedule::filterOutConcreteProcess()
	{
		for (Order* o : model->orders.itsValue)
		{
			

		}

	}


}