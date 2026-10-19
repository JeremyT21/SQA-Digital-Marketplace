#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

/* user.cpp, which implements classes and methods initialized here, are compiled and called when main.cpp is run, executing the front end. 
user handles the user-related functionality within the marketplace.*/

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
	//returns true if the user has permission to buy, false otherwise
    bool canUserBuy();
	//returns true if the user has permission to sell, false otherwise
    bool canUserSell();
	//returns true if the user is an admin, false otherwise
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
	//starts a new session for the user account, storing the account and the listings available at the time of login
    void start(UserAccount account, vector<GameListing> listings);
	//returns true if a user is currently logged in, false otherwise
    bool isUserLoggedIn();
	//ends the current session, clearing the logged in account and listings available at login
    void end();
	//returns the total credit added for a specific user during the current session
    int creditAddedFor(string username);
	//returns true if a specific game listing was available when the user logged in, false otherwise
    bool wasAvailableWhenLoggedIn(string title, string seller);
	//records the total credit added for a specific user during the current session
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
    int amountCents = 0;
    string sellerName;
    string buyerName;
    GameListing listing;
    UserAccount buyerAccount;
    int code = 0;
};
