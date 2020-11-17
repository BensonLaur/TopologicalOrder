#include <vector>
#include <map>
#include <iostream>
#include "entity/ConditionOrderHelper.h"

using namespace std;

void printV(const vector<int>& list, char* title = nullptr)
{
	if(title)
		std::cout << title <<" :" << std::endl;

	if (!list.empty())
	{
		for (auto& i : list)
			std::cout << i << " ";

		std::cout << std::endl;
	}
}

void printP(const pair<int, int>& pairData, char* title = nullptr)
{
	printV({ pairData.first, pairData.second }, title);
}

//≤‚ ‘”√¿˝¿‡
using TestCase = class _TestCase{

public:
	_TestCase(const vector<vector<int>>& orders, const vector<pair<int, int>>& conditions)
		:m_orders(orders), m_conditions(conditions){}

	void Run()
	{
		ConditionOrderHelper helper;
		bool bRet = false;

		printV({}, "INPUT");
		for (auto& order : m_orders)
		{
			printV(order);
			bRet = helper.AppendNewOrders(order);
		}

		printV({}, "CONDITION");
		for (auto& condition : m_conditions)
		{
			bRet = helper.ApplyOrderCondition(condition);
			cout << (bRet ? "[Y]" : "[N]" )<< " ";
			printP(condition);
		}

		vector<int> result = helper.CalculateFinalOrder();
		printV(result, "RESULT");
	}

private:
	vector<vector<int>> m_orders;
	vector<pair<int, int>> m_conditions;
};

using TestCases = vector<TestCase>;

//≤‚ ‘”√¿˝
void testConditionOrderHelper()
{
	TestCases tcs = {
		TestCase(
		{
			{ 1,2,3 } ,
			{ 4,5,6 }
		},
		{
			{ 4, 2 }
		})
		,
		TestCase(
		{
			{ 1 },
			{ 2, 3, 4, 5, 6 } ,
			{ 7, 8, 9, 10, 11 },
		},
		{
			{ 5, 10 },
			{ 5, 8 },
			{ 3, 7 },
			{ 10, 6 },
			{ 7, 5 },
			{ 1, 6 },
			{ 1, 4 }
		})
		,
		TestCase(
		{
			{ 1 } ,
			{ 2 } ,
			{ 3 } ,
			{ 4 } ,
			{ 5 } ,
			{ 6 } ,
			{ 7 } ,
			{ 8 }
		},
		{
			{ 2, 3 },
			{ 1, 4 },
			{ 3, 4 },
			{ 1, 5 },
			{ 4, 5 },
			{ 2, 7 },
			{ 3, 8 },
			{ 6, 8 }
		})
	};

	for (size_t i = 0; i< tcs.size(); ++i)
	{
		cout << "========= Case " << 1+i <<" ===========" << endl;
		tcs[i].Run();
	}
	cout << "==========================" << endl;
}

int main(int argc, char** args)
{
	testConditionOrderHelper();

	return 0;
}