# Third-party notices

This inventory applies to Bootgen source releases and the binary packages
created by the CMake packaging workflows.

| Component | Location | Notice supplied in package |
| --- | --- | --- |
| Bootgen source | Repository source files | `LICENSE` (Apache-2.0) |
| GNU Bison 2.7 parser skeleton | `bisonflex/` generated parser files | `LICENSE` (Bison special exception) |
| Flex 2.5.35 scanner | `bisonflex/` generated scanner files and `win_include/FlexLexer.h` | `LICENSE` (BSD-style notice) |
| Keccak 3.3 | `common/src/Keccak-compact.cpp` and architecture variants | `LICENSE` (CC0/public-domain dedication) |
| LMS/HSS implementation | `lms-hash-sigs/` | `LMS-HSS-LICENSE.txt` (Cisco BSD-3-Clause) |
| OpenSSL | Dynamically linked host dependency | Not bundled; obtain its notices from the installed OpenSSL distribution |

The root `LICENSE` retains the notices already distributed by upstream. The
LMS/HSS notice is installed separately because it is maintained in
`lms-hash-sigs/license.txt` in the source tree. Release maintainers must review
this inventory whenever adding, updating, bundling, or statically linking a
third-party component.
