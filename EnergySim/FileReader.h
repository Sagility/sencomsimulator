#pragma once
#include <vector>
#include <algorithm>
#include "EnergySim.h"
using namespace std;
#include "Eventtypes.h"

namespace EnergySim 
{
	class ENERGYSIM_DLL_PUBLIC ITable
	{
	public:
		virtual string getValue(string column, int row)=0;
		virtual bool exist(string column)=0;
		virtual int rows()=0;
	};

	class Table;

	class ENERGYSIM_DLL_PUBLIC FileReader
	{
	public:
		FileReader(string fileName, char seperator);
		ITable* getTable();

	private:
		vector<string> getLine();
		Table* itsTable;
	};


}