/*
At a bank, we have to keep track of the balance of some accounts. Also, each account has an associated log (the list of records of operations performed on that account). Each operation record shall have a unique serial number, that is incremented for each operation performed in the bank.
We have concurrently run transfer operations, to be executer on multiple threads. Each operation transfers a given amount of money from one account to someother account, and also appends the information about the transfer to the logs of both accounts.
From time to time, as well as at the end of the program, a consistency check shall be executed. It shall verify that the amount of money in each account corresponds with the operations records associated to that account, and also that all operations on each account appear also in the logs of the source or destination of the transfer.
*/

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <mutex>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <chrono>

using namespace std;

mutex writingLock;
mutex checkLock;
int isActivity;

int nrThreads = 1;
int nrTotalActions = 1000000;
int nrAccounts = 100;
int startingAmmount = 10000;

class Operation{
public:
    Operation(int id, int recvId, int sendId, int sum) : id(id), recvId(recvId), sendId(sendId), sum(sum) {}

    Operation() = default;

public:
    int id{};
    int recvId{};
    int sendId{};
    int sum{};
};

class Account{
public:

    Account(int id, int balance) : id(id), balance(balance) {
        log.reserve(100);
		lock = new mutex();
    }

    Account() = default;

public:
    int id{};
    int balance{};
    vector<Operation*> log;
    mutex* lock;

};


class Bank{
public:
    Bank() {
        accounts.reserve(100);
        lastId = 0;
		lastIdLock = new mutex();
    }

public:
    vector<Account*> accounts;
    int lastId;
	mutex* lastIdLock;
};


void consistencyCheck(Bank* b)
{
	while(true)
	{
		//this_thread::sleep_for(chrono::milliseconds(5));
		b->lastIdLock->lock();
		
		if (b->lastId % (nrTotalActions/10) == 0)
		{
			cout << "Performed consistency check on id: " << b->lastId << "\n";
			for (int i = 0; i < nrAccounts; ++i)
			{
				int s = startingAmmount;
				int f = b->accounts[i]->balance;
				cout << "id: " << i << " sum: " << b->accounts[i]->balance << "\n";
				for (auto entry : b->accounts[i]->log)
				{
					int am = entry->sum;
					if (entry->sendId == i)
						am *= -1;
					s += am;
					//cout << am << "\n";
				}
				if (s != f)
				{
					cout << "checkFailed\n";
					b->lastIdLock->unlock();
					return;
				}
			}
			cout << "checkOk\n";
			b->lastIdLock->unlock();
			this_thread::sleep_for(chrono::milliseconds(1));
		}
		else
		{ 
			b->lastIdLock->unlock();
		}
	}
}

void performTransaction(Bank* b, int threadId, int seed)
{
	srand(seed);
	writingLock.lock();
	int seed2 = rand() % 1000;
	cout << "seed " << seed2 << "\n";
	writingLock.unlock();

	srand(seed2);
	
	for (int i = 0; i <  nrTotalActions / nrThreads; ++i)
	{
		int fromId;
		int toId;
		do 
		{
			fromId = rand() % nrAccounts ;
			toId = rand() % nrAccounts;
		} while (fromId == toId);

		

		int min = std::fmin(fromId, toId);
		int max = std::fmax(fromId, toId);

		b->accounts[min]->lock->lock();  //deadlock risk
		b->accounts[max]->lock->lock();

		

		int maxAmm = 10;
		int sum;
		if (maxAmm > 1)
		{ 
			int sum = rand() % (maxAmm - 1) + 1;

			b->lastIdLock->lock();
			Operation* o = new Operation(b->lastId, toId, fromId, sum);
			b->lastId++;
			b->lastIdLock->unlock();
			

			/*
			writingLock.lock();
			cout << "from " << fromId << " to " << toId << " sum " << sum << " in thread " << threadId << " opId "<< o->id <<  "\n";
			writingLock.unlock();
			*/
		
			b->accounts[fromId]->balance -= sum;
			b->accounts[toId]->balance += sum;

			b->accounts[fromId]->log.push_back(o);
			b->accounts[toId]->log.push_back(o);
		}
		b->accounts[min]->lock->unlock();  
		b->accounts[max]->lock->unlock();
	}
	

}

int main() {
	Bank b;
	for (int i = 0; i < nrAccounts; ++i)
	{
		b.accounts.push_back(new Account(i, startingAmmount));
	}	
   
	
	

    thread threads[100];
	
	srand(time(NULL));

	clock_t begin = clock();

	

	


    for(int i = 0; i < nrThreads; ++i)
    {
		int seed = rand() % 1000;
        threads[i] = thread(performTransaction, &b,i , seed);
		
    }
	thread checkT = thread(consistencyCheck, &b);

    for(int i = 0; i < nrThreads; ++i)
    {
        threads[i].join();
    }


	clock_t end = clock();
	double elapsed_secs = double(end - begin);
	cout << "time: " << elapsed_secs;
	checkT.detach();
	cout << "\n";


	//final check
	for (int i = 0; i < nrAccounts; ++i)
	{
		int s = startingAmmount;
		int f = b.accounts[i]->balance;
		cout << "id: " << i << " sum: " << b.accounts[i]->balance << "\n";
		for (auto entry : b.accounts[i]->log)
		{
			int am = entry->sum;
			if (entry->sendId == i)
				am *= -1;
			s += am;
			//cout << am << "\n";
		}
		if (s != f)
		{
			cout << "checkFailed\n";
			return 0;
		}
	}
	cout << "checkOk\n";









    return 0;
}