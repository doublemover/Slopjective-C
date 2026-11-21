# Actor receiver bindings and immutable aliases

Source 2.185 / System 2.111 preserves the current actor's receiver identity in
both ARC and MRC. An actor-isolated instance entry has an immutable, nonnull
`self`, including initializing and consuming entries. Replacing that slot cannot
silently turn a same-actor operation into a call on another actor. Explicitly
nonisolated initializers retain ordinary receiver replacement, including
`self = [super init]`; ordinary nonactor receiver rules remain native.

The shared receiver proof admits automatic const bindings rooted in that exact
`self`, including nested aliases, readonly C++ references, exact address/load
pairs and readonly pointer cells. Both conditional alternatives must prove the
same actor; comma expressions and selected C generic/choose expressions retain
native evaluation. A readonly unsafe view is admitted only when its exact
current receiver is proven; this grants no ownership or general Sendable promise.
Mutable, volatile, weak, static/TLS, unknown and mixed-actor bindings do not prove
current-actor identity. The proof has one 128-node budget, depth 32 and at most
eight pointer loads, with cycle rejection. Exhaustion retains hop requirements.
Messages, ivar access and property reads/writes share this proof.

Nonisolated actor `instancetype` results retain their logical actor reference
for Sendable checking. An ordinary erased `id` result still requires its own
Sendable contract. Native AST import copies the checked implicit `self`/`_cmd`
declarations rather than reconstructing them before enclosing actor attributes
arrive. Const/nullability, consumed and pseudo-strong ownership, and references
from imported bodies remain attached to the exact imported parameters.

Source-free actor class/member identities require
`objc3.actor-receiver-bindings.v1`. Older readers, missing capabilities and
markers on unrelated declaration kinds reject. Canonical source replay retains
actor declarations, explicit nonisolation and native method contracts after
provider source removal.

Focused evidence includes 72 source/PCH commands across C17/C++20/C++23,
ARC/MRC and all four profiles; native package controls cover C++20/23 ARC/MRC
with independent consumers, provider removal, actor state access through the
admitted aliases, real executor hops and resumption on the original executor.
All 98 LLVM AST controls pass, together with the complete isolation, direct-import,
source-bridge and codec suites, the selected isolation-publication group and
26 tooling checks (one optional skip). AST controls exercise receiver types/ownership and exact body-reference identity
through import after destroying the source AST. Codec controls reject older or
inconsistent claims and forged declaration owners.

This span does not claim complete Strict Concurrency enforcement, unrestricted
alias analysis, effectful key paths or a freshly accepted SDK. Matching source
SDK/ABI 24 and native-host qualification remain separate delivery work; SDK
2.62 / ABI 21 remains the accepted archive until replacement gates pass.
