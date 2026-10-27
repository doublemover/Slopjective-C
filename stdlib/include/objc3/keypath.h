#ifndef OBJC3_KEYPATH_H
#define OBJC3_KEYPATH_H

#include "core.h"
#include <objc3c/runtime/objc3_keypath.h>

#define OBJC3_KEYPATH_MODULE_SEMVER_MAJOR 2u
#define OBJC3_KEYPATH_MODULE_SEMVER_MINOR 2u
#define OBJC3_KEYPATH_MODULE_SEMVER_PATCH 0u

/* KeyPath<Root, Value> and WritableKeyPath<Root, Value> are compiler-owned
 * native record type constructors. @keypath(Class, property.chain) checks
 * declared instance properties without evaluating or capturing self.
 *
 * KeyPathGet(kp, root), KeyPathSet(kp, root, value), KeyPathMetadata(kp), and
 * KeyPathAsReadOnly(kp) are compiler-owned applications. Each written operand
 * is evaluated once in source order. Retainable operands have independent
 * precise lifetime snapshots through the enclosing full expression, including
 * native exception unwinding in ARC and MRC. Accessors use ordinary
 * Objective-C dispatch, ownership, and declared nullability. KeyPathSet requires a WritableKeyPath value.
 * A native record-field set reads one property snapshot, updates the field,
 * and writes that snapshot through the original setter exactly once. The
 * receiver remains alive through that complete read/modify/write operation.
 *
 * Metadata is immutable and valid for the lifetime of its native image. No
 * literal construction uses no registry, reset generation, integer handle,
 * or captured root owner.
 * Nullable intermediates are supported only for readonly traversal to a
 * nullable Objective-C object or Block result. The first missing intermediate
 * returns nil without evaluating later components. Such paths never permit
 * KeyPathSet, even when individual component properties have setters.
 * Nonnull/scalar/aggregate results through nullable intermediates diagnose.
 * Named bit-fields preserve native value truncation and sibling storage. C++
 * inherited record fields use native lookup and base adjustment, with normal
 * access, hiding and ambiguity rules. Field setters update one property
 * snapshot and write it back once.
 * KVC string boxing and optional root[kp] syntax remain outside this surface. */

#endif
