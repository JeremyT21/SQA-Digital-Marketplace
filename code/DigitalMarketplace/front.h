#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "market.h"
using namespace std;


// frontEndFileManager handles reading and writing for the txt files that store data for the marketplace, getting and writing new values to the files when info is needed or changed
class FrontEndFileManager {
public:
    vector<GameListing> readListings();
    vector<UserOwnsGame> readCollections();
    vector<UserAccount> readAccounts();
    bool logTransactionsFromUser(vector<TransactionLog> records);

private:
    string formatTransaction(TransactionLog record);
};


// frontend handles user interface and is run by the main function, taking commands from the user and running the appropriate function.
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
