#pragma once

#include "config.h"
#include <string>

using namespace std;

namespace EnergySim
{
	class AttributeHandler;
	class ENERGYSIM_DLL_PUBLIC IParser
	{
	public:
		static IParser* createParser(AttributeHandler* theHandler);
		virtual void test(string aStr)=0;
		virtual void addAttribute(string theExpression, string theName, double startValue) = 0;
		virtual AttributeHandler* getAttributeHandler() = 0;
	};
}