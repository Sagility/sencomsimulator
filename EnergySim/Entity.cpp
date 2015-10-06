#pragma once
#include "stdafx.h"
//#include "Entity.h"
#include <memory>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "Entity.h"
#include "CRResource.h"
#include "Routes.h"
#include "TimeValues.h"
#include <fstream>

using namespace std;

// WORK IN PROGRESS

namespace EnergySim
{
	const string NamedEntity = "NamedEntity_";
	vector<IEntity*> IEntity::itsEntities = vector<IEntity*>();
	IEntity* IEntity::getByName (string theName)
	{
		for (IEntity* aIE : itsEntities)
			if (aIE->itsName == theName)
					return aIE;
		return NULL;
	}
	IEntity* IEntity::getByID(long theID)
	{
		for (IEntity* aIE : itsEntities)
			if (aIE->itsEID == theID)
				return aIE;    
		return NULL;
	}

	IEvent::~IEvent() { flush(); }
	unsigned _int8* IEvent::aBuffer = (unsigned _int8*) malloc(1100000);
	int IEvent::counter = 0;
	string IEvent::fileName = "buffer.abc";
	bool IEvent::started = false;
	bool IEvent::stringStarted = false;
	bool IEvent::startedEnt = false;


	Clock* IEvent::itsClock = NULL;
	void IEvent::publishEvent(EventType theEventType, long MainID, vector<long> IDs)
	{
		publishEvent(theEventType, MainID, IDs, itsClock->getTime());
	}


	/* File structure
	Long	= size of definition including itself
	Long    = ID of deinition
	String	= Name
	Long	= 0
	Data
	*/
	void IEvent::writeDefinition(ofstream& theStream, long theValue, string theName)
	{
		long aSize = theName.size();
 		theStream << theValue << " ";
		theStream << theName << endl;
	}

	void IEvent::publishEvent(EventType theEventType, vector<string> IDs)
	{
		return;
		string myFileName = fileName;
		myFileName = "events.txt";// +myFileName;
		ofstream pFile;
		if ( !stringStarted  )
			pFile.open(myFileName.c_str(),std::ios_base::trunc);
		else
			pFile.open(myFileName.c_str(), std::ios_base::app);
		stringStarted = true;

		int i = theEventType;
		string s = "";
		switch (i)
		{
		case EventType::ET_FIRST:
			s= "ET_FIRST";
			break;
		case EventType::ET_JOB_END:
			s= "ET_JOB_END";
			break;
		case EventType::ET_JOB_START:
			s= "ET_JOB_START";
			break;
		case EventType::ET_LOAD_CREATED:
			s= "ET_LOAD_CREATED";
			break;
		case EventType::ET_LOAD_END_STEP:
			s= "ET_LOAD_END_STEP";
			break;
		case EventType::ET_LOAD_FINISHED:
			s= "ET_LOAD_FINISHED";
			break;
		case EventType::ET_LOAD_START_STEP:
			s= "ET_LOAD_START_STEP";
			break;
		case EventType::ET_PROCESS_START:
			s= "ET_PROCESS_START";
			break;
		case EventType::ET_PROCESS_END:
			s= "ET_PROCESS_END";
			break;
		case EventType::ET_RESOURCE_CLAIM:
			s= "ET_RESOURCE_CLAIM";
			break;
		case EventType::ET_RESOURCE_RELEASE:
			s= "ET_RESOURCE_RELEASE";
			break;
		case EventType::ET_RESOURCE_SETUP_CHANGE:
			s= "ET_RESOURCE_SETUP_CHANGE";
			break;
		case EventType::ET_RESOURCE_STATE_CHANGE:
			s= "ET_RESOURCE_STATE_CHANGE";
			break;
		case EventType::ET_RESOURCE_USES:
			s= "ET_RESOURCE_USES";
			break;
		case EventType::ET_SIMULATION_END:
			s= "ET_SIMULATION_END";
			break;
		case EventType::ET_SIMULATION_START:
			s= "ET_SIMULATION_START";
			break;
		case EventType::ET_RESOURCE_CAPACITY:
			s = "ET_RESOURCE_CAPACITY";
			break;
		case EventType::ET_SIMULATION_COST:
			s = "ET_SIMULATION_COST";
			break;
		case EventType::ET_SIMULATION_UPDATE:
			s = "ET_SIMULATION_UPDATE";
			break;
			
		case EventType::ET_LAST:
			s= "ET_LAST";
			break;
		default:
			s= "ET_UNKNOWN";
		}
		for each (string str in IDs)
		{
			s += "," + str;
		}
		s += "," + std::to_string(itsClock->getTime());
		pFile << s;
		pFile  << endl;
		pFile.close();
	}


	void IEvent::publishEvent(EventType theEventType, long MainID, vector<long> IDs, double Time)
	{
		return;
		unsigned _int8 * aPtr = aBuffer;
		long aTmp = IDs.size();

		if (theEventType == EventType::ET_PROCESS_START)
			aTmp = aTmp;
		if (theEventType == EventType::ET_SIMULATION_END)
		{
			aTmp = aTmp;
		//	cout << "TIME IS: "   << Time << endl;
		}
		if (Time > 14000)
			aTmp = aTmp;
		if (theEventType == EventType::ET_PROCESS_END)
			aTmp = aTmp;
		if (theEventType == EventType::ET_PROCESS_START)
			if (IDs.size() > 0)
				if (IDs.front() == 3)
					aTmp = aTmp;
		/*
			Format of event
			Long				Size off vector IDS
			Sizeof(Eventtype)	Eventtype
			Long				MainID
			Vector<long>		IDs
			double				Timestamp
		*/
		aPtr += counter;
		_memccpy(aPtr, &aTmp, 1, 4);
		aPtr += 4;
		_memccpy(aPtr, &theEventType, 1, sizeof(EventType));
		aPtr += 4;
		_memccpy(aPtr, &MainID, 1, 4);
		aPtr += 4;
		if (IDs.size()>0)
		{
			_memccpy(aPtr, &IDs, 1, 4 * IDs.size());
			aPtr += IDs.size() * 4;
		}
		_memccpy(aPtr, &Time, 1, 8);
		aPtr += 8;
		counter += IDs.size() * 4 + 20; 

		flush();
	}
	void IEvent::flush()
	{
		//return;
		if (counter < 1000000)
			return;
		FILE* pFile;
		if (started)
			pFile = fopen(fileName.c_str(), "ab");
		else
		{
			pFile = fopen(fileName.c_str(), "wb");
			init();
			started = true;
		}
		fwrite(aBuffer, 1, counter, pFile);
		fclose(pFile);
		counter = 0;
	}
	void IEvent::publish_entity(long id, string name)
	{
		return;
		if (startedEnt)
		{
			std::ofstream myFile("Entities" + fileName, ios::app | ios::out | ios::binary);
			int count = 0;
			myFile << id;
			myFile << name;
			myFile.close();
		}
		else
		{
			startedEnt = true;
			std::ofstream myFile("Entities" + fileName, ios::out | ios::binary);
			int count = 0;
			myFile << id;
			myFile << name;
			myFile.close();
		}
	}
	void IEvent::init()
	{
		return;
		std::ofstream myFile("EventTypes" + fileName, ios::out | ios::binary);
		int count = 0;
		for (int i = EventType::ET_FIRST; i < EventType::ET_LAST; i++)
		{
			switch (i)
			{
			case EventType::ET_FIRST:
				writeDefinition(myFile, i, "ET_FIRST");
				break;
			case EventType::ET_JOB_END:
				writeDefinition(myFile, i, "ET_JOB_END");
				break;
			case EventType::ET_JOB_START:
				writeDefinition(myFile, i, "ET_JOB_START");
				break;
			case EventType::ET_LOAD_CREATED:
				writeDefinition(myFile, i, "ET_LOAD_CREATED");
				break;
			case EventType::ET_LOAD_END_STEP:
				writeDefinition(myFile, i, "ET_LOAD_END_STEP");
				break;
			case EventType::ET_LOAD_FINISHED:
				writeDefinition(myFile, i, "ET_LOAD_FINISHED");
				break;
			case EventType::ET_LOAD_START_STEP:
				writeDefinition(myFile, i, "ET_LOAD_START_STEP");
				break;
			case EventType::ET_PROCESS_START:
				writeDefinition(myFile, i, "ET_PROCESS_START");
				break;
			case EventType::ET_PROCESS_END:
				writeDefinition(myFile, i, "ET_PROCESS_END");
				break;
			case EventType::ET_RESOURCE_CLAIM:
				writeDefinition(myFile, i, "ET_RESOURCE_CLAIM");
				break;
			case EventType::ET_RESOURCE_RELEASE:
				writeDefinition(myFile, i, "ET_RESOURCE_RELEASE");
				break;
			case EventType::ET_RESOURCE_SETUP_CHANGE:
				writeDefinition(myFile, i, "ET_RESOURCE_SETUP_CHANGE");
				break;
			case EventType::ET_RESOURCE_STATE_CHANGE:
				writeDefinition(myFile, i, "ET_RESOURCE_STATE_CHANGE");
				break;
			case EventType::ET_RESOURCE_USES:
				writeDefinition(myFile, i, "ET_RESOURCE_USES");
				break;
			case EventType::ET_SIMULATION_END:
				writeDefinition(myFile, i, "ET_SIMULATION_END");
				break;
			case EventType::ET_SIMULATION_START:
				writeDefinition(myFile, i, "ET_SIMULATION_START");
				break;
			case EventType::ET_RESOURCE_CAPACITY:
				writeDefinition(myFile, i, "ET_RESOURCE_CAPACITY");
				break;
			case EventType::ET_LAST:
				writeDefinition(myFile, i, "ET_LAST");
				break;
			default:
				writeDefinition(myFile, i, "ET_UNKNOWN");
			}
		}
		myFile.close();
	}



}


