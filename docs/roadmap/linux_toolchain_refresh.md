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
all tool and source caches on a second clean runner. Before the new workflow
reaches the default branch, dispatch `llvm-assurance.yml` with
`dependency_cache=any`, then `dependency_cache=warm`; these modes run dependency
checks without starting another compiler build. Both runs pass: the second
restored all eight tool/source cache entries, admitted all six tool prefixes and
reused all three pristine source repositories. Its complete job took 101 seconds
versus 179 seconds cold. Quick CI independently restored Python, packages and
CMake and passed 1,553 tooling tests plus 33 subtests. All Node setup callers now
cache npm, and installs prefer cached package downloads.

The preceding complete core run attempted all 50 frontend programs: 47 passed
and three exposed remaining failures. Its compiler foundation passed 563 compiler
regressions and 461 native unit cases. CTest now records the actual executable
owner for every profile, builds shared programs once, runs every variant and
collects independent assertion-based failures before rejecting the suite.

Local repairs now pass all **164 emitter, 17 import and 11 borrowed-pointer
cases**, including six existing emitter cases that were never enrolled. Interface
identities preserve exact unnamed template arguments without physical header paths,
avoid factory-result recursion and correctly resolve re-exported dependency owners.
Provider-owned extern specializations retain their complete storage contract.
Ordinary/native coroutine Blocks use their own callable, promise and written-body
context. Native printing retains consumed parameter attributes and omits synthetic
scope wrappers, preserving stable repeated replay. The controls retain capability
removal rejection, private-dependency rejection and changed-body sensitivity.
Fifty focused tooling controls pass; quick CI at the preceding pushed checkpoint
passes 1,553 tests and 33 subtests, with 107 explicit capability/platform skips.

These local diagnostics reuse a verified, failure-only CI compiler foundation and
a separately compiled canonical printer repair; they are not SDK acceptance.
One-day compiler inputs can be split into four bounded, checksummed transport parts
without executing another compiler build. The hosted transport and complete local
reassembly pass; intermediate copies are reclaimed after verification.

The preceding compiler identity passes three frontend fuzz campaigns and six
controls, plus all 21 LLVM IR/debugger executions, 63 generated semantic-oracle
cases and seven controls. Coverage reached the repaired key-path fixture mapping;
the older implementation-analysis job remains active. A complete current-source
release/native, analysis, LLVM/oracle and fresh-host archive pass is still required
before accepting or activating the candidate. Release scheduling uses the exact
source revision so an older analysis job does not hold a repaired release behind
its completed native build. Compiler diagnostic selection cannot satisfy the full
aggregate or publish an SDK. The [refresh receipt](../support/linux_toolchain_refresh_progress.json)
records each checkpoint and its scope.
