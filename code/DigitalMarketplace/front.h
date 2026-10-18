#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "market.h"
using namespace std;

// front end manager
class FrontEndFileManager {
public:
    vector<GameListing> readListings();
    vector<UserOwnsGame> readCollections();
    vector<UserAccount> readAccounts();
    bool logTransactionsFromUser(vector<TransactionLog> records);

private:
    string formatTransaction(TransactionLog record);
};


//front end
class FrontEnd {
private:
    Marketplace marketplace;
    istream &input;
    ostream &output;
    FrontEndFileManager files;

public:
    FrontEnd(istream& in, ostream& out) : input(in), output(out) {}
    void executeFrontEnd();

private:
    void showStats(ResultingChange result);
    void infoMarketplace();
    int getUserMoney(string text);
    string readValueInField(string prompt);
    void getFromMarketplace(string command);
};
