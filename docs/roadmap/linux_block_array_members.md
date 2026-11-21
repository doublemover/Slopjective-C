# ARC Block array member initialization and cleanup

Historical System 2.47 receipt. The [System 2.48 span](linux_computed_block_arrays.md)
extends nested/computed values and supersedes v4 admission with v5.

System source 2.47 adds per-element copy registration for bounded one-dimensional
ARC Block member arrays. Each successful strong-slot copy advances the initialized
prefix; a failed copy cleans that prefix and preceding completed subobjects,
without registering the destination element or the complete field. Weak slots
initialize without a strong copy operation and retain their native cleanup.

Explicit slot loads, null values, omitted-element fillers and zero/default
initialization are supported. Empty arrays perform no element operation. Singleton
and longer arrays keep distinct prefix boundaries. Destruction uses a private
nonthrowing release group with the exact element count; this adds no public AST
layout or runtime ABI. Constant nested arrays may be destroyed, while nested
construction and counts above 64 retain explicit proof gates.

The primitive copy/capture predicate is unchanged. Captured literals and effectful
initializer expressions retain their full-expression temporary gates. Read/write
and callable mutation contracts outside the admitted slot initializer recipe
remain tracked separately.

Publication makes a hard cutover to `objc3.native-subobject-cleanup.v4` and matching
identity markers. The shared production reader catalog and fuzz dictionary use
that version; prior versions are not retained. Source-free replay includes full
and null-filled Block array fields in C++20/23 ARC/MRC.

All 66 affected compiler cases, four new source-free profiles, thirteen adjacent
frontend groups, 40 AST controls and twelve ASan/UBSan native configurations / 42
commands pass. AST paths verify exact prefixes at every copy failure and
zero/one/many boundaries. Native faults are armed after parameter retention and
fail copies 1, 2 and 3, plus constructor/destructor bodies. Exact owner counts,
weak storage, deferred ordering and sparse/default-null fields are checked.
Codec/isolation controls and forty exact production-decoder replays also pass.
Nullable-receiver analysis includes hidden scalar/array copy failures before a
constructor-body assignment; handler refinement remains path-sensitive.

Extension ABI 24, native frame 1, runtime services 4 and schema 2.2 remain unchanged.
These are development-source checks. One replacement matched SDK and installed
package qualification follow the remaining implementation sprint.
