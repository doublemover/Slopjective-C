# Native Linux typed key paths

This development slice implements the language and library boundary in Part 3
§3.6 and standard-library contract S.2.6/S.3.3. It is authored against the
canonical development LLVM sourcetree and the Linux GNUstep 2.2 native runtime.
Native compiler regressions and all 24 source/PCH runtime configurations pass
across C17/C++20/C++23, ARC/MRC and O0/O2, including exact-once record snapshots
and C++ operand failures. The same 24 configurations pass under ASan/UBSan and
TSan. Native interface round trips, the facade suite and installed SDK gate also
pass; see the [integration acceptance receipt](../support/linux_native_feature_integration_acceptance.json).
Full profile conformance remains reserved.

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
`objc3.cap.keypath`. `keypath.h` supplies semantic version 2.1.0 and the public
immutable metadata ABI. `KeyPath<Root, Value>` and
`WritableKeyPath<Root, Value>` are compiler-owned native record type
constructors. Root class names and class pointer types normalize to a nonnull
class pointer. A literal with an entirely settable property chain has writable
type; a readonly chain has readonly type. `KeyPathAsReadOnly` explicitly narrows
a writable value without reevaluating it. Native C assignment, argument, and
return conversion also narrow writable paths to the exact readonly Root/Value
type. C++ uses an implicit const conversion member, including native overload
resolution. Root and Value variance and readonly-to-writable conversions are
not admitted.

The parser owns `@keypath(RootType, property.chain)` and
`@keypath(self, property.chain)`. `self` requires an instance method and is a type
root: creating the literal does not load, capture, or retain the receiver.
Concrete Objective-C generic roots substitute the declared component types.
Sema resolves every declared instance property, checks availability, verifies
object intermediate types, and retains the final property's nullability.
Missing, inaccessible, or incompatible components fail before lowering.

`KeyPathGet`, `KeyPathSet`, and `KeyPathMetadata` are compiler-owned applications.
Each written operand is evaluated once, in source order, and the selected
keypath record is copied before later operands can mutate its source.
Retainable roots and setter values acquire independent precise lifetime
snapshots before the next operand. ARC uses owned weak loads and native Block
copies; MRC retains an independent ownership claim even for a conventional +1
operand. Normal and exceptional cleanup release each completed snapshot at the
full-expression boundary. An operand with its own temporary cleanup has a
rollback slot protecting the completed snapshot if a destructor throws.
A generated typed function performs each selected getter once, and a setter
dispatches through the declared final setter. These are ordinary Objective-C message expressions with the declared
selectors; strong, copy, weak, atomic, native result ownership, dispatch, and
unwinding use the existing compiler/runtime behavior. Application rejects an
incompatible concrete root or nullable root. Setting a nonnull pointer property
rejects a nullable or nil value. Readonly values cannot call `KeyPathSet`.

Nullable intermediate traversal is admitted only as a readonly path with an
explicitly nullable Objective-C object or Block final Value. Each selected
receiver is bound once with precise ARC/MRC lifetime, tested once, and a missing
receiver returns nil without evaluating later components. Native record
components in such a chain use one value snapshot. Nullable chains never permit
setting, even when each component property has a setter. Scalar, aggregate, and
nonnull final results through nullable intermediates remain diagnosed.

Native MRC getter results use an ordinary +0 convention: a declared +1 getter
result is autoreleased, while an unowned object or Block result gains an
independent autoreleased owner before intermediate receiver cleanup. Blocks
use native copying, including native C++ copy-constructor unwinding. ARC retains
its ordinary result ownership lowering.

Named, trivially copyable native records may be traversed through their direct
accessible fields, including nested record fields. For a terminal record field,
setting retains the exact property receiver through the operation, snapshots
the record once, updates the selected field, and sends the original property's
setter once. Sibling values survive the writeback. Precise lifetime bindings
use the same ARC/MRC retain/release and unwind path as `withLifetime`; a native
exception from the getter or setter releases the retained receiver. Atomic
property accessors keep their individual native semantics; the complete
read/modify/write sequence is not a new atomic transaction.

The record contains typed getter/setter function pointers and an immutable
descriptor pointer. It contains no root object, registry index, synthetic
integer handle, or reset-generation state. Readonly and writable records carry
distinct native type identities. Names derive from canonical source types,
recursive value nullability, concrete runtime class/protocol names, and generic
arguments, rather than source offsets or Sema addresses. The ordinary records,
functions, constants, and attributes use native AST/PCH serialization and AST import.
`clang/AST/ObjC3KeyPath.h` exposes their recognized Root/Value/writability to
native interface consumers. Type printing preserves `KeyPath<...>` spelling;
literal printing reconstructs `@keypath(...)` without private helper names.
Applications retain a public syntactic call in a native `PseudoObjectExpr`,
separate from their semantic operand snapshots and typed calls. The public AST
helper exposes the operator token, complete invocation range, and source-order
arguments for native editor tooling and AST import. Native tree transforms
reconstruct applications from those public operands.

Type signatures and typed get/set/read-only operations need only the public
opaque descriptor tag. The compiler reuses an existing compatible forward
or definition, rejects conflicting declarations, or registers a compatible
public forward that a later module header definition completes. Literal
construction and reflection still require the complete validated module ABI.
This permits native source interfaces to carry keypath signatures without
serializing producer-local literal helper bodies. This opaque descriptor
support is independent of class identity: Root, concrete object Value types,
and nested generic/callback object types require complete interfaces/protocols
before the keypath type is formed. Forward-only classes and open Objective-C
type parameters diagnose, preventing a later native runtime-name annotation
from changing an already emitted keypath record identity.

ABI-v2 descriptors retain source/native root and value identity, the dotted
path, component receiver/value types, declaration owner identity, actual getter
and setter selectors, and readonly/nullable/nonnull/weak/copy/strong/atomic,
retainable-value, and effective getter-returns-retained flags. Effective native
getter ownership includes method families under MRC and explicit ownership
attributes. Nullable traversal is an additive descriptor flag with a readonly,
nullable-retainable final-value invariant; descriptor layout remains v2.
Metadata storage has native-image lifetime. The runtime validates its ABI/shape, performs checked component lookup, and compares full structural
identity across separately emitted descriptors. A readonly view retains the
underlying chain descriptor; its static type still prevents setting.
Native record fields carry a component-kind flag, empty selector strings, and
their declaring record identity. Paths containing native fields clear the KVC
compatibility bit.

Current fail-closed boundaries are nullable intermediate traversal to a
nonnull/scalar/aggregate result, nullable-chain writes, incomplete class/protocol
identity, open Objective-C generic arguments, dependent types,
nontrivial/anonymous/union record traversal, inherited fields, bit-fields,
reference fields, nontrivial value payloads, async/throwing accessors,
actor-isolated/executor-affine properties, and borrowed or scalar resource
ownership that would lose provenance through a value getter. Native
`objc_returns_inner_pointer` accessors and consuming accessor receivers or setter
parameters also diagnose, since the stored Root/Value callable contract cannot
erase those ownership requirements. Nullable final properties are supported.
Optional `root[path]` sugar and explicit NSString KVC boxing are not implemented. These boundaries must remain visible in inventory
and conformance reporting.

Maintained fixtures cover parser/sema diagnostics, C17 and C++20/23 PCH import,
native AST import, source printing/reparse, concrete generic roots, incompatible
library metadata, native metadata-service failures/equality, and separate native
translation units exercising exact-once evaluation, aggregates, custom selectors,
implicit readonly conversion, mutation during later operands, copy/strong/weak
property behavior, and self without a capture. Additional AST fixtures check
complete source ranges and public operation spelling across native import;
PCH fixtures cover signatures preceding the metadata header and nested generic,
callback, pointer, and member-pointer nullability identity. Runtime fixtures also
cover nullable short-circuit counts, Block results, root release by a later
operand, exceptional snapshot cleanup, and retained getter result balancing.
`tests/clang_foundation/typed_keypath_packages` supplies the separately authored
native source-package fixture. It was missing from the 31-gate published SDK
qualification. The new `qualify_typed_keypath_packages` gate distributes and
executes it across 12 C17/C++20/C++23 ARC/MRC O0/O2 configurations: source build,
full reuse, publication/relocation, provider-source removal and source-free
consumption, plus a rejected readonly setter in each mode. It also compares
producer and consumer key-path metadata identities. The expanded 32-gate SDK and fresh Debian/Ubuntu qualification pass: 12 modes,
36 executions and 12 readonly rejections. The [package acceptance receipt](../support/linux_keypath_package_acceptance.json)
records the exact archive and scope. The earlier 24-case runtime matrix did not
establish this package coverage.

The later 33-gate SDK retains this complete package matrix and passes it on fresh
Debian and Ubuntu with the newer compiler; see the
[current acceptance receipt](../support/linux_async_handle_join_acceptance.json).
