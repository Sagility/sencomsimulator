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

#include "Attributes.h"

using namespace std;

namespace EnergySim
{
	class ConstantValue : public BaseValue
	{
	public: ConstantValue(double theValue)
	{
		itsValue = theValue;
	}
	public: double value()
	{
		return itsValue;
	}
	private: double itsValue;
	};
	class AttributeLookUpValue : public BaseValue
	{
	public: AttributeLookUpValue(string theName, AttributeHandler* theHandler)
	{
		itsName = theName;
		itsHandler = theHandler;
	}
	public:  double value();
	private: string itsName;
	private: AttributeHandler* itsHandler;
	};
	class AdditionValue : public BaseValue
	{
	public: AdditionValue(BaseValue* theA, BaseValue* theB)
	{
		itsA = theA;
		itsB = theB;
	}
	public: double value()
	{
		return itsA->value() + itsB->value();
	}
			~AdditionValue() { delete itsA; delete itsB; };
	private: BaseValue *itsA, *itsB;
	};
	class SubstractionValue : public BaseValue
	{
	public: 
		SubstractionValue(BaseValue* theA, BaseValue* theB)
		{
			itsA = theA;
			itsB = theB;
		}
		double value()
		{
			return itsA->value() - itsB->value();
		}
		~SubstractionValue() { delete itsA; delete itsB; };
	private:
		BaseValue *itsA, *itsB;
	};
	class MultiplicationValue : public BaseValue
	{
	public: MultiplicationValue(BaseValue* theA, BaseValue* theB)
	{
		itsA = theA;
		itsB = theB;
	}
	public: double value()
	{
		return itsA->value() * itsB->value();
	}
			~MultiplicationValue() { delete itsA; delete itsB; };
	private: BaseValue *itsA, *itsB;
	};
	class DivisionValue : public BaseValue
	{
	public: DivisionValue(BaseValue* theA, BaseValue* theB)
	{
		itsA = theA;
		itsB = theB;
	}
	public:  double value()
	{
		return itsA->value() / itsB->value();
	}
			 ~DivisionValue() { delete itsA; delete itsB; };
	private: BaseValue *itsA, *itsB;
	};
	class FunctionValue : public BaseValue
	{
	public:
		FunctionValue(BaseValue* theA, string theFunctionName)
		{
			itsValues = theA;
			myFunc = AttributeFunctionHandler::getFunctionByName(theFunctionName);
		}
		double value()
		{
			vector<double> aVec = vector<double>();
			for (BaseValue* aBV : *(itsValues->values()))
				aVec.push_back(aBV->value());
			return myFunc(aVec);
		}
	private:
		std::function<double(vector<double>)> myFunc;
		BaseValue* itsValues;
	};
	class ListValue : public BaseValue
	{
	public: ListValue(BaseValue* theA, BaseValue* theB)
	{
		itsList.push_back(theA);
		itsList.push_back(theB);
	}
	public: double value()
	{
		return itsList.front()->value();
	}
			list<BaseValue*>* values()
			{
				list<BaseValue*>* aList = new list<BaseValue*>();
				for (BaseValue* aBV : itsList)
					for (BaseValue* bBV : *(aBV->values()))
						aList->push_back(bBV);
				return aList;
			}
			~ListValue() { for (BaseValue* aBV : itsList) delete aBV; };
	private: list<BaseValue*> itsList = list<BaseValue*>();
	};
}