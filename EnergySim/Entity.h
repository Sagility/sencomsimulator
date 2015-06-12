#pragma once
#include <vector>
#include <algorithm>
#include "config.h"
#include "SimLogEnvironment.h"
#include "SimEngine.h"
#include <string>
#include "Job.h"
//#include "CRResource.h"
//#include "Eventtypes.h"

using namespace std;

namespace EnergySim {

	extern const string NamedEntity;

	class IEntity
	{
	public:
		IEntity()
		{
			itsEID = itsUniqueID++;
			itsEntities.push_back(this);
			itsName = to_string(itsEID);
			IEvent::publish_entity(itsEID, itsName);
		}
		IEntity(string theName)
		{
			itsEID = itsUniqueID++;
			itsEntities.push_back(this);
			itsName = NamedEntity + theName;
			IEvent::publish_entity(itsEID, itsName);
		}
		IEntity(string theName, bool addUniqueID)
		{
			itsEID = itsUniqueID++;
			itsEntities.push_back(this);
			if (addUniqueID)
				itsName = NamedEntity + theName + to_string(itsEID);
			else
				itsName = NamedEntity + theName;
			IEvent::publish_entity(itsEID, itsName);
		}
		virtual ~IEntity()
		{
		}
		long eID() { return itsEID; }
		string name() { return itsName; }
		static IEntity* getByID(long theID);
		static IEntity* getByName(string theName);
	private:
		long itsEID;
		static long itsUniqueID;
		static vector<IEntity*> itsEntities; 
		string itsName;
	};
	enum ResourceState
	{
		RS_WORKING, RS_IDLE, RS_SEARCHING, RS_DOWN, RS_SETUP, RS_LOADING, RS_UNLOADING, RS_STARTING, RS_STARTED
	};
	class IRouteFollower;
	class Resource;
}

