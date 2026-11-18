# Native metaprogramming storage

Source 2.218 / System 2.144 adds optional `ivars` programs to locked manifest
entries and the `objc3.meta.storage.v1` host capability. Programs use the same
pure queries, instructions, fingerprinting and explicit lock model as declaration
and implementation programs. An entry can synthesize storage alone, or combine
it with methods and protocols. `declarations` and `implementations` remain
required arrays; `ivars` is an optional array.

A stored property behavior can loop over its selected property and emit an ivar
such as `${member.type} __meta_${member.name};`, then implement its getter and
setter using that storage. The package supplies the bodies and ownership policy;
this does not assume a scalar setter is an owning object setter. Native Clang
checks declared types, ownership, collisions, access, actor isolation and accessor
dispatch/executor requirements.

The parser appends generated private ivars after authored ivars, before field
layout is finalized. It preserves root-class `isa` placement and authored field
order. When the interface has no ivar block, it creates one owned in-memory
block. Generated fields are excluded from the authored query model, so they do
not recursively become expansion inputs. Multiple properties compose in request
order; duplicate generated/authored storage is diagnosed by normal Sema.

Storage, declarations and implementations share the existing interpreter work
and output limits. The class output bound includes inserted separators. Token
validation rejects unmatched/crossed delimiters and nesting beyond 128 levels,
in addition to preprocessing, recursive requests and container escape checks.
Expanded private layout and provenance appear in the ordinary canonical
interface; downstream consumers do not load the generating packages.

The direct-call storage lane also exposed missing publication support for
`objc_direct_members`. Native policy admission and canonical printing now retain
that class-wide default, including property accessor dispatch, under the existing
required native-object policy contract. It is not silently dropped during import.

All 24 storage configurations pass across C17/C++20/C++23 and ARC/MRC, covering
dynamic, direct and actor accessors, interfaces with and without authored ivars,
root-class layout, source-free imports and generated/authored collisions. The 18
non-actor configurations execute linked native providers and source-free consumers
and return 42, including direct dispatch and an authored-field canary. Actor
storage passes object emission and awaited source-free checking. The complete
expanded metaprogramming test, package/program/CLI controls, fresh maintained-patch
application and 90 tooling checks pass; four archive-dependent tooling cases are
skipped. General
protocol/category behavior application and optional portable derives remain
separate work; no full-profile or matching SDK acceptance is claimed.
