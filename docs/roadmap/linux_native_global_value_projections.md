# Global value provenance at native concurrent boundaries

Source 2.228 / System 2.154 retains known borrowed and cleanup-managed values
through global, external, static-local and thread-local storage. The bounded
native-scope write index follows direct assignments, immutable address views,
reference aliases and record fields. Fixed-array cells with constant in-bounds
indices participate without treating an ordinary element assignment as an
unknown whole-array escape. Array and aggregate source sets are conservative
unions; this does not establish independent field/element state.

Bindings use canonical declarations for write lookup. Initializers are recovered
across redeclarations, including block-scope extern declarations. A copied local
snapshot therefore retains the original loan/resource even when its imported or
redeclared global slot has an ordinary unqualified scalar type. Boolean
observations and ordinary constant writes retain their independent value roles.
The same source relation feeds concurrent captures and executor transfers.

Analysis remains a bounded may-source union within the current native scope.
It does not prove what arbitrary other functions, dynamic initializers, external
libraries or threads may write; it does not establish whole-program global
lifetime, race freedom or temporal strong updates. Strict System's separate
borrowed-store restrictions remain enforced. Unknown aliases and exhausted
indexing remain conservative.

The preceding frontend misses seven direct C17 regression diagnostics. Maintained
controls cover C17/C++20/C++23, ARC/MRC, four profiles and source/PCH input, with
additional redeclaration, initializer and constant-array controls. A separate
native owner publishes an interface containing global pointer/integer, array and
record slots, then compiles source-free consumers in all six language/ownership
configurations. It checks known loan/resource rejection and ordinary/boolean
admission, including awaited executor crossings with unsafe-Sendable loan
spelling. All 72 source/PCH controls, six source-free configurations and 100
focused tooling checks pass. Full current-source SDK and native ARM acceptance remain separate.
