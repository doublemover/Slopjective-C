# Native inline anonymous objects

Source 2.223 / System 2.149 publishes immutable, trivially copyable inline
`constexpr` objects whose record types have no authored name. Their complete
record definitions, initializers, methods, generic members and nested record
members travel with the owning variable. Native internal linkage is preserved;
this does not invent an externally linked object symbol for an unnamed type.

Stable identities bind the variable's qualified name and each nested lexical
record/field path. Identical member names on distinct objects cannot merge.
Relocating provider inputs leaves the interface fingerprint unchanged. Structural
non-type template parameters preserve their authored `decltype(object)` spelling
instead of emitting Clang's unparseable diagnostic name for an anonymous record.
Published dependencies and SPI groups follow the owning object; missing exports
and private method dependencies cannot be hidden by printing a complete record.

Source 2.281 / System 2.207 publishes a generic inline closure object's pattern
independently of private instantiations. Public signatures bind the owning
object. A selected specialization binds its template arguments and callable
type, and dependency analysis checks its instantiated body for private lookup
targets. Consumers can instantiate the published pattern with their own types.
The updated identity binds the public pattern and its owning declaration.

The C++20 and C++23 native consumers instantiate namespace and static member
closure objects after provider removal. Private specializations leave published
bytes and fingerprints identical. A selected argument-dependent call to a
private helper is rejected during publication.

Source 2.282 / System 2.208 also admits `inline const` anonymous records with
constant initialization, including explicit `constinit`. Admission requires
nonvolatile storage and the same immutable, trivially copyable subobjects.
The canonical declaration retains native `constinit` and the exact unnamed
record owner across aliases and references. C++20 and C++23 source-free clients
link and read both forms. The `objc3.cxx-inline-closure-objects.v6` reader
requires the updated semantic identity and owner.

Admission bounds record ancestry to 32 contexts, owner discovery to 32 following
declarations, and immutable subobject inspection to 1,024 steps / 32 levels.
Mutable subobjects and nontrivial copy/destruction remain outside this lane.
Pointer members retain their normal native pointer semantics; this mechanism does
not prove external pointee lifetime or transitive immutability.

C++20/23 source-free consumers compile, link and execute; controls cover distinct
owners, nested/generic member access, structural arguments, relocation, export
closure, private dependencies, mutable/nontrivial exclusions and SPI import
admission. Matching archive, full profiles and native ARM acceptance remain
separate work. Production codec controls, twelve selected existing closure/factory
regression groups, 90 tooling checks (four archive-dependent skips) and fresh
maintained-patch reconstruction pass. Selected groups do not qualify the full
frontend catalogue.
