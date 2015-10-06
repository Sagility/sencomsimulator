#include "stdafx.h"
#include <sstream>
#include <cassert>
#include "FileReader.h"
#include <list>

using namespace std;

// Work in progress

namespace EnergySim {

	class Table : public ITable
	{
	public:
		Table(vector<string>* theColumns)
		{
			columns = theColumns;
			itsRows = 0;
		}
		void addRow(list<pair<string,string>>* theList)
		{
			list<string> aList = list < string >();
			for each (pair<string,string> p in *theList)
			{
				// int colNumber(p.first);
			}
			data.push_back(aList);
		}
		virtual string getValue(string column, int row)
		{
			return "";
		}
		virtual bool exist(string column)
		{
			for each (string s in *columns)
			{
				if (column.compare(s)==0)
					return true;
			}
			return false;
		}
		int colNumber(string column)
		{
			int i = 0;
			for each (string s in *columns)
			{
				if (column.compare(s) == 0)
					return i;
				i++;
			}
			return -1;
		}
		virtual int rows()
		{
			return itsRows;
		}
	private:
		vector<string>* columns;
		list <list < string >> data;
		list <list < pair<string, string> >> itsData = list <list < pair<string, string> >>();
		int itsRows;
	};
	vector<string> FileReader::getLine()
	{
		return vector < string>();
	}
	FileReader::FileReader(string fileName, char seperator)
	{
		std::ifstream infile;
		infile = std::ifstream(fileName);
		if (infile.bad())
			return;
		if (!infile.is_open())
			return;
		vector<string> columns = getLine();
		itsTable = new Table(&columns);


		vector<string> aList = getLine();
		int i = 0;
		while (!(aList.empty()))
		{
			for each (string s1 in aList)
			{
				string s2 = columns[i];
				//itsTable.
				//itsTable
				i++;
			}
			aList = getLine();
		}
	}
	ITable* FileReader::getTable()
	{
		return itsTable;
	}
}