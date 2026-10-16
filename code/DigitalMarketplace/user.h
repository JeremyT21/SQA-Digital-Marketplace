#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

//user account
class UserAccount {
public:
    int credit;
    string type;
    string username;

    bool canUserBuy();
    bool canUserSell();
    bool isAdmin();
};


//session
class UserSession {
private:
    UserAccount loggedInAccount;
    map<string, int> creditAddedByUser;
    set<string> listingsAvailableAtLogin;

public:
    void start(UserAccount account, vector<GameListing> listings);
    bool isUserLoggedIn();
    void end();
    int creditAddedFor(string username);
    bool wasAvailableWhenLoggedIn(string title, string seller);
    void creditAddedTotal(string username, int creditAmount);
    string formatTransactionRecord();
};


//check for user owning game
class UserOwnsGame {
public:
    string gameTitle;
    string ownerUsername;
};


//transaction log
class TransactionLog {
public:
    int amountDollars;
    string sellerName;
    string buyerName;
    GameListing listing;
    UserAccount buyerAccount;
    int code;
};