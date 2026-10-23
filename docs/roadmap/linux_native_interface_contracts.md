# Native ownership and isolation in Linux module interfaces

The semantic interface now keeps language contracts that an ABI-canonical Clang
type deliberately erases. Resource typedef contracts record the effective cleanup
function, family and converted sentinel; integer expressions normalize by value,
and external address sentinels use declaration identity plus offset/one-past
state. Nested function, pointer and array positions retain resource and borrowed
markers. Borrowed returns retain the owner parameter index, and noescape/nothrow
facts remain part of the declaration identity.

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

Authored witnesses cover source round trips, sentinel/alias identity, semantic
fingerprints and stripped-capability rejection. This implementation batch has
not been built or executed. C async frame interfaces across C/C++ physical ABI
views remain explicitly rejected pending their separate declaration contract.
Opposite-ownership projections retain their existing bounded C API rules.
