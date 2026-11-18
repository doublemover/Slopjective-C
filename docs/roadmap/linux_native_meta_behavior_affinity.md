# Native behavior affinity and typed queries

Source 2.217 / System 2.143 extends locked property behaviors to the compiler's
native actor and executor contracts. The package's `executor` field accepts:

- `none`: no actor binding or required executor;
- `actor`: actor binding with its object-owned default executor;
- `main`, `global`, or `named:NAME`: an exact executor identity;
- `actor@main`, `actor@global`, or `actor@named:NAME`: both requirements.

Named identities preserve the compiler's `named:` discriminator. Names are
bounded printable literal identities, never generated source fragments. Unknown
contract forms and control characters are rejected. Package fingerprints and
locks bind the exact contract. Each getter and writable setter must match the
declared direct/dynamic dispatch and effective affinity. Implementation methods
are checked independently when present. Inherited class affinity and explicit
nonisolated properties use the same native queries as ordinary authored methods;
a package cannot erase or silently introduce a required hop.

Expanded methods retain ordinary native isolation checking and lowering.
Source-free imports contain expanded declarations and provenance; consumers do
not reload the package. Awaited calls retain actor/executor hop lowering, and
unawaited crossings remain rejected. This introduces no alternate scheduler,
async property syntax or new ownership/ABI representation.

The AST query model also preserves borrowed and resource facts carried by native
type sugar, typedef aliases and nested borrowed types. Previously those flags
only inspected declaration attributes, losing ordinary borrowed/resource typedef
contracts on properties and ivars. The collected/final model identity includes
the corrected flags.

The expanded compilation test passes six C17/C++20/C++23 × ARC/MRC modes, each
with six affinity configurations, source-free imports, mismatched-contract
controls, unawaited crossing rejection and native hop IR checks. Typed property
and ivar queries retain all four expected generated markers in each mode. The
existing native expansion execution witnesses, C++ marker identities,
CLI/embedding, dependency, lock and publication controls remain in the same test.
Package grammar checks pass. Matching SDK and full-profile acceptance remain
separate work; default portable derives and generated storage are not claimed.
