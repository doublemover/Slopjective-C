# Expression lifetime pins and primitive ARC cleanup graphs

System source 2.30 connects `withLifetime` and `keepAlive` field/expression
snapshots to borrowed-owner proofs. Fields, concrete elements, tracked aliases
and NSObject-backed pointers can supply an independent pin. Existing loans stay
live while the pin owns the original value, then expire at pin end unless a
matching live source binding still owns that value. A pin created after manual
release diagnoses even in a body without a borrowed getter.

Each pin retains its exact single source object when known. CFG joins keep this
identity only when all paths agree; writes discard the unchanged-source proof.
Two nested snapshots taken from the same unchanged binding retain a separate
object-equality proof after the source is cleared. Ending the inner pin can hand
its loan to an active equivalent outer pin. Equal may-identity sets alone never
establish equality between unrelated bindings. Pin state and these definite
relations propagate into nested closure analyses.

The cleanup CFG now matches native source-release ordering for records and
arrays whose ARC destruction consists only of primitive object/weak/NSObject
operations. Structurally nonthrowing conditional initializers are admitted.
Throwing partial aggregate initialization, user destructor actions, resource
callbacks, Block copies and nontrivial capture destruction retain their distinct
checks. C++ base/user-destructor cases keep their native source action position.
Plain MRC stack Blocks with trivial capture copies, null/local slot loads and
assignments also use the ordinary EH graph without inventing a heap-copy effect.

The new regression runs eighteen C17/C++20/C++23 ARC/MRC source/PCH commands.
It covers field/element/alias pins, unchanged-source restoration, nested equal
claims, joined owner values, rebinding and bypass, weak/unsafe/NSObject fields,
released inputs, nested Block/C++ analyses and rejected throwing partial storage.
Source-free frontend replay publishes a pin-protected local record body.
Native explicit-capture controls count exact object destruction for field pins,
keepAlive and both choices of a nested two-pin source after clearing its owners.
All thirty-eight selected borrowed/lifetime/cleanup compiler regressions and
three affected native frontend groups pass. Four ARC C17 O0/C++23 O2 source/PCH
configurations pass fourteen compile/PCH/link/execution commands with ASan/UBSan
instrumentation. These are development checks, before matched SDK qualification.

Extension ABI 24, physical runtime ABI 4 and accepted SDK 2.62 remain unchanged.
General interprocedural/global effects, ambiguous owner correlations, managed
Block/throwing initialization, manual ownership counts and broader exceptional
ownership remain open. Matched SDK qualification follows consolidated development.
