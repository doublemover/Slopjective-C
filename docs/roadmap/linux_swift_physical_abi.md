# Swift physical ABI in published native declarations

Source 2.154 / System 2.80 preserves `swiftcall`, `swiftasynccall`,
`swift_context`, `swift_async_context`, `swift_error_result` and
`swift_indirect_result` through semantic-interface emission and AST import.
These are physical calling conventions and register/result roles, independent
of Swift-facing API naming metadata.

The native declaration printer retains written and inherited ABI attributes and
reconstructs Swift calling conventions stored only in the function type.
CodeGen gives `sret` the actual pointee object layout. Erased and incomplete
results use an opaque zero-byte buffer type, preserving the result register
without inventing a pointer-valued object. This prevents optimized stores from
being discarded under the wrong result shape.

Function-type identity includes calling convention and per-parameter ABI roles,
including callbacks nested in pointer/reference/member/array and callable types.
The `objc3.native-declaration-attributes.v3` hard reader cutover covers those
identities; v2 readers must rebuild interfaces. Extension ABI 24, service ABI 4
and frame ABI 1 remain unchanged.

AST import restores parameter declarations for callback prototypes that have no
owning function. This preserves their declaration attributes instead of retaining
only the function QualType. Actual function-owned parameters keep the existing
cycle-safe import ordering. An upstream AST unit control covers both callback
typedef shapes and ordinary function parameters in C++20/23 ARC/MRC.

The native source-free provider/two-consumer fixture exercises context/error
registers, indirect results, indirect callback calls and async context calls,
checks the LLVM `swiftcc`, `swifttailcc`, `swiftself`, `swiftasync`, `swifterror`
and `sret` contracts, and links/runs consumers after deleting provider source.
Canonical byte replay, changed-role fingerprints and old-reader rejection are
also checked. The installed async package fixture carries the same APIs.

Source 2.268 / System 2.194 extends canonical publication to C++ function
pointer and alias template type-ids with Swift call and async call conventions,
x86 vector and register calls, System V, and AArch64 vector and SVE PCS. The
printer places the GNU convention specifier before the type so the C++ alias
declarator reparses. The type identity still binds the native ABI and each Swift
parameter role.

Linked x86-64 consumers exercise the imported aliases in three C++20/C++23
ownership and optimization configurations. A separate AArch64 provider and
source-free consumer retain both PCS opcodes in LLVM IR and execute under
emulation. Plain function pointers fail conversion to the vector alias.

This work does not establish the complete Swift interoperability or profile
roadmap. The SDK must be rebuilt from the maintained compiler patches before
claiming archive acceptance for this span.
