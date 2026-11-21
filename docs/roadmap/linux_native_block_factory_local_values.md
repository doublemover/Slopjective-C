# Immutable local Block values inside work-item factories

Source 2.209 / System 2.135 admits closed factories that construct a literal
Block in an immutable local, then return it through one or more immutable
aliases. Conditional literal environments and null results retain their ordinary
selection rules. Scalar/object aliases and fixed-arity concrete template
captures preserve actual caller provenance as in the preceding factory spans.

Local construction is validated separately from the returned environment.
An unused literal does not transport its captures, including an unsafe object or
a non-async callback. Explicit capture initializers, mutable locals and opaque
initializers cannot establish this closed recipe. Returned environments still
resolve every captured binding against the original actual caller's contract.
Block-valued parameters recursively prove their actual environments.

ARC uses native local Block and return lowering. MRC must copy every returned
capturing environment through the reserved runtime contract: copying at return
and copying into an immutable local both preserve their real ownership. An
uncopied local literal is rejected at the task boundary as well as by the native
stack-Block diagnostic. The proof synthesizes no ownership operation. Its work,
scope-scan, depth and cycle limits remain unchanged; runtime ABI 24, interface
schema 2.2 and reader capabilities remain unchanged.

The source/PCH controls cover returned literal/copy aliases, conditionals, nulls,
concrete templates, unused unsafe environments, resource inputs in both value
positions, unknown callbacks, mutable/open initializers and uncopied MRC results.
Native helper tasks exercise both copy placements through two direct and two
source-directed consumers, with source removal, exact retirement/work/defer
counts and pre-entry cancellation. Nested captures of locally constructed Block
environments and arbitrary local producer calls remain outside this recipe.

The 72-command source/PCH matrix passes across C17/C++20/C++23, ARC/MRC
and all four profiles. The positive header fails on Source 2.208 and passes with
this implementation. Four optimized native configurations pass 1,792 task
scenarios; isolated case selection runs all four configurations concurrently and
requires each independent pass receipt. Fresh maintained patch application and
16 tooling checks pass. The preceding span's full isolation/codec/122-AST receipt
remains distinct; matching SDK/archive and full-profile acceptance remain pending.
