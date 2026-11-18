#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <stdexcept>

#include "password-generator/passwordgen.h"

using namespace std;
using Clock = chrono::steady_clock;

const string border = "==================================================================";
const string usersFileDB = "test_acc/users.txt"; //I will only add my fake credentials here sa file, add yours na lang guys
const int maxAttempts = 3;
const chrono::seconds lockoutDuration(15);
struct Credential {
    string email;
    string password;
    string appName;
}; //naka-structure para nasa iisang unit lang tong dalawa since codependent sila sa isa't isa
// pinalitan ko Credential


vector<Credential> loadUsersFromTextDB(const string& filename) { //I used vector instead of an array kasi suggested 
    vector<Credential> users;
    ifstream file(filename);
    Credential u;
    while (getline(file, u.email, ':') && getline(file, u.password)) {
        users.push_back(u);
    }
    return users;
}

bool credentialsMatch(const string& email, const string& password, const vector<Credential>& users) {
    for (const Credential& u : users) {
        if (u.email == email && u.password == password) {
            return true;
        }
    }
    return false;
}

bool userLogin(const vector<Credential>& users, string& emailLoggedIn) {
    static int attempts = 0;
    static Clock::time_point lockoutUntil = Clock::time_point::min();
    string email, password;

    //etong block for lockout mechanism, i tried using threads para sa countdown pero d ko ma-implement  
    if (Clock::now() < lockoutUntil) {
        return false;
    }
    if (attempts >= maxAttempts) attempts = 0; //reset attempts after isang lockout period
    
    cout << "\n" << border << endl;
    cout << "             Welcome User! Enter Credentials to Sign-in           " << endl;
    cout << border << endl;
    cout << "Email: "; getline(cin, email);
    cout << "Password: "; getline(cin, password);
    
    if (!cin) return false; //input failure 

    if (credentialsMatch(email, password, users)) {
            attempts = 0; //reset attempts on successful login
            emailLoggedIn = email;
            cout << "Login Successful!" << endl;
            return true;
    } else {
        attempts++;
        cout << "Login Failed! You have used " << attempts << " of " << maxAttempts << " attempts." << endl;
            if (attempts >= maxAttempts) {
                lockoutUntil = Clock::now() + lockoutDuration;
                cout << "Too many failed attempts. Wait for 15 seconds before trying again." << endl;
            }
    }

    return false;
}

bool promptYesNo(const string& prompt, bool& answer) {
    string choice;
    while (true) {
        cout << prompt;
        if (!getline(cin, choice)) {
            return false;
        }

        if (choice == "y" || choice == "Y") {
            answer = true;
            return true;
        }
        if (choice == "n" || choice == "N") {
            answer = false;
            return true;
        }

        cout << "Invalid input. Please enter 'y' or 'n'." << endl;
    }
}

bool userLogout() {
    cout << "\n";    
    cout << "                     You have been logged out.                    " << endl;
    cout << border << endl;

    bool logInAgain;
    if (!promptYesNo("Do you want to log in again? (y/n): ", logInAgain)) {
        return false;
    }
    if (!logInAgain) {
        cout << "Thank you for using LPMS! Bye :)" << endl;
    }
    return logInAgain;
}

// YOOHOO DITO SI SAVING CONTENT TO FILE
void saveCredentialToFile(const string& userEmail, Credential &cred) {
    // [GOOGLE.COM] usernumber1@yahoo.com | passwordnumber1HEHE!
    string filename = userEmail + ".txt";
    ofstream outFile(filename, ios::app); // append
    if (outFile.is_open()) {
        outFile << "[" << cred.appName << "] " << cred.email << " | " << cred.password << "\n";
        outFile.close();
    } else {
        cout << "Error: Could not open file to save password.\n"; // pangsalo lang to pag may nangyaring ewan
    }
}

void addPassword(const string& currentUserEmail) {
    Credential cred;
    bool useCurrentEmail;

    cout << "\n" << border << "\n";
    cout << "                   [3] Add Password\n";
    cout << border << "\n";

    cout << "Enter App / Website Name: ";
    getline(cin, cred.appName);

    if (!promptYesNo("Link your logged in email (" + currentUserEmail + ") to this app? (y/n): ",
                     useCurrentEmail)) {
        return;
    }
    if (useCurrentEmail) {
        cred.email = currentUserEmail;
    } else {
        cout << "Enter custom email / username for " << cred.appName << ": ";
        if (!getline(cin, cred.email)) return;
    }

    bool generatePassword;
    if (!promptYesNo("Do you want to generate a password? (y/n): ", generatePassword)) {
        return;
    }
    if (generatePassword) {
        passwordGenerator generator;
        passwordOptions options;

        cout << "Enter desired length (default 12, min 8, max 25): ";
        string lenStr;
        if (!getline(cin, lenStr)) return;
        bool validLength = false;
        if (!lenStr.empty()) {
            try {
                size_t charactersRead = 0;
                const int length = stoi(lenStr, &charactersRead);
                if (charactersRead == lenStr.size() && length >= 8 && length <= 25) {
                    options.length = length;
                    validLength = true;
                }
            } catch (const invalid_argument&) {
            } catch (const out_of_range&) {
            } //nag try catch lang to para di mag-crash yung program pag may invalid input
        }
        if (!validLength) {
            cout << "Invalid or missing length. Using the default length of 12.\n";
            options.length = 12;
        }

        cred.password = generator.generate(options);
        cout << "Generated Password: " << cred.password << "\n";
    } else {
        cout << "Enter Password: ";
        if (!getline(cin, cred.password)) return;
    }
    
    saveCredentialToFile(currentUserEmail, cred);
    cout << "\nPassword for [" << cred.appName << "] successfully saved!\n";
}

// PART 6: Browse saved passwords without revealing passwords by default.
vector<Credential> loadSavedPasswords(const string& userEmail) {
    vector<Credential> credentials;
    ifstream file(userEmail + ".txt");
    string line;

    while (getline(file, line)) {
        const size_t appEnd = line.find("] ");
        if (line.empty() || line.front() != '[' || appEnd == string::npos) {
            continue;
        }

        const size_t emailStart = appEnd + 2;
        const size_t separator = line.find(" | ", emailStart);
        if (separator == string::npos) {
            continue;
        }

        Credential credential;
        credential.appName = line.substr(1, appEnd - 1);
        credential.email = line.substr(emailStart, separator - emailStart);
        credential.password = line.substr(separator + 3);
        credentials.push_back(credential);
    }
    return credentials;
}

string lowercaseText(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char character) {
                  return static_cast<char>(tolower(character));
              });
    return value;
}

void showSavedPasswords(const vector<Credential>& credentials) {
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

bool getEntryNumber(const string& input, size_t entryCount, size_t& selected) {
    try {
        size_t charactersRead = 0;
        const unsigned long number = stoul(input, &charactersRead);
        if (charactersRead != input.size() || number == 0 || number > entryCount) {
            return false;
        }
        selected = static_cast<size_t>(number - 1);
        return true;
    } catch (...) {
        return false;
    }
}

void browsePasswordManager(const string& currentUserEmail) {
    const vector<Credential> allCredentials = loadSavedPasswords(currentUserEmail);

    cout << "\n" << border << endl;
    cout << "                  [6] Browse Password Manager" << endl;
    cout << border << endl;

    if (allCredentials.empty()) {
        cout << "No saved passwords found.\n";
        return;
    }

    cout << "Search app / website or email (leave blank to show all): ";
    string searchTerm;
    getline(cin, searchTerm);

    const string searchText = lowercaseText(searchTerm);
    vector<Credential> matchingCredentials;
    for (const Credential& credential : allCredentials) {
        if (searchText.empty() ||
            lowercaseText(credential.appName).find(searchText) != string::npos ||
            lowercaseText(credential.email).find(searchText) != string::npos) {
            matchingCredentials.push_back(credential);
        }
    }

    if (matchingCredentials.empty()) {
        cout << "No matching accounts found.\n";
        return;
    }

    showSavedPasswords(matchingCredentials);
    cout << "Enter an entry number to view its password, or 0 to return: ";
    string input;
    getline(cin, input);
    if (input == "0") {
        return;
    }

    size_t selected = 0;
    if (!getEntryNumber(input, matchingCredentials.size(), selected)) {
        cout << "Invalid entry number. Returning to the menu.\n";
        return;
    }

    cout << "Password for [" << matchingCredentials[selected].appName << "]: "
         << matchingCredentials[selected].password << '\n';
}

// PART 5: Delete one saved password after displaying a masked list and confirmation.
bool rewritePasswordFile(const string& userEmail,
                         const vector<Credential>& credentials) {
    ofstream file(userEmail + ".txt", ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    for (const Credential& credential : credentials) {
        file << "[" << credential.appName << "] " << credential.email
             << " | " << credential.password << '\n';
    }
    return true;
}

void deletePassword(const string& currentUserEmail) {
    vector<Credential> credentials = loadSavedPasswords(currentUserEmail);

    cout << "\n" << border << endl;
    cout << "                    [5] Delete Password" << endl;
    cout << border << endl;
    showSavedPasswords(credentials);
    if (credentials.empty()) {
        return;
    }

    cout << "Enter the entry number to delete, or 0 to cancel: ";
    string input;
    getline(cin, input);
    if (input == "0") {
        return;
    }

    size_t selected = 0;
    if (!getEntryNumber(input, credentials.size(), selected)) {
        cout << "Invalid entry number. Nothing was deleted.\n";
        return;
    }

    bool confirmDelete;
    if (!promptYesNo("Delete [" + credentials[selected].appName + "] " +
                     credentials[selected].email + "? (y/n): ",
                     confirmDelete)) {
        return;
    }
    if (!confirmDelete) {
        cout << "Deletion cancelled.\n";
        return;
    } 

    const string deletedApp = credentials[selected].appName;
    credentials.erase(credentials.begin() + static_cast<long>(selected));
    if (rewritePasswordFile(currentUserEmail, credentials)) {
        cout << "Deleted [" << deletedApp << "] successfully.\n";
    } else {
        cout << "Error: Could not update the password file.\n";
    }
}

void passwordManagerMenu(const string& currentUserEmail) {
    while (true) {
        cout << "\n" << border << endl;
        cout << "                  LOCAL PASSWORD MANAGER" << endl;
        cout << border << endl;
        cout << "[3] Add password" << endl;
        cout << "[5] Delete password" << endl;
        cout << "[6] Browse manager" << endl;
        cout << "[7] Logout" << endl;
        cout << "Choose an option: ";

        string choice;
        getline(cin, choice);
        if (choice == "3") {
            addPassword(currentUserEmail);
        } else if (choice == "5") {
            deletePassword(currentUserEmail);
        } else if (choice == "6") {
            browsePasswordManager(currentUserEmail);
        } else if (choice == "7") {
            return;
        } else {
            cout << "Invalid menu choice. Please choose 3, 5, 6, or 7.\n";
        }
    }
}

int main() {
    //GENERATED THIS IS FOR TESTING PURPOSES HUHU, D MUNA AKO GUMAWA NG MENU
    bool running = true;
    while (running) {
        vector<Credential> users = loadUsersFromTextDB(usersFileDB);   // reloaded each round
        if (users.empty()) return 1;

        string currentUser;
        while (!userLogin(users, currentUser)) {
            if (!cin) return 0;
        }

        passwordManagerMenu(currentUser);
        running = userLogout();
    }
    return 0;
}
