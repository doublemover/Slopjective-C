# Linux implementation sprint after source 2.90

This sprint works across the PR's remaining implementation backlog before a
combined validation batch. Source changes target Linux Objective-C 3.0 as defined
by the specification. SDK 2.62 remains the accepted archive during development.

The first implementation wave advances concurrency **2.91**, System **2.16** and
keypath **2.6** together:

- Borrowed flow splits finite written local destinations and joins complete
  provenance, alias, owner and callable-effect facts. Conditional stores cannot
  erase loans on another possible path. Tracked array pointers retain exact
  signed constant offsets where representable.
- Independent ARC strong aliases can retain an acquired loan after source-owner
  rebinding. Definite identity checks, precise lexical lifetime and bounded alias
  relations preserve expiry when the last proven owner disappears. MRC/foreign
  counts and unknown/dynamic aliases retain their existing bounds.
- Raw native-frame callbacks adapt into structured C++ task groups with typed
  adoption and failure disposal. Explicit group callable spawning retains the
  closure through inner-frame destruction. Group-handle copies own independent
  leases to synchronized shared runtime state and carry an explicit Sendable
  review marker.
- Typed key paths admit named native C++ record values and record-field writeback
  through ordinary copying and destruction. Generated snapshots retain native
  exception cleanup and accessor rules. Borrowed/resource erasure and managed
  union traversal stay gated; nullable result-policy limits remain explicit.
- Native interface attributes retain allocator size/alignment, nonnull, format
  and format-argument payloads and supported Linux calling conventions. The
  declaration printer reconstructs convention bits stored only in callable
  types. Allocator/deallocator references bind the deallocator declaration and
  pointer parameter index, enforce export visibility and replay source-free.

Authored witnesses cover conditional writes and expiry, forward/reverse strong
aliases, nontrivial key-path values, lvalue/temporary subscript assignment and
throwing copy/setter cleanup, raw-frame
and task group callbacks, allocator/format fingerprint mutations and a provider
calling-convention boundary. Validation is in progress.

The remaining PR categories remain implementation obligations: dependent unnamed
C++ types/lookup and general standard-library publication; broader indirect,
borrowed-result, generic/lazy callable and exceptional ownership effects;
remaining abstract group/spawn forms; MRC/CF/init-family and managed-union
isolation; dependent/effectful/isolated and ownership-bearing key paths; the rest
of the specified language/library/interop and profile contracts. Native Linux
AArch64 qualification requires a native host. These categories are not closed by
admitting a narrower subset. Undefined nullable key-path result policies and the
Part 8 resource-union capability boundary are not speculative implementations.
