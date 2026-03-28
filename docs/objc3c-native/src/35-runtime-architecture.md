<!-- markdownlint-disable-file MD041 -->

## Runtime Execution Architecture (Current)

The live executable path is a single compile-to-runtime pipeline. Later runtime
closure work must extend this path, not bypass it.

## Working Boundary

- compiler-owned compile path:
  - `scripts/build_objc3c_native.ps1`
  - `scripts/objc3c_native_compile.ps1`
  - `native/objc3c/src/main.cpp`
  - `native/objc3c/src/driver/objc3_driver_main.cpp`
  - `native/objc3c/src/driver/objc3_compilation_driver.cpp`
  - `native/objc3c/src/driver/objc3_objc3_path.cpp`
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- runtime-owned installation and execution path:
  - `native/objc3c/src/runtime/objc3_runtime.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- authoritative emitted artifacts:
  - `<prefix>.obj`
  - `<prefix>.ll`
  - `<prefix>.manifest.json`
  - `<prefix>.runtime-registration-manifest.json`
  - `<prefix>.runtime-registration-descriptor.json`
  - `<prefix>.compile-provenance.json`
- validation-owned proof path:
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `scripts/check_objc3c_execution_replay_proof.ps1`
  - `scripts/check_objc3c_native_execution_smoke.ps1`
  - `scripts/objc3c_runtime_launch_contract.ps1`
  - `scripts/shared_compiler_runtime_acceptance_harness.py`

## Execution Flow

1. `scripts/build_objc3c_native.ps1` publishes `artifacts/bin/objc3c-native.exe`
   and `artifacts/lib/objc3_runtime.lib`.
2. `scripts/objc3c_native_compile.ps1` or the native executable drives the live
   `.objc3` compile path through driver, lowering, IR emission, and object
   emission.
3. The compile path emits the object, LLVM IR, registration manifest, and
   compile provenance as one coupled artifact set.
4. The runtime installs emitted image state through
   `objc3_runtime_register_image`, resolves selectors through
   `objc3_runtime_lookup_selector`, and executes calls through
   `objc3_runtime_dispatch_i32`.
5. Acceptance and replay proof only count when the emitted object, registration
   manifest, and linked probe all come from that same compile path.

## State Publication Surface

- front-door emitted surface:
  - `<prefix>.manifest.json`
  - key: `runtime_state_publication_surface`
- coupled runtime-owned surface:
  - `<prefix>.runtime-registration-manifest.json`
- required compile-owned artifacts:
  - `<prefix>.obj`
  - `<prefix>.ll`
- required public runtime ABI boundary:
  - `objc3_runtime_register_image`
  - `objc3_runtime_lookup_selector`
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_reset_for_testing`

The compile manifest is the authoritative front-door runtime state publication
surface. It must point at the coupled registration manifest, emitted object and
IR artifacts, the runtime archive path, the registration entrypoint, the runtime
state snapshot symbol, and the published descriptor counts.

## Bootstrap Registration Source Surface

- authoritative compile-manifest key:
  - `runtime_bootstrap_registration_source_surface`
- coupled emitted artifacts:
  - `<prefix>.manifest.json`
  - `<prefix>.runtime-registration-manifest.json`
  - `<prefix>.runtime-registration-descriptor.json`
  - `<prefix>.obj`
  - `<prefix>.ll`
- source inputs it composes:
  - `objc_runtime_registration_descriptor_image_root_source_surface`
  - `objc_runtime_registration_descriptor_frontend_closure`
  - `objc_runtime_translation_unit_registration_manifest`
  - `objc_runtime_bootstrap_lowering_contract`

This is the authoritative bootstrap registration source audit boundary. It
freezes the emitted registration descriptor artifact, image-root identity,
registration entrypoint, constructor root, translation-unit identity key, and
registration order ordinal as one coupled compile output instead of leaving
downstream work to reconstruct that source of truth from separate manifest
fragments.

## Bootstrap Lowering And Registration Artifact Surface

- authoritative compile-manifest key:
  - `runtime_bootstrap_lowering_registration_artifact_surface`
- authoritative composed source inputs:
  - `objc_runtime_bootstrap_lowering_contract`
  - `objc_runtime_translation_unit_registration_manifest`
  - `objc_runtime_startup_bootstrap_semantics`
  - `objc_runtime_registration_descriptor_frontend_closure`
- authoritative emitted artifacts:
  - `<prefix>.manifest.json`
  - `<prefix>.runtime-registration-manifest.json`
  - `<prefix>.runtime-registration-descriptor.json`
  - `<prefix>.obj`
  - `<prefix>.ll`
- authoritative lowered registration-descriptor fields:
  - `constructor_init_stub_symbol`
  - `bootstrap_registration_table_symbol`
  - `bootstrap_image_local_init_state_symbol`
  - `bootstrap_registration_table_layout_model`
  - `bootstrap_image_local_initialization_model`
  - `bootstrap_registration_table_abi_version`
  - `bootstrap_registration_table_pointer_field_count`
  - `translation_unit_registration_order_ordinal`
- authoritative lowered symbol fields:
  - `constructor_root_symbol`
  - `init_stub_symbol_prefix`
  - `registration_table_symbol_prefix`
  - `image_local_init_state_symbol_prefix`
  - `registration_entrypoint_symbol`
- authoritative loader-table freeze fields:
  - `registration_table_layout_model`
  - `registration_table_abi_version`
  - `registration_table_pointer_field_count`
- authoritative emission-state fields:
  - `constructor_root_emission_state`
  - `init_stub_emission_state`
  - `registration_table_emission_state`
  - `bootstrap_ir_materialization_landed`
  - `image_local_initialization_landed`

This is the authoritative bootstrap lowering and registration artifact
boundary. It freezes the lowered constructor-root/init-stub/registration-table
symbol family and loader-table ABI facts against the emitted manifest, object,
and IR artifacts instead of leaving later loader-table work to infer those
details from the semantic packet alone. The registration descriptor must now
carry the exact derived loader-table symbols/layout that appear in the emitted
LLVM IR, and acceptance must fail if the coupled `module.ll` stops lowering the
constructor-root to loader-table edge.

## Multi-Image Startup Ordering Source Surface

- authoritative compile-manifest key:
  - `runtime_multi_image_startup_ordering_source_surface`
- authoritative composed source inputs:
  - `objc_runtime_bootstrap_legality_semantics`
  - `objc_runtime_bootstrap_failure_restart_semantics`
  - `objc_runtime_bootstrap_api_contract`
  - `objc_runtime_bootstrap_reset_contract`
  - `objc_runtime_bootstrap_registrar_contract`
  - `objc_runtime_bootstrap_archive_static_link_replay_corpus`
- authoritative live proof path:
  - fixture: `tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3`
  - probe: `tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp`
  - acceptance command: `python scripts/check_objc3c_runtime_acceptance.py`
  - public workflow command: `python scripts/objc3c_public_workflow_runner.py validate-runtime-architecture`
- authoritative runtime fields:
  - `last_rejected_module_name`
  - `last_rejected_translation_unit_identity_key`
  - `next_expected_registration_order_ordinal`
  - `last_successful_registration_order_ordinal`
  - `last_rejected_registration_order_ordinal`
- fail-closed diagnostic statuses:
  - duplicate translation-unit identity key: `-2`
  - out-of-order registration ordinal: `-3`
- diagnostic models:
  - duplicate install rejection publishes rejected module, identity key, and ordinal without advancing runtime state
  - out-of-order install rejection publishes rejected module, identity key, and ordinal without advancing runtime state

This is the authoritative multi-image startup ordering source boundary. It
freezes the duplicate-registration policy, monotonic registration-order model,
restart/replay symbols, duplicate/out-of-order rejection diagnostics, runtime
header paths, and the live installation lifecycle proof command surface as one
coupled artifact instead of leaving later bootstrap work to infer ordering
truth from scattered semantic and runtime-side reports.

## Object-Model Realization Source Surface

- authoritative compile-manifest key:
  - `runtime_object_model_realization_source_surface`
- authoritative composed source inputs:
  - `objc3c.executable.realization.records.v1`
  - `objc3c.runtime.class.realization.freeze.v1`
  - `objc3c.runtime.metaclass.graph.root.class.baseline.v1`
  - `objc3c.runtime.category.attachment.protocol.conformance.v1`
  - `objc3c.runtime.canonical.runnable.object.sample.support.v1`
- authoritative runtime boundary:
  - public ABI:
    - `objc3_runtime_register_image`
    - `objc3_runtime_lookup_selector`
    - `objc3_runtime_dispatch_i32`
  - private query boundary:
    - `objc3_runtime_copy_realized_class_graph_state_for_testing`
    - `objc3_runtime_copy_realized_class_entry_for_testing`
    - `objc3_runtime_copy_protocol_conformance_query_for_testing`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_provider.objc3`
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_consumer.objc3`
    - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
    - `tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
    - `tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp`

This is the authoritative object-model realization source boundary. It freezes
the emitted class/metaclass/protocol/category realization contracts, coupled
registration artifacts, and the live runtime query/dispatch entrypoints that
later realization and reflection work must consume. Downstream work must extend
this emitted surface rather than rederiving realized graph truth from
sidecars, stale milestone notes, or synthetic probes.

## Reflection Query Surface

- authoritative compile-manifest key:
  - `runtime_reflection_query_surface`
- authoritative composed source inputs:
  - `runtime_object_model_realization_source_surface`
  - `objc3c.runtime.dispatch_accessor.abi.surface.v1`
  - `objc3c.runtime.property.metadata.reflection.v1`
  - `objc3c.runtime.backed.object.ownership.attribute.surface.v1`
- authoritative query API boundary model:
  - `private-testing-snapshots-over-runtime-owned-realized-class-property-and-protocol-metadata-with-no-public-reflection-abi`
- authoritative private query symbols:
  - `objc3_runtime_copy_realized_class_graph_state_for_testing`
  - `objc3_runtime_copy_realized_class_entry_for_testing`
  - `objc3_runtime_copy_property_registry_state_for_testing`
  - `objc3_runtime_copy_property_entry_for_testing`
  - `objc3_runtime_copy_protocol_conformance_query_for_testing`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
    - `tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3`
    - `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3`
  - probes:
    - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
    - `tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp`
    - `tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp`

This is the authoritative reflection-query source and API boundary. It freezes
the fact that current reflection queries are runtime-backed, compile-coupled,
and exposed only through the private testing snapshots over realized
class/property/protocol metadata. Downstream work must extend this surface
instead of inventing a public reflection ABI or recovering reflection answers
from source-side manifests alone.

## Realization And Lookup Semantics Surface

- authoritative compile-manifest key:
  - `runtime_realization_lookup_semantics_surface`
- authoritative composed source inputs:
  - `runtime_object_model_realization_source_surface`
  - `runtime_reflection_query_surface`
  - `objc3c.runtime.dispatch_accessor.abi.surface.v1`
- authoritative runtime boundary:
  - public ABI:
    - `objc3_runtime_register_image`
    - `objc3_runtime_lookup_selector`
    - `objc3_runtime_dispatch_i32`
  - private lookup query boundary:
    - `objc3_runtime_copy_selector_lookup_table_state_for_testing`
    - `objc3_runtime_copy_selector_lookup_entry_for_testing`
    - `objc3_runtime_copy_method_cache_state_for_testing`
    - `objc3_runtime_copy_method_cache_entry_for_testing`
    - `objc3_runtime_copy_realized_class_entry_for_testing`
    - `objc3_runtime_copy_protocol_conformance_query_for_testing`
- frozen semantic models:
  - lookup resolution order:
    - `seeded-cache-then-live-class-chain-then-attached-category-and-protocol-checks-then-deterministic-fallback`
  - selector materialization:
    - `metadata-selectors-materialized-at-registration-and-dynamic-misses-interned-at-first-lookup`
  - unresolved selector behavior:
    - `negative-cache-entry-preserved-and-deterministic-fallback-returned`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3`
    - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
    - `tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3`
  - probes:
    - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`
    - `tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp`
    - `tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp`

This is the authoritative realization and lookup semantics boundary. It freezes
the live selector-table, method-cache, realized-class, category-attachment, and
protocol-conformance lookup rules against compile-coupled executable probes
instead of letting later runtime work infer lookup order from individual probe
payloads or stale planning notes.

## Class/Metaclass/Protocol Realization Surface

- authoritative compile-manifest key:
  - `runtime_class_metaclass_protocol_realization_surface`
- authoritative composed source inputs:
  - `runtime_object_model_realization_source_surface`
  - `runtime_reflection_query_surface`
  - `runtime_realization_lookup_semantics_surface`
- authoritative runtime boundary:
  - public ABI:
    - `objc3_runtime_register_image`
    - `objc3_runtime_lookup_selector`
    - `objc3_runtime_dispatch_i32`
  - private realization query boundary:
    - `objc3_runtime_copy_realized_class_graph_state_for_testing`
    - `objc3_runtime_copy_realized_class_entry_for_testing`
    - `objc3_runtime_copy_protocol_conformance_query_for_testing`
- frozen semantic models:
  - class realization:
    - `registration-installs-runtime-backed-class-records-before-live-dispatch-and-reflection`
  - metaclass lineage:
    - `realized-class-entries-publish-stable-class-metaclass-superclass-and-super-metaclass-owner-identities`
  - protocol conformance:
    - `realized-class-entries-and-runtime-conformance-queries-publish-direct-and-attached-protocol-conformance`
- authoritative proof paths:
  - fixtures:
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_provider.objc3`
    - `tests/tooling/fixtures/native/m258_d002_runtime_packaging_consumer.objc3`
    - `tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3`
  - probes:
    - `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`
    - `tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp`

This is the authoritative class/metaclass/protocol realization boundary. It
freezes the fact that live registration publishes runtime-backed class graphs,
stable metaclass lineage, and runtime-queryable direct/attached protocol
conformance through the same compile-coupled executable path used for dispatch
and reflection. Downstream work must extend this emitted surface instead of
recovering class lineage or protocol truth from ad hoc sidecars, stale
milestone notes, or synthetic summaries.

## Installation ABI And Loader Lifecycle

- public installation ABI:
  - `objc3_runtime_register_image`
  - `objc3_runtime_copy_registration_state_for_testing`
  - `objc3_runtime_reset_for_testing`
- private loader lifecycle testing boundary:
  - `objc3_runtime_stage_registration_table_for_bootstrap`
  - `objc3_runtime_copy_image_walk_state_for_testing`
  - `objc3_runtime_replay_registered_images_for_testing`
  - `objc3_runtime_copy_reset_replay_state_for_testing`
- authoritative acceptance case:
  - `installation-lifecycle`
- fail-closed legality statuses:
  - duplicate translation-unit identity key: `-2`
  - out-of-order registration ordinal: `-3`
  - invalid registration roots / discovery root legality: `-4`
- machine-readable keys:
  - `runtime_installation_abi_surface`
  - `runtime_loader_lifecycle_surface`

The installation ABI is the stable front door for ingesting emitted runtime
images and reading installation state. Loader lifecycle proof remains on the
private testing boundary, where runtime acceptance verifies startup
installation, duplicate/out-of-order rejection without state advance, explicit
rejection of malformed staged registration roots and discovery roots without
state advance, reset retention of the bootstrap catalog, and deterministic
replay of registered images from the retained catalog.

## Acceptance Suite Surface

- authoritative suite:
  - `scripts/check_objc3c_runtime_acceptance.py`
- authoritative report:
  - `tmp/reports/runtime/acceptance/summary.json`
- machine-readable key:
  - `acceptance_suite_surface`

The acceptance suite surface defines which cases may claim published runtime
state. It is authoritative only when the suite consumes the emitted compile
manifest, the coupled registration manifest, and the linked runtime probe path
together. The composite public workflow report carries this same surface
forward when a composite action runs runtime acceptance.

## Shared Executable Acceptance Harness

- harness:
  - `scripts/shared_compiler_runtime_acceptance_harness.py`
- live executable suites:
  - `runtime-acceptance`
  - `public-test-fast`
  - `public-test-full`
- harness summary root:
  - `tmp/reports/runtime/shared-executable-acceptance-harness`

The shared executable acceptance harness is an integration runner over the live
runtime acceptance and public workflow entrypoints. It does not create a new
proof surface. Instead, it executes those suites, requires their emitted
reports to publish the runtime claim boundary, runtime state publication
surface, and acceptance suite surface contracts, and then writes a shared
summary that points back to those child executable reports.

## Integrated Proof Packet

- runner:
  - `scripts/check_objc3c_runtime_architecture_proof_packet.py`
- public action:
  - `python scripts/objc3c_public_workflow_runner.py proof-runtime-architecture`
- packet path:
  - `tmp/reports/runtime/architecture-proof/summary.json`

The integrated runtime architecture proof packet is a generic integration
artifact over the shared harness, the public workflow report, and the direct
runtime acceptance report. It only passes when all three agree on the runtime
claim boundary, runtime state publication surface, acceptance suite surface,
runtime installation ABI surface, and runtime loader lifecycle surface.

## Integrated Validation Path

- runner:
  - `scripts/check_objc3c_runtime_architecture_integration.py`
- public action:
  - `python scripts/objc3c_public_workflow_runner.py validate-runtime-architecture`
- summary path:
  - `tmp/reports/runtime/architecture-integration/summary.json`

The integrated runtime architecture validation path runs the shared harness over
`public-test-full`, then requires the resulting full public-workflow report to
stay aligned with the runtime architecture proof packet and the direct runtime
acceptance report. It fails closed if the full workflow drops the smoke,
runtime-acceptance, or replay child steps, or if any published runtime
architecture surface drifts between the full workflow and the proof packet.

## Claim Boundary

- runnable:
  - behavior proven by real emitted objects linked against
    `artifacts/lib/objc3_runtime.lib`
  - runtime state derived from emitted registration tables and descriptors
  - probe observations produced through the public runtime entrypoints or
    compile-coupled manifests
- claim-only until later closure work lands:
  - any surface described only by comments, sidecars, or private placeholders
  - synthetic `.ll` or hand-authored artifacts with no matching compile output
  - proof that depends on compatibility shims without a coupled emitted object
  - future runtime capability that would require widening
    `native/objc3c/src/runtime/objc3_runtime.h`

## Explicit Non-Goals

- no milestone-specific compile wrappers, proof packets, or closeout sidecars
- no parallel dispatch or installation ABI outside the current runtime header
- no authoritative proof from replay text alone without emitted object and probe
- no widening of public runtime claims beyond what the live acceptance and probe
  path can execute today

The live runtime-acceptance, replay-proof, and composite public-workflow
reports publish this same claim boundary as machine-readable JSON so later work
cannot silently overclaim from sidecars or synthetic artifacts.

The runtime-owned subsystem dependency model is anchored in
`native/objc3c/src/runtime/ARCHITECTURE.md` and enforced by
`python scripts/check_objc3c_dependency_boundaries.py --strict`.
