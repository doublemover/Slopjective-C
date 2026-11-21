# Native owned-union key paths

Source 2.144 / System 2.70 / keypath 2.12 extends native key paths to C++ unions
with explicit, nondeleted, nonthrowing destruction, including active arms with
strong, weak and Block fields. Native constructors and destructors retain the
active-arm contract through whole-value copies, enclosing records, reads and
nested field writeback. Destructor exception specifications are resolved before
admission, including user-provided destructors without an explicit `noexcept`.

Field paths have the ordinary C++ active-member precondition. The compiler does
not invent a discriminator or implicit union cleanup. A native explicit owner's
by-value subobject region can traverse a resource-containing arm to a scalar
field; a new object accessor starts a separate region. Borrowed storage, raw
resource extraction, missing native union lifetime contracts and throwing
union destructors remain rejected. This does not claim implicit annotated-union
cleanup under Part 8 §8.3.6.2.

Writeback retains one property receiver, constructs an owned native snapshot,
updates its selected field and calls the setter. Getter, snapshot-copy,
setter-argument and setter failures use ordinary native cleanup and preserve the
stored value until the setter commits. Nullable routes skip accessor work after
evaluating independent operands. Copies preserve the selected native arm and
managed field ownership; partially copied arms release completed fields.

The hard cutover `objc3.keypath-resource-values.v2` governs whole native unions,
resource owners and published scalar paths through those owners. Declaration-only
signatures and published bodies retain exact capability requirements. Older v1
readers, missing/invented claims, relocation and source-free consumers have
controls. There is no change to descriptor ABI v2, extension ABI 24, native frame
1, runtime services 4 or schema 2.2.

All 30 dedicated compiler commands, 60 AST controls, two new publication profiles,
580 production decoder replays and eight ASan/UBSan native source/PCH
configurations / sixty commands pass. The native matrix covers C++20/23,
ARC/MRC and disabled copy elision, explicit resource and managed unions,
whole/enclosing/field operations, weak/Block fields and injected copy/setter
failures. The [receipt](../support/linux_native_union_keypaths_progress.json)
retains the prepared source identity. Current broad consolidation and one matched
SDK follow remaining implementation.
