#pragma once
#include <string>
#include <vector>
#include "user.h"
using namespace std;

/* market.cpp, which implements classes and methods initialized here, are compiled and called when main.cpp is run.
market handles the marketplace and the functions that can be called by the front end, as well as the data stored in the 4 txt files. */

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
	//finds a specific listing based on title and seller, returning the GameListing object if found, or an empty GameListing object if not found
    GameListing findListing(string title, string seller);
	//finds a specific account based on username, returning the UserAccount object if found, or an empty UserAccount object if not found
    UserAccount findAccount(string accountUsername);

public:
	// loads the data from the 3 txt files (listing, accounts, collections) into the marketplace, storing the data in their respective vectors
    void loadData(vector<UserAccount> accounts,
        vector<GameListing> listings,
        vector<UserOwnsGame> collections);
	// logs out the current user, ending the session and returning a ResultingChange object (contains success status, message, and sessionEnded status)
    ResultingChange logout();
	// logs in a user based on their username, starting a new session and returning a ResultingChange object
    ResultingChange login(string username);
	// creates a new user account with the specified username, type, and credit, returning a ResultingChange
    ResultingChange createAccount(string username, string type, int credit);
	// deletes a user account based on the specified username, returning a ResultingChange
    ResultingChange deleteAccount(string username);
	// adds credit to a user's account based on the specified username and credit amount, returning a ResultingChange
    ResultingChange addToCredit(string username, int creditAmount);
	// allows a user to buy a game based on the specified game title and seller, returning a ResultingChange
    ResultingChange userBuyGame(string gameTitle, string seller);
	// allows a user to sell a game based on the specified game title and price, returning a ResultingChange
    ResultingChange sellGame(string gameTitle, int price);
	// allows an admin to refund a user based on the specified buyer, seller, and credit amount, returning a ResultingChange
    ResultingChange refund(string buyer, string seller, int creditAmount);

	// returns the current available games for sale in the marketplace as a vector of GameListing objects
    vector<GameListing> currentAvailableGames();
	// returns the current user collections as a vector of UserOwnsGame objects
    vector<TransactionLog> transactions();
	// resets the pending transactions, clearing the vector of TransactionLog objects
    void resetTransactions();
};
