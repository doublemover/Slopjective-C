# Nullable-chain key-path writes

Keypath 2.4.0 preserves writable type for an entirely settable nullable chain
whose final Value is an explicitly nullable Objective-C object or Block. A
missing intermediate makes `KeyPathSet` and subscript assignment skip the update.
Their written root, key and value operands still evaluate once, in source order,
before traversal begins. Subscript assignment yields the converted input even
when no setter runs. Readonly components and readonly static views reject writes.

Each intermediate receiver through the last nullable edge is read once, pinned
through the remaining operation and tested before traversal continues. ARC/MRC
normal and exceptional exits use native precise lifetime cleanup. Native record
routing prefixes are copied once without writeback; a modified record suffix
retains the existing exact-once property writeback. Declared copy/strong/weak and
native accessor exception behavior remain in force. Missing getter traversal
still returns nil. Scalar, aggregate and nonnull final values through nullable
intermediates remain gated, as do dependent, effectful, isolated and resource
paths.

The descriptor layout remains ABI v2. Writable nullable descriptors require
`OBJC3_KEYPATH_NULLABLE_WRITES`, in addition to writable and nullable-traversal
flags. Runtime validation requires descriptor writability to agree with every
component and rejects missing or impossible flags. Previously emitted forced-
readonly nullable metadata with an entirely settable chain must be regenerated.
A readonly view retains its underlying descriptor and its static write restriction.
Published inline bodies and initializers containing writable nullable literals
require `objc3.keypath-nullable-writes.v1` and `objc3.typed-keypaths.v1`; readers
reject unknown capabilities and inconsistent declaration markers.

Compiler diagnostics/codegen, native runtime metadata, interface re-emission,
source-free packages and runtime witnesses cover nil and present chains,
subscripts, operand order, nested nullable edges, object and Block setters,
record routing/writeback and receiver destruction during throwing setters.
Focused validation results are recorded when complete. Broader source and
sanitizer validation remain batched. SDK 2.33 is the accepted archive and predates
this change; installed delivery requires the rebuilt compiler and runtime plus
regenerated artifacts. Full profile and native AArch64 qualification remain open.
