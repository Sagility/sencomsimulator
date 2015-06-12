#pragma once
using namespace std;

namespace EnergySim {

	class Clock;
	enum EventType : long
	{
		ET_FIRST,
		ET_PROCESS_START, ET_PROCESS_END,
		ET_RESOURCE_SETUP_CHANGE, ET_RESOURCE_STATE_CHANGE, ET_RESOURCE_CLAIM, ET_RESOURCE_RELEASE, ET_RESOURCE_USES, ET_RESOURCE_CAPACITY,
		ET_LOAD_START_STEP, ET_LOAD_END_STEP, ET_LOAD_CREATED, ET_LOAD_FINISHED,
		ET_JOB_START,ET_JOB_END,
		ET_SIMULATION_START, ET_SIMULATION_END, ET_SIMULATION_UPDATE, ET_SIMULATION_COST,
		ET_LAST
	};
	class ENERGYSIM_DLL_PUBLIC IEvent
	{
	public:
		static void publish_entity(long id, string name);
		static void publishEvent(EventType theEventType, long MainID, vector<long> IDs, double Time);
		static void publishEvent(EventType theEventType, long MainID, vector<long> IDs);
		static void publishEvent(EventType theEventType, vector<string> IDs);
		virtual ~IEvent();
		static void flush();

		static Clock* itsClock;
	private:
		static void init();
		static void writeDefinition(ofstream& theStream, long theValue, string theName);
		
		static unsigned _int8* aBuffer;
		static int counter;
		static string fileName;
		static bool started;
		static bool stringStarted;
		static bool startedEnt;
	};
}

