#pragma once
#include <string>
#include <vector>
#include "user.h"
using namespace std;


// ResultingChange class is used to track the results of functions used, whether or not they succeed, have ended, and a message to display to the user if needed.
class ResultingChange {
public:
    bool success;
    string message;
    bool sessionEnded;
};

/* Marketplace is the primary class for controlling the users interaction with the market, handling the current session, as well as the information stored in the 4 txt files
(containing accounts, listings, collections, and transactions). */
class Marketplace {
private:
    UserSession session;
    vector<UserAccount> accounts;
    vector<GameListing> listings;
    vector<UserOwnsGame> collections;
    vector<TransactionLog> pendingRecords;

    GameListing findListing(string title, string seller);
    UserAccount findAccount(string accountUsername);

public:
    void loadData(vector<UserAccount> accounts,
        vector<GameListing> listings,
        vector<UserOwnsGame> collections);

    ResultingChange logout();
    ResultingChange login(string username);
    ResultingChange createAccount(string username, string type, int credit);
    ResultingChange deleteAccount(string username);
    ResultingChange addToCredit(string username, int creditAmount);
    ResultingChange userBuyGame(string gameTitle, string seller);
    ResultingChange sellGame(string gameTitle, int price);
    ResultingChange refund(string buyer, string seller, int creditAmount);

    vector<GameListing> currentAvailableGames();
    vector<TransactionLog> transactions();
    void resetTransactions();
};
