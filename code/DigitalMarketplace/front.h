#pragma once
#include <string>
#include <vector>
#include "market.h"
using namespace std;

//front end
class FrontEnd {
private:
    Marketplace marketplace;
    istream input;
    ostream output;
    FrontEndFileManager files;

public:
    void executeFrontEnd();

private:
    void showStats(action);
    void infoMarketplace();
    int getUserMoney(text);
    string readValueInField(prompt);
    void getFromMarketplace(command);
};


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