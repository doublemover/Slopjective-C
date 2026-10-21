# Native checked-error control flow

The next prerequisite for native `guard` and optional binding is a truthful
Clang control-flow graph for checked errors. Checked `throws` uses its own
transport channel: native C++/Objective-C exception handlers do not catch it,
and checked handlers do not catch native exceptions.

The implementation gives `throwError`, `tryValue`, `tryOptional`, `tryForce`
and `handle`/`catch` explicit graph behavior. Propagation cleans automatic
objects only through the destination handler boundary. Handler bodies use the
outer checked destination, so rethrows cannot catch themselves. A normal
`tryValue`/`tryForce` continuation remains distinct from exceptional exit so
`-Wreturn-type` still diagnoses missing ordinary returns.

A source `noreturn` promise excludes ordinary success. It does not exclude the
hidden ABI return carrying a checked error. Lowering removes LLVM `noreturn`
from the checked transport function and its calls; the graph still models the
source promise on the success edge. Local handlers and `tryOptional` may
therefore continue after a checked failure from such a function.

This milestone does not expose `guard` yet. The symbolic static analyzer keeps
its existing diagnostic until its checked-error path model is implemented;
constructing a CFG does not establish symbolic-analysis support. Match and
carrier-transfer CFG modeling remain separate obligations.

Validation is in progress. The isolated exit-proof prototype passes 34 native,
checked-error and mixed-exception cases. The admitted build adds maintained CFG
reachability/cleanup tests, return-warning regressions and optimized runtime
coverage for direct, indirect, member and virtual checked calls. The activated
match SDK remains identified by
[`linux_match_results_acceptance.json`](../support/linux_match_results_acceptance.json)
until a new SDK delivery receipt is recorded.
