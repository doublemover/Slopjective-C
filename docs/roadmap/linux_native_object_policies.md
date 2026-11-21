# Native Objective-C API policies

Source 2.196 / System 2.122 publishes native Objective-C declaration policies
through source-free C17, C++20/C++23 and ARC/MRC interfaces. The admitted contracts
are explicit method families, designated initializers, required super calls,
restricted subclassing, explicit property definitions, explicit protocol
implementation, non-runtime protocols, boxable records, precise local lifetimes,
non-lazy classes and runtime-visible classes.

Canonical printing retains inherited contracts, coalesces duplicate flags and
uses GNU spelling across C and C++ attribute forms. Method-family identity binds
the exact `none`, `alloc`, `copy`, `init`, `mutableCopy` or `new` payload. Every
policy participates in typed native declaration/local-expression identity. A new
`objc3.native-object-policies.v1` reader promise is mandatory; production decoder
and contract admission reject erased capabilities and invalid owner kinds.
Unpublished implementation bodies do not create a local-policy promise.

Native AST import preserves the interface's designated-initializer state as well
as the individual method attributes. Complete interface/protocol merges compare
policy-bearing declarations and common member contracts before accepting an
existing definition, including both method kind and selector spelling. Selector
identifiers belong to their AST context, so cross-context comparisons use their
actual spelling instead of foreign selector pointers. Boxable record contracts
also participate in structural equivalence. Forward declarations can acquire a
complete imported contract; conflicting definitions cannot erase one.

The native witness removes the provider before two independent direct consumers
and two source-directed consumers.
Controls cover ordinary property/message execution, explicit new-family object
retirement, eager class startup, source-free native boxing and rejected
subclassing, missing super calls, initializer delegation, property definitions,
explicit protocol implementation and runtime access to a non-runtime protocol.
An ARC precise-lifetime witness drops the last external owner inside a callback
and observes that the local hold survives through the observation before exact
final destruction. MRC executes its explicit source-owned release path.

AST controls destroy the provider context before querying retained contracts and
all six method families; conflicting merges are exercised in both directions.
They also canonically print and reparse the imported graphs. Attribute position
selection handles an imported definition completing a forward declaration from
an independently rooted file, without asking SourceManager to order unrelated
translation-unit roots. Object policies print once in their valid declaration
position. The Objective-C++ parser admits the `new` family enum operand while
preserving ordinary C++ allocation-expression parsing.
Metadata controls remove reader promises, forge owner kinds and rehash altered
policy source while preserving the old semantic identity.

All six optimized source-free C17/C++20/C++23 × ARC/MRC policy configurations,
72 source/PCH commands across all four profiles, all 109 AST controls, all 17
import groups, production codec and 16 tooling checks pass. Six adjacent native
object-expression configurations and 18 publication groups pass as well. Changed
LLVM translation units compile as C++17 with warnings treated as errors; fresh
patch application reproduces the authored sources and all manifest digests match.

These are native Linux API/publication contracts. They do not add a Foundation
boxing runtime, qualify another operating system or establish complete property,
metaprogramming, foreign-interoperability or profile conformance. Runtime layouts,
extension ABI 24 and interface schema 2.2 are unchanged. Matching SDK/archive and
native-host acceptance remain pending.
