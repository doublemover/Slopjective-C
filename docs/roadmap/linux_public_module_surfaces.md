# Linux public module surfaces

Packages select their public native declarations with an explicit `exports`
policy. A selected root includes its supported members and redeclarations.
Unselected declarations stay in the implementation translation unit and do not
appear in the canonical interface or its semantic declaration identities.

```sh
objc3c provider.mm --objc3 --objcxx --arc \
  --emit-object provider.o --emit-interface Surface.objc3i \
  --module Surface --package surface-v1 \
  --export-declaration SurfaceValue --export-declaration surface_value \
  -- -std=c++23
```

An explicit name selects every eligible root with that exact Clang qualified
name, including an overload set or a same-name native tag/typedef pair. Public
records retain their supported fields; Objective-C interfaces retain their
supported members. Select types referenced by the public signatures as well as
the functions that use them. A signature cannot expose a type excluded from the
policy. Imported types require their existing resolved dependency identities.

Names are distinct, nonempty UTF-8 strings of at most 256 bytes, with no control
characters or spaces. A policy contains at most 256 names. A single `"*"` requests
the complete supported public surface and keeps normal unsupported-API
diagnostics. An omitted native CLI selection has the same complete-surface
behavior. Missing roots, duplicate names and a wildcard mixed with names are
errors. Rejected emission preserves prior object, interface and sidecar outputs.

## Source and immutable packages

Source package/workspace v4 and binary package/consumer v3 are hard cutovers.
Every source package and binary module requires an `exports` array. The source
builder passes it directly to the native compiler and records its canonical
sorted form in the installation inventory:

```json
{
  "schema": "objc3c.source-package.v4",
  "name": "fixture.SurfaceLeaf",
  "package": "surface-leaf-v1",
  "source": "module.mm",
  "language": "objective-c++",
  "standard": "c++23",
  "ownership": "arc",
  "provider_ownership": "arc",
  "exports": ["SurfaceValue", "surface_value"],
  "dependencies": []
}
```

The policy participates in the source build recipe and immutable content identity.
For a named selection, binary verification requires exact agreement with the
native sidecar's exported root names before invoking the consumer compiler.
The existing semantic-interface schema 2 describes the resulting declarations
without an extra projection of API metadata. Extension ABI 13 remains unchanged.
Rebuild previous source and binary package formats with the current SDK.

Native `@import` and `@Module.Path.Identifier` expose only the selected API.
Public dependency edges retain transitive re-exports. Both qualified and
unqualified lookup reject private declarations. Private C++ constructors,
templates and helpers may remain in an unselected implementation; the supported
public C++ data API remains bounded. Function bodies and initializers do not
contribute interface declarations. Native same-name record/typedef pairs resolve
as one canonical lookup type while keeping both semantic declaration identities.

## Ownership and incremental development

An ARC/MRC projection applies the export policy before validating its closed C
ABI. Its canonical producer proof contains exactly that selected API. Consumer
mode locks and native return-ownership requirements remain unchanged.

A private implementation edit rebuilds its provider object while preserving the
public fingerprint and reusing dependent compilations. Expanding the export
policy changes the fingerprint and rebuilds the dependent graph. Root selection
controls semantic module visibility. Native object symbol linkage follows its
ordinary C, Objective-C or C++ rules.

## Qualification and remaining scope

Installed qualification covers eight language/standard/ownership configurations
at O0/O2: Objective-C17 ARC/MRC, Objective-C++20/23 ARC/MRC and C++20/23. Sixteen
cases execute cold and reused graphs, a private implementation edit, a public
policy expansion and a relocated source-free binary consumer. The gate records
80 executions and 128 rejection checks. It also checks optimization-independent
fingerprints and failure reports after every concurrent case has joined.

Privileged SPI, availability metadata expansion, additional public C++ constructs
and module-owned linker/runtime naming require separate contracts. Concurrency
features retain their reserved status until their native execution and ownership
requirements are implemented and qualified.
