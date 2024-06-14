// DPCPPLab.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "banking.h"
#include <iostream>
#include <memory>
using namespace std;
using namespace banking;
void print(const IAccount& p)
{
	cout << typeid(p).name() << ' ' << p.getBalance() << endl;
}
int main()
{
	auto acc = Bank::instance().createSavingAccount(1000);
	acc->deposit(1000);
	
	//print(*acc);
	//acc->withdraw(100);
	//print(*acc);
	
	//acc = Bank::instance().createSavingAccount(100);
	//print(*acc);
	
}
