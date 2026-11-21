# Native availability expression packages

Source 2.194 / System 2.120 imports and canonically prints native availability
guards in C17, C++20/C++23 and Objective-C interfaces. Both the builtin and
Objective-C spellings produce a target-owned expression with its exact selected
version, type and source range. Imported statements retain those facts after the
provider AST is destroyed.

Canonical interfaces bind their target profile. On Linux, clauses for unrelated
platforms normalize to `__builtin_available(*)`, retaining the always-available
wildcard behavior without a runtime version-check dependency. When a selected
platform version exists and the printer has its AST context, it prints that
platform/version and the wildcard. Both source spellings normalize to the C
builtin grammar. A context-free diagnostic with a selected version retains its
diagnostic placeholder; Linux canonical publication does not use that path.

`objc3.native-object-expressions.v2` replaces v1 without a compatibility alias.
The updated emitter, declaration identity, contract and production codec require
the current reader promise for availability and the preceding property,
subscript, reflection, literal and writeback forms. Availability identity retains
the exact selected version or the all-target-versions policy. Unpublished
implementation bodies do not create an expression transport promise. The fuzzer
dictionary and source-coverage inventory use the new capability and witness.

The native witness covers nine optimized C17/C++20/C++23 × pure/MRC/ARC
configurations with the provider removed before two independent consumers.
Objective-C configurations additionally use source-directed imports. It checks
exact source/fingerprint replay, native branch effects, templates and default
arguments, old/missing reader admission and a validly rehashed changed branch
result. AST controls cover all four source languages on Linux and a selected
version target, including canonical reparse and provider AST destruction. The
selected-version controls establish AST/compiler transport, not native macOS
qualification.

Standard `move`, `forward`, `forward_like` and `as_const` reference views also
preserve the surrounding read/write/escape role and exact local reference cell.
They no longer make an ordinary integer snapshot appear to escape through an
opaque helper call. Single-argument free functions in the standard namespace are
recognized; methods and multi-argument algorithms retain ordinary call analysis.
Known managed initializers/writes still contribute provenance through these
views. The strict native descriptor witness additionally executes ordinary move
and reference-view transfers, and rejects moved or forwarded unadmitted resource
copies. Imported `<utility>` bindings survive provider removal and exact replay.

All 31 native configurations pass, including nine availability, six preceding
object-expression, four strict descriptor/reference-view, four inferred group,
four MRC record payload and four standard owned-payload configurations. All 108
AST controls, all 17 import groups, 18 related publication groups, production
codec and 16 tooling checks pass. Changed LLVM translation units compile as
C++17 with warnings treated as errors; maintained patch replay and digests match.

Public runtime layouts, extension ABI 24 and interface schema 2.2 are unchanged.
The production decoder's unused object helper and partial aggregate test
initializers are removed to keep the affected host build warning-free. Broader
property behaviors, metaprogramming/profile conformance and matching SDK/archive
and native-host acceptance remain separate obligations.
