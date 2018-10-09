/*
At a bank, we have to keep track of the balance of some accounts. Also, each account has an associated log (the list of records of operations performed on that account). Each operation record shall have a unique serial number, that is incremented for each operation performed in the bank.
We have concurrently run transfer operations, to be executer on multiple threads. Each operation transfers a given amount of money from one account to someother account, and also appends the information about the transfer to the logs of both accounts.
From time to time, as well as at the end of the program, a consistency check shall be executed. It shall verify that the amount of money in each account corresponds with the operations records associated to that account, and also that all operations on each account appear also in the logs of the source or destination of the transfer.
*/
#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <mutex>

using namespace std;


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
    }

    Account() = default;

public:
    int id{};
    int balance{};
    vector<Operation*> log;
    mutex lock;

};


class Bank{
public:
    Bank() {
        accounts.reserve(100);
        lastId = 0;
    }

public:
    vector<Account*> accounts;
    int lastId;
};


void performTransaction()
{



}

int main() {
    Bank b;
    b.accounts.push_back(new Account(1,100));
    b.accounts.push_back(new Account(2,200));
    b.accounts.push_back(new Account(3,300));
    b.accounts.push_back(new Account(4,400));


    thread threads[3];
    for(int i = 0; i < 3; ++i)
    {
        threads[i] = thread(performTransaction);
    }
    for(int i = 0; i < 3; ++i)
    {
        threads[i].join();
    }









    return 0;
}