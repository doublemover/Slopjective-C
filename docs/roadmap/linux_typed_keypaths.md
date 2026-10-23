# Native Linux typed key paths

This development slice implements the language and library boundary in Part 3
§3.6 and standard-library contract S.2.6/S.3.3. It is authored against the
canonical development LLVM sourcetree and the Linux GNUstep 2.2 native runtime.
Builds, execution, PCH tests, native interface round trips, and SDK qualification
are deliberately postponed. This document records implementation and pending
fixtures, not successful validation or a profile conformance claim.

```objc
@import objc3.keypath;

WritableKeyPath<Person *, int> agePath(void) {
  return @keypath(Person, address.age);
}

int update(Person * _Nonnull person) {
  WritableKeyPath<Person *, int> path = agePath();
  KeyPathSet(path, person, 42);
  return KeyPathGet(path, person);
}
```

The canonical module is the singular `objc3.keypath`, with capability
`objc3.cap.keypath`. `keypath.h` supplies semantic version 2.0.0 and the public
immutable metadata ABI. `KeyPath<Root, Value>` and
`WritableKeyPath<Root, Value>` are compiler-owned native record type
constructors. Root class names and class pointer types normalize to a nonnull
class pointer. A literal with an entirely settable property chain has writable
type; a readonly chain has readonly type. `KeyPathAsReadOnly` explicitly narrows
a writable value without reevaluating it. An implicit writable-to-readonly
conversion is not currently provided.

The parser owns `@keypath(RootType, property.chain)` and
`@keypath(self, property.chain)`. `self` requires an instance method and is a type
root: creating the literal does not load, capture, or retain the receiver.
Concrete Objective-C generic roots substitute the declared component types.
Sema resolves every declared instance property, checks availability, verifies
object intermediate types, and retains the final property's nullability.
Missing, inaccessible, or incompatible components fail before lowering.

`KeyPathGet`, `KeyPathSet`, and `KeyPathMetadata` are compiler-owned applications.
Each written operand is evaluated once. A generated typed function performs
each selected getter once, and a setter dispatches through the declared final
setter. These are ordinary Objective-C message expressions with the declared
selectors; strong, copy, weak, atomic, native result ownership, dispatch, and
unwinding use the existing compiler/runtime behavior. Application rejects an
incompatible concrete root or nullable root. Setting a nonnull pointer property
rejects a nullable or nil value. Readonly values cannot call `KeyPathSet`.

The record contains typed getter/setter function pointers and an immutable
descriptor pointer. It contains no root object, registry index, synthetic
integer handle, or reset-generation state. Readonly and writable records carry
distinct native type identities. Names derive from canonical source types,
value nullability, concrete runtime class names, and generic arguments, rather
than source offsets or Sema addresses. The ordinary records, functions,
constants, and attributes use native AST/PCH serialization and AST import.
`clang/AST/ObjC3KeyPath.h` exposes their recognized Root/Value/writability to
native interface consumers. Type printing preserves `KeyPath<...>` spelling;
literal printing reconstructs `@keypath(...)` without private helper names.

ABI-v2 descriptors retain source/native root and value identity, the dotted
path, component receiver/value types, declaration owner identity, actual getter
and setter selectors, and readonly/nullable/nonnull/weak/copy/strong/atomic
flags. Metadata storage has native-image lifetime. The runtime validates its
ABI/shape, performs checked component lookup, and compares full structural
identity across separately emitted descriptors. A readonly view retains the
underlying chain descriptor; its static type still prevents setting.

Current fail-closed boundaries are nullable intermediate traversal, record-field
traversal, dependent types, nontrivial value payloads, async/throwing accessors,
actor-isolated/executor-affine properties, and borrowed or scalar resource
ownership that would lose provenance through a value getter. Nullable final
properties are supported. Optional `root[path]` sugar and explicit NSString KVC
boxing are not implemented. These boundaries must remain visible in inventory
and conformance reporting.

Pending fixtures cover parser/sema diagnostics, C17 and C++20/23 PCH import,
native AST import, source printing/reparse, concrete generic roots, incompatible
library metadata, native metadata-service failures/equality, and separate native
translation units exercising exact-once evaluation, aggregates, custom selectors,
readonly conversion, copy/strong/weak property behavior, and self without a
capture. `tests/clang_foundation/typed_keypath_packages` supplies native
source-package/import fixtures. None has been executed in this development
phase.
