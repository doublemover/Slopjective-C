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
source promise on the success edge of qualified calls. Nullable Objective-C
receivers and virtual methods with potentially returning overrides retain a
normal continuation. Local handlers and `tryOptional` may
therefore continue after a checked failure from such a function.

This milestone does not expose `guard` yet. The symbolic static analyzer keeps
its existing diagnostic until its checked-error path model is implemented;
constructing a CFG does not establish symbolic-analysis support. Match and
carrier-transfer CFG modeling remain separate obligations.

The [development acceptance receipt](../support/linux_checked_error_control_flow_acceptance.json)
records 229 passing Clang regressions, 60 AST contract tests, 24 CFG reachability
cases, two scope-cleanup tests and 32 focused tooling tests. The experimental
guard exit checker passes 34 cases against the admitted libraries. Runtime
validation passes 108 commands, eight ordinary/PCH executions and 64 expected
traps across C++20/23, ARC/MRC and O0/O2, covering direct, indirect, member and
virtual checked calls. Disposable runtime binaries are reclaimed. The activated
match SDK remains identified by
[`linux_match_results_acceptance.json`](../support/linux_match_results_acceptance.json)
until a new SDK delivery receipt is recorded.
