#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "market.h"
using namespace std;

/* front.cpp, which implements classes and methods initialized here, are compiled and called when main.cpp is run.
front handles the user interface and is run by the main function, taking commands from the user and running the appropriate function. */


// frontEndFileManager handles reading and writing for the txt files that store data for the marketplace, getting and writing new values to the files when info is needed or changed
class FrontEndFileManager {
public:
    string accountpath;
    string listingpath;
    string collectionpath;
    string transactionpath;
	// reads the game listings from the listings.txt file, returning a vector of GameListing objects
    vector<GameListing> readListings();
	// reads the user collections from the collections.txt file, returning a vector of UserOwnsGame objects
    vector<UserOwnsGame> readCollections();
	// reads the user accounts from the accounts.txt file, returning a vector of UserAccount objects
    vector<UserAccount> readAccounts();
	// writes the transaction records to the transactions.txt file, returning true if successful, false otherwise
    bool logTransactionsFromUser(vector<TransactionLog> records);

private:
	// formats a transaction record as a string for writing to the transactions.txt file
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
    FrontEnd(istream& in, ostream& out, string accountpath, string listingpath,
        string collectionpath, string transactionpath) : input(in), output(out) {
        files.accountpath = accountpath;
        files.listingpath = listingpath;
        files.collectionpath = collectionpath;
        files.transactionpath = transactionpath;
    }
	// executes the front end of the marketplace program, taking commands from the user and running the appropriate function
    void executeFrontEnd();

private:
	// displays the results of a function call to the user, including any messages and whether or not the session has ended
    void showStats(ResultingChange result);
	// displays the current available games for sale in the marketplace to the user
    void infoMarketplace();
	// gets a balance in cents, returning as integer
    int getUserMoney(string text);
	// gets a value from the user in a specific field, returning as string
    string readValueInField(string prompt);
	// takes a command from the user and runs the appropriate function in the marketplace, displaying any results to the user
    void getFromMarketplace(string command);
};
