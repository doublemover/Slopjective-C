"""Interop packaging imported-runtime replay acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter

from ..case_result import CaseResult
from ..native_build import (
    ROOT,
    compile_fixture_expect_failure,
    compile_fixture_outputs_with_args,
    compile_fixture_with_args,
)
from ..probes import compile_probe, parse_json_output, parse_key_value_output, run_probe
from ..assertions import expect
from ..core import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
    INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
    INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
    INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
    MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_IMPORT_VERSION_FEATURE_CLAIM_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_COMPATIBILITY_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADING_INTEROP_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_PUBLIC_HEADER_PATH,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
)
_EXPORTED_CASE_NAMES = [
    "check_imported_runtime_packaging_replay_case",
]

def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_imported_runtime_packaging_replay_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "imported-runtime-packaging-replay"
    provider_fixture = ROOT / Path(IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE)
    probe = ROOT / Path(IMPORTED_RUNTIME_PACKAGING_PROBE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    provider_obj = compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    if not provider_import_surface.is_file():
        raise RuntimeError(
            f"imported runtime provider did not publish {provider_import_surface}"
        )
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_registration_manifest = json.loads(
        (provider_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )

    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    consumer_obj = compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    consumer_registration_manifest = json.loads(
        (consumer_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    link_plan_path = consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
    if not link_plan_path.is_file():
        raise RuntimeError(
            f"imported runtime consumer did not publish {link_plan_path}"
        )
    link_plan = json.loads(link_plan_path.read_text(encoding="utf-8"))

    expect(
        link_plan.get("bootstrap_live_registration_contract_id")
        == "objc3c.runtime.live.registration.discovery.replay.v1",
        "expected cross-module link plan to preserve the live registration replay contract",
    )
    expect(
        link_plan.get("bootstrap_live_restart_hardening_contract_id")
        == "objc3c.runtime.live.restart.hardening.v1",
        "expected cross-module link plan to preserve the live restart hardening contract",
    )
    expect(
        link_plan.get("bootstrap_replay_registered_images_symbol")
        == "objc3_runtime_replay_registered_images_for_testing",
        "expected cross-module link plan to preserve the replay_registered_images symbol",
    )
    expect(
        link_plan.get("bootstrap_reset_replay_state_snapshot_symbol")
        == "objc3_runtime_copy_reset_replay_state_for_testing",
        "expected cross-module link plan to preserve the reset/replay snapshot symbol",
    )
    expect(
        link_plan.get("bootstrap_reset_for_testing_symbol")
        == "objc3_runtime_reset_for_testing",
        "expected cross-module link plan to preserve the reset_for_testing symbol",
    )
    expect(
        link_plan.get(
            "runtime_cross_module_realized_metadata_replay_preservation_surface_contract_id"
        )
        == RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        "expected cross-module link plan to publish the realized-metadata replay preservation surface contract",
    )
    expect(
        link_plan.get("runtime_object_model_realization_source_surface_contract_id")
        == RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        "expected cross-module link plan to preserve the object-model realization source contract",
    )
    expect(
        link_plan.get(
            "runtime_realization_lowering_reflection_artifact_surface_contract_id"
        )
        == RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
        "expected cross-module link plan to preserve the realization/reflection artifact surface contract",
    )
    expect(
        link_plan.get(
            "runtime_dispatch_table_reflection_record_lowering_surface_contract_id"
        )
        == RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "expected cross-module link plan to preserve the dispatch/reflection-record lowering surface contract",
    )
    expect(
        link_plan.get("realized_metadata_replay_preservation_model")
        == "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests",
        "expected cross-module link plan to publish the realized-metadata replay preservation model",
    )
    expect(
        link_plan.get("imported_live_registration_replay_ready") is True,
        "expected cross-module link plan to mark imported live registration replay ready",
    )
    expect(
        link_plan.get("imported_live_restart_hardening_ready") is True,
        "expected cross-module link plan to mark imported live restart hardening ready",
    )
    imported_modules = link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected cross-module link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        provider_import_payload.get("module_name") == imported_module.get("module_name"),
        "expected imported runtime surface module name to match the cross-module link plan",
    )
    expect(
        imported_module.get("module_name") == "runtimePackagingProvider",
        "expected cross-module link plan to preserve the imported provider module name",
    )
    expect(
        imported_module.get("translation_unit_registration_order_ordinal") == 1,
        "expected imported module registration ordinal to be preserved in the link plan",
    )
    expect(
        imported_module.get("ready_for_live_registration_discovery_replay") is True,
        "expected imported module to preserve live registration replay readiness",
    )
    expect(
        imported_module.get("ready_for_live_restart_hardening") is True,
        "expected imported module to preserve live restart hardening readiness",
    )
    for field_name, expected_value in (
        ("bootstrap_live_registration_contract_id", "objc3c.runtime.live.registration.discovery.replay.v1"),
        ("bootstrap_live_restart_hardening_contract_id", "objc3c.runtime.live.restart.hardening.v1"),
        ("bootstrap_live_replay_registered_images_symbol", "objc3_runtime_replay_registered_images_for_testing"),
        ("bootstrap_live_reset_replay_state_snapshot_symbol", "objc3_runtime_copy_reset_replay_state_for_testing"),
        ("bootstrap_live_restart_reset_for_testing_symbol", "objc3_runtime_reset_for_testing"),
        ("bootstrap_live_restart_replay_registered_images_symbol", "objc3_runtime_replay_registered_images_for_testing"),
        ("bootstrap_live_restart_reset_replay_state_snapshot_symbol", "objc3_runtime_copy_reset_replay_state_for_testing"),
    ):
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported module to preserve {field_name}",
        )
    for field_name in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        expect(
            imported_module.get(field_name) == provider_registration_manifest.get(field_name),
            f"expected imported module to preserve {field_name}",
        )
    local_module = link_plan.get("local_module")
    expect(
        isinstance(local_module, dict)
        and local_module.get("translation_unit_registration_order_ordinal") == 2,
        "expected cross-module link plan to preserve the local registration ordinal",
    )
    for field_name in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        expect(
            local_module.get(field_name) == consumer_registration_manifest.get(field_name),
            f"expected local module to preserve {field_name}",
        )
    expected_imported_counts = {
        "imported_class_descriptor_count": provider_registration_manifest["class_descriptor_count"],
        "imported_protocol_descriptor_count": provider_registration_manifest["protocol_descriptor_count"],
        "imported_category_descriptor_count": provider_registration_manifest["category_descriptor_count"],
        "imported_property_descriptor_count": provider_registration_manifest["property_descriptor_count"],
        "imported_ivar_descriptor_count": provider_registration_manifest["ivar_descriptor_count"],
        "imported_total_descriptor_count": provider_registration_manifest["total_descriptor_count"],
    }
    expected_local_counts = {
        "local_class_descriptor_count": consumer_registration_manifest["class_descriptor_count"],
        "local_protocol_descriptor_count": consumer_registration_manifest["protocol_descriptor_count"],
        "local_category_descriptor_count": consumer_registration_manifest["category_descriptor_count"],
        "local_property_descriptor_count": consumer_registration_manifest["property_descriptor_count"],
        "local_ivar_descriptor_count": consumer_registration_manifest["ivar_descriptor_count"],
        "local_total_descriptor_count": consumer_registration_manifest["total_descriptor_count"],
    }
    for field_name, expected_value in expected_imported_counts.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in expected_local_counts.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("module_image_count") == 2,
        "expected cross-module link plan to preserve a two-image runtime topology",
    )
    expect(
        link_plan.get("module_names_lexicographic")
        == ["runtimePackagingConsumer", "runtimePackagingProvider"],
        "expected cross-module link plan to preserve the stable module-name ordering",
    )
    expect(
        link_plan.get("direct_import_input_count") == 1,
        "expected cross-module link plan to preserve one direct imported runtime surface",
    )
    direct_import_surface_artifact_paths = link_plan.get(
        "direct_import_surface_artifact_paths"
    )
    expect(
        isinstance(direct_import_surface_artifact_paths, list)
        and len(direct_import_surface_artifact_paths) == 1
        and direct_import_surface_artifact_paths[0].endswith(
            "provider/module.runtime-import-surface.json"
        ),
        "expected cross-module link plan to preserve the imported runtime surface artifact path",
    )
    for field_name, expected_value in (
        (
            "transitive_class_descriptor_count",
            provider_registration_manifest["class_descriptor_count"]
            + consumer_registration_manifest["class_descriptor_count"],
        ),
        (
            "transitive_protocol_descriptor_count",
            provider_registration_manifest["protocol_descriptor_count"]
            + consumer_registration_manifest["protocol_descriptor_count"],
        ),
        (
            "transitive_category_descriptor_count",
            provider_registration_manifest["category_descriptor_count"]
            + consumer_registration_manifest["category_descriptor_count"],
        ),
        (
            "transitive_property_descriptor_count",
            provider_registration_manifest["property_descriptor_count"]
            + consumer_registration_manifest["property_descriptor_count"],
        ),
        (
            "transitive_ivar_descriptor_count",
            provider_registration_manifest["ivar_descriptor_count"]
            + consumer_registration_manifest["ivar_descriptor_count"],
        ),
        (
            "transitive_total_descriptor_count",
            provider_registration_manifest["total_descriptor_count"]
            + consumer_registration_manifest["total_descriptor_count"],
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    link_object_artifacts = link_plan.get("link_object_artifacts")
    expect(
        isinstance(link_object_artifacts, list) and len(link_object_artifacts) == 2,
        "expected cross-module link plan to publish two ordered link objects",
    )

    exe_path = case_dir / "import_module_execution_matrix_probe.exe"
    probe_link_started = perf_counter()
    compile_probe(clangxx, probe, exe_path, [provider_obj, consumer_obj])
    probe_link_ms = int((perf_counter() - probe_link_started) * 1000)
    probe_run_started = perf_counter()
    payload = parse_json_output(
        run_probe(exe_path), "imported runtime cross-module packaging probe"
    )
    probe_run_ms = int((perf_counter() - probe_run_started) * 1000)
    case_total_ms = int((perf_counter() - case_started) * 1000)

    provider_identity = provider_registration_manifest["translation_unit_identity_key"]
    consumer_identity = consumer_registration_manifest["translation_unit_identity_key"]
    expect(payload.get("startup_registration_copy_status") == 0, "expected imported-runtime startup registration snapshot copy to succeed")
    expect(payload.get("startup_registered_image_count") == 2, "expected imported-runtime startup to install two images")
    expect(
        payload.get("startup_registered_image_count") == link_plan.get("module_image_count"),
        "expected imported-runtime startup image count to match the cross-module link plan",
    )
    expect(payload.get("startup_next_expected_registration_order_ordinal") == 3, "expected imported-runtime startup to advance the next registration ordinal to three")
    expect(payload.get("startup_image_walk_status") == 0, "expected imported-runtime startup image-walk snapshot copy to succeed")
    expect(payload.get("startup_walked_image_count") == 2, "expected imported-runtime startup to walk both imported and local images")
    expect(payload.get("startup_last_walked_module_name") == "runtimePackagingConsumer", "expected imported-runtime startup to walk the local image last")
    expect(payload.get("startup_graph_status") == 0, "expected imported-runtime startup realized-class graph snapshot copy to succeed")
    expect(payload.get("startup_realized_class_count") == 2, "expected imported-runtime startup to realize both imported and local classes")
    expect(payload.get("startup_root_class_count") == 2, "expected imported-runtime startup to publish both classes as roots")
    expect(payload.get("startup_metaclass_edge_count") == 0, "expected imported-runtime startup realized-class graph to avoid metaclass edges")
    expect(payload.get("imported_entry_status") == 0 and payload.get("imported_entry_found") == 1, "expected imported provider runtime metadata to be realized at startup")
    expect(payload.get("local_entry_status") == 0 and payload.get("local_entry_found") == 1, "expected local consumer runtime metadata to be realized at startup")
    expect(payload.get("imported_registration_order_ordinal") == 1, "expected imported provider runtime metadata to preserve registration ordinal one")
    expect(payload.get("local_registration_order_ordinal") == 2, "expected local consumer runtime metadata to preserve registration ordinal two")
    expect(payload.get("imported_direct_protocol_count") == 1, "expected imported provider class to publish one direct protocol")
    expect(payload.get("imported_attached_protocol_count") == 0, "expected imported provider class to publish no attached protocols")
    expect(payload.get("imported_runtime_property_accessor_count") == 0, "expected imported provider class to publish no runtime property accessors")
    expect(payload.get("imported_module_name") == "runtimePackagingProvider", "expected imported provider class entry to preserve the provider module name")
    expect(isinstance(payload.get("imported_translation_unit_identity_key"), str) and payload.get("imported_translation_unit_identity_key") != "", "expected imported provider class entry to publish a non-empty translation unit identity key")
    expect(isinstance(payload.get("imported_class_owner_identity"), str) and payload.get("imported_class_owner_identity") != "", "expected imported provider class owner identity to be non-empty")
    expect(payload.get("local_direct_protocol_count") == 0, "expected local consumer class to publish no direct protocols")
    expect(payload.get("local_attached_protocol_count") == 0, "expected local consumer class to publish no attached protocols")
    expect(payload.get("local_runtime_property_accessor_count") == 0, "expected local consumer class to publish no runtime property accessors")
    expect(payload.get("local_module_name") == "runtimePackagingConsumer", "expected local consumer class entry to preserve the consumer module name")
    expect(isinstance(payload.get("local_translation_unit_identity_key"), str) and payload.get("local_translation_unit_identity_key") != "", "expected local consumer class entry to publish a non-empty translation unit identity key")
    expect(isinstance(payload.get("local_class_owner_identity"), str) and payload.get("local_class_owner_identity") != "", "expected local consumer class owner identity to be non-empty")
    expect(payload.get("protocol_query_attached_category_count") == 0, "expected imported-runtime protocol conformance query to publish no attached categories")
    imported_provider_class_value = payload.get("imported_provider_class_value")
    imported_provider_protocol_value = payload.get("imported_provider_protocol_value")
    local_consumer_class_value = payload.get("local_consumer_class_value")
    expect(
        isinstance(imported_provider_class_value, int)
        and imported_provider_class_value == 43,
        "expected imported provider class dispatch to execute the provider class method",
    )
    expect(
        isinstance(imported_provider_protocol_value, int)
        and imported_provider_protocol_value == 41,
        "expected imported provider protocol method dispatch to execute the provider implementation",
    )
    expect(
        isinstance(local_consumer_class_value, int)
        and local_consumer_class_value == 53,
        "expected local consumer class dispatch to execute the local class method",
    )
    expect(payload.get("selector_table_status") == 0, "expected imported-runtime startup selector-table snapshot copy to succeed")
    expect(payload.get("selector_table_entry_count") == 3, "expected imported-runtime startup to publish three selector entries")
    expect(payload.get("selector_metadata_backed_selector_count") == 3, "expected imported-runtime startup to publish three metadata-backed selectors")
    expect(payload.get("selector_dynamic_selector_count") == 0, "expected imported-runtime startup to avoid dynamic selector entries")
    expect(payload.get("provider_selector_status") == 0 and payload.get("provider_selector_found") == 1, "expected provider class selector metadata to be installed at startup")
    expect(payload.get("provider_selector_metadata_backed") == 1, "expected provider class selector metadata to stay metadata-backed")
    expect(payload.get("provider_selector_provider_count") == 1, "expected provider class selector metadata to name one provider")
    expect(payload.get("provider_selector_first_ordinal") == 1, "expected provider class selector metadata to retain the provider registration ordinal")
    expect(payload.get("provider_selector_last_ordinal") == 1, "expected provider class selector metadata to end at the provider registration ordinal")
    expect(payload.get("imported_protocol_selector_status") == 0 and payload.get("imported_protocol_selector_found") == 1, "expected imported protocol selector metadata to be installed at startup")
    expect(payload.get("imported_protocol_selector_metadata_backed") == 1, "expected imported protocol selector metadata to stay metadata-backed")
    expect(payload.get("imported_protocol_selector_provider_count") == 1, "expected imported protocol selector metadata to name one provider")
    expect(payload.get("imported_protocol_selector_first_ordinal") == 1, "expected imported protocol selector metadata to retain the provider registration ordinal")
    expect(payload.get("imported_protocol_selector_last_ordinal") == 1, "expected imported protocol selector metadata to end at the provider registration ordinal")
    expect(payload.get("local_selector_status") == 0 and payload.get("local_selector_found") == 1, "expected local class selector metadata to be installed at startup")
    expect(payload.get("local_selector_metadata_backed") == 1, "expected local class selector metadata to stay metadata-backed")
    expect(payload.get("local_selector_provider_count") == 1, "expected local class selector metadata to name one provider")
    expect(payload.get("local_selector_first_ordinal") == 2, "expected local class selector metadata to retain the local registration ordinal")
    expect(payload.get("local_selector_last_ordinal") == 2, "expected local class selector metadata to end at the local registration ordinal")
    expect(payload.get("method_cache_state_status") == 0, "expected imported-runtime startup method-cache snapshot copy to succeed")
    expect(payload.get("method_cache_entry_count") == 3, "expected imported-runtime startup to publish three method-cache entries")
    expect(payload.get("method_cache_live_dispatch_count") == 3, "expected imported-runtime startup to publish three live dispatch entries")
    expect(payload.get("method_cache_strict_dispatch_error_count") == 0, "expected imported-runtime startup to avoid metadata-backed strict dispatch errors")
    expect(payload.get("method_cache_last_selector") == "localClassValue", "expected imported-runtime startup to publish the last resolved selector")
    expect(payload.get("method_cache_last_resolved_class_name") == "LocalConsumer", "expected imported-runtime startup to resolve the last method-cache class name")
    expect(payload.get("method_cache_last_resolved_owner_identity") == "implementation:LocalConsumer::class_method:localClassValue", "expected imported-runtime startup to resolve the last method-cache owner identity")
    expect(payload.get("provider_method_status") == 0 and payload.get("provider_method_found") == 1 and payload.get("provider_method_resolved") == 1, "expected provider class method metadata to resolve at startup")
    expect(payload.get("provider_method_owner_identity") == "implementation:ImportedProvider::class_method:providerClassValue", "expected provider class method metadata to publish the resolved owner identity at startup")
    expect(payload.get("imported_protocol_method_status") == 0 and payload.get("imported_protocol_method_found") == 1 and payload.get("imported_protocol_method_resolved") == 1, "expected imported protocol method metadata to resolve at startup")
    expect(payload.get("imported_protocol_method_owner_identity") == "implementation:ImportedProvider::class_method:importedProtocolValue", "expected imported protocol method metadata to publish the resolved owner identity at startup")
    expect(payload.get("local_method_status") == 0 and payload.get("local_method_found") == 1 and payload.get("local_method_resolved") == 1, "expected local class method metadata to resolve at startup")
    expect(payload.get("local_method_owner_identity") == "implementation:LocalConsumer::class_method:localClassValue", "expected local class method metadata to publish the resolved owner identity at startup")
    expect(payload.get("protocol_query_status") == 0, "expected imported-runtime startup protocol-conformance query snapshot copy to succeed")
    expect(payload.get("protocol_query_class_found") == 1 and payload.get("protocol_query_protocol_found") == 1 and payload.get("protocol_query_conforms") == 1, "expected imported provider protocol conformance to survive cross-module startup")
    expect(payload.get("protocol_query_visited_protocol_count") == 1, "expected imported-runtime startup to visit one protocol during conformance evaluation")
    expect(payload.get("protocol_query_attached_category_count") == 0, "expected imported-runtime startup to avoid category-backed protocol conformance")
    expect(payload.get("protocol_query_matched_protocol_owner_identity") == "", "expected imported-runtime startup protocol conformance to leave the matched protocol owner identity empty")
    expect(payload.get("post_reset_registration_copy_status") == 0, "expected post-reset registration snapshot copy to succeed")
    expect(payload.get("post_reset_replay_copy_status") == 0, "expected post-reset replay snapshot copy to succeed")
    expect(payload.get("post_reset_registered_image_count") == 0, "expected reset to clear installed images before replay")
    expect(payload.get("post_reset_retained_bootstrap_image_count") == 2, "expected reset to retain both imported and local bootstrap images for replay")
    expect(payload.get("post_reset_generation") == 1, "expected reset to advance the reset generation before replay")
    expect(payload.get("replay_status") == 0, "expected imported runtime replay to succeed")
    expect(payload.get("post_replay_registration_copy_status") == 0, "expected post-replay registration snapshot copy to succeed")
    expect(payload.get("post_replay_image_walk_status") == 0, "expected post-replay image-walk snapshot copy to succeed")
    expect(payload.get("post_replay_graph_status") == 0, "expected post-replay realized-class graph snapshot copy to succeed")
    expect(payload.get("post_replay_replay_copy_status") == 0, "expected post-replay replay snapshot copy to succeed")
    expect(payload.get("post_replay_registered_image_count") == 2, "expected replay to restore both imported and local images")
    expect(
        payload.get("post_replay_registered_image_count")
        == link_plan.get("module_image_count"),
        "expected replay image count to match the cross-module link plan",
    )
    expect(payload.get("post_replay_next_expected_registration_order_ordinal") == 3, "expected replay to restore the next registration ordinal to three")
    expect(payload.get("post_replay_walked_image_count") == 2, "expected replay to walk both imported and local images")
    expect(payload.get("post_replay_last_walked_module_name") == "runtimePackagingConsumer", "expected replay to walk the local image last")
    expect(payload.get("post_replay_realized_class_count") == 2, "expected replay to restore both realized classes")
    expect(payload.get("post_replay_replay_generation", 0) >= 1, "expected replay to advance the replay generation")
    expect(payload.get("post_replay_retained_bootstrap_image_count") == 2, "expected replay to preserve both retained bootstrap images")
    expect(payload.get("post_replay_imported_entry_status") == 0 and payload.get("post_replay_imported_entry_found") == 1, "expected imported provider runtime metadata to survive replay")
    expect(payload.get("post_replay_local_entry_status") == 0 and payload.get("post_replay_local_entry_found") == 1, "expected local consumer runtime metadata to survive replay")
    expect(payload.get("post_replay_imported_module_name") == "runtimePackagingProvider", "expected replay to preserve the provider module name")
    expect(payload.get("post_replay_imported_translation_unit_identity_key") == provider_identity, "expected replay to preserve the provider translation unit identity key")
    expect(payload.get("post_replay_local_module_name") == "runtimePackagingConsumer", "expected replay to preserve the consumer module name")
    expect(payload.get("post_replay_local_translation_unit_identity_key") == consumer_identity, "expected replay to preserve the consumer translation unit identity key")
    expect(
        payload.get("post_replay_imported_provider_class_value")
        == imported_provider_class_value
        == 43,
        "expected imported provider class dispatch value to survive replay",
    )
    expect(
        payload.get("post_replay_imported_provider_protocol_value")
        == imported_provider_protocol_value
        == 41,
        "expected imported provider protocol dispatch value to survive replay",
    )
    expect(
        payload.get("post_replay_local_consumer_class_value")
        == local_consumer_class_value
        == 53,
        "expected local consumer class dispatch value to survive replay",
    )
    expect(payload.get("post_replay_selector_table_status") == 0, "expected replay selector-table snapshot copy to succeed")
    expect(payload.get("post_replay_selector_table_entry_count") == 3, "expected replay to restore three selector entries")
    expect(payload.get("post_replay_selector_metadata_backed_selector_count") == 3, "expected replay to restore three metadata-backed selectors")
    expect(payload.get("post_replay_provider_selector_status") == 0 and payload.get("post_replay_provider_selector_found") == 1, "expected provider selector metadata to survive replay")
    expect(payload.get("post_replay_imported_protocol_selector_status") == 0 and payload.get("post_replay_imported_protocol_selector_found") == 1, "expected imported protocol selector metadata to survive replay")
    expect(payload.get("post_replay_local_selector_status") == 0 and payload.get("post_replay_local_selector_found") == 1, "expected local selector metadata to survive replay")
    expect(payload.get("post_replay_method_cache_state_status") == 0, "expected replay method-cache snapshot copy to succeed")
    expect(payload.get("post_replay_method_cache_entry_count") == 3, "expected replay to restore three method-cache entries")
    expect(payload.get("post_replay_method_cache_live_dispatch_count") == 3, "expected replay to restore three live dispatch entries")
    expect(payload.get("post_replay_method_cache_strict_dispatch_error_count") == 0, "expected replay to avoid metadata-backed strict dispatch errors")
    expect(payload.get("post_replay_method_cache_last_selector") == "localClassValue", "expected replay to preserve the last resolved selector")
    expect(payload.get("post_replay_method_cache_last_resolved_class_name") == "LocalConsumer", "expected replay to preserve the last resolved method-cache class name")
    expect(payload.get("post_replay_method_cache_last_resolved_owner_identity") == "implementation:LocalConsumer::class_method:localClassValue", "expected replay to preserve the last resolved method-cache owner identity")
    expect(payload.get("post_replay_provider_method_status") == 0 and payload.get("post_replay_provider_method_found") == 1 and payload.get("post_replay_provider_method_resolved") == 1, "expected provider class method metadata to resolve after replay")
    expect(payload.get("post_replay_provider_method_owner_identity") == "implementation:ImportedProvider::class_method:providerClassValue", "expected provider class method metadata to preserve its resolved owner identity after replay")
    expect(payload.get("post_replay_imported_protocol_method_status") == 0 and payload.get("post_replay_imported_protocol_method_found") == 1 and payload.get("post_replay_imported_protocol_method_resolved") == 1, "expected imported protocol method metadata to resolve after replay")
    expect(payload.get("post_replay_imported_protocol_method_owner_identity") == "implementation:ImportedProvider::class_method:importedProtocolValue", "expected imported protocol method metadata to preserve its resolved owner identity after replay")
    expect(payload.get("post_replay_local_method_status") == 0 and payload.get("post_replay_local_method_found") == 1 and payload.get("post_replay_local_method_resolved") == 1, "expected local class method metadata to resolve after replay")
    expect(payload.get("post_replay_local_method_owner_identity") == "implementation:LocalConsumer::class_method:localClassValue", "expected local class method metadata to preserve its resolved owner identity after replay")

    return CaseResult(
        case_id="imported-runtime-packaging-replay",
        probe=IMPORTED_RUNTIME_PACKAGING_PROBE,
        fixture=IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "provider_fixture": IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            "provider_import_surface": str(provider_import_surface.relative_to(ROOT)).replace("\\", "/"),
            "link_plan": str(link_plan_path.relative_to(ROOT)).replace("\\", "/"),
            "provider_translation_unit_identity_key": provider_identity,
            "consumer_translation_unit_identity_key": consumer_identity,
            "provider_compile_ms": provider_compile_ms,
            "consumer_compile_ms": consumer_compile_ms,
            "probe_link_ms": probe_link_ms,
            "probe_run_ms": probe_run_ms,
            "case_total_ms": case_total_ms,
        },
    )

__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
