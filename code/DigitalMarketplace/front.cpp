#include "front.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

// CLASS EXPLANATIONS IN ASSOCIATED HEADER FILES

vector<GameListing> FrontEndFileManager::readListings() {
    vector<GameListing> result;
    ifstream file("listings.txt");
    string title, seller;
    int price;
    while (file >> quoted(title) >> quoted(seller) >> price) {
        GameListing listing;
        listing.gameTitle = title;
        listing.sellerName = seller;
        listing.priceInMoney = price;
        result.push_back(listing);
    }
    return result;
}

vector<UserOwnsGame> FrontEndFileManager::readCollections() {
    vector<UserOwnsGame> result;
    ifstream file("collections.txt");
    string owner, title;
    while (file >> quoted(owner) >> quoted(title)) {
        UserOwnsGame item;
        item.ownerUsername = owner;
        item.gameTitle = title;
        result.push_back(item);
    }
    return result;
}

vector<UserAccount> FrontEndFileManager::readAccounts() {
    vector<UserAccount> result;
    ifstream file("accounts.txt");
    string username, type;
    int credit;
    while (file >> quoted(username) >> quoted(type) >> credit) {
        UserAccount account;
        account.username = username;
        account.type = type;
        account.credit = credit;
        result.push_back(account);
    }
    return result;
}

bool FrontEndFileManager::logTransactionsFromUser(vector<TransactionLog> records) {
    ofstream file("transactions.txt", ios::app);
    for (const auto& record : records) file << formatTransaction(record) << '\n';
    return true;
}

string FrontEndFileManager::formatTransaction(TransactionLog record) {
    ostringstream out;
    out << record.code << ' ' << record.buyerName << ' ' << record.sellerName
        << ' ' << record.amountDollars << ' ' << record.listing.gameTitle;
    return out.str();
}

void FrontEnd::executeFrontEnd() {
    marketplace.loadData(files.readAccounts(), files.readListings(), files.readCollections());
    output << "Marketplace. Type help for commands.\n";
    string command;
    while (command != "exit") {
        output << "\n> ";
        getline(input, command);
        if (command != "exit") getFromMarketplace(command);
    }
    files.logTransactionsFromUser(marketplace.transactions());
}

void FrontEnd::showStats(ResultingChange result) {
    output << result.message << '\n';
}

void FrontEnd::infoMarketplace() {
    vector<GameListing> games = marketplace.currentAvailableGames();
    for (const auto& game : games) {
        output << game.gameTitle << " | seller: " << game.sellerName
            << " | $" << fixed << setprecision(2) << game.priceInMoney / 100.0 << '\n';
    }
}

int FrontEnd::getUserMoney(string text) {
    return static_cast<int>(stod(text) * 100 + 0.5);
}

string FrontEnd::readValueInField(string prompt) {
    output << prompt;
    string value;
    getline(input, value);
    return value;
}

void FrontEnd::getFromMarketplace(string command) {
    if (command == "help") {
        output << "Commands: login, logout, list, buy, sell, credit, create, delete, refund, exit\n"
            << "Money amounts are entered as dollars, e.g. 12.50.\n";
    }
    else if (command == "login") {
        showStats(marketplace.login(readValueInField("Username: ")));
    }
    else if (command == "logout") {
        showStats(marketplace.logout());
    }
    else if (command == "list") {
        infoMarketplace();
    }
    else if (command == "buy") {
        string title = readValueInField("Game title: ");
        string seller = readValueInField("Seller username: ");
        showStats(marketplace.userBuyGame(title, seller));
    }
    else if (command == "sell") {
        string title = readValueInField("Game title: ");
        int price = getUserMoney(readValueInField("Price in dollars: "));
        showStats(marketplace.sellGame(title, price));
    }
    else if (command == "credit") {
        string username = readValueInField("Account username: ");
        int amount = getUserMoney(readValueInField("Amount in dollars: "));
        showStats(marketplace.addToCredit(username, amount));
    }
    else if (command == "create") {
        string username = readValueInField("New username: ");
        string type = readValueInField("Type (AA/FS/BS/SS): ");
        int credit = getUserMoney(readValueInField("Starting credit in dollars: "));
        showStats(marketplace.createAccount(username, type, credit));
    }
    else if (command == "delete") {
        showStats(marketplace.deleteAccount(readValueInField("Username to delete: ")));
    }
    else if (command == "refund") {
        string buyer = readValueInField("Buyer username: ");
        string seller = readValueInField("Seller username: ");
        int amount = getUserMoney(readValueInField("Refund amount in dollars: "));
        showStats(marketplace.refund(buyer, seller, amount));
    }
    else {
        output << "Unknown command. Type help.\n";
    }
}
