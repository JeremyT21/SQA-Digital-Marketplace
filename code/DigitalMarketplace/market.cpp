#include "market.h"
#include <algorithm>

GameListing Marketplace::findListing(string title, string seller)
{
    for (const auto& listing : listings)
    {
        if (listing.gameTitle == title && listing.sellerName == seller)
        {
            return listing;
        }
    }
    return GameListing();
}

UserAccount Marketplace::findAccount(string username)
{
    for (const auto& account : accounts)
    {
        if (account.username == username)
        {
            return account;
        }
    }
    return UserAccount();
}

void Marketplace::loadData(vector<UserAccount> a, vector<GameListing> l, vector<UserOwnsGame> c)
{
    accounts = a;
    listings = l;
    collections = c;
    pendingRecords.clear();
    deletedUsers.clear();
}

bool Marketplace::isLoggedIn()
{
    return session.isUserLoggedIn();
}

bool Marketplace::isCurrentAdmin()
{
    return isLoggedIn() && session.currentAccount().isAdmin();
}

string Marketplace::currentUsername()
{
    return session.currentAccount().username;
}

vector<UserOwnsGame> Marketplace::currentCollections()
{
    return collections;
}

ResultingChange Marketplace::logout()
{
    if (!isLoggedIn())
    {
        return { false, "no user is currently logged in", false };
    }
    TransactionLog record;
    record.code = 0;
    //use current account details when ending a session
    record.buyerAccount = findAccount(currentUsername());
    pendingRecords.push_back(record);
    session.end();
    return { true, "logged out", true };
}

ResultingChange Marketplace::login(string username)
{
    if (isLoggedIn())
    {
        return { false, "already logged in", false };
    }
    UserAccount account = findAccount(username);
    if (account.username.empty())
    {
        return { false, "user does not exist", false };
    }
    session.start(account, listings);
    return { true, "logged in as " + username, false };
}

ResultingChange Marketplace::createAccount(string username, string type, int credit)
{
    //validate account details before adding an account
    if (!isCurrentAdmin())
    {
        return { false, "Need admin privileges.", false };
    }
    if (username.empty())
    {
        return { false, "Username required.", false };
    }
    if (username.length() > 15)
    {
        return { false, "Username exceeded 15 characters, invalid.", false };
    }
    if (!findAccount(username).username.empty())
    {
        return { false, "Username already exists.", false };
    }
    if (type != "AA" && type != "FS" && type != "BS" && type != "SS")
    {
        return { false, "Invalid account type.", false };
    }
    if (credit < 0 || credit > 99999999)
    {
        return { false, "Invalid numeric credit.", false };
    }
    UserAccount account;
    account.username = username;
    account.type = type;
    account.credit = credit;
    accounts.push_back(account);
    deletedUsers.erase(username);
    TransactionLog record;
    record.code = 1;
    record.buyerName = username;
    record.buyerAccount = account;
    record.amountCents = credit;
    pendingRecords.push_back(record);
    return { true, "Account " + username + " created.", false };
}

ResultingChange Marketplace::deleteAccount(string username)
{
    if (!isCurrentAdmin())
    {
        return { false, "Admin privileges required.", false };
    }
    if (username.empty())
    {
        return { false, "Target username required.", false };
    }
    auto it = find_if(accounts.begin(), accounts.end(), [&](const UserAccount& account)
    {
        return account.username == username;
    });
    //check the account search before deleting
    if (it == accounts.end())
    {
        return { false, "User does not exist.", false };
    }
    UserAccount target = *it;
    bool deletingSelf = currentUsername() == username;
    accounts.erase(it);
    deletedUsers.insert(username);
    for (int index = static_cast<int>(listings.size()) - 1; index >= 0; index--)
    {
        if (listings[index].sellerName == username)
        {
            listings.erase(listings.begin() + index);
        }
    }
    for (int index = static_cast<int>(collections.size()) - 1; index >= 0; index--)
    {
        if (collections[index].ownerUsername == username)
        {
            collections.erase(collections.begin() + index);
        }
    }
    TransactionLog record;
    record.code = 2;
    record.buyerName = username;
    record.buyerAccount = target;
    record.amountCents = target.credit;
    pendingRecords.push_back(record);
    if (deletingSelf)
    {
        //record automatic logout before clearing the session
        record.code = 0;
        pendingRecords.push_back(record);
        session.end();
    }
    return { true, "Account " + username + " deleted.", deletingSelf };
}

ResultingChange Marketplace::addToCredit(string username, int amount)
{
    if (!isLoggedIn())
    {
        return { false, "no user is currently logged in", false };
    }
    if (!isCurrentAdmin())
    {
        username = currentUsername();
    }
    UserAccount target = findAccount(username);
    if (target.username.empty())
    {
        return { false, "User does not exist.", false };
    }
    if (amount < 0)
    {
        return { false, "Invalid numeric amount.", false };
    }
    if (!isCurrentAdmin() && amount > 100000 - session.creditAddedFor(username))
    {
        return { false, "Session credit limit exceeded.", false };
    }
    if (amount > 99999999 - target.credit)
    {
        return { false, "Maximum credit exceeded.", false };
    }
    for (auto& account : accounts)
    {
        if (account.username == username)
        {
            account.credit += amount;
        }
    }
    session.creditAddedTotal(username, amount);
    TransactionLog record;
    record.code = 6;
    record.buyerName = username;
    record.buyerAccount = target;
    record.amountCents = amount;
    pendingRecords.push_back(record);
    return { true, "Credit added to " + username + ".", false };
}

ResultingChange Marketplace::userBuyGame(string title, string seller)
{
    if (!isLoggedIn())
    {
        return { false, "User does not exist, login required.", false };
    }
    UserAccount buyer = findAccount(currentUsername());
    if (!buyer.canUserBuy())
    {
        return { false, "Account cannot buy.", false };
    }
    UserAccount sellerAccount = findAccount(seller);
    if (sellerAccount.username.empty())
    {
        if (deletedUsers.count(seller) != 0)
        {
            return { false, "Seller deleted or unavailable.", false };
        }
        return { false, "Invalid seller.", false };
    }
    GameListing listing = findListing(title, seller);
    if (listing.gameTitle.empty() || !session.wasAvailableWhenLoggedIn(title, seller))
    {
        return { false, "Listing unavailable in current session.", false };
    }
    for (const auto& owned : collections)
    {
        if (owned.ownerUsername == buyer.username && owned.gameTitle == title)
        {
            return { false, "Game already owned.", false };
        }
    }
    if (buyer.credit < listing.priceInMoney)
    {
        return { false, "Insufficient credit.", false };
    }
    if (listing.priceInMoney > 99999999 - sellerAccount.credit)
    {
        return { false, "Seller credit would exceed maximum.", false };
    }
    //change balances only after all purchase checks pass
    for (auto& account : accounts)
    {
        if (account.username == buyer.username)
        {
            account.credit -= listing.priceInMoney;
        }
        if (account.username == seller)
        {
            account.credit += listing.priceInMoney;
        }
    }
    UserOwnsGame owned;
    owned.ownerUsername = buyer.username;
    owned.gameTitle = title;
    collections.push_back(owned);
    TransactionLog record;
    record.code = 4;
    record.amountCents = listing.priceInMoney;
    record.buyerName = buyer.username;
    record.sellerName = seller;
    record.listing = listing;
    record.buyerAccount = buyer;
    pendingRecords.push_back(record);
    return { true, "Bought " + title + " from " + seller + ".", false };
}

ResultingChange Marketplace::sellGame(string title, int price)
{
    UserAccount account = findAccount(currentUsername());
    if (!isLoggedIn())
    {
        return { false, "no user is currently logged in", false };
    }
    if (!account.canUserSell())
    {
        return { false, "Account cannot sell.", false };
    }
    if (title.empty())
    {
        return { false, "Game name required.", false };
    }
    if (title.length() > 25)
    {
        return { false, "Game name exceeds 25 characters.", false };
    }
    for (const auto& listing : listings)
    {
        if (listing.gameTitle == title)
        {
            return { false, "Game name already exists.", false };
        }
    }
    if (price < 0)
    {
        return { false, "Invalid numeric price.", false };
    }
    if (price > 99999)
    {
        return { false, "Price exceeds 999.99.", false };
    }
    GameListing listing;
    listing.gameTitle = title;
    listing.sellerName = account.username;
    listing.priceInMoney = price;
    listings.push_back(listing);
    TransactionLog record;
    record.code = 3;
    record.sellerName = account.username;
    record.listing = listing;
    record.amountCents = price;
    pendingRecords.push_back(record);
    return { true, "Listed " + title + " for sale.", false };
}

ResultingChange Marketplace::refund(string buyer, string seller, int amount)
{
    if (!isCurrentAdmin())
    {
        return { false, "Admin privileges required.", false };
    }
    UserAccount buyerAccount = findAccount(buyer);
    UserAccount sellerAccount = findAccount(seller);
    if (buyerAccount.username.empty())
    {
        return { false, "Buyer does not exist.", false };
    }
    if (sellerAccount.username.empty())
    {
        return { false, "Seller does not exist.", false };
    }
    if (amount < 0)
    {
        return { false, "Invalid numeric amount.", false };
    }
    if (sellerAccount.credit < amount)
    {
        return { false, "Insufficient seller credit.", false };
    }
    if (amount > 99999999 - buyerAccount.credit)
    {
        return { false, "Buyer credit would exceed maximum.", false };
    }
    for (auto& account : accounts)
    {
        if (account.username == buyer)
        {
            account.credit += amount;
        }
        if (account.username == seller)
        {
            account.credit -= amount;
        }
    }
    TransactionLog record;
    record.code = 5;
    record.buyerName = buyer;
    record.sellerName = seller;
    record.amountCents = amount;
    pendingRecords.push_back(record);
    return { true, "Refund issued to " + buyer + ".", false };
}

vector<GameListing> Marketplace::currentAvailableGames()
{
    vector<GameListing> result;
    for (const auto& listing : listings)
    {
        if (!isLoggedIn() || session.wasAvailableWhenLoggedIn(listing.gameTitle, listing.sellerName))
        {
            result.push_back(listing);
        }
    }
    return result;
}

vector<TransactionLog> Marketplace::transactions()
{
    return pendingRecords;
}

void Marketplace::resetTransactions()
{
    pendingRecords.clear();
}
