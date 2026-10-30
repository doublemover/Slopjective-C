# Native Linux CI assurance

Issue [#8283](https://github.com/doublemover/Slopjective-C/issues/8283) separates
three results: tooling contract tests, source-based native execution, and shipped
SDK qualification. A green Python job cannot substitute for native execution.

Native workflow concurrency finishes the active cold build and queues the newest
pending revision. Implementation pushes do not repeatedly discard a compiler
build. Each result remains bound to its own exact head/base/tested SHA; a prior
revision's success cannot satisfy the latest revision's gate.

## Source gate

`Linux native assurance` runs for every pull request and push to `main`.
Classification happens inside the guaranteed workflow. The plan records the
PR head, base and tested merge commit; a push records its before/after commits.
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
  a shipped-archive install/relocation witness in the Linux job's distribution.
  Debian/Ubuntu release qualification remains its separate workflow.

`Linux native assurance gate` always evaluates explicit plan/native job outcomes.
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
Receipts report elapsed time, descendant peak RSS and available disk. Tooling CI
also exports actual selected/deselected pytest node IDs, collection failures,
per-phase outcomes, explicit skips and expected failures, plus JUnit and `-ra`.

The jobs use existing ephemeral Linux runners, read-only contents, SHA-pinned
actions and checkout without persisted credentials. PR source never executes
through `pull_request_target` or a privileged workflow-run consumer.

## Acceptance still required

Implementation and boundary controls are prepared. A live exact-revision native
CI run, cold/reuse/invalidation cost measurements and administrator confirmation
that `Linux native assurance gate` is required remain outstanding. The six-hour
native job timeout is a ceiling, not a measured timing claim. No repository
protection setting or billing/private-schedule variable was changed. Keep #8283
open until those acceptance conditions are established.
