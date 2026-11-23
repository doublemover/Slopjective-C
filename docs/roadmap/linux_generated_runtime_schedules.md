# Generated public-runtime histories

Issue [#8289](https://github.com/doublemover/Slopjective-C/issues/8289) has a
bounded manual-scheduler and gated serial-worker campaign. An independent Python
model owns logical task/frame/context IDs, caller references, cancellation,
waiter delivery, actor admission/destruction and group-result consumption.
The public C API defines the model's expected outcomes. Completion-order
selection is checked against the set of available results.

```sh
python3 -B -m scripts.objc3c_toolchain.runtime_schedules \
  --compiler-root /path/to/matched/compiler \
  --resource-root /path/to/pinned/llvm-23-resources \
  --output out/runtime-schedules
```

Eight fixed seeds select legal histories from a reviewed 64-seed grammar.
They cover all twenty-one modeled transition labels and thirty-four adjacent
pairs in that grammar. Histories have at most ninety-six operations, eight main
tasks, one typed next-result probe, eight actor contexts, one group, one actor
and one worker. Native commands have deadlines and worker gates use bounded
condition variables. The typed probe registers from an active resume, retains
its output slots through frame destruction, and checks that a mismatch leaves
the result available to a later consumer.

Both manual replay and one gated worker epoch run at client O0/O2 under separate
ASan/UBSan and TSan configurations. Canonical service profiles use O1 and matching
GNUstep dependencies, explicitly distinct from client optimization. The fixture
retains every task handle it accesses, keeps callback/frame/output storage alive,
and closes, drains and joins before final release. Atomics protect observed
counters. Every operation emits a ledger snapshot checked by the model. Actor
admission failure preserves caller ownership and hop-output facts. Task frames
must be destroyed once, registered waiters delivered once, actor jobs serialized
and group results consumed once. Manual runs repeat and require identical traces.

The campaign executes ninety-six generated histories across the backends and
instrumentation configurations. The runner shrinks a missing-destruction
history through valid operation-range deletions, preserves its failure class and
executes a fresh reduced replay.
Invalid producer removal, unsafe release, missing closure/quiescence and resource
bound violations are rejected before native execution. Reduction has explicit
trial/time limits and reports its actual minimality boundary. `--replay` uses the
requested validated history and keeps the negative controls on their fixed
witness history.

## Optional split-epoch exploration

Add `--explore` to the command above and select a fresh output directory for a
larger periodic or manual campaign. The release invocation retains its eight
fixed histories. Exploration considers seeds 64 through 127 and selects at
most twelve histories. `--explore-limit` accepts one through twelve for a smaller
run. Twelve histories allow at most 144 native executions across both
optimizations, both sanitizer configurations, and the three replay backends.
The exploration runs the generated fixture and sanitizer detector controls.
The fixed release invocation owns the maintained fixtures and semantic mutants.

Each history advances the first admitted task either before or after its group
registration. The second worker gate enters only after the first task's native
frame cleanup. Later tasks and actor jobs stay gated until the ordinary drain.
The model checks terminal observation and waiter delivery before that final
drain. Selection covers both gate placements, then prefers new transition
labels, adjacent pairs, and combinations of gate placement, group cancellation,
and actor cancellation. Ties use increasing seed order. The receipt records
selected and rejected seeds, rejection reasons, the native execution budget,
coverage gains over the fixed histories, and reviewed combinations left open by
the budget.

An exploration history records the gate operation in the replay protocol and
retains the history and protocol SHA-256 digests. Replay it with `--replay
out/runtime-schedules-explore/work/history-0.json` and a fresh output directory.
The version-3 history selects the split-epoch campaign and preserves the original
gate position. The version-2 release histories retain their original digests and
execution matrix.

The local x86-64 campaign selected seeds 64 and 65 and passed all 24 native
executions. It exercised both gate placements and added two transition labels
and six adjacent pairs relative to the fixed histories. The receipt identifies
the 62 budget exclusions and six gate/cancellation combinations left for a
larger pass. A fresh replay of seed 65 passed all twelve cases with matching
history, protocol, and observation digests.

## Receipts and maintained checks

Receipts bind model/generator versions, exact JSON histories and native input
protocols, compile/link/execution commands, source/header closures, tool/resource
and runtime identities, effective sanitizer controls, diagnostics, observations
and compressed product bytes. Receipts also record the Linux child-process peak
RSS before and after the campaign. A native watchdog failure retains the last
completed events and the pending generated operation. The product-evidence CLI
can restore and execute retained clients. The independent model checks their
observations.

The integrated owner runs six maintained public-contract fixtures in each of
the four sanitizer/client-optimization configurations. They cover concurrent
registration/cancellation/publication,
global-pool multi-actor exclusion, queued discard and named-binding retirement,
nested/last-owner abandoned awaits, nonconsuming typed next-result mismatch,
allocation/publication/copy failure injection, unforced worker stress and the
focused lifetime cases. The lifetime fixture covers direct and indirect worker
release, reentrant C++ result extraction, malformed callable frames, deep group
failure propagation and dependency wake failures at queue saturation and actor
closure. It bounds callback stack displacement to 32 KiB on the 512 KiB test
thread for both two-task and 4,096-task chains. The join fixture checks an
allocation-failed wake with a shared dependency and deep scope cleanup through
quiescence. Separate ARC and MRC scenarios verify deterministic actor generation
reuse, competing explicit and automatic first entries, and C++ result bridge
ownership. The twenty-four scenario executions have their own fixture inventory.

Five sanitizer-clean controls exercise missing frame destruction, duplicate
waiter delivery, premature quiescence, overlapping actor jobs and consuming a
type-mismatched next result. The typed-next control removes an actual group
result after the probe reports a mismatch. The semantic model detects its absence
in the following consumption sequence. The actor control uses a second live
callback while the first owns the logical actor. The owner requires each control's
expected failure class and retains a fresh replay of the reduced destruction
history.

The owner in published preview revision `8fe5c2fd89008d1c090a9355c11bdda01ceb9309`
passed on both release hosts. That owner ran ninety-six generated histories,
twelve maintained scenarios and five semantic controls against the twenty-label
grammar. The expanded fixture, typed operation and receipt checks described here
postdate those hosted runs. [Linux x86-64 job](https://github.com/doublemover/Slopjective-C/actions/runs/44865570421/job/136754940272) · [Linux AArch64 job](https://github.com/doublemover/Slopjective-C/actions/runs/44865570382/job/136756540976)

Local Linux x86-64 validation used the published SDK compiler, pinned LLVM
23.1.5 sanitizer resources and freshly instrumented runtime services and GNUstep.
The expanded owner completed ninety-six generated executions, twenty-four
maintained fixture executions, five controls and a fresh reduced replay. The four
public callable entry points also passed the installed source fixture under
ASan/UBSan at O0 and O2 in ARC and MRC. Hosted execution of the expanded matrix
remains pending.
