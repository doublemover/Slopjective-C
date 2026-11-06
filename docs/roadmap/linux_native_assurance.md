# Native Linux CI assurance

Issue [#8283](https://github.com/doublemover/Slopjective-C/issues/8283) separates
three results: tooling contract tests, source-based native execution, and shipped
SDK qualification. A green Python job cannot substitute for native execution.

Native workflow concurrency finishes the active cold build and queues the newest
pending revision. Implementation pushes do not repeatedly discard a compiler
build. Each result remains bound to its own exact head/base/tested SHA; a prior
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

## Fast PR feedback and optional native qualification

Routine pull requests run the existing tooling, documentation/capability and
workflow-policy checks. Tooling and documentation have 15- and 10-minute ceilings;
newer revisions cancel obsolete quick runs. These checks provide fast contract,
configuration and regression feedback. They do not qualify a rebuilt compiler,
sanitizer profile, fuzz campaign, source coverage or SDK archive.

`Extended Linux native assurance` is explicitly dispatched or runs on its existing
weekly schedule. `Extended Linux implementation analysis` is dispatched, weekly,
or called by SDK release qualification. The extended LLVM IR/debugger campaign
is manual. Full native results remain required inside their own aggregate and
release owners, while ordinary PR feedback does not wait for cold LLVM builds.

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
- The frontend owner reconciles its generated manifest with CTest, builds and
  executes each registration serially, hashes the binary, and reclaims successful
  test executables. Every selected registration also needs passing JUnit evidence.
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

The repaired `adaptive-closeout.yml` is reusable-only. It consumes the dispatcher's
immutable source SHA and runs the same native surface, actual runtime sanitizers,
and upstream AST, Objective-C Sema/CodeGen, PCH, Modules and Driver regressions.
Whole `check-clang`/`check-llvm` promotion awaits measured cost. This repair adds no
cron, runner class, cadence opportunity or private-schedule activation. Existing
monthly/14-day activity rules, pause and failed-attempt accounting remain intact.
Policy tests parse the actual workflow files and reject missing/renamed workloads,
conditional execution, mutable source forwarding and missing terminal verification.

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

Implementation and boundary controls are prepared. A live exact-revision native
CI run, cold/reuse/invalidation cost measurements and repository check-policy review remain outstanding. Required PR checks should
refer to automatic quick checks; optional extended checks must not be required
for every PR. Repository protection settings were not changed by this workflow
cutover. Each shard's
six-hour timeout is a ceiling, not a measured timing claim. The split publisher,
selection and aggregate pass 116 focused assurance/cost tests and 63 workflow
policy controls; hosted completion of the new layout is still required. No repository
protection setting or billing/private-schedule variable was changed. Keep #8283
open until those acceptance conditions are established.


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
