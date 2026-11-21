# Composed native work-item factory recipes

Source 2.208 / System 2.134 composes closed factories that return another
factory's constructed Block. Pure branches, immutable aliases and scalar
calculations can feed inner factories. Inner captured bindings are remapped to
the complete set of original outer caller arguments before transfer checking.
Unknown callback environments, unsafe objects and cleanup-managed values cannot
be erased by another helper boundary or a plain formal parameter type.

Inner recipes are recomputed symbolically under the same work/depth limits;
expression memoization never reuses a different outer caller's argument mapping.
An active-definition set rejects direct and mutual factory recursion. Every
construction argument must be part of the closed value recipe, including ignored
arguments: an opaque stateful call cannot hide behind an unused parameter.
Unresolved producers, dependent call targets and unrelated conversions remain
unproved. Static C++ factories retain the same rules as free functions.

ARC return lowering and the existing MRC runtime-copy contract remain native.
Returning an inner admitted copied Block introduces no synthetic copy, claim or
release. Actual callback values recursively prove their environments under the
shared capture budget. Runtime ABI 24, interface schema 2.2 and reader
capabilities remain unchanged.

The source/PCH controls cover multiple nested levels, branch alternatives,
static methods, object/callback forwarding and rejected resource copies in both
input positions, recursive/open factories and hidden stateful arguments. The
native witness adds independent nested-factory and callback-chain tasks to the
existing 24 shapes. Two direct and two source-directed consumers retain provider
removal, exact retirement/work/defer checks and pre-entry cancellation.

The new matrix and six adjacent suites pass 504 source/PCH commands across
C17/C++20/C++23, ARC/MRC and all four profiles. Four optimized native
configurations pass 1,664 task scenarios. Full isolation, production codec,
122 AST controls, fresh maintained patch application and 16 tooling checks
pass. Matching SDK/archive and full-profile acceptance remain separate.
