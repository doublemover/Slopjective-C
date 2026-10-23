These are pending native-library execution fixtures. Compile `provider.m` and
`main.m` as separate translation units in Objective-C C17 and Objective-C++
C++20/C++23, under ARC and MRC, with the installed `objc3.keypath` module and
`objc3_runtime_services` archive. `runtime.h` is also the PCH input; define
`TYPED_KEYPATH_RUNTIME_PCH` for both sources when importing that PCH.
Enable native exceptions with `-fexceptions -fobjc-exceptions
-fobjc-arc-exceptions` for the getter/setter unwind witnesses.

The checks cover typed record ABI across translation units, nested getter and
setter dispatch, custom selectors, exact-once source-order operand/component
evaluation, selected-path snapshots under mutation of later operands,
metadata and declaration identity, implicit readonly argument/return/assignment
conversion, aggregate payloads,
record-field snapshot/writeback, sibling preservation and native unwinding,
copy/strong/weak property semantics, and the absence of a captured self owner.
No execution or qualification result is claimed by these authored fixtures.
