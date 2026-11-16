# AI assistance log: features 5 and 6

This contribution records AI assistance transparently. The student remains responsible for understanding, running, and explaining the code.

| ID | Prompt/request | Response summary | Decision | Verification |
|---|---|---|---|---|
| AI-01 | Review the starter project and identify what is needed for the assigned browse and delete features. | Identified the missing manager menu, vault parser, vault rewrite operation, masked browse output, and input validation. | Modified and accepted | Implemented the functions, compiled the project, and ran interactive browse/delete tests. |
| AI-02 | Investigate the compiler error in the password generator. | Explained that the header declared an `auto` member while the source defined an unrelated function with an invalid return declaration. | Accepted | Changed the declaration/definition to a matching `string passwordGenerator::makePool` member and confirmed compilation. |
| AI-03 | Suggest a way to reveal passwords while browsing. | A direct reveal after listing could expose every password. | Rejected/modified | Modified the behavior so Browse masks all passwords first and reveals only the entry number explicitly selected by the user. Tested with two fictional records. |
| AI-04 | Suggest a more practical Browse design for many saved accounts. | Add a case-insensitive search by app/website name or email before displaying entries. | Accepted | Added filtering, tested a matching search, tested a no-match search, and confirmed passwords remain hidden in the results list. |
