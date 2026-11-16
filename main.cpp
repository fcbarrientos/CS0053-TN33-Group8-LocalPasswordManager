#include <chrono>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "password-generator/passwordgen.h"

using namespace std;
using Clock = chrono::steady_clock;

const string border = "==================================================================";
const string usersFileDB = "test_acc/users.txt";
const int maxAttempts = 3;
const chrono::seconds lockoutDuration(15);

struct Credential {
    string email;
    string password;
    string appName;
};

string vaultFilename(const string& userEmail) { return userEmail + ".txt"; }

vector<Credential> loadUsersFromTextDB(const string& filename) {
    vector<Credential> users;
    ifstream file(filename);
    Credential user;
    while (getline(file, user.email, ':') && getline(file, user.password)) {
        if (!user.email.empty() && !user.password.empty()) users.push_back(user);
    }
    return users;
}

bool credentialsMatch(const string& email, const string& password,
                      const vector<Credential>& users) {
    for (const Credential& user : users) {
        if (user.email == email && user.password == password) return true;
    }
    return false;
}

bool userLogin(const vector<Credential>& users, string& emailLoggedIn) {
    static int attempts = 0;
    static Clock::time_point lockoutUntil = Clock::time_point::min();
    string email;
    string password;

    if (Clock::now() < lockoutUntil) {
        cout << "Login is temporarily locked. Waiting for the 15-second cooldown...\n";
        this_thread::sleep_until(lockoutUntil);
        attempts = 0;
        return false;
    }
    if (attempts >= maxAttempts) attempts = 0;

    cout << "\n" << border << "\n"
         << "             Welcome User! Enter Credentials to Sign-in\n"
         << border << "\n";
    cout << "Email: ";
    getline(cin, email);
    cout << "Password: ";
    getline(cin, password);
    if (!cin) return false;

    if (credentialsMatch(email, password, users)) {
        attempts = 0;
        emailLoggedIn = email;
        cout << "Login Successful!\n";
        return true;
    }

    ++attempts;
    cout << "Login Failed! You have used " << attempts << " of "
         << maxAttempts << " attempts.\n";
    if (attempts >= maxAttempts) {
        lockoutUntil = Clock::now() + lockoutDuration;
        cout << "Too many failed attempts. Wait for 15 seconds before trying again.\n";
    }
    return false;
}

vector<Credential> loadVault(const string& userEmail) {
    vector<Credential> credentials;
    ifstream file(vaultFilename(userEmail));
    string line;
    while (getline(file, line)) {
        if (line.size() < 4 || line.front() != '[') continue;
        const size_t appEnd = line.find("] ");
        const size_t emailStart = appEnd == string::npos ? string::npos : appEnd + 2;
        const size_t separator = line.find(" | ", emailStart);
        if (appEnd == string::npos || separator == string::npos) continue;

        Credential credential;
        credential.appName = line.substr(1, appEnd - 1);
        credential.email = line.substr(emailStart, separator - emailStart);
        credential.password = line.substr(separator + 3);
        if (!credential.appName.empty() && !credential.email.empty()) {
            credentials.push_back(credential);
        }
    }
    return credentials;
}

bool saveVault(const string& userEmail, const vector<Credential>& credentials) {
    ofstream file(vaultFilename(userEmail), ios::trunc);
    if (!file) return false;
    for (const Credential& credential : credentials) {
        file << "[" << credential.appName << "] " << credential.email
             << " | " << credential.password << '\n';
    }
    return true;
}

void saveCredentialToFile(const string& userEmail, const Credential& credential) {
    vector<Credential> credentials = loadVault(userEmail);
    credentials.push_back(credential);
    if (!saveVault(userEmail, credentials)) {
        cout << "Error: Could not open the password vault.\n";
    }
}

void printVaultSummary(const vector<Credential>& credentials) {
    if (credentials.empty()) {
        cout << "No saved passwords found.\n";
        return;
    }
    cout << "\nSaved accounts (passwords are hidden):\n";
    for (size_t i = 0; i < credentials.size(); ++i) {
        cout << i + 1 << ". [" << credentials[i].appName << "] "
             << credentials[i].email << " | ********\n";
    }
}

string lowercase(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char character) { return static_cast<char>(tolower(character)); });
    return value;
}

bool parseEntryNumber(const string& input, size_t entryCount, size_t& selected) {
    try {
        size_t consumed = 0;
        const unsigned long value = stoul(input, &consumed);
        if (consumed != input.size() || value == 0 || value > entryCount) return false;
        selected = static_cast<size_t>(value - 1);
        return true;
    } catch (...) {
        return false;
    }
}

void browseManager(const string& currentUserEmail) {
    const vector<Credential> credentials = loadVault(currentUserEmail);
    cout << "\n" << border << "\n             [6] Browse Password Manager\n"
         << border << "\n";
    if (credentials.empty()) return;

    cout << "Search app/website or email (leave blank to show all): ";
    string searchTerm;
    getline(cin, searchTerm);

    const string searchText = lowercase(searchTerm);
    vector<Credential> matchingCredentials;
    for (const Credential& credential : credentials) {
        const string appName = lowercase(credential.appName);
        const string email = lowercase(credential.email);
        if (searchText.empty() || appName.find(searchText) != string::npos ||
            email.find(searchText) != string::npos) {
            matchingCredentials.push_back(credential);
        }
    }

    if (matchingCredentials.empty()) {
        cout << "No matching accounts found.\n";
        return;
    }
    printVaultSummary(matchingCredentials);

    cout << "Enter an entry number to view its password, or 0 to return: ";
    string input;
    getline(cin, input);
    if (input == "0") return;

    size_t selected = 0;
    if (!parseEntryNumber(input, matchingCredentials.size(), selected)) {
        cout << "Invalid entry number. Returning to the menu.\n";
        return;
    }
    cout << "Password for [" << matchingCredentials[selected].appName << "]: "
         << matchingCredentials[selected].password << '\n';
}

void deletePassword(const string& currentUserEmail) {
    vector<Credential> credentials = loadVault(currentUserEmail);
    cout << "\n" << border << "\n             [5] Delete Password\n"
         << border << "\n";
    printVaultSummary(credentials);
    if (credentials.empty()) return;

    cout << "Enter the entry number to delete, or 0 to cancel: ";
    string input;
    getline(cin, input);
    if (input == "0") return;

    size_t selected = 0;
    if (!parseEntryNumber(input, credentials.size(), selected)) {
        cout << "Invalid entry number. Nothing was deleted.\n";
        return;
    }

    cout << "Delete [" << credentials[selected].appName << "] "
         << credentials[selected].email << "? (y/n): ";
    string confirmation;
    getline(cin, confirmation);
    if (confirmation != "y" && confirmation != "Y") {
        cout << "Deletion cancelled.\n";
        return;
    }

    const string deletedApp = credentials[selected].appName;
    credentials.erase(credentials.begin() + static_cast<long>(selected));
    if (saveVault(currentUserEmail, credentials)) {
        cout << "Deleted [" << deletedApp << "] successfully.\n";
    } else {
        cout << "Error: Could not update the password vault.\n";
    }
}

void addPassword(const string& currentUserEmail) {
    Credential credential;
    string choice;
    cout << "\n" << border << "\n                   [3] Add Password\n"
         << border << "\n";
    cout << "Enter App / Website Name: ";
    getline(cin, credential.appName);
    if (credential.appName.empty()) {
        cout << "App / Website Name cannot be empty.\n";
        return;
    }

    cout << "Link your logged-in email (" << currentUserEmail << ") to this app? (y/n): ";
    getline(cin, choice);
    if (choice == "y" || choice == "Y") {
        credential.email = currentUserEmail;
    } else {
        cout << "Enter custom email / username: ";
        getline(cin, credential.email);
    }

    cout << "Do you want to generate a password? (y/n): ";
    getline(cin, choice);
    if (choice == "y" || choice == "Y") {
        passwordGenerator generator;
        passwordOptions options;
        cout << "Enter desired length (default 12, min 8, max 25): ";
        string lengthInput;
        getline(cin, lengthInput);
        options.length = 12;
        if (!lengthInput.empty()) {
            try {
                const int length = stoi(lengthInput);
                if (length >= 8 && length <= 25) options.length = length;
                else cout << "Invalid length. Using default length 12.\n";
            } catch (...) {
                cout << "Invalid length. Using default length 12.\n";
            }
        }
        credential.password = generator.generate(options);
        cout << "Generated Password: " << credential.password << '\n';
    } else {
        cout << "Enter Password: ";
        getline(cin, credential.password);
    }

    if (credential.email.empty() || credential.password.empty()) {
        cout << "Email and password cannot be empty. Nothing was saved.\n";
        return;
    }
    saveCredentialToFile(currentUserEmail, credential);
    cout << "Password for [" << credential.appName << "] successfully saved!\n";
}

bool userLogout() {
    cout << "\nYou have been logged out.\n" << border << '\n';
    string choice;
    while (true) {
        cout << "Do you want to log in again? (y/n): ";
        if (!getline(cin, choice)) return false;
        if (choice == "y" || choice == "Y") return true;
        if (choice == "n" || choice == "N") {
            cout << "Thank you for using LPMS! Bye :)\n";
            return false;
        }
        cout << "Invalid input. Please enter 'y' or 'n'.\n";
    }
}

void managerMenu(const string& currentUserEmail) {
    while (true) {
        cout << "\n" << border << "\n                  LOCAL PASSWORD MANAGER\n"
             << border << "\n[3] Add password\n[5] Delete password\n"
             << "[6] Browse manager\n[7] Logout\nChoose an option: ";
        string choice;
        getline(cin, choice);
        if (choice == "3") addPassword(currentUserEmail);
        else if (choice == "5") deletePassword(currentUserEmail);
        else if (choice == "6") browseManager(currentUserEmail);
        else if (choice == "7") return;
        else cout << "Invalid menu choice. Please choose 3, 5, 6, or 7.\n";
    }
}

int main() {
    while (true) {
        const vector<Credential> users = loadUsersFromTextDB(usersFileDB);
        if (users.empty()) {
            cerr << "Error: No fictional login account was found in " << usersFileDB << ".\n";
            return 1;
        }

        string currentUser;
        while (!userLogin(users, currentUser)) {
            if (!cin) return 0;
        }
        managerMenu(currentUser);
        if (!userLogout()) return 0;
    }
}
