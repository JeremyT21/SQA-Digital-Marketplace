#pragma once
#include <string>
#include <vector>
#include "user.h"
using namespace std;

// marketplace
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


// resulting change
class ResultingChange {
public:
    bool success;
    string message;
    bool sessionEnded;
};


// game listing
class GameListing {
public:
    int priceInMoney;
    string sellerName;
    string gameTitle;
};