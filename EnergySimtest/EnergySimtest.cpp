// EnergySimtest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include<string>

#include "SimLogEnvironment.h"

#include "EnergySim.h"
#include "SimEngine.h"
#include "CombinedJobController.h"
#include "ClaimRelease.h"
#include "Parsing.h"
#include "Jobs.h"
#include "TimeValues.h"
#include "SimAdmin.h"
#include "Entity.h"
#include "SimModel.h"
#include <ctime>
#include "Admin.h"
#include "FileReader.h"

#include "ComplexResources.h"			// PROJECT DEPENDENT	
#include "Counter.h"					// PROJECT DEPENDENT

//#include "xercesc\parsers\XercesDOMParser.hpp"
//#include "xercesc\dom\DOM.hpp"
//#include "xercesc\sax\HandlerBase.hpp"
//#include "xercesc\util\XMLString.hpp"
//#include "xercesc\util\PlatformUtils.hpp"
#include<iostream>
#include "SemconInterface.h"

using namespace SemconInterface;

using namespace std;
using namespace EnergySim;


void writeLog(int i)
{
//	return;
	ofstream myfile;
	myfile.open("mytestlog.txt",std::ios::app);
	myfile << i;
	myfile << "\n";
	myfile.close();
	return;
}
// int _tmain(int argc, _TCHAR* argv[])
//int main2(int argc, char* argv[]);
int main3(int argc, char* argv[]);


int main(int argc, char* argv[])
{
	return main3(argc, argv);


	return SemconInterface::internalMain(argc, argv);
	//return main3(argc, argv);
}

/*
int main2(int argc, char* argv[])
{
	writeLog(10);
	SimModel* aModelino = new SimModel();
	writeLog(11);
	SiloPlant*  aSP = new SiloPlant(aModelino);
	writeLog(12);
	string name = string(argv[1]);
	writeLog(14);
	try
	{
		aSP->readSiloPlant(name);
		writeLog(15);
		aSP->start();
		writeLog(171);
		aModelino->runModel();
		writeLog(172);
	}
	catch (...)
	{
		writeLog(666);
		aSP->writeResult(name + "_result.txt", false);
		return 0;
	}
	writeLog(18);
	aSP->writeResult(name + "_result.txt", true);
	writeLog(19);
	return 0;
}
*/


class LoggerData
{
public:
	void start(){};
	void end(){};
};

class Logger
{
public:
	Logger(LoggerData* theData)
	{
		itsData = theData;
		itsData->start();
	}
	~Logger()
	{
		itsData->end();
	}
private:
	LoggerData* itsData;
};

void createLogger()
{
	static LoggerData* aLD = new LoggerData();
	Logger aLog(aLD);


	int i = 23;

	if (i == 12)
	{


	}
	return;
}




void doTest(string s)
{
	string ret = s;
	int k = 23;
}
int main3(int argc, char* argv[])
{
	createLogger();
	writeLog(10);
	SimModel* aM = new SimModel();
	writeLog(11);
	string name = string(argv[1]);

	aM->readModel();
	writeLog(12);
	try
	{
		for (int i = 0; i < 5; i++)
		{
			string s = to_string(i);
			std::function<void()> f_func2 = std::bind(doTest, s);
			delayAndDoJobFunction(f_func2, aM, 12);
		}

		std::function<void()> f_funcen = std::bind(doTest, "4");
		aM->runModel();
		double slutet = aM->context()->engine()->simulated_time();
		AttributeHandler* aAH =  aM->itsParser.itsValue->getAttributeHandler();


		delete aM;
		return 0;
	}
	catch (...)
	{
		writeLog(666);
		return 0;
	}

	writeLog(18);
	writeLog(19);
	return 0;
}