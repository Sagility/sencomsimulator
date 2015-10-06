// EnergySim.h

#pragma once
#include "Stdafx.h"
#include "config.h"
#include "SimEngine.h"
#include "Routes.h"
using namespace std;

namespace EnergySim {

	// forward declarations
	class SimContext; 

	template<class T, class C> 
	class ENERGYSIM_DLL_PUBLIC getSet
	{
	public:
			getSet(){}
			getSet(T theValue) { itsValue = theValue; }
	//protected:
		operator T() { return &itsValue; }
		T itsValue;
		int i2;
		friend C;
	};

	template<class T, class C>
	class ENERGYSIM_DLL_PUBLIC myGetSet
	{
	public:

	};

	template<class T1, class CT2>
	class ENERGYSIM_DLL_PUBLIC MyComplex
	{
	public:
		

	};
	
	class SimModel;
	class IReader
	{
	public:
		virtual void readModel()	
		{
			readSchedule();
			readResources();
			readRoutes();
			readAttributes();
			readOrders();
		}
		virtual void readRoutes() = 0;
		virtual void readAttributes() = 0;
		virtual void readResources() = 0;
		virtual void readSchedule() = 0;
		virtual void readpreRequisits() = 0;
		virtual void readOrders() =0;
		SimModel* model;
	};

	class TextFileReader
	{
	public:
	};

	class TextReader : public IReader, public TextFileReader
	{
	public:
		TextReader(string baseName) { itsBaseName = baseName; };
	protected:
		virtual void readRoutes();
		virtual void readAttributes();
		virtual void readResources() ;
		virtual void readSchedule() ;
		virtual void readpreRequisits();
		virtual void readOrders();

		bool openFile(string fileName);
		void closeFile();
		bool next();
	private:
		string itsBaseName;
		std::ifstream infile;
		list<string> tokens;
	};

	template <class T, class C>
	void myFunc(T aT, getSet<T, C> aGS)
	{
		aGS.itsValue = aT;
	}

	extern SimModel* aM;

	class Test
	{
	public:
		Test(){ its = aM; }
		SimModel* its;
	};
		 
	class ResourceHandler;
	class ENERGYSIM_DLL_PUBLIC SimModel : public EngineFinishListener
	{
	private:
		SimContext* _ctx;
	public:
		 Test aT;
		 SimModel();

		void readModel(){ reader.itsValue->readModel(); }
		void runModel();

		void OnFinish(ISimEngine *theEngine, EventArgs *theArgs) { finish(); }
		getSet<list<Route*>, SimModel> routes;
		getSet<IParser*, SimModel> itsParser;
		getSet<list<Resource*>, SimModel> resources;
		getSet<Schedule*, SimModel> inSchedule;
		getSet<Schedule*, SimModel> outSchedule;
		getSet<PreReqs*, SimModel> preReqs;	
		getSet<TextReader*, SimModel> reader;
		getSet<list<Order*>, SimModel> orders;
		ResourceHandler* itsResHandler;


		void(*ptr)(PreReqs*, getSet<PreReqs*, SimModel>) = myFunc < PreReqs*, SimModel >;

		void addRoute(Route* aR){routes.itsValue.push_back(aR);}

		int aResID;

		void setRes();
		void finish();

		SimModel(SimContext ctx){ _ctx = &ctx; }


		SimLogEnvironment* env;
		virtual ~SimModel()
		{
			if (env != NULL)
				delete env;
		}
		


		SimContext* context() const{ return _ctx;}
	};

	void ENERGYSIM_DLL_PUBLIC setAValue(SimModel* aModel);

};