# Native operator, array and pointer-bit transfer views

Source 2.224 / System 2.150 extends the bounded native field-getter proof to
zero-argument member operators, including call, dereference, unary and address
operators. Pending concrete generic operator bodies use the same selected-body
preparation as ordinary getters. Virtual targets still require qualification or
final dispatch; volatile receivers, side-effectful bodies and unresolved targets
do not acquire a closed proof.

Known borrowed identities survive integer casts, arithmetic, shifts and bitwise
operations in both direct expressions and getter bodies. Boolean conversions,
comparisons and logical results remain observations; scalar resource telemetry
such as `descriptor + 1` retains its established behavior. The previous compiler
misses all eleven original operator/bit-operation capture diagnostics in the new
control file. A separate fixed-array getter probe also previously passed without
the required borrowed-transfer rejection.

Field paths now include constant, in-bounds indices into native fixed arrays,
including nested record/array paths, array-to-pointer decay at element zero and
concrete template indices. Pointer-based indexing and dynamic indices remain
outside this closed storage proof. Selected borrowed pointer fields and elements
retain their identity; independent numeric/status observations remain usable.

Returned storage addresses are distinct from field-value copies. Immutable
pointer bindings can resolve their containing local storage for subsequent writes;
unknown or mutable pointer escapes invalidate the receiver proof. This preserves
resource writes through overloaded references and addresses while retaining the
existing bounded alias/escape rules. Shared work, recursion, target-set and scope
index bounds remain enforced.

The same span completes publication of multiple constants sharing one anonymous
record declaration. Later variables print `decltype(first)` references to the
same type, preserving their own initializers. The temporary printing projection
restores both semantic type and source type information. Source-free C++20/23
controls cover shared types in namespaces, aliases, structural variable templates,
member-produced local records, relocation and missing owner exports.

The new source/PCH matrix covers C17/C++20/C++23, ARC/MRC and all four profiles.
Native source-free transfer tests cover operators, transformed pointer bits and
array getters, including awaited unchecked-parameter boundaries. Matching SDK,
full-profile and native ARM acceptance remain separate work.

All 648 source/PCH commands pass across the new matrix and eight adjacent
projection suites. The complete native isolation test passes, including four
source-free C++20/23 × ARC/MRC operator/array configurations and awaited unchecked
parameter transfers. The expanded anonymous-object execution test, fresh patch
reconstruction and 90 tooling checks pass, with four archive-dependent skips.
