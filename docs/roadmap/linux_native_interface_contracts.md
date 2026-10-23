# Native ownership and isolation in Linux module interfaces

The semantic interface now keeps language contracts that an ABI-canonical Clang
type deliberately erases. Resource typedef contracts record the effective cleanup
function, family and converted sentinel; integer expressions normalize by value,
and external address sentinels use declaration identity plus offset/one-past
state. Nested function, pointer and array positions retain resource and borrowed
markers. Borrowed returns retain the owner parameter index, and noescape/nothrow
facts remain part of the declaration identity. Consumed parameters retain their
native ownership contract in both type positions and declarations, including MRC
source attributes; consuming method receivers and explicit method return ownership
are modeled too. Polished Objective-C method source retains parameter noescape
and consumption attributes inside their type parentheses. Unmodeled public
parameter attributes are rejected instead of being silently removed, including
attributes that introduce hidden ABI arguments.

Actor, effective nonisolated accessor, executor, unchecked Sendable and structured
task-boundary metadata populate the existing isolation identity fields. Typed key
paths keep stable Root/Value/writability identities while the dependency scanner
visits their public types rather than compiler-private record machinery.

Canonical interface source preserves typedef and borrowed type sugar. Native
attribute printers preserve the modeled contracts; source reparse and AST import
still compare the complete declaration identity. New required semantic names
protect ownership, isolation and typed key paths in encoders, decoders, immutable
packages, module resolution and source imports. Removing a required semantic
while retaining its contract is rejected. Resource cleanup functions must be
externally visible and available through the export/dependency graph; private
storage sentinels and unrepresentable constants are rejected explicitly.

Async signatures now carry a separate position-tagged physical entry identity:
`c-frame-v1` or `cxx-task-v1`. The canonical native function type retains an
`AsyncFrame` discriminator and prints frame declarations with the persistent
`__objc3_async_frame` spelling, including indirect callbacks, Blocks and methods.
Interface collection recursively records nested callable positions in arrays,
records, native carriers and key paths. Logical async effects alone never select
the importing source language's physical return carrier. The required semantic
`objc3.async-entry-abi.v1` protects this distinction in fingerprints, source
reparse, AST import, dependency records and immutable packages.

Semantic interface schema **2.1** is a hard cutover: the ABI identity field is
required, and schema 2.0 artifacts must be rebuilt. Missing, malformed, unknown
or duplicate-position identities and stripped required semantics are rejected.
Canonical frame declarations include the trusted SDK runtime header and bind
its exact header semantic. This does not relax the module resolver's existing
source-language/ownership profile compatibility rules; mixed native AST import
and C++ frame-await bridges are developed separately from profile projections.

Authored witnesses cover source round trips, sentinel/alias identity, semantic
fingerprints, ABI tampering, recursive callback contracts, mixed-language AST
type import and stripped-capability rejection. This implementation batch has
not been built or executed.
Opposite-ownership projections retain their existing bounded C API rules.

Native extension ABI **21** accompanies the new serialized function effects,
capture fields and attributes. Older ABI 20 PCH/PCM inputs are rejected before
record decoding, independently of ordinary PCH validation switches. Runtime
frame/task-service record versions remain unchanged; all compiler/header/object/
interface/package inputs must be rebuilt together.
