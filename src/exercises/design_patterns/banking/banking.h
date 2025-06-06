/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <memory>
#include <iostream>

namespace banking {
	using namespace std;
	class IReadOnlyAccount {
	public:
		virtual double getBalance() const = 0;
		virtual ~IReadOnlyAccount() = default;
	};
	class IAccount : public IReadOnlyAccount
	{
	public:
		virtual void withdraw(double val) = 0;
		virtual void deposit(double val) = 0;
	};
	class AccountBase : public IAccount
	{
	public:
		AccountBase(double balance = 0) :_balance(balance) {}
		double getBalance() const override { return _balance; }
		void withdraw(double val) override {
			Authenticate();
			ConnectRemoteDB();
			if (_balance >= val) {
				_balance -= val;
				SaveChanges();
			}
			else {
				ReportError();
			}
			CloseConnection();
		}
		void deposit(double val) override { _balance += val; }
		void Authenticate() { cout << "Authenticate" << endl; }
		void ConnectRemoteDB() { cout << "ConnectRemoteDB" << endl; }
		void SaveChanges() { cout << "SaveChanges" << endl; }
		void ReportError() { cout << "ReportError" << endl; }
		void CloseConnection() { cout << "CloseConnection" << endl; }
	private:
		double _balance;
	};
	class CheckingAccount : public AccountBase
	{
	public:
		CheckingAccount(double balance = 0, double overdraft = 0) :AccountBase(balance), _overdraft(overdraft) {}
		void withdraw(double val) override { AccountBase::withdraw(val); }
		~CheckingAccount() { cout << "bye checking" << endl; }
	private:
		double _overdraft;
	};
	class SavingAccount : public AccountBase
	{
	public:
		using AccountBase::AccountBase;
		void setInterestRate(double val) { _intRate = val; }
		~SavingAccount() { cout << "bye saving" << endl; }
	private:
		static double _intRate;
	};
	class AccountDecorator : public IAccount
	{
	public:
		AccountDecorator(unique_ptr<IAccount> account) :_account(move(account)) {}
	protected:
		unique_ptr<IAccount> _account;
	};
	class TraceAccount : public AccountDecorator {
	public:
		using AccountDecorator::AccountDecorator;
		double getBalance() const override {
			cout << "get balance called" << endl;
			return _account->getBalance();
		}
		void withdraw(double val)override {
			cout << "withdraw called" << endl;
			 _account->withdraw(val);
		}
		void deposit(double val) override {
			cout << "deposit called" << endl;
			_account->deposit(val);
		}
	private:

	};
	class CommissionAccount : public AccountDecorator {
	public:
		CommissionAccount(unique_ptr<IAccount> account,double amount):AccountDecorator(move(account)),_amount(amount){}
		double getBalance() const override {
			_account->withdraw(_amount);
			return _account->getBalance();
		}
		void withdraw(double val) override {
			_account->withdraw(val+_amount);
		}
		void deposit(double val) override {
			_account->deposit(val-_amount);
		}
	private:
		double _amount;
	};


	class SavingAccountProxy : public IAccount
	{
	public:
		SavingAccountProxy(double balance):_balance(balance){}
		double getBalance() const override {
			Load();
			return _account->getBalance();
		}
		void withdraw(double val) override {
			Load();
			_account->withdraw(val);
		}
		void deposit(double val) override {
			Load();
			_account->deposit(val);
		}
	private:
		void Load() const {
			if (_account == nullptr) {
				_account = make_unique<SavingAccount>(_balance);
				cout << "proxy loading real object" << endl;
			}
		}
		mutable unique_ptr<SavingAccount> _account;
		double _balance;
	};
	class Bank {
	public:
		static Bank& instance() {
			static Bank bank;
			return bank;
		}
		unique_ptr<IAccount> createCheckingAccount(double bal, double overdraft)
		{
#ifdef _DEBUG
			return make_unique<TraceAccount>(make_unique<CommissionAccount>(make_unique<CheckingAccount>(bal, overdraft),10));
#else
			return make_unique<CheckingAccount>(bal,overdraft);
#endif
		}
		unique_ptr<IAccount> createSavingAccount(double bal)
		{
			return make_unique<SavingAccountProxy>(bal);
		}
		~Bank() { cout << "bye bank" << endl; }
	private:
		Bank() = default;
		Bank(const Bank&) = delete;
	};
}
