# Native cleanup overload selection and template deduction

Source 2.162 / System 2.88 uses native overload resolution for automatic cleanup,
local resource owners and `requires_cleanup` typedefs. C++ ordinary overloads,
static members, constrained templates and deduced template callbacks select
against the actual storage type. C17 `overloadable` callbacks use the same path.

Selection prefers `F(value)`. Only a lack of viable value candidates permits
`F(&value)`; ambiguity, a deleted best candidate and inaccessible callbacks retain
ordinary native diagnostics. The selected declaration must still meet the unary,
synchronous, result and exact no-record-copy/no-writeback cleanup contract.
Dependent and `auto` storage retain recipes until substitution/deduction makes
native selection meaningful. Scalar resources retain their existing type gates.

Canonical publication writes complete selected template arguments. A deduced
non-type argument formerly contributed an extra source expression only after
replay, changing an otherwise identical body fingerprint. Concrete cleanup and
resource identities now bind their selected declaration/specialization/signature
once; deferred recipes still bind their complete lookup expression. Native array
callbacks and publication controls defend this distinction.

`objc3.native-storage-lifetime.v6` is a hard reader cutover. Extension/runtime ABI
layouts remain unchanged. Eighteen source/PCH commands cover C17 and C++20/23
ARC/MRC, including constraint ranking, value-before-address selection, deduced
array bounds, dependent/auto storage, typedef contracts, ambiguity/deletion/access
errors, forbidden copies and async callbacks. The 77 AST controls include exact
concrete callback and deferred recipe checks. Publication and source-free runtime
controls cover selected overloads, inferred templates, constrained/static members,
whole-array deduction and resource contracts. Rebuilt compiler/SDK acceptance
remains separate.
