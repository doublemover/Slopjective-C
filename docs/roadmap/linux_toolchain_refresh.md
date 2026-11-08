# Current Linux toolchain and SDK refresh

The last accepted archive is **SDK 2.62.0**, built with LLVM 23.1.3 and extension
ABI 21. Later source milestones were developed and checked without producing a
replacement accepted archive. The [historical acceptance receipt](../support/linux_source_batch_2_62_acceptance.json)
records that delivery; its version has not silently advanced.

The current candidate is **SDK 2.153.0**, with System 2.79 and keypath 2.12,
rebased onto stable **LLVM 23.1.5**. The complete canonical patch stack applies
and its prepared source seal verifies. Extension ABI **24** is the newest
implemented revision. Services ABI **4**, async frame ABI **1**, keypath metadata
ABI **2** and interface schema **2.2** retain their current layouts. These are
independent version numbers; no additional ABI revision is invented for this
release refresh. All provider objects, PCH, interfaces and packages must be
regenerated against the new source identity.

Development now uses Node **26.13.0 LTS**, npm **12.2.2**, CMake **4.4.5**,
uv **0.13.7**, pip **26.3**, pytest **9.2.0** and independently signed Swift
**6.4.2**. Python 3.15.0, Ninja 1.13.2, ccache 4.14.2, jsonschema 4.26.0,
libobjc2 2.3 and robin-map 1.4.1 remain current. Native GNUstep ABI 2.2 is the
compiler/runtime contract of libobjc2 2.3; the two versions serve different roles.
The active language-tool locks and CI action pins have been refreshed together.

Native assurance now passes the same periodic source revision through planning,
execution, evidence publication and verification. The prior dispatch rejected
its plan before compiling because execution omitted that mode. Implementation
analysis now preserves both periodic mode and a caller's explicit source SHA.
Executable workflow controls defend all phases under PR, manual, scheduled and
workflow-call configurations.

The refreshed clean-host run exposed transient upstream GitHub release download
failures. The installer can now retrieve the identical immutable asset through
GitHub's official release API, retaining the pinned checksum and ccache publisher
signature. Regression controls reject transfer failures and corrupted downloads.
The prior LLVM campaign also exposed an undefined libclang visitor for dependent
key-path types. The visitor now traverses both root and value type locations;
an indexing regression covers read, write and nested Optional component types.

The hosted release workflow reuses the extended native owner
for the current compiler, GNU runtime, frontend, installed consumers and one
sealed archive, then waits for matched
implementation analysis before fresh Debian 13 and Ubuntu 26.04 installation,
relocation, embedding, debugger and ordinary-user qualification. Extended native,
sanitizer, fuzz, coverage and LLVM/debugger owners provide their separate current
checks. A candidate is not accepted or activated until those required results
pass. The [refresh receipt](../support/linux_toolchain_refresh_progress.json)
records actual status; full language/profile conformance and remaining feature
implementation are separate roadmap obligations.
