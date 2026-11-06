# Native publication completion and exact memory byte ranges

Concurrency source 2.110 and System source 2.22 close the concrete failures
found by executing the postponed frontend publication controls against extension
ABI 24. The accepted SDK 2.62 archive remains unchanged.

Public function-template declarations, member function declarations and ordinary
and template static-data member declarations preserve their complete signatures
without requiring a body or initializer. Canonical replay retains module-owned
names, redeclarations and later out-of-line definitions. Existing borrowed,
consumption, family, Sendable and executor/task attributes receive the same
admission as their ordinary declaration counterparts. Unknown attributes and
excluded/private dependencies still reject. CF return attributes retain source
spelling and native attribute identity.

ISO C inline definitions publish their written bodies with
`objc3.c-inline-definitions.v1`. Their identity binds the body and ISO inline
linkage. Codec and artifact readers require matching function owners and reject
missing/invented capabilities. External C inline definitions remain rejected
until their single-provider definition projection is implemented; consumer replay
must not accidentally emit another external definition.

Canonical task-header promises remain SDK implementation dependencies during
ordinary C++ coroutine publication. Promise types and protocol calls are skipped
only through the qualified runtime task owner; custom promises retain ordinary
public dependency checks. Source coroutine operations still participate in
identity. Native type-tag flags print as identifiers, builtin argument mappings
print as one-based source indices, and zero indices diagnose before parameter
lookup. Empty Block parameter lists remain explicit before async/noexcept effects.

Constant byte-offset memory operations recover a complete tracked object only
when native field/array layout, local provenance, root bounds and exact size all
agree. Aliased and conditional bases, commuted addition and balanced offsets
preserve complete target sets. Full copies carry loan, callable, address and
owner facts; returned destinations retain the recovered target. Partial/padding,
dynamic/overflowing, dependent and unsupported union/base paths stay conservative.
Unknown provenance cannot become a complete write proof. Template controls verify
that layout is resolved after substitution.

Twenty-five compiler regressions pass against the rebuilt development compiler:
all twenty-two borrowed cases plus the new diagnostic and Block printing cases.
The new cases execute 105 source/PCH/AST-print/reparse commands across C17 and
C++20/23, ARC/MRC. All twelve selected frontend groups pass, covering ISO C inline
capability admission, declaration-only template replay and later definitions,
overload/native diagnostic attributes, NSObject/CF storage, borrowed callable and
static results, memory aliases/byte ranges, dependent key paths, concurrent
captures/Block values and dependent native awaits. The groups include relocation,
changed identities and valid/invalid consumer controls.

Frontend execution uses current ABI 24 development headers and DSO with generated
current runtime-header identities. It does not reseal the foundation or claim
installed SDK qualification. Source-free package execution, remaining/wider
native and instrumentation matrices and coordinated SDK delivery remain pending.
