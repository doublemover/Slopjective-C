# Native work-item factory scalar calculations

Source 2.207 / System 2.133 extends immutable factory-local recipes with builtin
scalar arithmetic, bitwise, comparison and logical operators, unary value
operators and arithmetic casts. The result retains the union of contributing
actual caller arguments. Narrowing, an intermediate alias or a template-local
calculation cannot silently erase a cleanup-managed argument's transfer check.
Both alternative value arms contribute; condition-only arguments retain the
existing selection rule. A statically unevaluated short-circuit operand does not
become a transported value.

Only the bounded recipe changes. Calculations execute normally during factory
construction; no frontend interpretation, cached result or runtime ownership
operation is introduced. Assignment, increment/decrement, dereference, pointer
arithmetic, address formation, comma expressions, overloaded operators, opaque
calls and unrelated dependent conversions cannot establish a recipe.

Dependent builtin expressions have a placeholder type before instantiation.
A dependent local can use that expression only when its exact pattern type
matches the scalar actual parameter types supplying it. An unrelated dependent
trait cannot smuggle a record constructor or user conversion into this proof.
The existing actual-argument, recursive callback, MRC copy, work/depth/cycle and
scope-scan checks remain in force. Runtime ABI 24, interface schema 2.2 and
reader capabilities do not change.

The source/PCH controls cover multiple contributing inputs, arithmetic casts,
unary operations, conditional choices, constant short-circuit operands, concrete
templates and rejected mutations/dereferences/opaque calls. The positive header
fails against Source 2.206 and succeeds with the implementation. The native
package witness adds a calculated value alongside its owner and checks exact
retirement, work/defer counts and cancellation in direct and directed consumers.
Both input positions have imported resource-erasure negative controls.

Focused verification and matching SDK delivery are tracked separately; no full
profile or replacement-archive acceptance is claimed here.
