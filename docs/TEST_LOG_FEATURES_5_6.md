# Test log: delete and browse features

Test account used: testing@gmail.com / MyPassword123. The account and vault files are fictional and are not committed.

| ID | Test input/action | Expected result | Actual result |
|---|---|---|---|
| T01 | Log in with the fictional account, choose `6`, leave search blank | Manager list shows all saved entries and masks each password | **Passed:** Saved accounts were listed with `********`; no password was shown in the list |
| T02 | From Browse, search for `google`, then choose entry `1` | Only matching app/website records are listed, and only the selected password is revealed | **Passed:** Google Chrome matched regardless of capitalization and its password was shown only after selection |
| T03 | From Delete, choose entry `2`, confirm `y`, then browse again | The selected GitHub record is removed while the other record remains | **Passed:** GitHub was deleted and the next Browse showed only Google Chrome |
| T04 | At Browse, enter `9` when only one entry exists | Invalid selection is rejected without deleting or revealing a record | **Passed:** The program displayed `Invalid entry number. Returning to the menu.` |
| T05 | At Delete, enter `0` | Delete operation is cancelled and the vault remains unchanged | **Passed:** The delete function returned to the menu without changing the vault |
| T06 | At Delete, enter `x` | Non-numeric input is rejected without an exception or infinite loop | **Passed:** The program displayed `Invalid entry number. Nothing was deleted.` |
| T07 | Browse an empty vault | A helpful empty-state message is shown | **Passed:** The program displayed `No saved passwords found.` |
| T08 | From Browse, search for an app/email that is not saved | No unrelated records are shown | **Passed:** The program displayed `No matching accounts found.` |

## Compile verification

Command:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp password-generator/passwordgen.cpp -o lpms
```

Result: **Passed.** The project compiled and the interactive tests above completed normally.

## Tool availability note

`g++` was available for compilation. Formatter, static-analysis, and debugger evidence should be captured by the group using the tools installed in the group's VS Code environment; this log does not claim those tools were run here.
