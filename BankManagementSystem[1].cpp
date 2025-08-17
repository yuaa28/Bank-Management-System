#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <sstream>
#include <cmath>
using namespace std;

class Transaction {
private:
    string date;
    string type;
    double amount;
    double balanceAfter;

public:
    Transaction(string t, double amt, double bal) {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        stringstream ss;
        ss << 1900 + ltm->tm_year << "-"
           << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
           << setw(2) << setfill('0') << ltm->tm_mday << " "
           << setw(2) << setfill('0') << ltm->tm_hour << ":"
           << setw(2) << setfill('0') << ltm->tm_min << ":"
           << setw(2) << setfill('0') << ltm->tm_sec;
        date = ss.str();
        type = t;
        amount = amt;
        balanceAfter = bal;
    }

    string getTransactionDetails() const {
        stringstream ss;
        ss << left << setw(20) << date
           << setw(15) << type
           << setw(15) << fixed << setprecision(2) << amount
           << setw(15) << fixed << setprecision(2) << balanceAfter;
        return ss.str();
    }
};

class Account {
private:
    string accountNumber;
    string name;
    double balance;
    vector<Transaction> transactions;

public:
    Account(string accNum, string accName, double initialBalance)
        : accountNumber(accNum), name(accName), balance(initialBalance) {
        transactions.emplace_back("Initial Deposit", initialBalance, balance);
    }

    string getAccountNumber() const { return accountNumber; }
    string getName() const { return name; }
    double getBalance() const { return balance; }

    void deposit(double amount) {
        balance += amount;
        transactions.emplace_back("Deposit", amount, balance);
        cout << "Deposited: " << amount << ". New Balance: " << balance << endl;
    }

    void withdraw(double amount) {
        if (amount > balance) {
            cout << "Insufficient funds!" << endl;
        } else {
            balance -= amount;
            transactions.emplace_back("Withdrawal", amount, balance);
            cout << "Withdrew: " << amount << ". New Balance: " << balance << endl;
        }
    }

    void display() const {
        cout << "Account Number: " << accountNumber << "\nName: " << name
             << "\nBalance: " << balance << "\n";
    }

    void displayTransactions() const {
        cout << left << setw(20) << "Date"
             << setw(15) << "Type"
             << setw(15) << "Amount"
             << setw(15) << "Balance" << endl;
        cout << string(65, '-') << endl;
        for (const auto& t : transactions) {
            cout << t.getTransactionDetails() << endl;
        }
    }

    string toString() const {
        stringstream ss;
        ss << accountNumber << "," << name << "," << balance;
        return ss.str();
    }

    static Account fromString(const string& data) {
        stringstream ss(data);
        string accNum, accName;
        double bal;
        getline(ss, accNum, ',');
        getline(ss, accName, ',');
        ss >> bal;
        return Account(accNum, accName, bal);
    }

    const vector<Transaction>& getTransactions() const {
        return transactions;
    }
};

class Bank {
private:
    vector<Account> accounts;

    void saveToFile() {
        ofstream file("bank_data.txt");
        for (const auto& acc : accounts) {
            file << acc.toString() << endl;
        }
        file.close();
        ofstream transFile("transactions_data.txt");
        for (const auto& acc : accounts) {
            for (const auto& t : acc.getTransactions()) {
                transFile << acc.getAccountNumber() << ","
                          << t.getTransactionDetails() << endl;
            }
        }
        transFile.close();
    }

    void loadFromFile() {
        ifstream file("bank_data.txt");
        string line;
        while (getline(file, line)) {
            accounts.push_back(Account::fromString(line));
        }
        file.close();
    }

public:
    Bank() { loadFromFile(); }

    void createAccount(const string& accNum, const string& name, double balance) {
        accounts.emplace_back(accNum, name, balance);
        saveToFile();
    }

    void deposit(const string& accNum, double amount) {
        for (auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                acc.deposit(amount);
                saveToFile();
                return;
            }
        }
        cout << "Account not found!" << endl;
    }

    void withdraw(const string& accNum, double amount) {
        for (auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                acc.withdraw(amount);
                saveToFile();
                return;
            }
        }
        cout << "Account not found!" << endl;
    }

    void displayAccount(const string& accNum) {
        for (const auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                acc.display();
                return;
            }
        }
        cout << "Account not found!" << endl;
    }

    void displayAllAccounts() {
        for (const auto& acc : accounts) {
            acc.display();
            cout << "-------------------------\n";
        }
    }

    void displayTransactions(const string& accNum) {
        for (const auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                acc.displayTransactions();
                return;
            }
        }
        cout << "Account not found!" << endl;
    }

    void searchAccountByName(const string& searchName) {
        bool found = false;
        string searchLower = searchName;
        transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
        for (const auto& acc : accounts) {
            string accName = acc.getName();
            string accNameLower = accName;
            transform(accNameLower.begin(), accNameLower.end(), accNameLower.begin(), ::tolower);
            if (accNameLower.find(searchLower) != string::npos) {
                acc.display();
                cout << "-------------------------\n";
                found = true;
            }
        }
        if (!found) cout << "No account found with the name containing: " << searchName << endl;
    }
};

void displayMenu() {
    cout << "\n--- Bank Management System ---\n";
    cout << "1. Create Account\n2. Deposit\n3. Withdraw\n4. Display Account\n";
    cout << "5. Display All Accounts\n6. Display Transactions\n7. Search Account by Name\n0. Exit\n";
    cout << "Enter choice: ";
}

int main() {
    Bank bank;
    int choice;
    do {
        displayMenu();
        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        string accNum, name;
        double amount;
        switch (choice) {
            case 1:
                cout << "Enter Account Number: ";
                cin >> accNum;
                cout << "Enter Name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter Initial Balance: ";
                cin >> amount;
                bank.createAccount(accNum, name, amount);
                break;
            case 2:
                cout << "Enter Account Number: ";
                cin >> accNum;
                cout << "Enter Deposit Amount: ";
                cin >> amount;
                bank.deposit(accNum, amount);
                break;
            case 3:
                cout << "Enter Account Number: ";
                cin >> accNum;
                cout << "Enter Withdrawal Amount: ";
                cin >> amount;
                bank.withdraw(accNum, amount);
                break;
            case 4:
                cout << "Enter Account Number: ";
                cin >> accNum;
                bank.displayAccount(accNum);
                break;
            case 5:
                bank.displayAllAccounts();
                break;
            case 6:
                cout << "Enter Account Number: ";
                cin >> accNum;
                bank.displayTransactions(accNum);
                break;
            case 7:
                cout << "Enter Name to Search: ";
                cin.ignore();
                getline(cin, name);
                bank.searchAccountByName(name);
                break;
            case 0:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice!" << endl;
        }
    } while (choice != 0);
    return 0;
}
