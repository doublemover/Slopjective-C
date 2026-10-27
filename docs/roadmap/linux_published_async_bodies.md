# Published C++ async bodies

The concurrency 2.34.0 source milestone extends concrete C++ inline definitions
to awaited isolation entries, native-frame awaits and ordinary async task calls
under required `objc3.cxx-async-definitions.v1` semantics. Removing or adding the
capability without matching declaration identities is rejected. Canonical source retains
`__objc3_await` and the original callable expression. Private isolation thunks,
awaiter operations, promise calls and coroutine allocation/cleanup scaffolding
are execution details; they do not become public source dependencies or stable
body identities. Written operands and resolved callable references still affect
the contract and must satisfy export, SPI and dependency policy.

Native function crossings now retain both the written call and executable frame
call in a `PseudoObjectExpr`, matching message crossings. Printing reconstructs
the written invocation. The public AST helpers distinguish source projection
from logical effect checking, which still sees the actual frame requirement.
Native AST import preserves await operations and coroutine body state, including
parameter moves and implicit return/suspend flags. Required callable contracts
are installed before recursive body and redeclaration imports.

Inline ordinary C++ task bodies require the exact SDK task header. The frontend
embeds that header alongside the C runtime declaration contract and includes it
in its content identity. Both producer and consumer reparses select the complete
C++ bridge, reuse compiler resource headers and preserve aligned coroutine
allocation. Declaration-only task signatures retain their existing bounds.

The maintained source-free package fixture publishes native-frame and C++ task
wrappers around an executor entry. Its consumer starts both on a distinct
executor and checks the destination, completion and returned values after
provider source removal. The emitter fixture independently checks canonical
round trips, changed-operand fingerprints and rejection of a private target.

Focused qualification passes six compiler regressions, 120 native AST tests,
four interface suites and 14 tooling tests. Source-free packages pass eight
C++20/23 × ARC/MRC × O0/O2 configurations in each source-directed, eager and
ASan/UBSan mode (24 configurations / 96 commands). The isolation source/PCH
runtime matrix passes all 48 configurations / 360 commands in each normal and
ASan/UBSan mode. The [focused receipt](../support/linux_published_async_bodies_progress.json)
binds the compiler, source, public header and reports.

SDK 2.33 remains the accepted installed archive; this milestone does not imply a
new SDK delivery or full profile conformance. Dependent/generic bodies, public
closure types and unmodeled runtime/custom awaiter operands remain separate work.
The [2.35.0 source span](linux_task_frame_interop.md) adds nonisolated frame
adapters in ordinary task bodies and native-frame bodies awaiting C++ tasks.
