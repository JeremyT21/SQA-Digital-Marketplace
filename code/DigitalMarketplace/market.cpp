#include "market.h"
#include <algorithm>

// CLASS EXPLANATIONS IN ASSOCIATED HEADER FILES

GameListing Marketplace::findListing(string title, string seller) {
    for (const auto& listing : listings) {
        if (listing.gameTitle == title && listing.sellerName == seller) return listing;
    }
    return GameListing();
}

UserAccount Marketplace::findAccount(string username) {
    for (const auto& account : accounts) {
        if (account.username == username) return account;
    }
    return UserAccount();
}

void Marketplace::loadData(vector<UserAccount> a, vector<GameListing> l,
    vector<UserOwnsGame> c) {
    accounts = a;
    listings = l;
    collections = c;
}

ResultingChange Marketplace::logout() {
    session.end();
    return { true, "Logged out.", true };
}

ResultingChange Marketplace::login(string username) {
    UserAccount account = findAccount(username);
    session.start(account, listings);
    return { true, "Logged in as " + username + ".", false };
}

ResultingChange Marketplace::createAccount(string username, string type, int credit) {
    UserAccount account;
    account.username = username;
    account.type = type;
    account.credit = credit;
    accounts.push_back(account);

    TransactionLog record;
    record.code = 1;
    record.buyerName = username;
    record.amountDollars = credit;
    pendingRecords.push_back(record);
    return { true, "Account " + username + " created.", false };
}

ResultingChange Marketplace::deleteAccount(string username) {
    auto it = find_if(accounts.begin(), accounts.end(),
        [&](const UserAccount& account) { return account.username == username; });

    bool deletingSelf = session.currentAccount().username == username;
    accounts.erase(it);

    if (deletingSelf) session.end();

    TransactionLog record;
    record.code = 2;
    record.buyerName = username;
    pendingRecords.push_back(record);
    return { true, "Account " + username + " deleted.", deletingSelf };
}

ResultingChange Marketplace::addToCredit(string username, int amount) {
    auto it = find_if(accounts.begin(), accounts.end(),
        [&](const UserAccount& account) { return account.username == username; });
    it->credit += amount;
    session.creditAddedTotal(username, amount);

    TransactionLog record;
    record.code = 6;
    record.buyerName = username;
    record.amountDollars = amount;
    pendingRecords.push_back(record);
    return { true, "Credit added to " + username + ".", false };
}

ResultingChange Marketplace::userBuyGame(string title, string seller) {
    UserAccount buyer = session.currentAccount();
    GameListing listing = findListing(title, seller);

    auto buyerIt = find_if(accounts.begin(), accounts.end(),
        [&](const UserAccount& account) { return account.username == buyer.username; });
    auto sellerIt = find_if(accounts.begin(), accounts.end(),
        [&](const UserAccount& account) { return account.username == seller; });

    buyerIt->credit -= listing.priceInMoney;
    sellerIt->credit += listing.priceInMoney;

    UserOwnsGame owned;
    owned.ownerUsername = buyer.username;
    owned.gameTitle = title;
    collections.push_back(owned);

    TransactionLog record;
    record.code = 4;
    record.amountDollars = listing.priceInMoney;
    record.buyerName = buyer.username;
    record.sellerName = seller;
    record.listing = listing;
    record.buyerAccount = buyer;
    pendingRecords.push_back(record);
    return { true, "Bought " + title + " from " + seller + ".", false };
}

ResultingChange Marketplace::sellGame(string title, int price) {
    UserAccount account = session.currentAccount();
    GameListing listing;
    listing.gameTitle = title;
    listing.sellerName = account.username;
    listing.priceInMoney = price;
    listings.push_back(listing);

    TransactionLog record;
    record.code = 3;
    record.sellerName = account.username;
    record.listing = listing;
    record.amountDollars = price;
    pendingRecords.push_back(record);
    return { true, "Listed " + title + " for sale.", false };
}

ResultingChange Marketplace::refund(string buyer, string seller, int amount) {
    auto sellerIt = find_if(accounts.begin(), accounts.end(),
        [&](const UserAccount& account) { return account.username == seller; });
    auto buyerIt = find_if(accounts.begin(), accounts.end(),
        [&](const UserAccount& account) { return account.username == buyer; });

    sellerIt->credit -= amount;
    buyerIt->credit += amount;

    TransactionLog record;
    record.code = 5;
    record.buyerName = buyer;
    record.sellerName = seller;
    record.amountDollars = amount;
    pendingRecords.push_back(record);
    return { true, "Refund issued to " + buyer + ".", false };
}

vector<GameListing> Marketplace::currentAvailableGames() {
    vector<GameListing> result;
    if (!session.isUserLoggedIn()) return result;
    for (const auto& listing : listings) {
        if (session.wasAvailableWhenLoggedIn(listing.gameTitle, listing.sellerName)) {
            result.push_back(listing);
        }
    }
    return result;
}

vector<TransactionLog> Marketplace::transactions() {
    return pendingRecords;
}

void Marketplace::resetTransactions() {
    pendingRecords.clear();
}
