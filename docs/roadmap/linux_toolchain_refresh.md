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

The public embedding API already uses **frontend ABI 3** for module-aware
compilation, including the shipped embedding example. Its embedded base options
and shared result accessors retain their explicit v2 identities by design; they
are fields of the current ABI contract, not a pending migration to another ABI.

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

The first refreshed compiler build also exposed an unavailable `LexTests`
target: upstream merges ordinary unit suites while this project explicitly
selects the lexer suite. Its distinct target is restored. The build owner now
checks every requested group against Ninja's configured target inventory before
compilation; real Ninja and operational-build controls cover that admission.
Quick tooling and documentation use the same admitted uv/Python provisioning
route as development, avoiding repeated hosted Python toolcache download failures.

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

Development dependencies now have separate exact caches for the LLVM bootstrap,
Python interpreter/environment, Node/npm, CMake, ccache and signed Swift consumer,
plus pristine LLVM, libobjc2 and robin-map Git objects. Quick checks reuse Python
and package downloads. Keys follow dependency pins and installer recipes rather
than feature commits. Restored tool bytes are admitted before execution; source
objects retain exact tag/commit checks and full Git integrity checks. Cold misses
are published immediately after successful setup, even if later compiler tests
fail. Compiler build trees and product SDK archives are excluded. The manual
`Linux dependency cache qualification` workflow checks cold setup and can require
all tool and source caches on a second clean runner.
