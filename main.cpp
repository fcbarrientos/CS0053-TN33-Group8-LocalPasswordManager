#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include <chrono>

using namespace std;
using Clock = chrono::steady_clock;

const string border = "==================================================================";
const string usersFileDB = "users.txt"; //I will only add my fake credentials here sa file, add yours na lang guys
const int maxAttempts = 3;
const chrono::seconds lockoutDuration(15);

struct user {
    string email;
    string password;
}; //naka-structure para nasa iisang unit lang tong dalawa since codependent sila sa isa't isa

vector<user> loadUsersFromTextDB(const string& filename) { //I used vector instead of an array kasi suggested 
    vector<user> users;
    ifstream file(filename);
    user u;
    while (getline(file, u.email, ':') && getline(file, u.password)) {
        users.push_back(u);
    }
    return users;
}

bool credentialsMatch(const string& email, const string& password, const vector<user>& users) {
    for (const user& u : users) {
        if (u.email == email && u.password == password) {
            return true;
        }
    }
    return false;
}

bool userLogin(const vector<user>& users, string& emailLoggedIn) {
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

int main() {
    vector<user> users = loadUsersFromTextDB(usersFileDB);
    //siguro maglagay ng checkpoint if empty yung DB

    string currentUser;
    while (!userLogin(users, currentUser)) {
        if (!cin) return 0; 
    }
    
    cout << "You are logged in as " << currentUser << endl;
    return 0;
}