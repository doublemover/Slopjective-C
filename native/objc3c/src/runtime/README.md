# ObjC3 Native Runtime Surface

`M251-D001` reserves `native/objc3c/src/runtime` as the canonical in-tree
runtime-library root for executable Objective-C 3 support.

Frozen surface:

- `objc3_runtime`
- `native/objc3c/src/runtime/objc3_runtime.h`
- `static`
- `objc3_runtime`
- `objc3_runtime_register_image`
- `objc3_runtime_lookup_selector`
- `objc3_runtime_dispatch_i32`
- `objc3_runtime_reset_for_testing`

Ownership boundary:

- the compiler owns source-derived metadata records and emitted IR/object
  payloads
- the runtime owns registration, lookup, and dispatch state once those payloads
  are ingested

`M251-D001` does not land the actual library skeleton or driver/link wiring.

`M251-D002` now instantiates that reserved surface and lands:

- `native/objc3c/src/runtime/objc3_runtime.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `artifacts/lib/objc3_runtime.lib` via `npm run build:objc3c-native`

`M251-D003` must preserve this surface while wiring the native driver/link path.

`M251-D003` now wires emitted-object consumers to the real archive without
changing the canonical runtime API surface:

- driver/manifest publication continues to expose the runtime archive path
- `scripts/check_objc3c_native_execution_smoke.ps1` links emitted objects
  against `artifacts/lib/objc3_runtime.lib`
- `native/objc3c/src/runtime/objc3_runtime.cpp` exports the compatibility
  bridge symbol `objc3_msgsend_i32`, which forwards to
  `objc3_runtime_dispatch_i32`

`M254-B002` extends the same runtime surface with live startup/bootstrap
semantics while preserving the canonical archive/header path:

- `objc3_runtime_register_image` now consumes a translation-unit identity key
  plus a strict positive registration ordinal
- duplicate translation-unit identity keys fail closed with status `-2`
- non-monotonic registration ordinals fail closed with status `-3`
- `objc3_runtime_copy_registration_state_for_testing` exposes committed state
  to the native runtime probe so partial commits are detectable

`M254-C001` does not extend the runtime library API. It freezes the lowering
boundary that will eventually materialize startup ctor roots, init stubs, and
registration tables from the emitted manifest:

- lowering contract `objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1`
- preserved ctor root `__objc3_runtime_register_image_ctor`
- preserved init-stub prefix `__objc3_runtime_register_image_init_stub_`
- reserved registration-table prefix `__objc3_runtime_registration_table_`
- no runtime API additions land in the freeze

`M254-D001` freezes the runtime-owned bootstrap API surface that later
registrar/image-walk and deterministic-reset issues must preserve:

- contract id `objc3c-runtime-bootstrap-api-freeze/m254-d001-v1`
- public header `native/objc3c/src/runtime/objc3_runtime.h`
- archive `artifacts/lib/objc3_runtime.lib`
- preserved entrypoints:
  - `objc3_runtime_register_image`
  - `objc3_runtime_lookup_selector`
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_copy_registration_state_for_testing`
  - `objc3_runtime_reset_for_testing`
- emitted startup invocation model
  `generated-init-stub-calls-runtime-register-image`
- image walk remains deferred to `M254-D002`
- deterministic reset expansion remains deferred to `M254-D003`

`M254-D002` lands the private registrar/image-walk bridge that keeps the public
runtime surface frozen while allowing emitted startup code to stage and walk the
registration table:

- private bootstrap header
  `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- private staging hook
  `objc3_runtime_stage_registration_table_for_bootstrap`
- private image-walk snapshot hook
  `objc3_runtime_copy_image_walk_state_for_testing`
- image-walk model
  `registration-table-roots-validated-and-staged-before-realization`
- selector-pool interning model
  `canonical-selector-pool-preinterned-during-startup-image-walk`
- realization staging model
  `registration-table-roots-retained-for-later-realization`

`M254-D003` expands that same private bootstrap surface with deterministic reset
and replay hooks for same-process runtime probes:

- private replay hook
  `objc3_runtime_replay_registered_images_for_testing`
- private reset/replay snapshot hook
  `objc3_runtime_copy_reset_replay_state_for_testing`
- reset lifecycle model
  `reset-clears-live-runtime-state-and-zeroes-image-local-init-cells`
- replay order model
  `replay-re-registers-retained-images-in-original-registration-order`
- image-local init-state reset model
  `retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay`
- bootstrap catalog retention model
  `bootstrap-catalog-retained-across-reset-for-deterministic-replay`

`M255-D001` then freezes the runtime-owned lookup/dispatch boundary that the
live lane-C call ABI now targets:

- contract id `objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1`
- canonical selector lookup symbol `objc3_runtime_lookup_selector`
- canonical dispatch symbol `objc3_runtime_dispatch_i32`
- canonical selector handle type `objc3_runtime_selector_handle`
- selector interning model
  `process-global-selector-intern-table-stable-id-per-canonical-selector-spelling`
- metadata-backed selector lookup tables remain deferred to `M255-D002`
- method-cache and runtime slow-path lookup remain deferred to `M255-D003`
- protocol/category-aware method resolution remains deferred to `M255-D004`
- the compatibility shim remains test-only evidence and is not the
  authoritative runtime-owned lookup/dispatch implementation

`M255-D002` then turns selector pools into a real runtime-owned selector table:

- contract id `objc3c-runtime-selector-lookup-tables/m255-d002-v1`
- selector pool registrations materialize stable IDs through
  `registered-selector-pools-materialize-process-global-stable-id-table`
- duplicate selector spellings inside one selector pool fail closed
- duplicate selector spellings across images merge onto one canonical stable ID
- `objc3_runtime_copy_selector_lookup_table_state_for_testing` and
  `objc3_runtime_copy_selector_lookup_entry_for_testing` expose deterministic
  snapshots for the emitted-selector-table proof surface
- unknown selector lookups stay dynamic until `M255-D003`

`M255-D003` then turns live dispatch into a method-cache plus deterministic
slow-path resolution surface:

- contract id `objc3c-runtime-method-cache-slow-path-lookup/m255-d003-v1`
- receiver normalization model
  `known-class-and-class-self-receivers-normalize-to-one-metaclass-cache-key`
- emitted class/metaclass records drive slow-path resolution through
  `registered-class-and-metaclass-records-drive-deterministic-slow-path-method-resolution`
- positive and negative cache entries are stored through
  `normalized-receiver-plus-selector-stable-id-positive-and-negative-cache`
- `objc3_runtime_copy_method_cache_state_for_testing` and
  `objc3_runtime_copy_method_cache_entry_for_testing` expose deterministic
  snapshots for the live method-cache proof surface
- unresolved or unsupported lookup continues to fall back through
  `unsupported-or-ambiguous-runtime-resolution-falls-back-to-compatibility-dispatch-formula`

`M255-D004` then extends that same runtime surface across protocol/category
metadata:

- category implementation records provide the next live lookup tier after class
  bodies under
  `class-bodies-win-first-category-implementation-records-supply-next-live-method-tier`
- adopted/inherited protocol method lists provide declaration-aware negative
  lookup evidence under
  `adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-resolution`
- probe and cache snapshots now record category/protocol traversal counts
- conflicting category/protocol outcomes fail closed through
  `conflicting-category-or-protocol-resolution-fails-closed-to-compatibility-dispatch`

`M265-D002` keeps that runtime helper boundary narrow while landing the first
live typed key-path runtime support for the runnable Part 3 tranche:

- optional sends and optional-member access still execute through
  `objc3_runtime_lookup_selector` plus `objc3_runtime_dispatch_i32`
- lowering still owns nil short-circuit semantics
- validated single-component typed key-path literals now contribute stable
  descriptor handles and retained `objc3.runtime.keypath_descriptors` payloads
  to a private runtime registry

`M265-D003` extends that proof across imported runtime surfaces: cross-module
link plans and imported semantic summaries must preserve the same optional/
key-path runtime packets so multi-image programs keep imported typed key-path
metadata visible after startup registration.
- private helper/probe entrypoints now expose validated key-path component
  counts and root-kind queries without widening the public runtime header
- full multi-component key-path evaluation remains intentionally deferred to
  later runtime work

`M256-D001` freezes the next runtime-owned object-model boundary that consumes
the emitted `M256-C003` realization records:

- contract id `objc3c-runtime-class-realization-freeze/m256-d001-v1`
- class realization model
  `registered-class-bundles-realize-one-deterministic-class-metaclass-chain-per-class-name`
- metaclass graph model
  `known-class-and-class-self-receivers-normalize-onto-the-metaclass-record-chain`
- category attachment model
  `preferred-category-implementation-records-attach-after-class-bundle-resolution`
- protocol check model
  `adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-runtime-checks`
- fail-closed model
  `invalid-bundle-graphs-category-conflicts-and-ambiguous-runtime-resolution-fail-closed`
- non-goals:
  - property/ivar storage realization
  - accessor synthesis
  - executable protocol-body dispatch
  - cross-image class coalescing beyond the current ordered image walk

`M257-D001` freezes the next truthful property/layout runtime boundary above the
lane-C accessor work:

- contract id `objc3c-runtime-property-layout-consumption-freeze/m257-d001-v1`
- runtime consumption model
  `runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery`
- allocator model
  `alloc-new-return-one-canonical-realized-instance-identity-per-class-before-true-instance-slot-allocation`
- storage model
  `synthesized-accessor-execution-uses-lane-c-storage-globals-pending-runtime-instance-slots`
- proof surface remains the same public ABI plus private registration/selector
  and method-cache snapshots
- non-goals:
  - true instance allocation
  - per-instance slot storage
  - reflective property registration

`M257-D002` upgrades the property/layout runtime from the D001 freeze into true
per-instance allocation:

- contract id `objc3c-runtime-instance-allocation-layout-support/m257-d002-v1`
- runtime consumption model
  `runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery`
- allocator model
  `alloc-new-materialize-distinct-runtime-instance-identities-backed-by-realized-class-layout`
- storage model
  `synthesized-accessor-execution-reads-and-writes-per-instance-slot-storage-using-emitted-ivar-offset-layout-records`
- proof surface stays on the same public ABI plus private registration,
  selector, realized-graph, and method-cache snapshots

`M256-D002` promotes that freeze boundary into a runtime-owned realized graph:

- contract id `objc3c-runtime-metaclass-graph-root-class-baseline/m256-d002-v1`
- runtime-owned realized graph model
  `runtime-owned-realized-class-nodes-bind-receiver-base-identities-to-class-and-metaclass-records`
- root-class baseline model
  `root-classes-realize-with-null-superclass-links-and-live-instance-plus-class-dispatch`
- fail-closed model
  `missing-receiver-bindings-or-broken-realized-superclass-links-fall-closed-to-compatibility-dispatch`
- proof surface now includes:
  - `objc3_runtime_copy_realized_class_graph_state_for_testing`
  - `objc3_runtime_copy_realized_class_entry_for_testing`

`M256-D003` extends that runtime-owned realized graph into live category
attachment and protocol conformance queries:

- contract id `objc3c-runtime-category-attachment-protocol-conformance/m256-d003-v1`
- category attachment model
  `realized-class-nodes-own-preferred-category-attachments-after-registration`
- protocol conformance query model
  `runtime-protocol-conformance-queries-walk-class-category-and-inherited-protocol-closures`
- fail-closed model
  `invalid-attachment-owner-identities-or-broken-protocol-refs-disable-runtime-attachment-queries`
- proof surface now includes:
  - `objc3_runtime_copy_protocol_conformance_query_for_testing`

`M256-D004` closes the smallest truthful executable object sample above that
same realized graph:

- contract id `objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1`
- execution model
  `canonical-object-samples-use-runtime-owned-alloc-new-init-and-realized-class-dispatch`
- probe split model
  `metadata-rich-object-samples-prove-category-and-protocol-runtime-behavior-through-library-plus-probe-splits`
- fail-closed model
  `metadata-heavy-executable-samples-stay-library-probed-until-runtime-export-gates-open`
- proof surface now includes:
  - `tests/tooling/fixtures/native/m256_d004_canonical_runnable_object_sample.objc3`
  - `tests/tooling/fixtures/native/m256_d004_canonical_runnable_object_runtime_library.objc3`
  - `tests/tooling/runtime/m256_d004_canonical_runnable_object_probe.cpp`
  - `objc3_runtime_copy_method_cache_entry_for_testing`

`M257-D003` adds private runtime reflection helpers above that same realized
property graph:

- contract id `objc3c-runtime-property-metadata-reflection/m257-d003-v1`
- registration model
  `runtime-registers-reflectable-property-accessor-and-layout-facts-from-emitted-metadata-without-source-rediscovery`
- query model
  `private-testing-helpers-query-realized-property-metadata-by-class-and-property-name-including-effective-accessors-and-layout-facts`
- live proof surface now includes:
  - `objc3_runtime_copy_property_registry_state_for_testing`
  - `objc3_runtime_copy_property_entry_for_testing`
  - `tests/tooling/runtime/m257_d003_property_metadata_reflection_probe.cpp`

`M272-D001` freezes the existing Part 9 runtime boundary before any wider live
fast-path work lands:

- contract id `objc3c-part9-runtime-fast-path-integration-contract/m272-d001-v1`
- runtime model
  `direct-llvm-call-sites-bypass-runtime-while-dynamic-opt-out-and-unresolved-selectors-stay-on-the-existing-method-cache-slow-path-and-fallback-surface`
- proof surface remains:
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_copy_method_cache_state_for_testing`
  - `objc3_runtime_copy_method_cache_entry_for_testing`
- cross-module provenance stays visible through retained direct import surface
  paths in the runtime link-plan artifact
- the live widening step remains `M272-D002`

`M272-D002` widens the private Part 9 runtime cache surface without reopening the public ABI:

- contract id `objc3c-part9-live-dispatch-fast-path-and-cache-integration/m272-d002-v1`
- registration-time runtime rebuild now seeds direct/final/sealed cache entries
- the private testing surface now exposes:
  - `fast_path_seed_count`
  - `fast_path_hit_count`
  - `last_dispatch_used_fast_path`
  - `last_fast_path_reason`
  - seeded-entry flags for direct/final/sealed intent
- unresolved selectors remain on the deterministic cached fallback path
