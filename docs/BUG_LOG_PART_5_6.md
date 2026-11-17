# Bug Log - Parts 5 and 6

## BUG-001: Password generator blocked compilation

- **Observed behavior:** The project failed to compile because `makePool` was declared as a member function in the header but defined as a free function in the source file.
- **Correction:** Added `passwordGenerator::` to the existing definition so the declaration and definition refer to the same member function.
- **Scope:** Minimal build correction only; the generator logic was not redesigned.
- **Retest:** The project compiled with `g++ -std=c++17 -Wall -Wextra -pedantic`. The existing `\\Password` output warning remains unchanged from the teammate code.

## BUG-002: Saved records were not available to Browse or Delete

- **Observed behavior:** The existing program saved records but had no function to load the `[app] email | password` records back into `Credential` objects.
- **Correction:** Added a loader for the existing file format, a masked display, Browse/Search, and Delete functions.
- **Retest:** Browse listed records without passwords, and Delete rewrote the file after confirmation.

## BUG-003: Invalid Browse/Delete entry numbers

- **Observed behavior:** Users could enter a number that did not correspond to a displayed record.
- **Correction:** Added numeric parsing and range validation. Invalid values return to the menu without revealing or deleting a record.
- **Retest:** Invalid and out-of-range inputs displayed an error message and left the file unchanged.
