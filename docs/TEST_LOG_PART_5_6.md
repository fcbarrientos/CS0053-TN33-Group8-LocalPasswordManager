# Test Log - Parts 5 and 6

Test login used: `testing@gmail.com:MyPassword123`. All vault passwords used for testing were fictional.

| ID | Test action | Expected result | Actual result |
|---|---|---|---|
| T01 | Choose `[6]`, search `youtube` | Only matching accounts are displayed with passwords hidden | **Passed:** Only YouTube was displayed as `********` |
| T02 | After the YouTube search, enter entry number `1` | The selected password is revealed | **Passed:** YouTube's fictional password was displayed |
| T03 | Choose `[6]`, leave the search blank | All saved accounts are displayed with passwords hidden | **Passed:** Google Chrome and YouTube were listed with masked passwords |
| T04 | Choose `[5]`, select entry `2`, confirm `y` | The selected record is deleted from the vault | **Passed:** YouTube was deleted successfully |
| T05 | Browse after deletion | The deleted record is no longer listed | **Passed:** Only Google Chrome remained |
| T06 | Enter an invalid entry number such as `9` | No record is revealed or deleted | **Passed:** The program displayed an invalid-entry message |
| T07 | Enter `0` during Browse or Delete | The operation is cancelled | **Passed:** The program returned to the menu without changing the vault |

## Build verification

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp password-generator/passwordgen.cpp -o lpms.exe
```

Result: **Compiled successfully.** The existing warning from the teammate code about `\\Password` was not changed as part of Parts 5 and 6.
