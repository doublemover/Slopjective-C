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
CMake and passed 1,554 tooling tests plus 33 subtests. All Node setup callers now
cache npm, and installs prefer cached package downloads.

The latest complete core run at `9c675d3ab` passes **all 50 frontend programs**,
**563 compiler regressions and 461 native unit cases**, including all 164 emitter,
17 import and 11 borrowed-pointer cases. Its upstream selection reaches 4,381
cases and exposes 451 failures: 439 missing test utilities, 11 stale expectations
and one Objective-C message crash during C++ global initialization. Exact-fork
utilities are now explicit prerequisites. Updated expectations retain semantic
checks for native printing, Block ODR identity and ARC array initialization.
Installed-consumer and sanitizer checks also pass before that upstream failure.
The null current-function context is handled explicitly and has eight explicit lookup/direct-send, ARC/MRC, C++20/23 regressions. All 450 utility/expectation failures pass focused local lit
replay; the formerly crashing upstream case passes its original FileCheck with
the corrected code generator. These reused-library diagnostics are not SDK
qualification. The complete canonical patch stack and prepared source seal pass.

The preceding sealed source passes **three frontend fuzz campaigns and six
controls**, plus **21 LLVM IR/debugger executions, 63 semantic-oracle cases and
seven controls**. First-party coverage exceeded its 300-second emitter deadline
after two cases. Coverage now runs the entire emitter catalogue through 32
bounded shards, each with a 900-second instrumented deadline. The producer and
terminal verifier require all shards, identical catalogues, disjoint complete
case execution and one executable identity from retained native JUnit. The
ordinary core still runs the complete unsplit suite. Coverage profiles now merge
by binary build identity and module signature across fork and exec, avoiding a
full shared-compiler profile copy for every process. Real native controls verify
all child and exec branches survive the merge under the existing storage bound.
All 32 native shard commands pass; the retained JUnit independently verifies
all 164 cases. CTest output retention is explicitly bounded so its default
1-KiB passed-test truncation cannot erase that proof. Forty-six focused
coverage/tool controls, 125 export/owner/revision controls and five real native
suite-collection controls pass. Independent native owners now continue after a
sibling failure; prerequisite failures stop dependent work. Sanitizer products consume the sealed
private SDK candidate; delivery waits until every required native owner passes.

The candidate now has sealed source identity
`llvmorg-23.1.5-392dfff97d29-5916ec08b8a2-0fcb354b`. The older full analysis
reached its six-hour limit after 206/235 variants: 169 passed, thirty timed out
and seven reported findings. Those seven units are repaired; focused local
Static Analyzer replay clears all seventeen reported locations. Six modern
C++ constant-expression/decomposition/matrix suites are now enrolled and pass
local replay. The full Clang-Tidy campaign remains a separate matching gate.
Complete analysis now uses four independent bounded partitions; its terminal
gate verifies identical inventories/inputs and complete disjoint execution.
The superseded single-job analysis was stopped after retaining its partial
proof. No checks or variants were removed.

The preceding hosted prerequisite caught a dispatch assumption in the new global
initializer regression: the driver and `clang -cc1` have different GNUstep
message defaults. The test now selects lookup and direct sends explicitly and
checks the actual call result and owned global store. All eight ARC/MRC,
C++20/23 configurations and four export/upstream-tool controls pass focused
native replay; the canonical source seal verifies.

The [preceding source campaign](https://github.com/doublemover/Slopjective-C/actions/runs/42528854066)
at `6d7fc2ade` passes all 50 frontend programs, ten runtime tests, 37 installed
commands, four sanitizer executions, eight Swift cases and 64 semantic workload/
import summaries. Upstream testing now passes 4,157 cases with 220 unsupported
and three expected failures; one remaining test needs the exact-fork `opt` tool.
Its other two failures identify a private-package consumer ordering cycle and
lit dropping the nested Python no-bytecode policy. All three are repaired at
`facffee19`: 105 focused controls, the original module/optimizer case and a real
nested-Python regression pass. Removing the environment repair makes that
regression fail as intended. The source seal verifies. The preceding coverage
and four analysis partitions continue to collect independent results alongside
the repaired complete qualification.

That preceding source also passes three frontend fuzz campaigns/six controls and
[LLVM/oracle assurance](https://github.com/doublemover/Slopjective-C/actions/runs/42528858909):
21 IR/debugger executions, 63 oracle cases and seven controls. Current repaired-source quick gates pass 1,600 tests and 33 subtests, with 112
explicit capability/platform skips.
The repaired [release/native and four-part analysis](https://github.com/doublemover/Slopjective-C/actions/runs/42537224777)
and [LLVM/oracle](https://github.com/doublemover/Slopjective-C/actions/runs/42536852949)
campaigns use producer `5916ec08b8a2 / 0fcb354b`. Release runs at `14a04b4ec`;
LLVM retains its `87040229e` checkpoint because its inputs are unchanged.
Exact-source native queues now let repaired work start alongside older coverage
without cancelling active builds; 86 workflow and 99 Python controls pass.
Fresh Debian/Ubuntu archive checks follow a passing complete release campaign.
**SDK 2.62.0 remains the last accepted archive; the replacement is not accepted
or activated.** The [refresh receipt](../support/linux_toolchain_refresh_progress.json)
records each checkpoint and its scope. New compiler patches continue to reuse
all eight dependency cache entries; dependency identity does not depend on
feature commits.

The current frontend-fuzz owner completed all three discovery campaigns and
the paired codec fault controls, then exposed a runner-only minimization error:
libFuzzer preserved the injected frontend cleanup fault, reduced it to four
bytes and exited zero after it could not shrink it further. Minimization now
admits that documented natural completion in addition to the exact injected
fault exit and bounded wall-clock stop. The diagnostic class, nonempty bounded
artifact, two exact replays and original A/B/A replay remain mandatory. A real
pinned-libFuzzer regression reproduces the natural exit, and 14 focused fuzz
controls plus 33 native sanitizer/product-evidence controls pass locally. A
matched hosted frontend-fuzz retry remains part of the consolidated release
campaign; no SDK or sanitizer acceptance is inferred from these focused checks.

The preceding hosted coverage reached six passing emitter shards, then its
seventh exceeded 900 seconds after 16 passing cases. The completed repair now
uses 32 shards from one bounded policy, with at most six of the 164 cases per
command. CMake registration, explicit native index/count arguments and terminal
proof share that count. All 32 native commands pass with the unchanged catalogue
hash; six invalid CLI controls and 136 focused checks pass. Current quick CI
passes 1,600 tests plus 33 subtests with 112 explicit capability/platform skips.
This native replay does not replace complete instrumented qualification. The
running `14a04b4ec` product and analysis campaign continues to collect independent
results before the final consolidated coverage retry. Its preceding analysis
partition 0 passes all 52 assigned fresh variants of the 235-variant inventory.
