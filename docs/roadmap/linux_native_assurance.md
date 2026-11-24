# Native Linux CI assurance

Issue [#8283](https://github.com/doublemover/Slopjective-C/issues/8283) separates
three results: tooling contract tests, source-based native execution, and shipped
SDK qualification. A green Python job cannot substitute for native execution.

Native workflow concurrency preserves active cold builds and isolates exact
source revisions. An older coverage owner cannot hold a repaired source behind
its branch-wide queue; duplicate requests for the same revision serialize.
Each result remains bound to its own exact head/base/tested SHA; a prior
revision's success cannot satisfy the latest revision's gate.

Run `42076026163` reached the hosted six-hour ceiling while compiling the
fuzz-instrumented frontend, after ordinary native/SDK checks and the sanitizer
foundation had completed. Extended manual and periodic execution partition the same
canonical selection into `core`, `frontend-fuzz` and `source-coverage` jobs. Core
retains package-before-sanitizer ordering and shares its ASan compiler build with
runtime schedules; each other job builds only its own compiler profile. Sibling
failures do not cancel the remaining jobs. The terminal gate requires every
selected shard, exact source/plan identity, disjoint complete suite coverage and
the original combined 256 MiB evidence budget. This changes execution layout,
without adding a cadence or accepting an earlier revision's products.

The core SDK is a sealed private candidate needed by the installed sanitizer
product checks. It stays before those consumers in the execution graph. A failed
earlier check blocks packaging and its dependent products while independent
runtime schedules continue. Package failure also blocks its consumers. The
workflow publishes an archive only after the complete native shard succeeds;
release installation additionally waits for matched implementation analysis.

## Fast PR feedback and optional native qualification

Routine pull requests run the existing tooling, documentation/capability and
workflow-policy checks. Tooling and documentation have 15- and 10-minute ceilings;
newer revisions cancel obsolete quick runs. These checks provide fast contract,
configuration and regression feedback. They do not qualify a rebuilt compiler,
sanitizer profile, fuzz campaign, source coverage or SDK archive.

`Native Linux PR checks` runs the required bounded source gate for the tested
merge. An admitted base SDK supplies the foundation for matching source edits.
Foundation changes select a current rebuild. `Extended Linux native assurance`
runs by manual dispatch or on its Sunday schedule. `Extended Linux implementation
analysis` runs by dispatch, on its later Sunday schedule, or from SDK release
qualification. The extended LLVM campaign remains manual.

Run a long campaign for the desired branch when needed:

```sh
gh workflow run native-assurance.yml --ref linux-native-guard-bindings
gh workflow run implementation-analysis.yml --ref linux-native-guard-bindings
gh workflow run llvm-assurance.yml --ref linux-native-guard-bindings
```

Classification stays inside the extended workflow. The plan records immutable
head/base/tested identities; dispatched and periodic runs select one exact SHA.
The executor and terminal verifier independently reconstruct that plan from the
checked-out commit and Git comparison. Tracked modifications, mutable references,
changed plans and stale receipts fail admission. Deleted paths participate in
selection, including moves from implementation into documentation.

Only README/changelog/license and ordinary Markdown under `docs/` can receive an
explicit prose-only classification. Machine-consumed contracts, specs, support
receipts and fixtures remain native inputs. Unknown or empty comparisons select
native execution. No downloaded build cache is trusted: the canonical foundation
owner verifies its source/configuration identity and performs incremental native
builds and tests. Foundation changes require the changed foundation itself.

The stable source surface uses existing owners:

- The foundation builder compiles and runs its manifest-selected ObjC3 lit suite
  and native unit binaries. Optional CI output requests real lit JSON and Google
  Test XML from those same commands.
- Native runtime CMake tests compile emitted Objective-C/Objective-C++ and execute
  against the pinned runtime. CTest discovery must match JUnit execution exactly.
  The runner checks that the inventory, log, and JUnit paths are fresh before
  writing discovery evidence.
- The frontend owner reconciles its generated manifest with the full CTest
  inventory. After linking each owned binary, it rediscovers the exact selected
  registrations for that binary and checks their commands before execution.
  Discovery records its command, bounded output, elapsed time, and outcome.
  The owner hashes the binary, reclaims successful test executables, and keeps
  the executable and diagnostics on discovery or test failure. Each selected
  registration needs passing JUnit evidence.
- Installed source witnesses build and run O0/O2 applications, PCH consumers,
  embedding and debugger examples outside the checkout. This is an installed
  development stage, not a newly qualified release archive.
- Compiler/runtime and language-fixture changes select the existing structured
  ASan/UBSan and TSan O0/O2 runner, including paired detector controls and canonical
  runtime object/archive verification. This does not instrument the host compiler.
- Packaging/installer changes additionally select the canonical SDK pipeline and
  its already qualified content-addressed archive installation. The producer and
  terminal verifier share one execution-record contract, including archive hash,
  installed prefix and successful consumer-command count. The gate does not need
  the SDK tree or another archive qualification. Fresh Debian/Ubuntu distribution
  qualification remains in the release workflow.

`Extended Linux native assurance gate` always evaluates explicit plan/native job outcomes.
Only an admitted prose-only plan permits a skipped native job. Missing artifacts,
failed/cancelled/skipped prerequisites, nonzero commands, incomplete suite or test
inventories, changed artifact bytes and wrong revisions fail the aggregate.
Native CTest skips are failures. Lit preserves expected-failure and unsupported
outcomes; unsupported cases need a capability reason, owner and review condition.
An unexpected pass or other lit failure cannot become successful evidence.

## Cadence and evidence

The Saturday adaptive evaluation keeps its existing monthly anchor, 14-day
activity budget and pause rules. Only an eligible evaluation invokes the reusable
closeout. An unchanged source with a recent attempt skips the native invocation.
The closeout requires the dispatcher commit and calls the canonical native
workflow on that same revision. Its terminal job requires both source admission
and successful native verification. Fresh distribution installs and release
analysis remain owned by the separate manual SDK release.

The previous standalone closeout failed on the default branch at `0266e510`.
The core shard tried to read an SDK manifest before its producer had created it.
The coverage shard's suite passed, then the old artifact budget rejected its
evidence. The terminal job correctly failed the overall attempt. [Run 44920211861](https://github.com/doublemover/Slopjective-C/actions/runs/44920211861) · [Core receipt](https://github.com/doublemover/Slopjective-C/actions/runs/44920211861/artifacts/14715307710) · [Coverage receipt](https://github.com/doublemover/Slopjective-C/actions/runs/44920211861/artifacts/14724009006)

The canonical Sunday native and analysis workflows passed on the same revision.
The native producer's upstream receipt records eight directories across AST,
Objective-C and Objective-C++ Sema/CodeGen, PCH, Modules and Driver. Lit recorded
4,273 passes, 106 unsupported tests and three expected failures. The adaptive
closeout now uses this same producer and aggregate, including the existing
sanitizer, runtime, coverage and front-end owners. [Native gate](https://github.com/doublemover/Slopjective-C/actions/runs/45028112852/job/137305319523) · [Upstream receipt](https://github.com/doublemover/Slopjective-C/actions/runs/45028112852/artifacts/14761961903) · [Analysis gate](https://github.com/doublemover/Slopjective-C/actions/runs/45038463224/job/137297246552)

Evidence names include tested revision, run ID and attempt. Required uploads fail
when absent and expire after seven days. Native receipts hash the complete durable
JSON/XML/log set, with a 256 MiB budget; generated SDKs, build trees and passing
executables are excluded. Failure receipts and compiler diagnostics are preserved. A separate startup
receipt retains clean-host tool provisioning logs before native execution begins;
the signature verifier uses its own pinned private libsodium dependency.
Before upload, the publisher stages precisely the durable verifier inventory into
a shard namespace. It retains each shared product blob once, avoiding directory
symlink expansion and collisions between root receipts when artifacts merge.
Instrumented foundation results and raw phase logs join operational measurements
as well as the ordinary foundation and installer receipts.
Receipts report elapsed time, descendant peak RSS and available disk. Tooling CI
also exports actual selected/deselected pytest node IDs, collection failures,
per-phase outcomes, explicit skips and expected failures, plus JUnit and `-ra`.

Sanitizer product stores use a campaign-relative namespace. Extraction under a
different checkout cannot invalidate a complete result merely because the
producer's absolute output path no longer exists. The shared store retains each
compressed replay payload once; owner receipts resolve it locally even when the
artifact omits their private directory symlinks. Native replay uses that same
resolver and checks complete blob metadata before reusing a verified record.
Regression controls perform real ZIP relocation, remove the producer tree and
private links, verify producer/gate inventory equality, and replay an actual
sanitized executable from the relocated shared store.

The jobs use existing ephemeral Linux runners, read-only contents, SHA-pinned
actions and checkout without persisted credentials. PR source never executes
through `pull_request_target` or a privileged workflow-run consumer.

Native job containers permit process layout controls through
`--security-opt seccomp=unconfined`. The first cold run reached and passed the
foundation, runtime, frontend and installed witnesses, then TSan's clean control
failed because Docker blocked its `personality(2)` ASLR adjustment. This is a
job-container setting; sanitizer options, findings and exit requirements remain
strict. A real syscall preflight rejects an incompatible container before the
compiler build. Default-policy failure, corrected-policy success and paired
clean/faulting TSan controls were exercised in local Debian 13 containers.
Workflow controls reject removal of the required setting or preflight.

Clean-host execution also exposed two assumptions hidden by a populated local
build tree. Development LLVM tools must be built after foundation admission and
before complete tool-bundle admission. Source-graph freshness probes must replay
the facade's transitive `__objc3_imports` virtual includes. A temporary Clang VFS
overlay supplies only the selected interface artifacts; external filenames keep
token and dependency hashes stable across probes. Real preprocessing controls
cover nested aliases and a changed shared header.

Failed SDK packaging now copies its active candidate and nested native-validation
JSON/XML/log evidence into the uploaded suite. Preprocessing logs survive a failed
temporary graph as well as compile logs. This preserves the actual failure rather
than only the outer packaging exception. The development graph reproducer can run
one case or `--matrix` against existing sealed development tools without producing
an SDK archive:

```sh
python3 -B -m scripts.objc3c_toolchain.development_graph \
  --output out/graph-reproduction --facade /path/to/matched/objc3c \
  --services /path/to/libobjc3_runtime_services.a \
  --objc-runtime /path/to/libobjc.so --matrix
```

## Acceptance still required

Record these results before closing #8283.

- Admit the hosted cold PR execution and its independent `native-pr` result with
  the exact base, head and tested merge SHA, the selected complete foundation
  and its phase ledger, and the passing runtime and frontend inventories. Retain
  elapsed time, peak child RSS, disk and artifact bytes. The already-running
  cold PR checks the workflow version at its own SHA. A later terminal-gate
  change requires its own hosted receipt. [PR 8331 cold run](https://github.com/doublemover/Slopjective-C/actions/runs/45289344500)
- Pair the cold observation with the admitted warm SDK route and a changed
  foundation input that forces an exact rebuild. Record the selected source and
  baseline SDK identities, phase costs and the observed invalidation reason.
- On the first due default-branch adaptive invocation after the repair, confirm
  the dispatcher's source-admission job, the canonical native terminal gate and
  the outer terminal step. Verify that all evidence belongs to the same SHA and
  includes the upstream selection. A cadence decision without eligible work has
  no native results to admit.
- Have a repository administrator verify the applicable check requirements for
  `native-pr`, `tooling`, `documentation` and `policy` using their exact names
  as reported by GitHub. The extended and release gates have separate admission
  contexts.

Native checked commands now use bounded subprocess sessions. Timeout and
interruption kill compiler descendants even when they start separate process
groups. Fresh logs and CTest XML are required before execution, so a previous
success cannot satisfy a current command. Real child-process controls defend
both cancellation paths. Semantic selection includes the canonical shared-pool
workloads and bounded source/PCH/interface/cache pilot described in the
[benchmark owner](linux_semantic_benchmarks.md); required import evidence is
revalidated from retained logs/build snapshots after relocation.

The existing source gate also retains [operational build/install costs](linux_semantic_build_costs.md).
Foundation and installer owners record actual phase timers and initial reuse
state during their required execution. The aggregate checks the compact artifact
and recomputes it from raw owner receipts/logs; timing remains advisory. This
adds no archive build, qualification, schedule or paid runner.
