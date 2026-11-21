# Indirect family operations and exceptional retain claims

Source 2.178 / System 2.104 extends Strict System's manual lifetime checker to
bounded indirect foreign-family calls. Immutable local/global function slots,
conditional target sets, constant record fields and nested callback factories
preserve the native family operation. Every reachable target must agree on the
family and operation, with compatible physical and ownership signatures, before
a call grants a definite additional reference. Possible releases and scheduled
autoreleases remain effects even when only part of the target set is known.
Unknown family-shaped callbacks cannot manufacture a retain guarantee. Generic
opaque-pointer callbacks do not acquire a family contract from their shape.

A throwing retain grants its reference only on the normal CFG edge. Exceptional
replay preserves all earlier completed holds and possible consumption, including
consumption by the same call. Native MRC retain messages use the same rule;
branch and temporary-destruction edges retain their existing treatment. Deferred
foreign autorelease counts remain conservative until their pool ordering is
proved. This does not provide unbounded interprocedural ownership inference.

Addresses and decayed arrays embedded in native retainable-family records now
borrow their actual family owner, including nested record/array and explicit
dereference paths. Loading an unrelated pointer field does not borrow its
container's lifetime. Wrong-owner returns and uses after release reject; scalar
copies retain ordinary value semantics.

Published bodies require `objc3.manual-family-call-proofs.v1`, independently of
whether their own signature returns a borrowed value. Borrowed-result owners
advance to `objc3.borrowed-result-proofs.v13`. Both are hard reader cutovers;
the production catalogue, source emitter, identity, validator, codec and fuzz
dictionary agree. Missing, invented, older-reader and namespace-forged
capabilities reject.

Maintained controls cover C17/C++20/C++23, ARC/MRC and source/PCH: all 18 commands
pass. Four optimized native source-free configurations remove the provider,
import two consumers and check conditional/local/global/field/factory calls,
normal and exceptional counts, exact data use and one final destruction per
exercise. Negative consumers reject released loans and incomplete callbacks.
All 11 existing borrowed-pointer groups, 91 LLVM AST infrastructure controls
and the production codec pass; maintained patch replay matches the authoring
files byte-for-byte and verifies every manifest digest.
These focused development checks relink changed LLVM objects against retained
SDK libraries. Complete matching compiler/SDK qualification remains pending;
SDK 2.62 / ABI 21 remains the last accepted archive.
