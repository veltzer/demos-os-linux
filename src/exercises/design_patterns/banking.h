#pragma once
#include <memory>
#include <iostream>
namespace banking {
	using namespace std;
	class IReadOnlyAccount {
	public:
		virtual double getBalance() const = 0;
		virtual ~IReadOnlyAccount() = default;
	};
	/// <summary>
	/// 
	/// </summary>
	class IAccount : public IReadOnlyAccount
	{
	public:
		virtual void withdraw(double val) = 0;
		virtual void deposit(double val) = 0;
	};
	/// <summary>
	/// 
	/// </summary>
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
		void Authenticate() { cout << "Authenticate\n"; }
		void ConnectRemoteDB() { cout << "ConnectRemoteDB\n"; }
		void SaveChanges() { cout << "SaveChanges\n"; }
		void ReportError() { cout << "ReportError\n"; }
		void CloseConnection() { cout << "CloseConnection\n"; }
	private:
		double _balance;
	};
	/// <summary>
	/// 
	/// </summary>
	class CheckingAccount : public AccountBase
	{
	public:
		CheckingAccount(double balance = 0, double overdraft = 0) :AccountBase(balance), _overdraft(overdraft) {}
		void withdraw(double val) override { AccountBase::withdraw(val); }
		~CheckingAccount() { std::cout << "bye checking\n"; }
	private:
		double _overdraft;
	};
	/// <summary>
	/// 
	/// </summary>
	class SavingAccount : public AccountBase
	{
	public:
		using AccountBase::AccountBase;
		void setInterestRate(double val) { _intRate = val; }
		~SavingAccount() { std::cout << "bye saving\n"; }
	private:
		static double _intRate;
	};
	/// <summary>
	/// 
	/// </summary>
	class AccountDecorator : public IAccount
	{
	public:
		AccountDecorator(unique_ptr<IAccount> account) :_account(std::move(account)) {}
	protected:
		unique_ptr<IAccount> _account;
	};
	/// <summary>
	/// 
	/// </summary>
	class TraceAccount : public AccountDecorator {
	public:
		using AccountDecorator::AccountDecorator;
		double getBalance() const override {
			cout << "get balance called\n";
			return _account->getBalance();
		}
		void withdraw(double val)override {
			cout << "withdraw called\n";
			 _account->withdraw(val);
		}
		void deposit(double val) override {
			cout << "deposit called\n";
			_account->deposit(val);
		}
	private:

	};
	/// <summary>
	/// /
	/// </summary>
	class CommissionAccount : public AccountDecorator {
	public:
		CommissionAccount(unique_ptr<IAccount> account,double amount):AccountDecorator(std::move(account)),_amount(amount){}
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
				cout << "proxy loading real object\n";
			}
		}
		mutable unique_ptr<SavingAccount> _account;
		double _balance;
	};

	///
	/// 
	/// <summary>
	/// 
	/// </summary>
	class Bank {
	public:
		static Bank& instance() {
			static Bank bank;
			return bank;
		}
		std::unique_ptr<IAccount> createCheckingAccount(double bal, double overdraft)
		{
#ifdef _DEBUG
			return std::make_unique<TraceAccount>(std::make_unique<CommissionAccount>(std::make_unique<CheckingAccount>(bal, overdraft),10));
#else
			return std::make_unique<CheckingAccount>(bal);
#endif
		}
		std::unique_ptr<IAccount> createSavingAccount(double bal)
		{
			return std::make_unique<SavingAccountProxy>(bal);
		}
		~Bank() { std::cout << "bye bank\n"; }
	private:
		Bank() = default;
		Bank(const Bank&) = delete;
	};
}