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

const string border = "=================================================================================";
const string usersFileDB =
    "test_acc/users.txt"; //I will only add my fake credentials here sa file, add yours na lang guys
const int maxAttempts = 3;
const chrono::seconds lockoutDuration(15);
struct Credential {
    string email;
    string password;
    string appName;
}; //naka-structure para nasa iisang unit lang tong dalawa since codependent sila sa isa't isa

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
    if (attempts >= maxAttempts)
        attempts = 0; //reset attempts after isang lockout period

    cout << "\n" << border << endl;
    cout << "                   Welcome User! Enter Credentials to Sign-in                    " << endl;
    cout << border << endl;
    cout << "Email: ";
    getline(cin, email);
    cout << "Password: ";
    getline(cin, password);

    if (!cin)
        return false; //input failure

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

// PART 7: shared input helpers, validators, and the password-length prompt.

string trimText(const string& value) {
    const size_t first = value.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }
    const size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

string lowercaseText(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char character) { return static_cast<char>(tolower(character)); });
    return value;
}

// every rejection gets its own paragraph and space
void printInputError(const string& message) { cout << "\n" << message << "\n\n"; }

bool promptYesNo(const string& prompt, bool& answer) {
    while (true) {
        cout << prompt;
        string choice;
        if (!getline(cin, choice)) {
            return false;
        }

        choice = lowercaseText(trimText(choice));
        if (choice == "y" || choice == "yes") {
            answer = true;
            return true;
        }
        if (choice == "n" || choice == "no") {
            answer = false;
            return true;
        }

        printInputError("Invalid input. Please enter 'y' or 'n'.");
    }
}

// a hostname needs at least one dot so that "Google.com" and "x.com" pass
bool isValidHostname(const string& host) {
    if (host.empty() || host.size() > 253) {
        return false;
    }
    if (host.front() == '.' || host.back() == '.') {
        return false;
    }

    vector<string> labels;
    size_t start = 0;
    while (true) {
        const size_t dot = host.find('.', start);
        const size_t end = (dot == string::npos) ? host.size() : dot;
        labels.push_back(host.substr(start, end - start));
        if (dot == string::npos) {
            break;
        }
        start = dot + 1;
    }
    if (labels.size() < 2) {
        return false;
    }

    for (const string& label : labels) {
        if (label.empty() || label.size() > 63) {
            return false;
        }
        if (label.front() == '-' || label.back() == '-') {
            return false;
        }
        for (char character : label) {
            const unsigned char uc = static_cast<unsigned char>(character);
            if (!isalnum(uc) && character != '-') {
                return false;
            }
        }
    }

    const string& tld = labels.back();
    if (tld.size() < 2) {
        return false;
    }
    for (char character : tld) {
        if (!isalpha(static_cast<unsigned char>(character))) {
            return false;
        }
    }
    return true;
}

// stricter inputs for websites (needs a domain like .com)
bool isValidWebsiteName(const string& name) { return isValidHostname(name); }

bool isValidEmail(const string& email) {
    if (email.empty() || email.size() > 254) {
        return false;
    }

    const size_t at = email.find('@');
    if (at == string::npos || email.find('@', at + 1) != string::npos) {
        return false; // exactly one '@' is required
    }

    const string local = email.substr(0, at);
    const string domain = email.substr(at + 1);
    if (local.empty() || local.size() > 64) {
        return false;
    }
    if (local.front() == '.' || local.back() == '.' || local.find("..") != string::npos) {
        return false;
    }
    for (char character : local) {
        const unsigned char uc = static_cast<unsigned char>(character);
        if (!isalnum(uc) && string("._%+-").find(character) == string::npos) {
            return false;
        }
    }
    return isValidHostname(domain);
}

vector<Credential> loadSavedPasswords(const string& userEmail); // defined in PART 6 below

// asks for the default 12 characters first and only then offers a custom length
bool choosePasswordLength(int& length) {
    bool useDefault;
    if (!promptYesNo("Use the default password length (12 characters)? (y/n): ", useDefault)) {
        return false;
    }
    if (useDefault) {
        length = 12;
        return true;
    }

    while (true) {
        cout << "Enter desired password length (min 8, max 25): ";
        string input;
        if (!getline(cin, input)) {
            return false;
        }

        const string candidate = trimText(input);
        if (!candidate.empty()) {
            try {
                size_t charactersRead = 0;
                const int parsed = stoi(candidate, &charactersRead);
                if (charactersRead == candidate.size() && parsed >= 8 && parsed <= 25) {
                    length = parsed;
                    return true;
                }
            } catch (const invalid_argument&) {
            } catch (const out_of_range&) {
            }
        }

        printInputError("Invalid length. Please enter a whole number between 8 and 25.");
    }
}

bool userLogout() {
    cout << "\n";
    cout << "                            You have been logged out.                            " << endl;
    cout << border << endl;

    bool logInAgain;
    if (!promptYesNo("Do you want to log in again? (y/n): ", logInAgain)) {
        return false;
    }
    if (!logInAgain) {
        cout << "\nThank you for using LPMS! Bye :)" << endl;
    }
    return logInAgain;
}

// YOOHOO DITO SI SAVING CONTENT TO FILE
void saveCredentialToFile(const string& userEmail, Credential& cred) {
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

bool readWebsiteName(const vector<Credential>& existing, string& websiteName) {
    while (true) {
        cout << "Enter Website Name (e.g. Google.com or x.com): ";
        string input;
        if (!getline(cin, input)) {
            return false;
        }

        const string candidate = trimText(input);
        if (!isValidWebsiteName(candidate)) {
            printInputError("Invalid website name. Use a plain domain such as Google.com or x.com "
                            "(no spaces, no http://, and it must end with letters like .com).");
            continue;
        }

        bool alreadySaved = false;
        for (const Credential& credential : existing) {
            if (lowercaseText(credential.appName) == lowercaseText(candidate)) {
                alreadySaved = true;
                break;
            }
        }
        if (alreadySaved) {
            printInputError("[" + candidate + "] is already saved. Use modify password instead.");
            continue;
        }

        websiteName = candidate;
        return true;
    }
}

bool readEmail(const string& prompt, string& email) {
    while (true) {
        cout << prompt;
        string input;
        if (!getline(cin, input)) {
            return false;
        }

        const string candidate = trimText(input);
        if (!isValidEmail(candidate)) {
            printInputError("Invalid email. It needs exactly one '@' and a domain, e.g. user@example.com.");
            continue;
        }

        email = candidate;
        return true;
    }
}

void addPassword(const string& currentUserEmail) {
    Credential cred;
    bool useCurrentEmail;

    cout << "\n" << border << "\n";
    cout << "                                  [1] Add Password                                 " << endl;
    cout << border << "\n";

    if (!readWebsiteName(loadSavedPasswords(currentUserEmail), cred.appName)) {
        return;
    }

    if (!promptYesNo("Link your logged in email (" + currentUserEmail + ") to this app? (y/n): ", useCurrentEmail)) {
        return;
    }
    if (useCurrentEmail) {
        cred.email = currentUserEmail;
    } else {
        if (!readEmail("Enter custom email for " + cred.appName + ": ", cred.email))
            return;
    }

    bool generatePassword;
    if (!promptYesNo("Do you want to generate a password? (y/n): ", generatePassword)) {
        return;
    }
    if (generatePassword) {
        passwordGenerator generator;
        passwordOptions options;

        int length = 12;
        if (!choosePasswordLength(length)) {
            return;
        }
        options.length = length;

        cred.password = generator.generate(options);
        cout << "\nGenerated Password: " << cred.password << "\n";
    } else {
        cout << "Enter Password: ";
        if (!getline(cin, cred.password))
            return;
    }

    saveCredentialToFile(currentUserEmail, cred);
    cout << "\nPassword for [" << cred.appName << "] successfully saved!\n";
}

void modifyPassword(const string& currentUserEmail) {
    vector<Credential> credentials = loadSavedPasswords(currentUserEmail);

    cout << "\n" << border << endl;
    cout << "                              [2] Modify Password                                " << endl;
    cout << border << endl;
    showSavedPasswords(credentials);
    if (credentials.empty()) {
        return;
    }

    cout << "Enter the entry number to modify, or 0 to cancel: ";
    string input;
    getline(cin, input);
    if (input == "0") {
        return;
    }

    size_t selected = 0;
    if (!getEntryNumber(input, credentials.size(), selected)) {
        cout << "\nInvalid entry number. Nothing was modified.\n";
        return;
    }

    Credential& cred = credentials[selected];

    cout << "\nModifying entry " << (selected + 1) << " of " << credentials.size() << "\n";
    cout << "Website: " << cred.appName << "   (cannot be changed)\n";
    cout << "Email: " << cred.email << "   (cannot be changed)\n";
    cout << "Only the password can be modified.\n";

    bool generatePassword;
    if (!promptYesNo("Do you want to generate a new password? (y/n): ", generatePassword)) {
        return;
    }
    if (generatePassword) {
        passwordGenerator generator;
        passwordOptions options;

        int length = 12;
        if (!choosePasswordLength(length)) {
            return;
        }
        options.length = length;

        cred.password = generator.generate(options);
        cout << "\nGenerated Password: " << cred.password << "\n";
    } else {
        cout << "Enter new password (leave blank to keep current): ";
        getline(cin, input);
        if (!input.empty())
            cred.password = input;
    }

    bool saveChanges;
    if (!promptYesNo("Save changes to [" + cred.appName + "]? (y/n): ", saveChanges)) {
        return;
    }
    if (!saveChanges) {
        cout << "\nModification cancelled. Nothing was changed.\n";
        return;
    }

    if (rewritePasswordFile(currentUserEmail, credentials)) {
        cout << "\nUpdated [" << cred.appName << "] successfully.\n";
    } else {
        cout << "\nError: Could not update the password file.\n";
    }
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

void showSavedPasswords(const vector<Credential>& credentials) {
    if (credentials.empty()) {
        cout << "No saved passwords found.\n";
        return;
    }

    cout << "\nSaved accounts (passwords are hidden):\n";
    for (size_t i = 0; i < credentials.size(); ++i) {
        cout << i + 1 << ". [" << credentials[i].appName << "] " << credentials[i].email << " | ********\n";
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
    cout << "                           [4] Browse Password Manager                           " << endl;
    cout << border << endl;

    if (allCredentials.empty()) {
        cout << "No saved passwords found.\n";
        return;
    }

    cout << "Search website or email (leave blank to show all): ";
    string searchTerm;
    getline(cin, searchTerm);

    const string searchText = lowercaseText(searchTerm);
    vector<Credential> matchingCredentials;
    for (const Credential& credential : allCredentials) {
        if (searchText.empty() || lowercaseText(credential.appName).find(searchText) != string::npos ||
            lowercaseText(credential.email).find(searchText) != string::npos) {
            matchingCredentials.push_back(credential);
        }
    }

    if (matchingCredentials.empty()) {
        cout << "\nNo matching accounts found.\n";
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
        cout << "\nInvalid entry number. Returning to the menu.\n";
        return;
    }

    cout << "Password for [" << matchingCredentials[selected].appName << "]: " << matchingCredentials[selected].password
         << '\n';
}

// PART 5: Delete one saved password after displaying a masked list and confirmation.
bool rewritePasswordFile(const string& userEmail, const vector<Credential>& credentials) {
    ofstream file(userEmail + ".txt", ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    for (const Credential& credential : credentials) {
        file << "[" << credential.appName << "] " << credential.email << " | " << credential.password << '\n';
    }
    return true;
}

void deletePassword(const string& currentUserEmail) {
    vector<Credential> credentials = loadSavedPasswords(currentUserEmail);

    cout << "\n" << border << endl;
    cout << "                               [3] Delete Password                               " << endl;
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
        cout << "\nInvalid entry number. Nothing was deleted.\n";
        return;
    }

    bool confirmDelete;
    if (!promptYesNo("Delete [" + credentials[selected].appName + "] " + credentials[selected].email + "? (y/n): ",
                     confirmDelete)) {
        return;
    }
    if (!confirmDelete) {
        cout << "\nDeletion cancelled.\n";
        return;
    }

    const string deletedApp = credentials[selected].appName;
    credentials.erase(credentials.begin() + static_cast<long>(selected));
    if (rewritePasswordFile(currentUserEmail, credentials)) {
        cout << "\nDeleted [" << deletedApp << "] successfully.\n";
    } else {
        cout << "\nError: Could not update the password file.\n";
    }
}

void passwordManagerMenu(const string& currentUserEmail) {
    while (true) {
        cout << "\n" << border << endl;
        cout << "                               LOCAL PASSWORD MANAGER                            " << endl;
        cout << border << endl;
        cout << "[1] Add password" << endl;
        cout << "[2] Modify password" << endl;
        cout << "[3] Delete password" << endl;
        cout << "[4] Browse manager" << endl;
        cout << "[0] Logout" << endl;
        cout << "Choose an option: ";

        string choice;
        getline(cin, choice);
        if (choice == "1") {
            addPassword(currentUserEmail);
        } else if (choice == "2") {
            modifyPassword(currentUserEmail);
        } else if (choice == "3") {
            deletePassword(currentUserEmail);
        } else if (choice == "4") {
            browsePasswordManager(currentUserEmail);
        } else if (choice == "0") {
            return;
        } else {
            printInputError("Invalid menu choice. Please choose 1, 2, 3, 4, or 0.");
        }
    }
}

int main() {
    //GENERATED THIS IS FOR TESTING PURPOSES HUHU, D MUNA AKO GUMAWA NG MENU
    bool running = true;
    while (running) {
        vector<Credential> users = loadUsersFromTextDB(usersFileDB); // reloaded each round
        if (users.empty())
            return 1;

        string currentUser;
        while (!userLogin(users, currentUser)) {
            if (!cin)
                return 0;
        }

        passwordManagerMenu(currentUser);
        running = userLogout();
    }
    return 0;
}
