#ifndef OBJC3_KEYPATH_H
#define OBJC3_KEYPATH_H

#include "core.h"
#include <objc3c/runtime/objc3_keypath.h>

#define OBJC3_KEYPATH_MODULE_SEMVER_MAJOR 2u
#define OBJC3_KEYPATH_MODULE_SEMVER_MINOR 0u
#define OBJC3_KEYPATH_MODULE_SEMVER_PATCH 0u

/* KeyPath<Root, Value> and WritableKeyPath<Root, Value> are compiler-owned
 * native record type constructors. @keypath(Class, property.chain) checks
 * declared instance properties without evaluating or capturing self.
 *
 * KeyPathGet(kp, root), KeyPathSet(kp, root, value), KeyPathMetadata(kp), and
 * KeyPathAsReadOnly(kp) are compiler-owned applications. Each written operand
 * is evaluated once. Accessors use ordinary Objective-C dispatch, ownership,
 * and declared nullability. KeyPathSet requires a WritableKeyPath value.
 * A native record-field set reads one property snapshot, updates the field,
 * and writes that snapshot through the original setter exactly once. The
 * receiver remains alive through that complete read/modify/write operation.
 *
 * Metadata is immutable and valid for the lifetime of its native image. No
 * registry, reset generation, integer handle, or root retain is involved.
 * Nullable intermediate properties require explicit optional handling and
 * are rejected by the current literal grammar. KVC string boxing and the
 * optional root[kp] spelling are outside this module's current surface. */

#endif
