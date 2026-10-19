#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;


// GameListing class stores values of an associated game including price seller and title.
class GameListing {
public:
    int priceInMoney;
    string sellerName;
    string gameTitle;
};


//UserAccount stores values of an associated user including credit, type, and username. It also has functions to check for user permissions.
class UserAccount {
public:
    int credit;
    string type;
    string username;

    bool canUserBuy();
    bool canUserSell();
    bool isAdmin();
};


/*UserSession class stores values of current marketplace session such as the account logged in(if any).
It also contains multiple functions related to session tracking and transactions of the current account used*/
class UserSession {
private:
    UserAccount loggedInAccount;
    bool loggedIn = false;
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
    UserAccount currentAccount();
};


//UserOwnsGame tracks a users current ownership status
class UserOwnsGame {
public:
    string gameTitle;
    string ownerUsername;
};


//transactionLog is used to track any changes in a users data, such as purchases, sales, refunds, etc.
class TransactionLog {
public:
    int amountDollars = 0;
    string sellerName;
    string buyerName;
    GameListing listing;
    UserAccount buyerAccount;
    int code = 0;
};
