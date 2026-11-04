# Generated public-runtime histories

Issue [#8289](https://github.com/doublemover/Slopjective-C/issues/8289) now has a
bounded manual-scheduler and gated serial-worker pilot. An independent Python
model owns logical task/frame/context IDs, caller references, cancellation,
waiter delivery, actor admission/destruction and group-result consumption.
It does not inspect production runtime structs or compute expected counters
from native observations. Completion-order selection is checked against an
allowed unconsumed set rather than imposing an unspecified global order.

```sh
python3 -B -m scripts.objc3c_toolchain.runtime_schedules \
  --compiler-root /path/to/matched/compiler \
  --resource-root /path/to/pinned/llvm-23-resources \
  --output out/runtime-schedules
```

Eight fixed seeds select legal histories from a reviewed 64-seed grammar.
They cover all twenty modeled transition labels and thirty-three adjacent
pairs in that grammar. These are explicit model denominators, not source
coverage or every state of the public runtime. Histories have at most ninety-six
operations, eight tasks, eight actor contexts, one group, one actor and one
worker. Each native command has a deadline; worker gates use bounded condition
variables. There are no sleep-based schedule choices.

Both manual replay and one gated worker epoch run at client O0/O2 under separate
ASan/UBSan and TSan configurations. Canonical service profiles use O1 and matching
GNUstep dependencies, explicitly distinct from client optimization. The fixture
retains every task handle it accesses, keeps callback/frame/output storage alive,
and closes, drains and joins before final release. Atomics protect observed
counters. Every operation emits a ledger snapshot checked by the model. Actor
admission failure preserves caller ownership and hop-output facts. Task frames
must be destroyed once, registered waiters delivered once, actor jobs serialized
and group results consumed once. Manual runs repeat and require identical traces.

The initial native checkpoint passes ninety-six executions. Two test-owned
wrong-code controls omit logical frame destruction or duplicate waiter delivery;
both exit cleanly under the sanitizer and fail the semantic oracle. The expanded
runner also shrinks a missing-destruction history through valid operation-range
deletions, preserves its failure class and executes a fresh reduced replay.
Invalid producer removal, unsafe release, missing closure/quiescence and resource
bound violations are rejected before native execution. Reduction has explicit
trial/time limits and reports its actual minimality boundary.

Receipts bind model/generator versions, exact JSON histories and native input
protocols, compile/link/execution commands, source/header closures, tool/resource
and runtime identities, effective sanitizer controls, diagnostics, observations
and compressed product bytes. The existing product-evidence CLI can restore and
execute retained clients; the independent model remains the semantic oracle.

This pilot deliberately closes one epoch before quiescent observation. Concurrent
registration/cancellation/publication, global-pool multi-actor exclusion,
nested/abandoned awaits, typed next-result mismatch, named-binding retirement,
failure injection and unforced worker stress still need modeled families and
controlled schedules. CI budget/cadence integration and additional semantic
mutants remain open. No exhaustive scheduler, memory-model, compiler async,
Swift concurrency or public actor-language claim follows. #8289 remains open.
