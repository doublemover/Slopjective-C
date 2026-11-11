# Synchronous generic calls and read-only mutable record captures

System source 2.31 admits immediate synchronous C++ reference-capturing lambda
literals with explicit/default primitive arguments and materialized generic
specializations. Argument evaluation must be free of side effects; reference,
pointer, record and borrowed formal arguments retain their contract boundary.
The existing bounded body proof checks every concrete specialization, permits
local algorithms and unrelated scalar updates, and keeps pointer/record returns,
async work and outside mutation guarded. Completed immediate calls retire their
future reference-capture hazard.

Mutable C++ lambdas may read borrowed records and independently owned ARC record
captures, including generic specializations and independent closure copies.
Cached CFG summaries distinguish read-only record access from field writes,
address exposure, opaque methods/operators and writes through pointer aliases.
Concrete opaque calls retire affected owner proofs, including aggregate roots
without scalar slots. Summary roots remain symbolic dependencies after such a
write; independent ARC ownership does not suppress the mutation contract check.
Retargeting a captured sensitive record still requires an explicit lifetime
contract.

The new native compiler regression contains twelve C++20/23 ARC/MRC source/PCH
commands. Focused checks pass with the existing immediate, mutable and owned-record
regressions. Source-free frontend replay publishes generic immediate and mutable
owned-record helpers. Runtime witnesses check repeated calls, independent copies,
primitive results and exact ARC destruction after the original owners are cleared.
Four instrumented source/PCH configurations pass fourteen commands. All thirty-five borrowed compiler regressions and twelve affected native frontend
groups pass.

Named/open/escaping reference callables, arbitrary argument effects, persistent
capture mutation, lazy/async invocation, global/interprocedural effects and
exceptional ownership remain separate obligations. Extension ABI 24, physical
runtime ABI 4 and accepted SDK 2.62 remain unchanged. These development results
precede the single matched SDK delivery.
