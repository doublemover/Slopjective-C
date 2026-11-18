# Immutable native function-pointer targets

Source 2.232 / System 2.158 resolves a single immutable native function-pointer
target for the shared argument/receiver proofs and bounded scalar helper graph.
Const local/global bindings, canonical redeclarations, const reference aliases,
address expressions, no-op casts and standard reference views preserve that
target. A pure conditional admits only one canonical target, including a
constant-selected branch or two references to the same function.

The selected declaration's exact canonical function type must match the call's
pointee prototype, including native calling convention and effects. Mutable or
volatile bindings, incompatible prototypes, distinct runtime-selected targets,
opaque producers and exhausted/cyclic discovery do not establish this proof.
Target resolution has a shared 64-step budget and depth sixteen. The existing
closed-definition, ownership/effect/dispatch and helper/storage bounds still
apply. An address alone does not admit a weak, opaque or otherwise unproved body.

Known global stores/loads, reference writes and pure pointer-load results retain
their value sources through these calls. Runtime-selected callbacks, arbitrary
function-pointer producers/conversions, mutable pointee writers and unresolved
interprocedural callback parameters retain their separate requirements.

The preceding frontend misses four direct C17 loan/resource transfer diagnostics.
Maintained source/PCH controls cover C17/C++20/C++23, ARC/MRC and all four profiles,
including same/different-target conditionals, volatile/mutable bindings,
weakened noexcept prototypes and discovery beyond the bound. Source-free native
packages publish indirect helper implementations and check global writes,
resource pointer loads, independent values and real standard-library move/const
views. All 1152 new/adjacent source/PCH commands, six source-free
configurations and 100 tooling checks pass. Matching SDK, native ARM and
whole-profile acceptance remain pending.
