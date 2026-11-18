# Native pointer resource views and weak class policy

Source 2.220 / System 2.146 retains known cleanup/resource storage behind native
immutable pointer bindings such as `View *const alias = &owner`. Local alias
chains, conditional target sets, scalar/member dereferences and returned reference
writes preserve the original storage cell. Known alternatives are retained when
another conditional arm is opaque. Only complete sets establish write cells or
read-only alias roles; an unknown arm does not erase a known managed arm.

Discovery shares 128 work units, a 32-level depth bound, 16 targets and an active
binding cycle set. Target/depth/work truncation is retained explicitly and causes
resource reads to fail closed. Mutable/retargeted pointer bindings, arbitrary
pointer arithmetic, globals and general interprocedural state remain outside
this closed view contract. It does not establish ownership of arbitrary pointers
or complete unqualified pointer/capture lifetime analysis.

The scope index treats proved immutable address bindings and pure getter calls
as reads, tracks writes through their complete target sets, and records pointee
escapes at unknown calls, including member-pointer calls. Pointer initializer
conditions still undergo normal effect/write/escape indexing. Static methods
accessed through an object pointer do not gain an instance receiver. Pure
boolean/status observations keep their existing resource-identity rules.

An opaque, open virtual, member-pointer or operator result from a receiver with
an explicit cleanup/resource storage contract cannot silently discard that known
contract. Closed proved observations and argument-only projections remain
independent. This is a conservative managed-receiver rule, rather than inference
of every unannotated producer's ownership semantics.

Native interface publication also retains
`objc_arc_weak_reference_unavailable`. The policy now participates in native
object policy admission, canonical printing and identity under the existing
required reader capability. Source-free clients retain normal Clang weak-reference
restrictions on both the annotated class and its subclasses while ordinary weak
and strong declarations remain valid. A dedicated native test reproduces the
preceding publisher's rejection before the repair.

All 504 source/PCH commands pass across the pointer suite and six adjacent
projection suites, covering C17/C++20/C++23, ARC/MRC and all four profiles.
Controls cover partial alternatives, reference and scalar writes, unknown-call
and condition effects, opaque/open/operator/member-pointer results, static
non-receivers and discovery bounds. The complete native isolation test passes,
including C/C++ awaited pointer views and reference writes. All six source-free
weak-policy configurations pass with the expected class/subclass restrictions;
the preceding publisher fails the dedicated regression. Fresh maintained-patch
application and 90 tooling checks pass, with four archive-dependent skips.
Matching SDK/archive and full-profile acceptance remain separate.
