#pragma once
#include "CRResource.h"
using namespace std;
namespace EnergySim
{
	void writeMyLog(string s);
	class StateValue;
	class SiloOrder;
	class PlantLine;
	class SiloPlant;
	class PackingMachine;
	class Flow;
	class ENERGYSIM_DLL_PUBLIC Silo
	{
	public:
		Silo(double size, SimModel* theModel);
		StateValue* itsStateValue;
		string		itsLastGrade;
		double cleaningTime;
		double density;
		double sampleTime;
		string name;
		double samplePercentage;
		bool sampled;
		bool inputBeingUsed;
		bool outputBeingUsed = false;
		void full();
		bool empty(){
			return true;
		}
		bool used(){
			return _inUse;
		}
		Flow* startUsing(string theGrade, SiloOrder* theOrder);
		double startFillingTime;
		void use(bool value);
		void clean() { itsLastGrade = ""; }
		bool _used = false;
		bool _inUse = false;
		map<string, double> itsGradeSpeed = map<string, double>();
		SimModel* itsModel;
		PackingMachine* currentPacker = NULL;
	};
	class Flow;
	class PackingMachine
	{
	public:
		string	type;
		string siloName; // Changed 150908
		string name;
		double capacity;
		double streamFactor;
		bool used = false;
		void lookForWork();
		void donePacking(double amountPacked);
		void pack(Silo* theSilo, double theAmount, SiloOrder* theSiloOrder){};
		Flow* itsFlow = NULL;
		SiloPlant* itsPlant;
		SiloOrder* currentOrder = NULL;
		Silo* currentSilo = NULL;
	};
	enum GradeMixture
	{
		OK,
		OK_IF_CLEANED,
		NOK
	};
	class SiloOrder;
	class ENERGYSIM_DLL_PUBLIC PlantLine 
	{
	public:
		PlantLine(ISimEngine* theEngine, SiloPlant* thePlant, string theName) { itsEngine = theEngine; itsPlant = thePlant; name = theName; }
		void processNextOrder();
		void processNextOrderUpdate(int theCounter);
		void scheduleNextOrderUpdate(double theTime);
		void addSilo(Silo* theSilo){ itsSilos.push_back(theSilo); }
		void addMachine(Silo* theSilo){ itsOutputMachines.push_back(theSilo); }
		void addOrder(SiloOrder* theSOrder){ itsOrders.push_back(theSOrder); }
		friend SiloPlant;
		SiloPlant* itsPlant;
		string name="EMPTY";
		int itsNextUpdateCounter=1;
		int itsNextDoneUpdateCounter = 0;
		double itsNextUpdateTime = 100000.0;
		void finished()	{}		
		Flow* itsCurrentFlow=NULL;
		SiloOrder* itsCurrentOrder=NULL;
		Silo* itsCurrentSilo = NULL;
		ISimEngine* itsEngine;
		list<Silo*> itsSilos = list<Silo*>();
		list<Silo*> itsOutputMachines = list<Silo*>();
		list<SiloOrder*> itsOrders = list<SiloOrder*>();
	};
	class ENERGYSIM_DLL_PUBLIC SiloPlant
	{
	public:
		SiloPlant(SimModel* theModel){ itsModel = theModel; }
		~SiloPlant(){}; // Actually delete all
		void readSiloPlant(string theName);		
		GradeMixture isOK(string currentGrade, string nextGrade){
			return GradeMixture::OK;
		}
		int callCount = 0;
		void start();
		void fail(string theError)	
		{
			writeMyLog(theError);
			writeResult(name + "_result.txt", false);
			exit(0);
		}
		string name;
		friend PlantLine;
		bool done = false;
		SimModel* itsModel;
		string itsName;
		void readSilos(string fileName);
		void readMachines(string fileName);
		void readLines(string fileName);
		void readOrders(string fileName);
		void readMatrix(string fileName);
		void writeResult(string fileName, bool result);
		PlantLine* getPlantLine(int theNB);
		map<pair<string, string>, GradeMixture>* gradeMatrix = new map<pair<string, string>, GradeMixture>();
		list<Silo*> itsSilos = list<Silo*>();
		list<PackingMachine*> itsOutputMachines = list<PackingMachine*>();
		list<PlantLine*> itsLines = list<PlantLine*>();
		int itsCleaningTime = 0;
		int itsDownTime = 0;
		double itsFactor = 1;
		int itsHolidays = 0;
		int itsMaintenance = 0;
		int itsDaysPerWeek = 5;
		int itsHoursPerDay = 168;
	};
	struct Flow;
	struct SiloOrder 
	{
	public:
		string _grade="";
		double _speed=0;
		double _time=0;
		double _total=0;
		double _density = 0;
		string cleanComp = "";
		double _PercentPelletBags=0;
		double _PercentPelletBulk=0;
		double _PercentPowderBags=0;
		double _DoneIn=0;
		double _DonePelletBags=0;
		double _DonePelletBulk=0;
		double _DonePowderBags=0;
		double _TotalPelletBags = 0;
		double _TotalPelletBulk = 0;
		double _TotalPowderBags = 0;
		double sampleTime = 0;
		double samplePercentage = 0;
		double fillStartTime = -1;
		double _updateTime=0;
		Flow* itsIn;
		list<Flow*> itsOuts =  list<Flow*>();
		Silo* currentSilo = NULL;
		list<Silo*> usedSilos = list<Silo*>();
		bool done = false;
		bool started = false;
	};
	class IFlowFinished
	{
	public:
		virtual void flowFinished(Flow* theFlow) = 0;
	};
	class StateValue;
	struct ENERGYSIM_DLL_PUBLIC Flow
	{
	public:
		Flow(double* thePtrDouble, double theTime, SimModel* theModel)
		{
			_updateTime = theTime;
			_ptrDouble = thePtrDouble;
			itsModel = theModel;
			allFlows.push_back(this);
		}
		SimModel* itsModel;
		double* _ptrDouble;
		double _speed=0;
		double _realSpeed=0;
		double _updateTime=0;
		double _total=0;
		double _goal=0;
		StateValue* _StateValue;
		double update();
		void onoff(bool onOff);
		bool on = true;
		//list<IFlowFinished*> itsListners = list<IFlowFinished*>();
		static list<Flow*> allFlows;
		static void updateAll();
	};
	class IStateValueReached
	{
	public:
		virtual void valueReached(StateValue* theST, double theValue, bool theSign)=0;
	};
	class IStateValueMax
	{
	public:
		virtual void valueReached(StateValue* theST, void* theObject) = 0;
	};
	class IStateValueMin
	{
	public:
		virtual void valueReached(StateValue* theST, void* theObject) = 0;
	};
	class ENERGYSIM_DLL_PUBLIC StateValue
	{
	public:
		double value(){ return currentValue; };
		string name() { return _name; };
		double currentDelta;
		double updateTime=0;
		double currentValue=0;
		double maxValue=0;
		double minValue=0;
		StateValue(SimModel* theModel, double theStartValue = 0.0);
		string _name;
		void listen(IStateValueReached* theListener, double theValue, bool theSign)
		{
			itsListners.push_back(pair<IStateValueReached*, pair<bool, double>>(theListener, pair<bool, double>(theSign, theValue)));
		}
		void listenMax(IStateValueMax* theListener, void* theObject)
		{
			itsMaxListners.push_back(pair<IStateValueMax*, void*>(theListener, theObject));
		}
		void listenMin(IStateValueMin* theListener, void* theObject)
		{
			itsMinListners.push_back(pair<IStateValueMin*, void*>(theListener, theObject));
		}
		void connect(Flow* theFlow);
		void disConnect(Flow* theFlow);
		SimModel* itsModel;
		void updateSubscribtions();
		void updateState();
		double getSpeed();
		list<Flow*> itsFlows = list<Flow*>();
		list<pair<IStateValueReached*, pair<bool, double>>> itsListners = list<pair<IStateValueReached*, pair<bool, double>>>();
		list<pair<IStateValueMax*, void*>> itsMaxListners;
		list<pair<IStateValueMin*, void*>> itsMinListners;
	};
	class ENERGYSIM_DLL_PUBLIC UpdateOrder : public IStateValueReached
	{
	public:
		UpdateOrder(SiloOrder* theOrder) { itsOrder = theOrder; }
		virtual void valueReached(StateValue* theST, double theValue, bool theSign);
	private:
		SiloOrder* itsOrder;
	};
	class ENERGYSIM_DLL_PUBLIC FinishOrder : public IStateValueReached
	{
	public:
		FinishOrder(SiloOrder* theOrder, Flow* theFlow) { itsOrder = theOrder; itsFlow = theFlow; }
		virtual void valueReached(StateValue* theST, double theValue, bool theSign);
	private:
		SiloOrder* itsOrder;
		Flow* itsFlow;
	};
}