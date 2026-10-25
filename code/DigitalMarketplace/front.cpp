#include "front.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

// EXPLANATIONS IN ASSOCIATED HEADER FILES

vector<UserAccount> FrontEndFileManager::readAccounts() {
    vector<UserAccount> result;
    ifstream file(accountpath);
    string accountline;
    while (getline(file, accountline)) {
        if (accountline.substr(0, 15) == "END            ") {
            break;
        }
        if (accountline.length() != 28) {
            continue;
        }
        UserAccount account;
        account.username = accountline.substr(0, 15);
        while (!account.username.empty() && account.username.back() == ' ') {
            account.username.pop_back();
        }
        account.type = accountline.substr(16, 2);
        account.credit = static_cast<int>(stod(accountline.substr(19, 9)) * 100 + 0.5);
        result.push_back(account);
    }
    return result;
}

vector<GameListing> FrontEndFileManager::readListings() {
    vector<GameListing> result;
    ifstream file(listingpath);
    if (!file.is_open()) return result;

    string seller, title;
    int price;
    while (file >> quoted(seller)) {
        if (seller == "END") break;
        if (!(file >> quoted(title) >> price)) break;
        GameListing listing;
        listing.sellerName = seller;
        listing.gameTitle = title;
        listing.priceInMoney = price;
        result.push_back(listing);
    }
    return result;
}

vector<UserOwnsGame> FrontEndFileManager::readCollections() {
    vector<UserOwnsGame> result;
    ifstream file(collectionpath);
    if (!file.is_open()) return result;

    string owner, title;
    while (file >> quoted(owner)) {
        if (owner == "END") break;
        if (!(file >> quoted(title))) break;
        UserOwnsGame item;
        item.ownerUsername = owner;
        item.gameTitle = title;
        result.push_back(item);
    }
    return result;
}

bool FrontEndFileManager::logTransactionsFromUser(vector<TransactionLog> records) {
    ofstream file(transactionpath, ios::app);
    for (const auto& record : records) file << formatTransaction(record) << '\n';
    file.close();
    return !file.fail();
}

string FrontEndFileManager::formatTransaction(TransactionLog record) {
    ostringstream out;
    if (record.code == 0) {
        out << "00 " << left << setw(15) << record.buyerAccount.username << ' '
            << record.buyerAccount.type << ' ' << right << setfill('0') << setw(9)
            << fixed << setprecision(2) << record.buyerAccount.credit / 100.0;
        return out.str();
    }
    out << "0" << record.code << ' ' << record.buyerName << ' ' << record.sellerName
        << " $" << fixed << setprecision(2) << record.amountCents/100.0 << ' ' << record.listing.gameTitle;
    return out.str();
}

void FrontEnd::executeFrontEnd() {
    marketplace.loadData(files.readAccounts(), files.readListings(), files.readCollections());
    output << "marketplace type help for commands\n";

    string command;
    while (true) {
        output << "\n> ";
        if (!getline(input, command)) break;   // EOF: stop without saving the unfinished session
        if (command == "exit") break;

        getFromMarketplace(command);

        if (command == "logout") {             // session completed: save its records
            vector<TransactionLog> records = marketplace.transactions();
            if (!records.empty()) {
                if (files.logTransactionsFromUser(records)) {
                    marketplace.resetTransactions();   // clear only after a confirmed write
                }
                else {
                    output << "could not write transaction output records kept\n";
                }
            }
        }
    }
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
            << "Money amounts are entered as cents, e.g. 1250 = $12.50.\n";
    }
    else if (command == "login") {
        showStats(marketplace.login(readValueInField("username: ")));
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
