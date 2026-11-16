#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include <chrono>

#include "password-generator/passwordgen.h"

using namespace std;
using Clock = chrono::steady_clock;

const string border = "==================================================================";
const string usersFileDB = "users.txt"; //I will only add my fake credentials here sa file, add yours na lang guys
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

bool userLogout() {
    cout << "\n";    
    cout << "                     You have been logged out.                    " << endl;
    cout << border << endl;

    string choice;
    while (true) {
        cout << "Do you want to log in again? (y/n): "; 
        if (!getline(cin, choice)) {
            return false;
        } else {
            if (choice == "y" || choice == "Y") {
                return true; 
            }
            else if (choice == "n" || choice == "N") {
                cout << "Thank you for using LPMS! Bye :)" << endl; //LMPS muna hahaha abbreviation 
                return false; 
            }
            else {
                cout << "Invalid input. Please enter 'y' or 'n'." << endl; 
            }
        }
    }
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
    string choice;

    cout << "\n" << border << "\n";
    cout << "                   [3] Add Password\n";
    cout << border << "\n";

    cout << "Enter App / Website Name: ";
    getline(cin, cred.appName);

    cout << "Link oyur logged in email (" << currentUserEmail << ") to this app? (y/n): ";
    getline(cin, choice);
    if (choice == "y" || choice == "Y") {
        cred.email = currentUserEmail;
    } else {
        cout << "Enter custom email / username for " << cred.appName << ": ";
        getline(cin, cred.email);
    }

    cout << "Do you want to generate a password? (y/n): ";
    getline(cin, choice);

    if (choice == "y" || choice == "Y") {
        passwordGenerator generator;
        passwordOptions options;

        cout << "Enter desired length (default 12, min 8, max 25): ";
        string lenStr;
        getline(cin, lenStr);
        if (!lenStr.empty()) {
            int len = stoi(lenStr);
            options.length = (len >= 8 && len <= 25) ? len : 12;
        } else {
            options.length = 12;
        }

        cred.password = generator.generate(options);
        cout << "Generated Password: " << cred.password << "\n";
    } else {
        cout << "Enter Password: ";
        getline(cin, cred.password);
    }

    saveCredentialToFile(currentUserEmail, cred);
    cout << "\Password for [" << cred.appName << "] successfully saved!\n";
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

        running = userLogout();
    }
    return 0;
}