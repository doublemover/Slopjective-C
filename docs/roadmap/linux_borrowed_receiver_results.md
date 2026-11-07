# Receiver-owned native getter results

System source 2.36 models `objc_returns_inner_pointer` as a receiver-owned loan
under Strict System, including ordinary pointer returns and native C++ references.
Message sends and property reads use the existing owner snapshot, alias and
lifetime-pin machinery. Getter bodies must return a view of `self` or independently
stable storage; a separate object cannot satisfy the receiver contract. Scalar
copies from a referenced byte have independent values, while reference aliases,
addresses and writes retain and check the original address lifetime.

Explicit instance and stable Class bindings participate in the same proof.
Superclass dispatch uses the current `self` identity. Its arguments must be
bound independently when they have effects, because the source CFG has no
separately evaluated super receiver snapshot. Unproved class/temporary receivers,
expired owners, escaping views and conflicting formal/receiver owner annotations
remain diagnosed. Existing ordinary-send nullability rules continue to apply.
Known nil pointer results do not acquire a fabricated ownership relationship.

Native async methods cannot promise a receiver-owned result. Actor/executor
crossings also reject these views independently of the raw pointer's scalar
Sendable classification: copy to independent owned storage inside the executor.
The crossing gate applies in every profile, including property accessors. Same
isolation-domain synchronous operations retain the ordinary lifetime contract.

Source-free methods, properties and published expressions preserve the native
attribute and its ownership identity under
`objc3.borrowed-receiver-results.v1`. Codec, import and module readers require
exact identity/capability agreement. Older readers and stripped capability claims
are rejected. Physical extension ABI 24 and native service/frame layouts remain
unchanged.

Controls cover C17/C++20/23, ARC/MRC, source/PCH, actual getter bodies, property
reads, wrong owners, superclass dispatch, pointer/reference expiry, pinned views,
operand snapshots, scalar copies and conflicting attributes. Separate crossing
controls run Core, Strict, Strict Concurrency and Strict System, testing raw
pointer messages/properties and native async declarations. Source-free controls
exercise replay, relocation, semantic admission and expired consumers. The
installed catch-binding native fixture reads actual payloads through both direct
views and native getters, with C++ references and exact destruction after pools.
All forty-two new compiler commands and all forty-two affected compiler cases
pass. Six source-free profiles and twelve ASan/UBSan native configurations /
forty-two commands pass. Publication also repairs declaration polishing so the
native attribute survives both written and inherited contracts. Five adjacent
frontend groups and the codec/isolation controls also pass.

Effectful/resource key paths, wider native managed-cleanup CFG registration,
open callable and global/interprocedural ownership precision, complete profile
conformance and final matched SDK delivery remain separate roadmap work.
