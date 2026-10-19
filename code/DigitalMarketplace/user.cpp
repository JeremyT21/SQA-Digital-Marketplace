#include "user.h"
#include <sstream>

// CLASS EXPLANATIONS IN ASSOCIATED HEADER FILES

bool UserAccount::canUserBuy() {
    return type == "AA";
}

bool UserAccount::canUserSell() {
    return type == "AA" || type == "FS" || type == "BS";
}

bool UserAccount::isAdmin() {
    return type == "AA" || type == "FS" || type == "SS";
}

void UserSession::start(UserAccount account, vector<GameListing> listings) {
    loggedInAccount = account;
    loggedIn = true;
    creditAddedByUser.clear();
    listingsAvailableAtLogin.clear();
    for (const auto& listing : listings) {
        listingsAvailableAtLogin.insert(listing.gameTitle + "|" + listing.sellerName);
    }
}

bool UserSession::isUserLoggedIn() {
    return loggedIn;
}

void UserSession::end() {
    loggedIn = false;
    loggedInAccount = UserAccount();
    listingsAvailableAtLogin.clear();
}

int UserSession::creditAddedFor(string username) {
    return creditAddedByUser[username];
}

bool UserSession::wasAvailableWhenLoggedIn(string title, string seller) {
    return listingsAvailableAtLogin.count(title + "|" + seller) > 0;
}

void UserSession::creditAddedTotal(string username, int creditAmount) {
    creditAddedByUser[username] += creditAmount;
}

string UserSession::formatTransactionRecord() {
    ostringstream out;
    for (const auto& entry : creditAddedByUser) {
        out << entry.first << " added " << entry.second << " cents\n";
    }
    return out.str();
}

UserAccount UserSession::currentAccount() {
    return loggedInAccount;
}
