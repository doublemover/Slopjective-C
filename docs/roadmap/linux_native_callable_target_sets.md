# Native callable target sets

Source 2.242 / System 2.168 resolves immutable function and member-function
pointers selected by bounded conditional expressions. The analysis visits both
reachable arms and requires each target to have the exact prototype, calling
convention and effect type. Member pointers also require the same owning class.
Each target must have an admissible definition. Nonvirtual independent receivers
and final virtual receivers retain the existing dispatch rules.

Every admitted definition receives the original argument bindings. Writes to
global and reference storage and the sources of returned scalar values are
combined across targets. Reference-result analysis resolves each target against
its own call arguments before combining the returned cells. A write through the
result updates every possible cell. Two ordinary targets can write or return
ordinary data without an unnecessary transfer rejection.

Discovery remains conservative for unknown, mutable or volatile targets, open
virtual dispatch, owner conversions, weakened effects and side-effectful target
selection. Each lookup allows at most eight distinct targets, 64 expression
steps and depth 16. The existing enclosing graph and storage bounds still apply.

The preceding compiler misses seven diagnostics and rejects one ordinary use
in the focused comparison. The new and adjacent cases pass 288 source/PCH
commands. Six source-free ARC/MRC consumers retain the conditional target
graphs in published C17 and C++20/23 bodies. Fresh maintained patches reproduce
the production and test files. Matching SDK and native ARM acceptance remain
open.

Source 2.246 / System 2.172 also resolves initialized mutable automatic callable
bindings with a complete local use graph. Their initializer and all admitted
assignments contribute candidate targets. The analysis requires direct
assignments and rejects escaping references, addresses, captures and glvalue
assignment results. It uses the complete owning function body when an imported
helper introduces the binding. Any unresolved expression, mismatched signature
or exhausted graph bound keeps the conservative transfer checks.

The preceding compiler misses borrowed captures after mutable target writes.
The mutable target and adjacent reference, indirect and member-pointer cases
pass 312 source/PCH commands, including escaping assignment-result controls.
Six ARC/MRC source-free consumers preserve native global writes and returned
reference cells from imported mutable callable graphs.
