# CS0053-TN33-Group8-LocalPasswordManager

## Purpose

LPMS is a console-based local password manager written in C++. It authenticates a user from a local fictional account file and stores that user's saved website credentials in a local vault.

## Features implemented in this branch

- Login with three-attempt lockout handling.
- Browse saved accounts with passwords hidden by default.
- Search saved accounts by app/website name or email.
- Select one saved account to view its password.
- Delete a selected saved account after confirmation.
- Save and reload vault records from a local text file.

## Requirements

- VS Code with the C/C++ extension
- A C++17-compatible compiler such as `g++`

## Compile and run

From the project root:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp password-generator/passwordgen.cpp -o lpms
./lpms
```

The local fictional login database must exist at `test_acc/users.txt` using this format:

```text
fictional.manager@gmail.com:FakePass!2026
```

The account and vault `.txt` files are intentionally ignored by Git. Do not add real credentials to the project.

## Menu options in this branch

`3` adds a password, `5` deletes a password, `6` browses the manager, and `7` logs out. Browse displays only the app/site and email first; the password is shown only after the user chooses an entry.

## Git workflow evidence

Work for this contribution is kept on a feature branch, with separate commits for the build fix, feature implementation, and documentation/testing. Push the branch and open a pull request for a teammate to review before merging into `main`.
