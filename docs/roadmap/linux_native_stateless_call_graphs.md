# Stateless callable side-effect graphs

Source 2.231 / System 2.157 extends the closed scalar helper graph to native
C++ lambda operators with no captures. Caller storage and global writes remain
connected to formal arguments for operator syntax and explicit `operator()`
calls, including C++23 static operators. Ordinary reference stores are precise;
known loans written through those operators retain their concurrent-transfer
restriction.

Operator ASTs retain a syntactic object argument, including static operators.
One call-specific offset binds actual values to formal parameters and is reused
for arity admission, reference cells, value sources and read/pointee roles.
Definition caching retains per-call arity and receiver/dispatch validation.
The receiver is evaluated as an operand, without being mistaken for parameter
zero or allowing an out-of-range formal lookup.

This admission requires an empty capture set and the existing closed receiver
proof. Stateful captured callables, unresolved dependent bodies and open/escaping
receiver mutation remain unproved. Existing graph/node/depth/cell bounds and
conservative write/escape replay continue to apply; this does not provide
whole-program callable or global state analysis.

The preceding frontend misses three C++23 operator-to-global loan diagnostics
and rejects two independent reference-store observations. Maintained source/PCH
controls cover C17/C++20/C++23, ARC/MRC and all four profiles. Source-free native
packages publish nested stateless operator implementations, checking global and
reference writes, ordinary values and mixed explicit/implicit C++23 static calls.
The sixteen-parameter boundary also has an operator-object offset control. All
1080 new/adjacent source/PCH commands, six source-free configurations and 100
tooling checks pass. Matching SDK, native
ARM and full profile acceptance remain pending.
