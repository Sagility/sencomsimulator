#include "Stdafx.h"
#include <iostream>
#include <string>
#include <regex>
#include "Attributes.h"
#include "Attributes_Internal.h"
//#include "Parsing.h"

namespace EnergySim
{
	void AttributeHandler::OnNextStep(ISimEngine *theEngine, NextStepEventArgs *theArgs)
	{
		if (theEngine->simulated_time() > 20000)
			return;
		replace("simdelta", new ConstantValue(theArgs->delta()));
		updateAllAttributes();
		double d = this->getAttribute("cost");
		IEvent::publishEvent(ET_SIMULATION_UPDATE, vector<string>(0));
		IEvent::publishEvent(ET_SIMULATION_COST, vector<string>(1) = { std::to_string(d) });
	}


	double AttributeLookUpValue::value()
	{
		 return itsHandler->getAttribute(itsName);
	}
	void BaseAttributeValueHolder::replaceBaseValue(BaseValue* theBV)
	{
		itsExpression = theBV;
	}
	AttributeFunctionDelegate AttributeFunctionHandler::getFunctionByName(string name)
	{
		if (name == "lessthan")
			return &AttributeFunctionHandler::lessthan;
		if (name == "memoryswitch")
			return &AttributeFunctionHandler::memoryswitch;
		if (name == "checkone")
			return &AttributeFunctionHandler::checkone;
		return &AttributeFunctionHandler::defaultFunction;
	}
	double AttributeFunctionHandler::lessthan(vector<double> arguments)
	{
		if (arguments.size() < 2)
			return 1;
		if (arguments[0] < arguments[1])
			return 1;
		return 0;
	}
	double AttributeFunctionHandler::memoryswitch(vector<double> arguments)
	{
		return 1;
		if (arguments.size() < 3)
			return 1;
		if (arguments[0] > 0.5)
			return 1;
		if (arguments[1] > 0.5)
			return 0;
		return arguments[2];
	}
	double AttributeFunctionHandler::random(vector<double> arguments)
	{
		double aD = rand();
		aD = aD / RAND_MAX;
		if (arguments.size() <1)
			return aD;
		aD = aD * arguments[0];
		if (arguments.size() < 2)
			return aD;
		aD = aD + arguments[1];
		return aD;
	}
	double AttributeFunctionHandler::lookUp(vector<double> arguments)
	{
		int tableID = arguments[0];
		return 1000000;
	}
	double AttributeFunctionHandler::checkone(vector<double> arguments)
	{
			return 1000000;
	}
	double AttributeFunctionHandler::defaultFunction(vector<double> arguments)
	{
		return 1;
	}
	void AttributeHandler::updateSimVariabels(double time)
	{
		lastSimTime = simTime;
		simTime = time;
		simDelta = simTime - lastSimTime;
		replace("simtime", new ConstantValue(simTime));
		replace("simdelta", new ConstantValue(simDelta));
		reportAllAttributes(time);
	}
	//void BaseAttributeValue::report(AttributeReporter* theReporter)
	//{
	//	if (itsReport == Reporting::Full)
	//		itsID = theReporter->reportIteration(itsID, itsName, itsNewValue);
	//}
}

