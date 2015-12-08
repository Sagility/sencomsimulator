#pragma once

#include <iostream>       // std::cout
#include <stack>          // std::stack
#include <vector>         // std::vector
#include <deque>          // std::deque
#include "EnergySim.h"
#include "SimEngine.h"
#include "Job.h"
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <type_traits>
#include <functional>
using namespace std;

namespace EnergySim
{
	typedef double(*AttributeFunctionDelegate)(vector<double> arguments);
	class AttributeFunctionHandler
	{
	public:
		static AttributeFunctionDelegate getFunctionByName(string name);
	private:
		static double defaultFunction(vector<double> arguments);
		static double memoryswitch(vector<double> arguments);
		static double lookUp(vector<double> arguments);
		static double random(vector<double> arguments);
		static double lessthan(vector<double> arguments);
		static double checkone(vector<double> arguments);
	};

	class AttributeHandler;
	class BaseValue
	{
	public:		BaseValue(){};
				virtual double value() = 0;
				virtual ~BaseValue() {};
				virtual list<BaseValue*>* values()
				{ 
					list<BaseValue* >* aList = new list<BaseValue*>();
					aList->push_back(this);
					return aList;
				}
	};
	enum Reporting { Full, Accumulated, None };
	class AttributeReporter;
	class IParser;
	class BaseAttributeValueHolder
	{
		public: 
			BaseAttributeValueHolder(BaseValue* theBV, string theName, double startValue)
		{
				itsExpression = theBV;
				itsName = theName;
				itsValue = startValue;
		}
		bool BaseAttributeValueHolder::operator==(const BaseAttributeValueHolder &other) const
		{
			if (itsExpression==other.itsExpression )
				return true;
			return false;
		}
		double value() { return itsValue; }
		string name() { return itsName; }
		void prepareUpdate(){ itsNewValue = itsExpression->value(); }
		void update(){ itsValue = itsNewValue; }
		void replaceBaseValue(BaseValue* theBV);
		~BaseAttributeValueHolder(){ delete itsExpression; };
		private:  BaseValue* itsExpression;
				  string itsName;
				  double itsNewValue;
				  double itsValue;
				  double itsMin, itsMax, itsTotal;
	};
	class AttributeReporter
	{
		public: 
			AttributeReporter(string theFileName)
			{
				itsFileName = theFileName;
			}
			void startReporting(double theTime)
			{
				if (!itsStarted)
				{
					ofstream file;
					file.open(itsFileName + ".csv", std::ios_base::out);
					file.close();
					itsStarted = true;
				}
				itsTime = theTime;
			}
			void report(BaseAttributeValueHolder* theBAVH)
			{
				itsStr += theBAVH->name() + "\t" + to_string(theBAVH->value()) + "\t" + to_string(itsTime) + "\n";
			}
			void endReporting()
			{
				std::string txt;
				ofstream file;
				file.open(itsFileName + ".csv", std::ios_base::out);
				file << itsStr;
				file.close();
			}

	private: bool itsStarted = false;
		 std::map<int, double> itsValues =  std::map<int, double>();
		 std::map<int, string> itsLookUp =  std::map<int, string>();
		 std::string itsFileName;
		 std::string itsStr;
		 double itsTime;
	};
	template<typename ID, typename Value> class IListenChange
	{
	public:
		IListenChange(){};
		virtual void valueChanged(ID theID, Value theValue){};
		virtual ~IListenChange(){};
	};


	class ENERGYSIM_DLL_PUBLIC AttributeHandler : public IJobStartedListener, public IJobFinishedListener, public EngineStepListener, public EngineFinishListener
	{
	public:
		list<IListenChange<string, double>*> Changing = list<IListenChange<string, double>*>();
		bool shouldBeUpdated = true;
		double lastUpdateTime = 0;
		double simDelta = 0;
		double lastSimTime = 0;
		double simTime = 0;
		AttributeHandler() { }
		IParser* itsParser;
		void OnJobStarted(IJob *theJob, EventArgs *theArgs){};
		void OnJobFinished(IJob *theJob, EventArgs *theArgs){};

		list<BaseAttributeValueHolder*> itsAttributes =  list<BaseAttributeValueHolder*>();
		AttributeReporter itsReporter =  AttributeReporter("report");
		
		void registerForUpdates(ISimEngine *theEngine)
		{
			theEngine->AddEngineStepListener(this);
			theEngine->AddEngineFinishListener(this);
		}
		virtual void OnNextStep(ISimEngine *theEngine, NextStepEventArgs *theArgs);

		virtual void OnFinish(ISimEngine *theEngine, EventArgs *theArgs);
		//{
		//	replace("simdelta", new ConstantValue(0));
		//	updateAllAttributes();
		//	reportAllAttributes(theEngine->simulated_time());
		//	int i = 3;
		//	IEvent::publishEvent(ET_SIMULATION_END, vector<string>(0));
		//}

		
		bool addAttribute(BaseValue* theAttribute, string theName, double startValue)
		{
			itsAttributes.insert(itsAttributes.begin(), new BaseAttributeValueHolder(theAttribute, theName, startValue));
			return true;
		}
		bool addAttributeHolder(BaseAttributeValueHolder* theAttributeHolder)
		{
			itsAttributes.insert((itsAttributes.begin()),(theAttributeHolder));
			return true;
		}
		void removeAttribute(string theName)
		{
			for(BaseAttributeValueHolder* aBAV : itsAttributes)
			{
				if (aBAV->name() == theName)
				{
					itsAttributes.remove(aBAV);
					return;
				}
			}
		}
		void replace(string theName, BaseValue* theBV)
		{
			shouldBeUpdated = true;
			for (BaseAttributeValueHolder* aBAV : itsAttributes)
			{
				if (aBAV->name() == theName)
				{
					aBAV->replaceBaseValue(theBV);
					return;
				}
			}
			addAttributeHolder( new BaseAttributeValueHolder(theBV,theName,0));
		}
		void removeAllAttributes()
		{
			itsAttributes.clear();
		}
		bool checkAttributes()
		{
			bool aRet = true;
			try
			{
				//reportAndUpdateAllAttributes();
			}
			catch (int e)
			{
				aRet = false;
			}
			return aRet;
		}
		double getAttribute(string theAttributeName)
		{
			for (BaseAttributeValueHolder* aBAV : itsAttributes)
			{
				if (aBAV->name() == theAttributeName)
				{
					return aBAV->value();
				}
			}
			return 0;
		}
		string getAllAttributes()
		{
			updateAllAttributes();
			string aStr = "";
			for (BaseAttributeValueHolder* aBAV : itsAttributes)
			{
				aStr += aBAV->name() + "\t" + std::to_string(aBAV->value()) + "\n";
			}
			return aStr;
		}
		void updateSimVariabels(double time);

		void reportAllAttributes(double time)
		{
			itsReporter.startReporting(time);
			for (BaseAttributeValueHolder* aBAV : itsAttributes)
			{
				itsReporter.report(aBAV);
			}
			itsReporter.endReporting();
		}
		void updateAllAttributes()
		 {
			 for (BaseAttributeValueHolder* aBAV : itsAttributes)
			 	aBAV->prepareUpdate();
			 for (BaseAttributeValueHolder* aBAV : itsAttributes)
				 aBAV->update();
		 }
		void reportAllAttributes()
		 {
			// reportAllAttributes(this->);
		 }
		void publishUpdates(string theAttribute, double theValue)
		 {
			if (Changing.size()>0)
			{
				for(BaseAttributeValueHolder* aBAV : itsAttributes)
					for (IListenChange<string, double>* aFPtr : Changing)
						aFPtr->valueChanged(theAttribute, theValue);
			}
		 }
		void end()
		 {
		 }
	};
}