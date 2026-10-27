# Typed key-path subscripts

Keypath 2.3.0 adds `root[path]` for native typed key paths. Readonly paths support
reads. Writable paths additionally support assignment, compound assignment and
prefix/postfix increment and decrement. Ordinary Objective-C subscripts retain
their existing behavior. Invalid root types, nullable/nil roots, readonly writes
and incompatible or nullable values for nonnull properties diagnose through the
same type and nullability checks as explicit key-path applications.

Written operands evaluate once in root, key, then value order. The root has an
independent ARC/MRC ownership snapshot before evaluating the key or value; the
key is copied before evaluating a value that can replace its source. Normal and
exceptional exits clean these snapshots at the enclosing full expression. Block
copy failures unwind completed snapshots. Assignment yields the converted setter
input, including native record values. Compound assignment performs one getter
and one setter; postfix yields the prior value and prefix the updated value.
Property chains and record-field writeback use their existing native accessors.
These are ordinary property updates and do not promise atomic read/modify/write.

Published concrete C++ inline bodies preserve public subscript, literal and
explicit application spellings. Dependency traversal follows written operands,
without exposing compiler-owned descriptor globals, accessors or implicit key-
path copy operations. Published literal bodies reconstruct the pinned SDK
metadata header, and typed body fingerprints bind public operations and written
operands without private accessor names or producer source locations. User declarations still require normal export/dependency
visibility. Interfaces using subscript bodies require
`objc3.keypath-subscripts.v1` and `objc3.typed-keypaths.v1`; consumers reject an
unknown capability or the missing typed-key-path dependency. The interface schema
and key-path layout remain 2.2 and 2 respectively.

Eager consumers parse the pinned metadata header before native AST merging.
This binds opaque signature descriptors to the same complete SDK types used by
later explicit imports. Both the runtime and key-path preludes finish their
linkage scopes before the importer runs; combined task/key-path dependencies
share that readiness point. Source-free C17/C++20/23 package witnesses exercise
metadata operations and subscript updates under source-directed and eager import.

Native witnesses cover C17/C++20/23 diagnostics, source/PCH printing and codegen,
AST import, clangd operand hover/rename/highlighting, public inline interface re-emission, source-free provider/consumer
updates, operand order, record values, key replacement and root destruction.
The [combined source receipt](../support/linux_keypath_subscripts_progress.json)
records 405 compiler tests, 191 native unit tests, 33 frontend suites and 331
tooling tests. Normal, ASan/UBSan and TSan each pass 168 configurations / 1,158
commands. Source-free key-path and C++ packages pass 60 configurations / 240
commands. Installed SDK delivery remains pending. Nullable writable traversal, dependent/effectful/isolated paths, owned or
borrowed resource values and optional KVC boxing remain separate work.
