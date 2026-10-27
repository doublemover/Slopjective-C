# Mutable lambdas with stable lifetime-sensitive captures

System 2.11.0 admits concrete synchronous mutable C++ lambdas when their captured
borrowed values, required owners and invoked callable bindings stay stable.
Ordinary scalar state, such as an invocation counter, may change. Read-only const
reference uses remain accepted. Merely spelling `mutable` no longer rejects an
otherwise qualified closure.

Cached CFG summaries record writes to captured storage, pointer increments,
address exposure and potentially writable reference arguments. The checker
combines those facts with actual captured loans and borrowed/consumed/invoked
requirements. A captured owner overwritten before every borrowing operation need
not stay unchanged when only its replacement supplies the required identity.
Retargeting a required binding or exposing its writable storage retains the
explicit lifetime-contract diagnostic.

ARC holding captures are dependencies of the other borrowed payloads they keep
alive. Clearing one after using a payload cannot leave an unchecked future
invocation. Independent immutable ARC captures still protect payloads after the
original outer binding ends. Captured reference owner bindings which mutate
remain unqualified, including bodies whose enclosing caller first borrows after
the closure is created. Specific persistent-store errors retain precedence.

Generic lambdas, coroutine/async lifetimes, arbitrary captured records and
consuming private owning-capture claims retain qualification boundaries. This
span does not implement general mutable capture-state replay, higher-order
invocation counts, indirect/global effects or full Strict System conformance.

Native witnesses cover C++20/23, ARC/MRC and PCH, payload/owner counters, scalar
reference counters, readonly and writable references, address exposure,
consumption/expiry, overwritten aliases and independent/retargeted ARC holders.
Frontend witnesses add the key positive/negative cases and source-free publication
of a concrete mutable borrowing closure. Broader frontend/package and SDK checks
remain batched after implementation; runtime and extension ABI layouts are unchanged.
