#include "front.h"
#include <fstream>
#include <sstream>
#include <iomanip>

string trimPadding(string value)
{
    while (!value.empty() && (value.back() == ' ' || value.back() == '\r'))
    {
        value.pop_back();
    }
    return value;
}

int parseMoney(string text)
{
    //read money as cents without throwing conversion errors
    if (text.empty())
    {
        return -1;
    }
    long long amount = 0;
    bool decimal = false;
    int decimalPlaces = 0;
    int digits = 0;
    for (char character : text)
    {
        if (character == '.')
        {
            if (decimal || digits == 0)
            {
                return -1;
            }
            decimal = true;
        }
        else
        {
            if (character < '0' || character > '9')
            {
                return -1;
            }
            amount = amount * 10 + character - '0';
            digits++;
            if (decimal)
            {
                decimalPlaces++;
            }
            if (decimalPlaces > 2 || amount > 99999999)
            {
                return -1;
            }
        }
    }
    if (decimal && decimalPlaces == 0)
    {
        return -1;
    }
    while (decimalPlaces < 2)
    {
        amount *= 10;
        decimalPlaces++;
    }
    if (amount > 99999999)
    {
        return -1;
    }
    return static_cast<int>(amount);
}

vector<UserAccount> FrontEndFileManager::readAccounts()
{
    vector<UserAccount> result;
    ifstream file(accountpath);
    string accountline;
    while (getline(file, accountline))
    {
        if (trimPadding(accountline) == "END")
        {
            break;
        }
        if (accountline.length() < 28)
        {
            continue;
        }
        UserAccount account;
        account.username = trimPadding(accountline.substr(0, 15));
        account.type = accountline.substr(16, 2);
        account.credit = parseMoney(accountline.substr(19, 9));
        if (account.credit >= 0)
        {
            result.push_back(account);
        }
    }
    return result;
}

vector<GameListing> FrontEndFileManager::readListings()
{
    vector<GameListing> result;
    ifstream file(listingpath);
    string line;
    while (getline(file, line))
    {
        if (trimPadding(line) == "END")
        {
            break;
        }
        if (line.length() < 48)
        {
            continue;
        }
        //read game title seller and dollar price from fixed fields
        GameListing listing;
        listing.gameTitle = trimPadding(line.substr(0, 25));
        listing.sellerName = trimPadding(line.substr(26, 15));
        listing.priceInMoney = parseMoney(line.substr(42, 6));
        if (listing.priceInMoney >= 0)
        {
            result.push_back(listing);
        }
    }
    return result;
}

vector<UserOwnsGame> FrontEndFileManager::readCollections()
{
    vector<UserOwnsGame> result;
    ifstream file(collectionpath);
    string line;
    while (getline(file, line))
    {
        if (trimPadding(line) == "END")
        {
            break;
        }
        if (line.length() < 41)
        {
            continue;
        }
        UserOwnsGame item;
        item.gameTitle = trimPadding(line.substr(0, 25));
        item.ownerUsername = trimPadding(line.substr(26, 15));
        result.push_back(item);
    }
    return result;
}

bool FrontEndFileManager::logTransactionsFromUser(vector<TransactionLog> records)
{
    ofstream file(transactionpath, ios::app);
    for (const auto& record : records)
    {
        file << formatTransaction(record) << '\n';
    }
    file.close();
    return !file.fail();
}

string FrontEndFileManager::formatTransaction(TransactionLog record)
{
    ostringstream out;
    //use the transaction layouts supplied in phase one
    if (record.code == 0)
    {
        return "00" + string(20, ' ') + "000000000";
    }
    out << '0' << record.code << ' ' << left;
    if (record.code == 1 || record.code == 2 || record.code == 6)
    {
        out << setw(15) << record.buyerName << ' ' << record.buyerAccount.type << ' '
            << right << setfill('0') << setw(9) << fixed << setprecision(2)
            << record.amountCents / 100.0;
    }
    else if (record.code == 3 || record.code == 4)
    {
        out << setw(26) << record.listing.gameTitle << ' ' << setw(15) << record.sellerName << ' ';
        if (record.code == 4)
        {
            out << setw(15) << record.buyerName << ' ';
        }
        out << right << setfill('0') << setw(6) << fixed << setprecision(2)
            << record.amountCents / 100.0;
    }
    else if (record.code == 5)
    {
        out << setw(15) << record.buyerName << ' ' << setw(15) << record.sellerName << ' '
            << right << setfill('0') << setw(9) << fixed << setprecision(2)
            << record.amountCents / 100.0;
    }
    return out.str();
}

void FrontEnd::executeFrontEnd()
{
    marketplace.loadData(files.readAccounts(), files.readListings(), files.readCollections());
    output << "marketplace type help for commands\n";
    string command;
    while (true)
    {
        output << "\n> ";
        if (!getline(input, command) || command == "exit")
        {
            break;
        }
        getFromMarketplace(command);
    }
}

void FrontEnd::showStats(ResultingChange result)
{
    output << result.message << '\n';
    //save records after normal or automatic logout
    if (result.sessionEnded)
    {
        if (files.logTransactionsFromUser(marketplace.transactions()))
        {
            marketplace.resetTransactions();
        }
        else
        {
            output << "could not write transaction output records kept\n";
        }
    }
}

void FrontEnd::infoMarketplace()
{
    vector<GameListing> games = marketplace.currentAvailableGames();
    for (const auto& game : games)
    {
        output << game.gameTitle << " | seller: " << game.sellerName
            << " | $" << fixed << setprecision(2) << game.priceInMoney / 100.0 << '\n';
    }
    if (!marketplace.isLoggedIn())
    {
        //show loaded ownership data during file inspection
        for (const auto& owned : marketplace.currentCollections())
        {
            output << owned.ownerUsername << " owns " << owned.gameTitle << ".\n";
        }
    }
    else if (games.empty())
    {
        output << "No games available.\n";
    }
}

int FrontEnd::getUserMoney(string text)
{
    return parseMoney(text);
}

string FrontEnd::readValueInField(string prompt)
{
    output << prompt;
    string value;
    getline(input, value);
    return value;
}

void FrontEnd::getFromMarketplace(string command)
{
    if (command == "help")
    {
        output << "Commands: login, logout, list, buy, sell, addcredit, create, delete, refund, exit\n"
            << "Money amounts are entered in dollars, e.g. 12.50.\n";
    }
    else if (command == "login")
    {
        showStats(marketplace.login(readValueInField("username: ")));
    }
    else if (command == "logout")
    {
        showStats(marketplace.logout());
    }
    else if (command == "list")
    {
        infoMarketplace();
    }
    else if (command == "buy")
    {
        string title = readValueInField("Game title: ");
        string seller = readValueInField("Seller username: ");
        showStats(marketplace.userBuyGame(title, seller));
    }
    else if (command == "sell")
    {
        string title = readValueInField("Game title: ");
        int price = getUserMoney(readValueInField("Price in dollars: "));
        showStats(marketplace.sellGame(title, price));
    }
    else if (command == "addcredit" || command == "credit")
    {
        //standard users add credit to their own account
        string username = marketplace.currentUsername();
        if (marketplace.isCurrentAdmin())
        {
            username = readValueInField("Account username: ");
        }
        int amount = getUserMoney(readValueInField("Amount in dollars: "));
        showStats(marketplace.addToCredit(username, amount));
    }
    else if (command == "create")
    {
        string username = readValueInField("New username: ");
        string type = readValueInField("Type (AA/FS/BS/SS): ");
        int credit = getUserMoney(readValueInField("Starting credit in dollars: "));
        showStats(marketplace.createAccount(username, type, credit));
    }
    else if (command == "delete")
    {
        showStats(marketplace.deleteAccount(readValueInField("Username to delete: ")));
    }
    else if (command == "refund")
    {
        string buyer = readValueInField("Buyer username: ");
        string seller = readValueInField("Seller username: ");
        int amount = getUserMoney(readValueInField("Refund amount in dollars: "));
        showStats(marketplace.refund(buyer, seller, amount));
    }
    else
    {
        output << "Unrecognized command.\n";
    }
}
