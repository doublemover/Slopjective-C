# Native typed key-path witnesses

These native-library fixtures pass all 24 source/PCH configurations normally
and under ASan/UBSan and TSan, and pass the installed SDK feature gate. See the
[integration receipt](../../../docs/support/linux_native_feature_integration_acceptance.json).
Compile `provider.m` and
`main.m` as separate translation units in Objective-C C17 and Objective-C++
C++20/C++23, under ARC and MRC, with the installed `objc3.keypath` module and
`objc3_runtime_services` archive. `runtime.h` is also the PCH input; define
`TYPED_KEYPATH_RUNTIME_PCH` for both sources when importing that PCH.
Enable Blocks with `-fblocks` and native exceptions with
`-fexceptions -fobjc-exceptions`. ARC also uses `-fobjc-arc-exceptions` for the
unwind witnesses.

The checks cover typed record ABI across translation units, nested getter and
setter dispatch, custom selectors, exact-once source-order operand/component
evaluation, selected-path snapshots under mutation of later operands,
metadata and declaration identity, implicit readonly argument/return/assignment
conversion, aggregate payloads,
record-field snapshot/writeback, sibling preservation and native unwinding,
copy/strong/weak property semantics, nullable object/Block reads and nil no-op writes, nested and record-routed
chains, setter receiver pinning across owner removal and native unwinding,
short-circuit accessor counts, root retention through later operands and native
exception unwinding, +1 getter return normalization under MRC, and the absence
of a captured self owner.
Native interface and facade contracts pass in the coordinated SDK build.

`operand_failures.mm` is a separate Objective-C++ execution input linked with
`provider.m`. It makes native Block copying throw from a captured C++ copy
constructor after a later operand releases the root's source owner. The checks
require the completed root snapshot and partially copied capture to unwind
without leaks or an accessor call. This fixture passes in the same matrix.

The keypath 2.2 source additions exercise named signed/unsigned bit-fields and
C++ fields inherited from separate base subobjects. Field setters preserve
siblings and perform one property read/writeback. Provider and consumer paths
compare their structural metadata, including the actual base declaration owner.
These additions enter the next combined source validation batch.

The keypath 2.3 source additions exercise typed subscripts, assignment results,
compound updates, prefix/postfix operations, root/key/value order, frozen path
selection, record writeback and independent root lifetime snapshots. The C++
throwing-Block-copy witness runs through both explicit and subscript setters.
Native source/PCH diagnostics additionally distinguish sequenced key-path
operands from genuine conflicts within an operand or a surrounding expression.
