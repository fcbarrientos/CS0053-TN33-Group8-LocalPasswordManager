# Bug log: features 5 and 6

## BUG-001 - Password generator prevented compilation

- **Observed behavior:** The baseline project failed to compile. The compiler reported that `passwordGenerator::makePool` was used before its `auto` return type could be deduced and that the definition had two data types.
- **Root cause:** The header declared `constexpr auto makePool(...)` as a class member, while the source defined a separate `string auto makePool(...)` function outside the class.
- **Correction:** Declared `string makePool(...)` as a private member and defined it as `string passwordGenerator::makePool(...)`.
- **Retest:** The project compiled successfully with `g++ -std=c++17 -Wall -Wextra -pedantic`.

## BUG-002 - Vault records could not be browsed or deleted

- **Observed behavior:** The original program saved records but immediately logged out after login. It had no menu and no loader for the `[app] email | password` vault format.
- **Root cause:** The starter code did not provide a manager menu or parse saved vault records into `Credential` objects.
- **Correction:** Added `loadVault`, `saveVault`, `managerMenu`, `browseManager`, and `deletePassword`.
- **Retest:** Browse listed saved records with masked passwords; selecting an entry displayed its password; deletion rewrote the vault without the selected record.

## BUG-003 - Invalid generated-password length could terminate the program

- **Observed behavior:** Calling `stoi` directly on non-numeric input could throw an exception and terminate the program.
- **Root cause:** The input conversion had no exception handling.
- **Correction:** Wrapped conversion in `try/catch` and used the default length of 12 for invalid or out-of-range input.
- **Retest:** Invalid length input produced a warning and continued to the add-password flow.

## BUG-004 - Login lockout repeatedly printed without waiting

- **Observed behavior:** After three failed logins, the loop immediately called the login function again and printed the lockout message repeatedly.
- **Root cause:** The lockout branch returned immediately while the outer login loop continued without waiting.
- **Correction:** The lockout branch now waits until the cooldown expires before allowing the next login prompt.
- **Retest:** Three failed attempts produced one cooldown wait, then the next login prompt appeared after the cooldown.
