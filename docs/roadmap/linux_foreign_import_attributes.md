# Native foreign importer contracts

Source implementation 2.87 preserves Swift importer names, async/error
conventions, private-import markers, newtypes, bridge metadata and explicit Swift
attribute payloads in semantic interfaces. Declaration and parameter contracts
contribute to canonical identity; changing a payload invalidates dependent
interfaces. Source positions do not contribute to identity.

`objc3.foreign-import-attributes.v1` requires explicit capability admission and a
declaration owner. Namespace markers and missing capability declarations fail
with `O3IFC096`. Native declaration printing retains the supported attributes on
definitions and redeclarations. ABI-specific Swift calling-convention attributes
require their own contracts and are not covered by this capability.

This work preserves native importer metadata. It does not establish Swift
compiler/runtime interoperability or complete Objective-C 3.0 profile conformance.
Combined replay, source-free package and sanitizer checks remain pending. SDK
2.62 remains the accepted archive.
