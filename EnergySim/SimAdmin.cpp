#include "stdafx.h"

#include "SimAdmin.h"
#include <iostream>
#include <fstream>
#include "Jobs.h"
//#include "LogJob.h"
#include <regex>
#include "CRResource.h"



namespace EnergySim
{
	void Route::createThisJobOrder(Schedule* theS, Order* aOrder, CombinedJobController& cj1)
	{
		cj1.AddJob(new DelayJob(cj1.context(), (int)aOrder->Time));
		cj1.ID(aOrder->ID);
		for (Step* s : itsSteps)
			s->generateJobs(theS, cj1, aOrder);
	}

	Step* StepReader::readLine(string str)
	{
		stringstream ss(str); // Insert the string into a stream
		string buf;
		list<string> stepArguments;
		while (ss >> buf)
			stepArguments.push_back(buf);

		if (stepArguments.size() < 1)
			return NULL;
		return translateToStep(stepArguments);
	}


	void Step::generateJobs(Schedule* theS, CombinedJobController& theController, Order* theOrder)
	{
		//TODO theController.AddJob(new LogJob(itsContext, lotID, itsID, true));
	//	theController.AddJob(new WaitForScheduleJob(theS, theOrder->ID, itsID));
		generateSpecificJobs(theController);
	//	theController.AddJob(new PublishScheduleJob(theS, theOrder->ID, itsID));
		//TODO theController.AddJob(new LogJob(itsContext, lotID, itsID, false));
	}

	long SimAdmin::runModel()
	{
		list<CombinedJobController*> *aController = new list<CombinedJobController*>();

		for (Order *o : *itsOrders)
		{
			CombinedJobController* aCJC = new CombinedJobController(Context, "o->ID");
			for (Route *r : *itsRoutes)
			{
				if (r->name != o->Route)
					continue;
				r->createThisJobOrder(aScheduler, o, *aCJC);
				aController->push_back(aCJC);
				break;
			}
		}

		aScheduler->finish();
		return 0;
	}

	void SimAdmin::readRoute(string filename, string theName)
	{
		Route* aRoute = new Route();
		aRoute->name = theName;
		std::ifstream file(filename);
		std::string line;
		StepReader aSR;
		while (std::getline(file, line))
		{
			Step* aStep = aSR.readLine(line);
			aRoute->addStep(aStep);
		}
		file.close();
		itsRoutes->push_back(aRoute);
	}
	void SimAdmin::readFunctionAttributes(string filename)
	{
		IParser* aP = IParser::createParser(itsHandler);
		std::ifstream file(filename);
		std::string line;
		while (std::getline(file, line))
		{
		}
		file.close();
	}
	void SimAdmin::readExpressionAttributes(string filename)
	{

	}

}