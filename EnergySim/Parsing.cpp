#include "Stdafx.h"
#include "Parsing.h"
#include <iostream>
#include <string>
#include <regex>
#include "Attributes.h"
#include "Attributes_Internal.h"
#include <iostream>       // std::cout
#include <stack>          // std::stack
#include <vector>         // std::vector
#include <deque>          // std::deque
#include <set>
using namespace std;


namespace EnergySim
{

	enum OperatorToken
	{
		EMPTY,
		ADD,
		SUB,
		MUL,
		DIV,
		LP,
		RP,
		COMMA,
		NAME,
		VALUE
	};
	struct Token
	{
	public:
		OperatorToken itsOper;
		BaseValue* itsBV;
		string itsToken;
	};
	class ENERGYSIM_DLL_PUBLIC ConcreteParser : public IParser
	{
	public:
		ConcreteParser(AttributeHandler* theHandler);
		void test(string aStr);
		void addAttribute(string theExpression, string theName, double startValue);
		AttributeHandler* getAttributeHandler() { return itsAH; };
	private:
		void Tokenize();
		void Parse();
		void popOneAction();
		AttributeHandler* itsAH = NULL;
		string itsStr;
		std::stack<Token> src = std::stack<Token>();
		std::stack<Token> dst = std::stack<Token>();
		std::stack<Token> func = std::stack<Token>();
	};

	IParser* IParser::createParser(AttributeHandler* theHandler)
	{
		return new ConcreteParser(theHandler);
	}
	ConcreteParser::ConcreteParser(AttributeHandler* theHandler)
	{
		itsAH = theHandler;
	}
	void ConcreteParser::addAttribute(string theExpression, string theName, double startValue)
	{
		itsStr = theExpression;
		Tokenize();
		Parse();
		while (!func.empty())
			popOneAction();
		if (itsAH == NULL)
			itsAH = new AttributeHandler();
		itsAH->addAttribute(dst.top().itsBV, theName, startValue);
		return;
	}
	void ConcreteParser::test(string aStr)
	{
		string aALL = itsAH->getAllAttributes();
//		std::cout << aALL << std::endl;
		//aAH->replace();

		return;
	}
	void ConcreteParser::Tokenize()
	{
		while (!src.empty())
			src.pop();
		while (!dst.empty())
			dst.pop();
		while (!func.empty())
			func.pop();

		string s = itsStr;
		smatch m;
		regex e("[a-z]+|[0-9.]+|[(]|[)]|[+]|[-]|[*]|[/]|[ ]|[,]");

		while (regex_search(s, m, e)) 
		{
			for (auto x : m)
			{
				Token aM = Token();
				aM.itsOper = OperatorToken::EMPTY;
				
				string aStr = x;
				char aC = aStr[0];
		//		std::cout << x << " ";
				aM.itsToken = aStr;
				switch (aC)
				{

					case '+': aM.itsOper = OperatorToken::ADD; break;
					case '-': aM.itsOper = OperatorToken::SUB; break;
					case '/': aM.itsOper = OperatorToken::DIV; break;
					case '*': aM.itsOper = OperatorToken::MUL; break;
					case '(': aM.itsOper = OperatorToken::LP; break;
					case ')': aM.itsOper = OperatorToken::RP; break;
					case ',': aM.itsOper = OperatorToken::COMMA; break;
					case ' ': aM.itsOper = OperatorToken::EMPTY; break;

					 default: 
						 // This is a number or a name or empty
						 if (aC< '9' && aC > '0')
						 {
							 aM.itsOper = OperatorToken::VALUE;
							 aM.itsToken = aStr;
						 }
						 else
						 {
							 aM.itsOper = OperatorToken::NAME;
							 aM.itsToken = aStr;
						 }
						 break;
				}
				if (aM.itsOper != OperatorToken::EMPTY)
					dst.push(aM);
			}
	//		std::cout << std::endl;
			s = m.suffix().str();
		}

		while (!dst.empty())
		{
			src.push(dst.top());
			dst.pop();
		}
		return;
	}
	void ConcreteParser::popOneAction()
	{
		if (func.empty())
			return;
		if (func.top().itsOper == LP)
			return;
		if (func.top().itsOper == NAME)
		{
			Token aMPA = dst.top();
			dst.pop();
			Token bMPA = func.top();
			func.pop();
			aMPA.itsBV = new FunctionValue(aMPA.itsBV, bMPA.itsToken); // SHOULD BE A FUNCTION, LOOK BY NAME
			aMPA.itsOper = VALUE;
			dst.push(aMPA);
			return;
		}
		if (!func.empty())
		{
			Token bMPA = dst.top();
			dst.pop();
			Token aMPA = dst.top();
			dst.pop();
			Token cMPA = func.top();
			func.pop();
			switch (cMPA.itsOper)
			{
				case ADD: 
					cMPA.itsBV = new AdditionValue(aMPA.itsBV, bMPA.itsBV);
					break;
				case SUB:
					cMPA.itsBV = new SubstractionValue(aMPA.itsBV, bMPA.itsBV);
					break;
				case DIV:
					cMPA.itsBV = new DivisionValue(aMPA.itsBV, bMPA.itsBV);
					break;
				case MUL:
					cMPA.itsBV = new MultiplicationValue(aMPA.itsBV, bMPA.itsBV);
					break;
				case COMMA:
					cMPA.itsBV = new ListValue(aMPA.itsBV, bMPA.itsBV);
					dst.push(cMPA);
					if (!func.empty() && func.top().itsOper == COMMA)
						popOneAction();
					return;
					break;
				case LP: ; break;
				case RP: ; break;
				default:
					break;
			}
			dst.push(cMPA);
		}
	}
	void ConcreteParser::Parse()
	{
		while (!src.empty())
		{
			Token aMPA = src.top();
			src.pop();
			if (aMPA.itsOper == VALUE)
			{
				string aStr = aMPA.itsToken;
				double aD = std::atof(aStr.c_str());
				aMPA.itsBV = new ConstantValue(aD);
				dst.push(aMPA);
				continue;
			}
			if (aMPA.itsOper < OperatorToken::LP)
			{
				while (!func.empty() && dst.size()>1 && func.top().itsOper!=LP)
				{
					if (aMPA.itsOper == ADD || aMPA.itsOper == SUB)
					{
						popOneAction();
						continue;
					}
					if (func.top().itsOper == MUL || func.top().itsOper == DIV)
					{
						popOneAction();
						continue;
					}
					break;
				}
				func.push(aMPA);
			}
			if (aMPA.itsOper == OperatorToken::LP)
			{
				func.push(aMPA);
				if (!src.empty() && src.top().itsOper == OperatorToken::RP)
				{
					Token emptyMPA = Token();
					emptyMPA.itsOper = OperatorToken::VALUE;
					emptyMPA.itsBV = new ConstantValue(0);
					dst.push(emptyMPA);
				}
			}
			if (aMPA.itsOper == OperatorToken::COMMA)
			{
				func.push(aMPA);
			}
			if (aMPA.itsOper == OperatorToken::NAME)
			{
				if (!src.empty() && (src.top().itsOper == OperatorToken::LP))
					func.push(aMPA);
				else
				{
					aMPA.itsBV = new AttributeLookUpValue(aMPA.itsToken, itsAH);
					dst.push(aMPA);
				}					
			}
			if (aMPA.itsOper == OperatorToken::RP)
			{
				while (!func.empty() && func.top().itsOper != OperatorToken::LP)
				{
					popOneAction();
				}
				aMPA = func.top();
				func.pop();
				if (!func.empty())
				{
					if (func.top().itsOper == OperatorToken::NAME)
					{
						//  is a function call
						//  Function::COMMA
						aMPA = func.top();
						func.pop();

						Token bMPA = dst.top();
						dst.pop();

						aMPA.itsBV = new FunctionValue(bMPA.itsBV, aMPA.itsToken); // SHOULD BE A FUNCTION, LOOK BY NAME
						aMPA.itsOper = VALUE;
						dst.push(aMPA);
						// Arguments are ina list on the dst
					}
				}
			}
		}
	}
}