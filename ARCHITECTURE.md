## LOGIN

### Dependencies:
* Database
* Account information
* Time package

### Rundown
1. The terminal prompts the user to login using their account credentials (email and password)
2. After entering, the program cross-references the entered credentials with the available databases
3. Two results for login attempts: 
    - If the user enters the correct credentials, they enter the main program
    - If the user enters the incorrect credentials, they are warned that they've used 1 of their 3 attempts to login, and further failed login attempts after the initial 3 is blocked for 5 minutes

## MAIN MENU

### Dependencies:
* Login and logout
* Database
* File I/0
* Password generator

### Rundown
1. A menu opens where the user can browse their stored account credentials
2. Storing an account credential populates the .txt file of the respective account
3. Logout button

### Additions
- If a user has no credentials stored yet, the program prompts that they haven't exactly done that yet and that would they like to add accounts and passwords
    - Users have an option to add sites and apps they're using their emails for and their corresponding passwords
- Generate password option when adding a credential
- For already-stored credentials, even when viewing them, the password is hidden by default
    - The user has an option to unhide it

## LOGOUT

### Dependencies:
* The whole of login

### Rundown
1. Terminating the program or choosing the logout option logs the user back to the login screen