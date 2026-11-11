# Incomplete C++ template declarations and shared admission

The [frontend 2.120 span](linux_incomplete_specializations.md) now includes
opaque partial, explicit and member specializations and native standard-library
customizations. Current admission requires incomplete-declaration v2. The v1
capability and narrower boundaries below describe the original 2.119 span.

Frontend source 2.119 publishes primary class-template declarations and external
variable-template declarations in C++20/23. Class heads retain parameter kinds,
defaults, constraints and nominal ABI ownership without claiming a layout or
member definition. External data declarations preserve linkage and TLS identity;
they do not define or zero-initialize storage.

Opaque member templates are retained inside both ordinary records and enclosing
class templates. Later definitions replace declaration-only identities while
preserving inherited defaults and ABI ownership. Changed defaults alter the
semantic contract without changing the owning symbol. Private type/value defaults
remain dependency errors. Incomplete partial specializations keep their separate
admission gate.

The new required semantic is `objc3.cxx-incomplete-template-declarations.v1`.
Its marker follows the owning declaration graph, including enclosing ordinary
records. Decoder and contract validation require the matching capability and
admitted owner kind. Missing, invented, unavailable and wrongly owned capability
claims are rejected. Fully completed graphs lose declaration-only markers.

Production emission, projection, artifact reads, module resolution and AST import
now share one capability catalog. This removes five duplicate tables and fixes a
concrete importer omission of the already implemented C inline definition
capability. The defensive decoder fuzz runner uses the same production catalog,
so accepted source families reach its decode/encode/replay properties.

Two new source-free profiles and eight adjacent frontend groups pass, including
relocation, replay, later definitions, opaque nested heads, TLS, default identity
and negative dependency controls. Codec and isolation units pass. A registered
CTest integration builds real C-inline and incomplete-template artifacts, sends
them through the production fuzz runner, repeats forty exact replays and rejects
changed source bytes and unknown capabilities under ASan/UBSan. Two instrumented
C++20/O0 and C++23/O2 package-declaration witnesses also pass.

The native package fixture now includes opaque pointer identities and shared/TLS
variable-template specializations with exact provider/consumer address and value
checks. Its installed, source-removed execution remains part of final matched SDK
consolidation. This frontend-only span changes neither extension ABI 24 nor the
native frame/service ABIs or semantic schema 2.2. Broader dependent lookup,
incomplete specialization forms, package/profile obligations and final SDK
qualification remain in the roadmap.
