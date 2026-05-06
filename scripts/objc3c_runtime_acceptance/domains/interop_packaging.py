"""Interop Packaging runtime acceptance domain."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter
from typing import Any

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
    RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_LANGUAGE_REPLAY_IMPORT_SURFACE_PRESERVATION_SURFACE_CONTRACT_ID,
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
    "build_runtime_cross_module_package_interop_source_surface",
    "build_runtime_textual_binary_interface_parity_source_surface",
    "build_runtime_mixed_image_compatibility_interop_semantics_surface",
    "build_runtime_package_loading_module_identity_semantics_surface",
    "build_runtime_c_cpp_swift_bridge_compatibility_semantics_surface",
    "build_runtime_import_version_feature_claim_diagnostics_surface",
    "build_runtime_packaging_bridge_loader_artifact_surface",
    "build_runtime_mixed_image_package_lowering_bridge_emission_surface",
    "build_runtime_cross_language_replay_import_surface_preservation_surface",
    "build_runtime_package_loader_bridge_abi_surface",
    "build_runtime_package_loading_interop_implementation_surface",
    "check_imported_runtime_packaging_replay_case",
    "check_cross_module_runtime_package_interop_source_surface_case",
    "check_textual_binary_interface_parity_source_surface_case",
    "check_mixed_image_compatibility_interop_semantics_case",
    "check_c_cpp_swift_bridge_compatibility_semantics_case",
    "check_import_version_feature_claim_diagnostics_case",
    "check_runtime_packaging_bridge_loader_artifact_surface_case",
    "check_mixed_image_package_lowering_bridge_emission_case",
    "check_cross_language_replay_import_surface_preservation_case",
    "check_runtime_package_loader_bridge_abi_case",
    "check_live_package_loading_interop_runtime_implementation_case",
]

def build_runtime_cross_module_package_interop_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"cross-module-runtime-package-interop-source-surface"}
    ]
    return {
        "contract_id": RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.interop-bridge.h",
            "<emit-prefix>.interop-bridge.modulemap",
            "<emit-prefix>.interop-bridge.json",
        ],
        "source_contract_ids": [
            "objc3c.interop.foreign.declaration.import.source.closure.v1",
            "objc3c.interop.cpp.swift.interop.annotation.source.completion.v1",
            "objc3c.interop.foreign.surface.interface.preservation.v1",
            "objc3c.interop.header.module.and.bridge.generation.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/pipeline/objc3_frontend_types.h",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_source_fields": [
            "frontend.pipeline.semantic_surface.objc_interop_foreign_declaration_and_import_source_closure",
            "frontend.pipeline.semantic_surface.objc_interop_cpp_and_swift_interop_annotation_source_completion",
            "frontend.pipeline.semantic_surface.objc_interop_foreign_surface_interface_and_module_preservation",
            "frontend.pipeline.semantic_surface.objc_interop_header_module_and_bridge_generation",
        ],
        "source_surface_model": (
            "cross-module-runtime-packaging-publishes-one-compile-coupled-import-surface-and-bridge-artifact-boundary-for-foreign-cpp-and-swift-facing-interop-facts"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
        ],
        "explicit_non_goals": [
            "no-live-foreign-symbol-linking-claim",
            "no-cross-language-runnable-execution-claim",
            "no-public-runtime-abi-widening",
        ],
        "requires_runtime_import_surface": True,
        "requires_real_compile_output": True,
    }

def build_runtime_textual_binary_interface_parity_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"textual-binary-interface-parity-source-surface"}
    ]
    return {
        "contract_id": RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.interop-bridge.h",
            "<emit-prefix>.interop-bridge.modulemap",
            "<emit-prefix>.interop-bridge.json",
        ],
        "source_contract_ids": [
            "objc3c.interop.cpp.swift.interop.annotation.source.completion.v1",
            "objc3c.interop.foreign.surface.interface.preservation.v1",
            "objc3c.interop.header.module.and.bridge.generation.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/pipeline/objc3_frontend_types.h",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_source_fields": [
            "frontend.pipeline.semantic_surface.objc_interop_cpp_and_swift_interop_annotation_source_completion",
            "frontend.pipeline.semantic_surface.objc_interop_foreign_surface_interface_and_module_preservation",
            "frontend.pipeline.semantic_surface.objc_interop_header_module_and_bridge_generation",
        ],
        "parity_surface_model": (
            "generated-bridge-header-modulemap-bridge-json-and-runtime-import-surface-preserve-one-foreign-cpp-and-swift-facing-interface-shape-without-textual-binary-drift"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        ],
        "requires_runtime_import_surface": True,
        "requires_textual_bridge_artifacts": True,
        "requires_real_compile_output": True,
    }

def build_runtime_mixed_image_compatibility_interop_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"mixed-image-compatibility-interop-semantics"}
    ]
    return {
        "contract_id": (
            RUNTIME_MIXED_IMAGE_COMPATIBILITY_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.interop.foreign.surface.interface.preservation.v1",
            "objc3c.interop.header.module.and.bridge.generation.v1",
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.interop-bridge.h",
            "<emit-prefix>.interop-bridge.modulemap",
            "<emit-prefix>.interop-bridge.json",
        ],
        "language_profile_model": (
            "mixed-image-provider-and-consumer-compiles-share-one-fail-closed-registration-order-and-interop-bridge-compatibility-boundary-through-runtime-import-surfaces-and-cross-module-link-plans"
        ),
        "diagnostic_model": (
            "duplicate-registration-order-or-import-surface-drift-rejects-the-consumer-before-cross-module-link-plan-installation-advances"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_fixture_paths": [
            INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
            INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_real_compile_output": True,
    }

def build_runtime_package_loading_module_identity_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "imported-runtime-packaging-replay",
            "multi-image-registration-reset-replay",
        }
    ]
    return {
        "contract_id": (
            RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.obj",
        ],
        "runtime_probe": IMPORTED_RUNTIME_PACKAGING_PROBE,
        "package_loading_model": (
            "runtime-package-loading-and-reset-replay-preserve-imported-and-local-module-identities-registration-ordinals-and-realized-class-ownership-through-the-live-runtime"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_linked_runtime_probe": True,
        "requires_real_compile_output": True,
    }

def build_runtime_c_cpp_swift_bridge_compatibility_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"c-cpp-swift-bridge-compatibility-semantics"}
    ]
    return {
        "contract_id": (
            RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
            "<emit-prefix>.interop-bridge.h",
            "<emit-prefix>.interop-bridge.modulemap",
            "<emit-prefix>.interop-bridge.json",
        ],
        "language_profile_model": (
            "c-cpp-and-swift-facing-interop-annotations-survive-provider-emission-consumer-import-and-cross-module-link-planning-without-bridge-shape-drift"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_fixture_paths": [
            INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
            INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_real_compile_output": True,
    }

def build_runtime_import_version_feature_claim_diagnostics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"import-version-feature-claim-diagnostics"}
    ]
    return {
        "contract_id": (
            RUNTIME_IMPORT_VERSION_FEATURE_CLAIM_DIAGNOSTICS_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.objc3-advanced-feature-gate.json",
            "<emit-prefix>.objc3-release-candidate-matrix.json",
            "<emit-prefix>.diagnostics.json",
        ],
        "diagnostic_model": (
            "shipped-feature-claim-sidecars-stay-ready-while-drifted-import-contract-versions-fail-closed-before-consumer-packaging-claims-advance"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_manifest_artifacts.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_fixture_paths": [
            INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
            INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_release_artifact_sidecars": True,
        "requires_real_compile_output": True,
    }

def build_runtime_packaging_bridge_loader_artifact_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"runtime-packaging-bridge-loader-artifact-surface"}
    ]
    return {
        "contract_id": RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
        "source_contract_ids": [
            RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_MIXED_IMAGE_COMPATIBILITY_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID,
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.interop-bridge.h",
            "<emit-prefix>.interop-bridge.modulemap",
            "<emit-prefix>.interop-bridge.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
            "<emit-prefix>.cross-module-runtime-linker-options.rsp",
            "<emit-prefix>.runtime-metadata-linker-options.rsp",
        ],
        "artifact_surface_model": (
            "provider-bridge-artifacts-plus-consumer-link-plan-and-linker-response-sidecars-freeze-one-runtime-package-loader-boundary-for-mixed-image-interop-builds"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_manifest_artifacts.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
        ],
        "authoritative_fixture_paths": [
            INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
            INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_linker_response_artifact": True,
        "requires_real_compile_output": True,
    }

def build_runtime_mixed_image_package_lowering_bridge_emission_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"mixed-image-package-lowering-bridge-emission"}
    ]
    return {
        "contract_id": (
            RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
        ],
        "compile_artifact_set": [
            "<emit-prefix>.ll",
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.interop-bridge.h",
            "<emit-prefix>.interop-bridge.modulemap",
            "<emit-prefix>.interop-bridge.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
        ],
        "lowering_model": (
            "provider-lowering-emits-interop-abi-summary-metadata-and-bridge-call-declarations-while-consumer-packaging-preserves-the-emitted-bridge-boundary-as-a-mixed-image-import"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/lower/objc3_lowering_contract.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "authoritative_fixture_paths": [
            INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
            INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_real_compile_output": True,
    }

def build_runtime_cross_language_replay_import_surface_preservation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"cross-language-replay-import-surface-preservation"}
    ]
    return {
        "contract_id": (
            RUNTIME_CROSS_LANGUAGE_REPLAY_IMPORT_SURFACE_PRESERVATION_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
        ],
        "compile_artifact_set": [
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
        ],
        "preservation_model": (
            "provider-runtime-import-replay-keys-for-c-cpp-and-swift-interop-survive-consumer-import-surface-consumption-and-cross-module-link-plan-preservation"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/driver/objc3_objc3_path.cpp",
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_fixture_paths": [
            INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
            INTEROP_HEADER_MODULE_CONSUMER_FIXTURE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_real_compile_output": True,
    }

def build_runtime_package_loader_bridge_abi_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"runtime-package-loader-bridge-abi"}
    ]
    return {
        "contract_id": RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
        "source_contract_ids": [
            RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
            RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
        ],
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "authoritative_case_ids": authoritative_case_ids,
        "runtime_abi_model": (
            "private-runtime-snapshots-publish-package-loader-topology-and-bridge-generation-readiness-through-the-live-runtime-library-without-public-abi-widening"
        ),
        "authoritative_code_paths": [
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_probe_paths": [
            INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
            INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
        ],
        "requires_linked_runtime_probe": True,
        "requires_real_compile_output": True,
    }

def build_runtime_package_loading_interop_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"live-package-loading-interop-runtime-implementation"}
    ]
    return {
        "contract_id": (
            RUNTIME_PACKAGE_LOADING_INTEROP_IMPLEMENTATION_SURFACE_CONTRACT_ID
        ),
        "source_contract_ids": [
            RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
            RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
        ],
        "implementation_model": (
            "live-runtime-package-loader-snapshots-agree-with-the-emitted-interop-link-plan-and-bridge-artifacts-for-the-current-mixed-image-packaging-boundary"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/io/objc3_process.cpp",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_fixture_paths": [
            INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
            INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        ],
        "authoritative_probe_paths": [
            INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
            INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
        ],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_linked_runtime_probe": True,
        "requires_real_compile_output": True,
    }

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

def check_cross_module_runtime_package_interop_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "cross-module-runtime-package-interop-source-surface"
    fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(
        fixture,
        compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    manifest_path = compile_dir / "module.manifest.json"
    registration_manifest_path = (
        compile_dir / "module.runtime-registration-manifest.json"
    )
    runtime_import_surface_path = compile_dir / "module.runtime-import-surface.json"
    bridge_header_path = compile_dir / "module.interop-bridge.h"
    bridge_modulemap_path = compile_dir / "module.interop-bridge.modulemap"
    bridge_json_path = compile_dir / "module.interop-bridge.json"

    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    runtime_import_surface = json.loads(
        runtime_import_surface_path.read_text(encoding="utf-8")
    )
    bridge_json = json.loads(bridge_json_path.read_text(encoding="utf-8"))
    semantic_surface = (
        manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    )
    foreign_source_surface = semantic_surface.get(
        "objc_interop_foreign_declaration_and_import_source_closure", {}
    )
    annotation_source_surface = semantic_surface.get(
        "objc_interop_cpp_and_swift_interop_annotation_source_completion", {}
    )
    foreign_preservation_surface = semantic_surface.get(
        "objc_interop_foreign_surface_interface_and_module_preservation", {}
    )
    bridge_generation_surface = semantic_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )

    expect(
        foreign_source_surface.get("contract_id")
        == "objc3c.interop.foreign.declaration.import.source.closure.v1",
        "expected interop provider manifest to publish the foreign/import source closure surface",
    )
    expect(
        annotation_source_surface.get("contract_id")
        == "objc3c.interop.cpp.swift.interop.annotation.source.completion.v1",
        "expected interop provider manifest to publish the C++/Swift annotation source-completion surface",
    )
    expect(
        foreign_preservation_surface.get("contract_id")
        == "objc3c.interop.foreign.surface.interface.preservation.v1",
        "expected interop provider manifest to publish the foreign surface/interface preservation packet",
    )
    expect(
        bridge_generation_surface.get("contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected interop provider manifest to publish the header/module/bridge generation packet",
    )
    expect(
        runtime_import_surface.get("module_name") == bridge_json.get("module_name"),
        "expected interop provider import surface and bridge artifact to preserve one module identity",
    )
    expect(
        registration_manifest.get("translation_unit_registration_order_ordinal") == 1,
        "expected interop provider registration manifest to preserve the explicit registration order ordinal",
    )
    for artifact_path, label in (
        (bridge_header_path, "bridge header"),
        (bridge_modulemap_path, "bridge modulemap"),
        (bridge_json_path, "bridge json"),
    ):
        expect(
            artifact_path.is_file(),
            f"expected interop provider compile to publish the {label} artifact",
        )
    expect(
        runtime_import_surface.get(
            "objc_interop_foreign_surface_interface_and_module_preservation", {}
        ).get("contract_id")
        == "objc3c.interop.foreign.surface.interface.preservation.v1",
        "expected interop provider runtime import surface to preserve the foreign surface/interface preservation contract",
    )
    expect(
        runtime_import_surface.get(
            "objc_interop_header_module_and_bridge_generation", {}
        ).get("contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected interop provider runtime import surface to preserve the header/module/bridge generation contract",
    )
    expect(
        bridge_json.get("header_artifact_relative_path") == "module.interop-bridge.h"
        and bridge_json.get("module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and bridge_json.get("bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected interop provider bridge artifact to preserve the canonical textual/binary artifact paths",
    )
    expect(
        bridge_json.get("runtime_generation_ready") is True
        and bridge_json.get("cross_module_packaging_ready") is True
        and bridge_json.get("deterministic") is True,
        "expected interop provider bridge artifact to report runtime generation and cross-module packaging readiness",
    )

    return CaseResult(
        case_id="cross-module-runtime-package-interop-source-surface",
        probe="compile-manifest-runtime-import-surface-and-bridge-artifacts",
        fixture=INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "module_name": runtime_import_surface.get("module_name"),
            "runtime_import_surface_path": str(
                runtime_import_surface_path.relative_to(ROOT)
            ).replace("\\", "/"),
            "bridge_header_path": str(bridge_header_path.relative_to(ROOT)).replace(
                "\\", "/"
            ),
            "bridge_modulemap_path": str(
                bridge_modulemap_path.relative_to(ROOT)
            ).replace("\\", "/"),
            "bridge_json_path": str(bridge_json_path.relative_to(ROOT)).replace(
                "\\", "/"
            ),
        },
    )

def check_textual_binary_interface_parity_source_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "textual-binary-interface-parity-source-surface"
    fixture = ROOT / Path(INTEROP_HEADER_MODULE_PROVIDER_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(
        fixture,
        compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    manifest = json.loads((compile_dir / "module.manifest.json").read_text(encoding="utf-8"))
    runtime_import_surface = json.loads(
        (compile_dir / "module.runtime-import-surface.json").read_text(encoding="utf-8")
    )
    bridge_json = json.loads(
        (compile_dir / "module.interop-bridge.json").read_text(encoding="utf-8")
    )
    bridge_header = (compile_dir / "module.interop-bridge.h").read_text(
        encoding="utf-8"
    )
    bridge_modulemap = (compile_dir / "module.interop-bridge.modulemap").read_text(
        encoding="utf-8"
    )
    semantic_surface = (
        manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    )
    annotation_source_surface = semantic_surface.get(
        "objc_interop_cpp_and_swift_interop_annotation_source_completion", {}
    )
    foreign_preservation_surface = semantic_surface.get(
        "objc_interop_foreign_surface_interface_and_module_preservation", {}
    )
    bridge_generation_surface = semantic_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )
    runtime_bridge_packet = runtime_import_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )

    expect(
        annotation_source_surface.get("contract_id")
        == "objc3c.interop.cpp.swift.interop.annotation.source.completion.v1",
        "expected header/module bridge provider manifest to publish the C++/Swift annotation source-completion surface",
    )
    expect(
        foreign_preservation_surface.get("contract_id")
        == "objc3c.interop.foreign.surface.interface.preservation.v1",
        "expected header/module bridge provider manifest to publish the foreign surface/interface preservation packet",
    )
    expect(
        bridge_generation_surface.get("contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected header/module bridge provider manifest to publish the header/module/bridge generation packet",
    )
    expect(
        runtime_bridge_packet.get("contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected header/module bridge provider runtime import surface to preserve the bridge-generation contract",
    )
    expect(
        bridge_json.get("header_artifact_relative_path")
        == runtime_bridge_packet.get("header_artifact_relative_path")
        == "module.interop-bridge.h",
        "expected header/module bridge provider textual and binary surfaces to preserve the header artifact path",
    )
    expect(
        bridge_json.get("module_artifact_relative_path")
        == runtime_bridge_packet.get("module_artifact_relative_path")
        == "module.interop-bridge.modulemap",
        "expected header/module bridge provider textual and binary surfaces to preserve the modulemap artifact path",
    )
    expect(
        bridge_json.get("bridge_artifact_relative_path")
        == runtime_bridge_packet.get("bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected header/module bridge provider textual and binary surfaces to preserve the bridge-json artifact path",
    )
    expect(
        bridge_json.get("runtime_generation_ready") is True
        and bridge_json.get("cross_module_packaging_ready") is True
        and bridge_json.get("deterministic") is True
        and runtime_bridge_packet.get("runtime_generation_ready") is True
        and runtime_bridge_packet.get("cross_module_packaging_ready") is True
        and runtime_bridge_packet.get("deterministic") is True,
        "expected header/module bridge provider textual and binary surfaces to agree on readiness and determinism",
    )
    expect(
        bridge_json.get("module_name") == runtime_import_surface.get("module_name"),
        "expected header/module bridge provider bridge json and runtime import surface to preserve one module name",
    )
    expect(
        len(bridge_json.get("foreign_callables", [])) == 2
        and runtime_bridge_packet.get("local_foreign_callable_count") == 2
        and foreign_preservation_surface.get("local_foreign_callable_count") == 2,
        "expected header/module bridge provider textual and binary surfaces to preserve two foreign callables",
    )
    expect(
        annotation_source_surface.get("swift_name_annotation_sites") == 1
        and annotation_source_surface.get("cpp_name_annotation_sites") == 2
        and annotation_source_surface.get("header_name_annotation_sites") == 2,
        "expected header/module bridge provider source-completion surface to preserve the foreign C++/Swift annotation inventory",
    )
    for snippet, label in (
        ("module.interop-bridge.h", "header artifact path"),
        ("module.interop-bridge.modulemap", "modulemap artifact path"),
        ("ffiInbound", "primary foreign callable"),
        ("ffiHeaderBridge", "header-only foreign callable"),
        ("BridgeProviderShim", "C++ bridge annotation"),
        ("BridgeProvider.forward", "Swift bridge annotation"),
    ):
        expect(
            snippet in bridge_header,
            f"expected generated interop bridge header to preserve the {label}",
        )
    expect(
        "module.interop-bridge.h" in bridge_modulemap
        and "_objc3_interop_bridge" in bridge_modulemap,
        "expected generated interop bridge modulemap to preserve the bridge module identity",
    )

    return CaseResult(
        case_id="textual-binary-interface-parity-source-surface",
        probe="compile-manifest-runtime-import-surface-bridge-header-modulemap-and-bridge-json",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "module_name": runtime_import_surface.get("module_name"),
            "foreign_callable_count": len(bridge_json.get("foreign_callables", [])),
            "header_artifact_relative_path": bridge_json.get(
                "header_artifact_relative_path"
            ),
            "module_artifact_relative_path": bridge_json.get(
                "module_artifact_relative_path"
            ),
            "bridge_artifact_relative_path": bridge_json.get(
                "bridge_artifact_relative_path"
            ),
        },
    )

def check_mixed_image_compatibility_interop_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "mixed-image-compatibility-interop-semantics"
    provider_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_bridge_json = json.loads(
        (provider_compile_dir / "module.interop-bridge.json").read_text(
            encoding="utf-8"
        )
    )
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )

    consumer_compile_dir = case_dir / "consumer"
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_registration_manifest = json.loads(
        (
            consumer_compile_dir / "module.runtime-registration-manifest.json"
        ).read_text(encoding="utf-8")
    )
    link_plan_path = consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
    link_plan = json.loads(link_plan_path.read_text(encoding="utf-8"))

    expect(
        link_plan.get("interop_header_module_bridge_imported_module_count") == 1
        and link_plan.get("interop_ffi_imported_module_count") == 1,
        "expected mixed-image interop link plan to preserve one imported bridge/ffi provider module",
    )
    expect(
        link_plan.get("module_image_count") == 2
        and link_plan.get("ready") is True,
        "expected mixed-image interop link plan to publish a ready two-image package topology",
    )
    expect(
        link_plan.get("expected_interop_bridge_header_artifact_relative_path")
        == "module.interop-bridge.h"
        and link_plan.get("expected_interop_bridge_module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and link_plan.get("expected_interop_bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected mixed-image interop link plan to preserve the canonical bridge artifact paths",
    )

    local_module = link_plan.get("local_module", {})
    imported_modules = link_plan.get("imported_modules", [])
    expect(
        isinstance(local_module, dict) and isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected mixed-image interop link plan to publish one local module and one imported module summary",
    )
    imported_module = imported_modules[0]
    expect(
        local_module.get("translation_unit_registration_order_ordinal") == 2
        and consumer_registration_manifest.get("translation_unit_registration_order_ordinal") == 2,
        "expected mixed-image interop consumer compile to preserve registration ordinal 2",
    )
    expect(
        imported_module.get("translation_unit_registration_order_ordinal") == 1,
        "expected mixed-image interop link plan to preserve the imported provider registration ordinal",
    )
    expect(
        imported_module.get("interop_header_module_bridge_contract_id")
        == link_plan.get("expected_interop_header_module_bridge_contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected mixed-image interop link plan to preserve the imported bridge-generation contract",
    )
    expect(
        imported_module.get("interop_ffi_contract_id")
        == link_plan.get("expected_interop_ffi_contract_id")
        == "objc3c.interop.ffi.metadata.interface.preservation.v1",
        "expected mixed-image interop link plan to preserve the imported ffi metadata/interface preservation contract",
    )
    expect(
        imported_module.get("interop_ffi_preservation_contract_id")
        == link_plan.get("expected_interop_ffi_preservation_contract_id")
        == "objc3c.interop.foreign.surface.interface.preservation.v1",
        "expected mixed-image interop link plan to preserve the imported foreign surface/interface preservation contract",
    )
    expect(
        imported_module.get("interop_bridge_header_artifact_relative_path")
        == provider_bridge_json.get("header_artifact_relative_path")
        == "module.interop-bridge.h",
        "expected mixed-image interop link plan to preserve the provider header bridge path",
    )
    expect(
        imported_module.get("interop_header_module_bridge_cross_module_packaging_ready")
        is True
        and imported_module.get("interop_header_module_bridge_runtime_generation_ready")
        is True
        and provider_import_payload.get(
            "objc_interop_header_module_and_bridge_generation", {}
        ).get("cross_module_packaging_ready")
        is True,
        "expected mixed-image interop provider and consumer artifacts to agree on bridge packaging readiness",
    )

    duplicate_ordinal_negative = compile_fixture_expect_failure(
        consumer_fixture,
        case_dir / "negative-duplicate-registration-order",
        expected_snippets=[
            "cross-module runtime link-plan duplicate registration order ordinal: 1"
        ],
        expected_codes=[],
        extra_args=[
            "--objc3-bootstrap-registration-order-ordinal",
            "1",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
        allow_missing_structured_diagnostics=True,
    )

    return CaseResult(
        case_id="mixed-image-compatibility-interop-semantics",
        probe="compile-runtime-import-surface-link-plan-and-fail-closed-diagnostics",
        fixture=INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "provider_runtime_import_surface_path": str(
                provider_import_surface.relative_to(ROOT)
            ).replace("\\", "/"),
            "link_plan_path": str(link_plan_path.relative_to(ROOT)).replace("\\", "/"),
            "module_image_count": link_plan.get("module_image_count"),
            "imported_module_count": len(imported_modules),
            "duplicate_registration_order_returncode": duplicate_ordinal_negative[
                "returncode"
            ],
            "duplicate_registration_order_diagnostics_path": duplicate_ordinal_negative[
                "diagnostics_path"
            ],
        },
    )

def check_c_cpp_swift_bridge_compatibility_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "c-cpp-swift-bridge-compatibility-semantics"
    provider_fixture = ROOT / Path(INTEROP_HEADER_MODULE_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_HEADER_MODULE_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_import_surface = json.loads(
        (provider_compile_dir / "module.runtime-import-surface.json").read_text(
            encoding="utf-8"
        )
    )
    provider_bridge_header = (provider_compile_dir / "module.interop-bridge.h").read_text(
        encoding="utf-8"
    )
    provider_bridge_json = json.loads(
        (provider_compile_dir / "module.interop-bridge.json").read_text(
            encoding="utf-8"
        )
    )
    provider_ffi_surface = provider_import_surface.get(
        "objc_interop_foreign_surface_interface_and_module_preservation", {}
    )
    provider_bridge_surface = provider_import_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )

    expect(
        provider_ffi_surface.get("local_foreign_callable_count") == 2
        and provider_ffi_surface.get("local_cpp_name_annotation_count") == 2
        and provider_ffi_surface.get("local_header_name_annotation_count") == 2
        and provider_ffi_surface.get("local_swift_name_annotation_count") == 1,
        "expected provider import surface to preserve the C/C++/Swift-facing annotation counts",
    )
    expect(
        provider_bridge_surface.get("local_import_module_names_lexicographic")
        == ['"BridgeProviderKit"'],
        "expected provider bridge-generation surface to preserve the import-module name",
    )
    expect(
        "ffiHeaderBridge" in provider_bridge_header
        and "BridgeProviderExtrasShim" in provider_bridge_header
        and "ffiInbound" in provider_bridge_header
        and "BridgeProvider.forward" in provider_bridge_header,
        "expected generated bridge header to preserve the C, C++, and Swift-facing bridge names",
    )
    expect(
        isinstance(provider_bridge_json.get("foreign_callables"), list)
        and len(provider_bridge_json["foreign_callables"]) == 2,
        "expected provider bridge json to publish two foreign callables",
    )

    consumer_compile_dir = case_dir / "consumer"
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_compile_dir / "module.runtime-import-surface.json"),
        ],
    )
    link_plan = json.loads(
        (
            consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
        ).read_text(encoding="utf-8")
    )
    imported_modules = link_plan.get("imported_modules", [])
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected bridge-compatibility consumer compile to publish one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        link_plan.get("interop_ffi_imported_module_count") == 1
        and link_plan.get("interop_header_module_bridge_imported_module_count") == 1,
        "expected consumer link plan to preserve one imported ffi/bridge provider module",
    )
    expect(
        imported_module.get("interop_ffi_local_interface_annotation_sites") == 12
        and imported_module.get("interop_ffi_local_metadata_preservation_sites") == 2,
        "expected consumer link plan to preserve the imported C/C++/Swift annotation footprint",
    )
    expect(
        imported_module.get("interop_header_module_bridge_local_foreign_callable_count")
        == 2
        and imported_module.get("interop_bridge_header_artifact_relative_path")
        == "module.interop-bridge.h"
        and imported_module.get("interop_bridge_module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and imported_module.get("interop_bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected consumer link plan to preserve the imported bridge callable count and artifact paths",
    )
    expect(
        imported_module.get("interop_ffi_preservation_contract_id")
        == "objc3c.interop.foreign.surface.interface.preservation.v1"
        and imported_module.get("interop_header_module_bridge_contract_id")
        == "objc3c.interop.header.module.and.bridge.generation.v1",
        "expected consumer link plan to preserve the imported ffi and bridge contracts",
    )

    return CaseResult(
        case_id="c-cpp-swift-bridge-compatibility-semantics",
        probe="compile-runtime-import-surface-bridge-artifacts-and-cross-module-link-plan",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "provider_module_name": provider_import_surface.get("module_name"),
            "foreign_callable_count": provider_ffi_surface.get(
                "local_foreign_callable_count"
            ),
            "cpp_name_annotation_count": provider_ffi_surface.get(
                "local_cpp_name_annotation_count"
            ),
            "swift_name_annotation_count": provider_ffi_surface.get(
                "local_swift_name_annotation_count"
            ),
            "bridge_imported_module_count": link_plan.get(
                "interop_header_module_bridge_imported_module_count"
            ),
        },
    )

def check_import_version_feature_claim_diagnostics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "import-version-feature-claim-diagnostics"
    provider_fixture = ROOT / Path(INTEROP_HEADER_MODULE_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_HEADER_MODULE_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )

    advanced_feature_gate = json.loads(
        (
            provider_compile_dir / "module.objc3-advanced-feature-gate.json"
        ).read_text(encoding="utf-8")
    )
    release_candidate_matrix = json.loads(
        (
            provider_compile_dir / "module.objc3-release-candidate-matrix.json"
        ).read_text(encoding="utf-8")
    )
    expect(
        advanced_feature_gate.get("contract_id")
        == "objc3c.tooling.integrated.advanced.feature.gate.v1"
        and advanced_feature_gate.get("ready") is True,
        "expected provider compile to publish a ready advanced feature-gate artifact",
    )
    expect(
        release_candidate_matrix.get("contract_id")
        == "objc3c.tooling.release.candidate.execution.matrix.v1"
        and release_candidate_matrix.get("advanced_feature_gate_artifact")
        == "module.objc3-advanced-feature-gate.json"
        and release_candidate_matrix.get("ready") is True,
        "expected provider compile to publish a ready release-candidate matrix tied to the advanced feature-gate artifact",
    )

    corrupted_import_surface_path = case_dir / "provider-corrupted.runtime-import-surface.json"
    corrupted_import_surface = json.loads(
        (
            provider_compile_dir / "module.runtime-import-surface.json"
        ).read_text(encoding="utf-8")
    )
    corrupted_import_surface[
        "objc_interop_header_module_and_bridge_generation"
    ]["contract_id"] = "objc3c.interop.header.module.and.bridge.generation.v999"
    corrupted_import_surface_path.write_text(
        json.dumps(corrupted_import_surface, indent=2) + "\n", encoding="utf-8"
    )

    import_version_negative = compile_fixture_expect_failure(
        consumer_fixture,
        case_dir / "negative-import-version-drift",
        expected_snippets=[
            "unexpected Part 11 header/module/bridge generation contract id in import surface"
        ],
        expected_codes=[],
        extra_args=[
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(corrupted_import_surface_path),
        ],
        allow_missing_structured_diagnostics=True,
    )

    return CaseResult(
        case_id="import-version-feature-claim-diagnostics",
        probe="compile-feature-gate-sidecars-and-fail-closed-import-version-diagnostic",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "advanced_feature_gate_profiles": advanced_feature_gate.get(
                "targeted_profile_ids"
            ),
            "release_candidate_matrix_rows": len(
                release_candidate_matrix.get("matrix_rows", [])
            ),
            "import_version_negative_returncode": import_version_negative[
                "returncode"
            ],
            "import_version_negative_diagnostics_path": import_version_negative[
                "diagnostics_path"
            ],
        },
    )

def check_runtime_packaging_bridge_loader_artifact_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "runtime-packaging-bridge-loader-artifact-surface"
    provider_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    consumer_compile_dir = case_dir / "consumer"
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_compile_dir / "module.runtime-import-surface.json"),
        ],
    )

    link_plan = json.loads(
        (
            consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
        ).read_text(encoding="utf-8")
    )
    cross_module_linker_rsp = (
        consumer_compile_dir / "module.cross-module-runtime-linker-options.rsp"
    )
    runtime_metadata_linker_rsp = (
        consumer_compile_dir / "module.runtime-metadata-linker-options.rsp"
    )
    cross_module_linker_flags = cross_module_linker_rsp.read_text(encoding="utf-8")
    runtime_metadata_linker_flags = runtime_metadata_linker_rsp.read_text(
        encoding="utf-8"
    )

    for artifact_name in (
        "module.interop-bridge.h",
        "module.interop-bridge.modulemap",
        "module.interop-bridge.json",
    ):
        expect(
            (provider_compile_dir / artifact_name).is_file(),
            f"expected provider compile to publish {artifact_name}",
        )
    expect(
        cross_module_linker_rsp.is_file() and runtime_metadata_linker_rsp.is_file(),
        "expected consumer compile to publish both cross-module and runtime-metadata linker response artifacts",
    )
    expect(
        link_plan.get("linker_response_artifact")
        == "module.cross-module-runtime-linker-options.rsp",
        "expected runtime package loader link plan to preserve the cross-module linker response artifact name",
    )
    expect(
        link_plan.get("expected_interop_bridge_header_artifact_relative_path")
        == "module.interop-bridge.h"
        and link_plan.get("expected_interop_bridge_module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and link_plan.get("expected_interop_bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected runtime package loader link plan to preserve the bridge artifact paths",
    )
    expect(
        isinstance(link_plan.get("link_object_artifacts"), list)
        and len(link_plan["link_object_artifacts"]) == 2,
        "expected runtime package loader link plan to preserve both provider and consumer link objects",
    )
    expect(
        isinstance(link_plan.get("driver_linker_flags"), list)
        and len(link_plan["driver_linker_flags"]) == 2
        and "objc3_runtime_metadata_link_anchor" in cross_module_linker_flags
        and "objc3_runtime_metadata_link_anchor" in runtime_metadata_linker_flags,
        "expected runtime package loader artifacts to preserve the metadata anchor linker flags",
    )

    return CaseResult(
        case_id="runtime-packaging-bridge-loader-artifact-surface",
        probe="compile-artifact-and-linker-response-inspection",
        fixture=INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "linker_response_artifact": link_plan.get("linker_response_artifact"),
            "link_object_count": len(link_plan.get("link_object_artifacts", [])),
            "driver_linker_flag_count": len(link_plan.get("driver_linker_flags", [])),
            "bridge_header_path": "module.interop-bridge.h",
        },
    )

def check_mixed_image_package_lowering_bridge_emission_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "mixed-image-package-lowering-bridge-emission"
    provider_fixture = ROOT / Path(INTEROP_HEADER_MODULE_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_HEADER_MODULE_CONSUMER_FIXTURE)

    _, provider_ll_path, _ = compile_fixture_outputs_with_args(
        provider_fixture,
        case_dir / "provider",
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    compile_fixture_with_args(
        consumer_fixture,
        case_dir / "consumer",
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(case_dir / "provider" / "module.runtime-import-surface.json"),
        ],
    )

    provider_ll = provider_ll_path.read_text(encoding="utf-8")
    provider_bridge_json = json.loads(
        ((case_dir / "provider") / "module.interop-bridge.json").read_text(
            encoding="utf-8"
        )
    )
    link_plan = json.loads(
        ((case_dir / "consumer") / "module.cross-module-runtime-link-plan.json").read_text(
            encoding="utf-8"
        )
    )

    for needle, label in (
        ("interop_interop_lowering_abi_contract", "interop ABI lowering summary"),
        ("interop_ffi_metadata_interface_preservation", "ffi metadata/interface preservation summary"),
        ("interop_header_module_and_bridge_generation", "bridge-generation lowering summary"),
        ("declare i32 @ffiInbound(i32)", "imported bridge declaration"),
        ("declare i32 @ffiHeaderBridge(i32)", "local bridge declaration"),
    ):
        expect(
            needle in provider_ll,
            f"expected provider lowering to publish the {label} in LLVM IR",
        )
    expect(
        isinstance(provider_bridge_json.get("foreign_callables"), list)
        and {entry.get('name') for entry in provider_bridge_json['foreign_callables']}
        == {"ffiInbound", "ffiHeaderBridge"},
        "expected provider bridge emission to publish both interop callables",
    )
    expect(
        link_plan.get("interop_header_module_bridge_imported_module_count") == 1
        and link_plan.get("interop_ffi_imported_module_count") == 1,
        "expected mixed-image consumer packaging to preserve one imported interop module across both bridge surfaces",
    )
    expect(
        link_plan.get("expected_interop_bridge_artifact_relative_path")
        == "module.interop-bridge.json"
        and "m274_header_module_bridge_provider"
        in link_plan.get("interop_header_module_bridge_imported_module_names_lexicographic", []),
        "expected mixed-image consumer packaging to preserve the emitted bridge artifact identity",
    )

    return CaseResult(
        case_id="mixed-image-package-lowering-bridge-emission",
        probe="compile-llvm-ir-runtime-import-surface-and-cross-module-link-plan",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "provider_ll_path": str(provider_ll_path.relative_to(ROOT)).replace(
                "\\", "/"
            ),
            "foreign_callable_count": len(provider_bridge_json.get("foreign_callables", [])),
            "imported_bridge_module_count": link_plan.get(
                "interop_header_module_bridge_imported_module_count"
            ),
        },
    )

def check_cross_language_replay_import_surface_preservation_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "cross-language-replay-import-surface-preservation"
    provider_fixture = ROOT / Path(INTEROP_HEADER_MODULE_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_HEADER_MODULE_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_import_surface = json.loads(
        (provider_compile_dir / "module.runtime-import-surface.json").read_text(
            encoding="utf-8"
        )
    )
    consumer_compile_dir = case_dir / "consumer"
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_compile_dir / "module.runtime-import-surface.json"),
        ],
    )
    link_plan = json.loads(
        (
            consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
        ).read_text(encoding="utf-8")
    )
    imported_modules = link_plan.get("imported_modules", [])
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected replay-preservation consumer compile to publish one imported module",
    )
    imported_module = imported_modules[0]
    provider_ffi_surface = provider_import_surface.get(
        "objc_interop_foreign_surface_interface_and_module_preservation", {}
    )
    provider_bridge_surface = provider_import_surface.get(
        "objc_interop_header_module_and_bridge_generation", {}
    )

    expect(
        imported_module.get("interop_ffi_replay_key")
        == provider_bridge_surface.get("preservation_replay_key")
        and imported_module.get("interop_ffi_lowering_replay_key")
        in imported_module.get("interop_ffi_replay_key", ""),
        "expected consumer link plan to preserve the imported ffi replay and lowering replay keys",
    )
    expect(
        imported_module.get("interop_header_module_bridge_replay_key")
        == provider_bridge_surface.get("replay_key")
        and imported_module.get("interop_header_module_bridge_preservation_replay_key")
        == provider_bridge_surface.get("preservation_replay_key"),
        "expected consumer link plan to preserve the imported bridge replay and preservation replay keys",
    )
    expect(
        imported_module.get("interop_ffi_preservation_replay_key")
        == provider_ffi_surface.get("replay_key"),
        "expected consumer link plan to preserve the imported ffi preservation replay key through the full ffi preservation packet",
    )
    expect(
        imported_module.get("interop_ffi_source_contract_id")
        == "objc3c.interop.foreign.call.and.lifetime.lowering.v1"
        and imported_module.get("interop_header_module_bridge_source_contract_id")
        == "objc3c.interop.bridge.packaging.and.toolchain.contract.v1",
        "expected consumer link plan to preserve the imported replay source contracts",
    )

    return CaseResult(
        case_id="cross-language-replay-import-surface-preservation",
        probe="compile-runtime-import-surface-and-cross-module-link-plan-replay-key-inspection",
        fixture=INTEROP_HEADER_MODULE_PROVIDER_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "imported_module_name": imported_module.get("module_name"),
            "ffi_replay_key": imported_module.get("interop_ffi_replay_key"),
            "bridge_replay_key": imported_module.get(
                "interop_header_module_bridge_replay_key"
            ),
        },
    )

def check_runtime_package_loader_bridge_abi_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "runtime-package-loader-bridge-abi"

    packaging_probe = ROOT / Path(INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE)
    packaging_exe = case_dir / "bridge_packaging_toolchain_probe.exe"
    compile_probe(clangxx, packaging_probe, packaging_exe, [])
    packaging_payload = parse_key_value_output(
        run_probe(packaging_exe), "runtime package-loader packaging-topology ABI probe"
    )
    for field_name, expected_value in {
        "copy_status": 0,
        "packaging_topology_ready": 1,
        "operator_visible_evidence_ready": 1,
        "header_generation_ready": 0,
        "module_generation_ready": 0,
        "bridge_generation_ready": 0,
        "deterministic": 1,
    }.items():
        expect(
            packaging_payload.get(field_name) == expected_value,
            f"expected runtime package-loader ABI probe to preserve {field_name}",
        )

    bridge_probe = ROOT / Path(INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE)
    bridge_exe = case_dir / "header_module_bridge_generation_probe.exe"
    compile_probe(clangxx, bridge_probe, bridge_exe, [])
    bridge_payload = parse_key_value_output(
        run_probe(bridge_exe), "runtime bridge-generation ABI probe"
    )
    for field_name, expected_value in {
        "copy_status": 0,
        "runtime_generation_ready": 1,
        "cross_module_packaging_ready": 1,
        "header_generation_ready": 1,
        "module_generation_ready": 1,
        "bridge_generation_ready": 1,
        "deterministic": 1,
    }.items():
        expect(
            bridge_payload.get(field_name) == expected_value,
            f"expected runtime bridge-generation ABI probe to preserve {field_name}",
        )
    expect(
        bridge_payload.get("header_artifact_relative_path") == "module.interop-bridge.h"
        and bridge_payload.get("module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and bridge_payload.get("bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected runtime bridge-generation ABI probe to preserve the bridge artifact paths",
    )

    return CaseResult(
        case_id="runtime-package-loader-bridge-abi",
        probe="linked-runtime-abi-probes",
        fixture=None,
        claim_class="runtime-linked-execution",
        passed=True,
        summary={
            "packaging_topology_ready": packaging_payload.get(
                "packaging_topology_ready"
            ),
            "bridge_generation_ready": bridge_payload.get("bridge_generation_ready"),
            "header_artifact_relative_path": bridge_payload.get(
                "header_artifact_relative_path"
            ),
        },
    )

def check_live_package_loading_interop_runtime_implementation_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "live-package-loading-interop-runtime-implementation"
    provider_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    consumer_compile_dir = case_dir / "consumer"
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_compile_dir / "module.runtime-import-surface.json"),
        ],
    )
    link_plan = json.loads(
        (
            consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
        ).read_text(encoding="utf-8")
    )
    provider_bridge_json = json.loads(
        (provider_compile_dir / "module.interop-bridge.json").read_text(
            encoding="utf-8"
        )
    )

    packaging_probe = ROOT / Path(INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE)
    packaging_exe = case_dir / "bridge_packaging_toolchain_probe.exe"
    compile_probe(clangxx, packaging_probe, packaging_exe, [])
    packaging_payload = parse_key_value_output(
        run_probe(packaging_exe), "live package-loading interop packaging-topology probe"
    )

    bridge_probe = ROOT / Path(INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE)
    bridge_exe = case_dir / "header_module_bridge_generation_probe.exe"
    compile_probe(clangxx, bridge_probe, bridge_exe, [])
    bridge_payload = parse_key_value_output(
        run_probe(bridge_exe), "live package-loading interop bridge-generation probe"
    )

    expect(
        packaging_payload.get("runtime_support_library_archive_relative_path")
        == link_plan.get("runtime_support_library_archive_relative_path"),
        "expected live package-loader runtime snapshot to preserve the emitted runtime archive path",
    )
    expect(
        bridge_payload.get("header_artifact_relative_path")
        == link_plan.get("expected_interop_bridge_header_artifact_relative_path")
        == provider_bridge_json.get("header_artifact_relative_path")
        == "module.interop-bridge.h",
        "expected live package-loading runtime snapshot to preserve the emitted bridge header path",
    )
    expect(
        bridge_payload.get("module_artifact_relative_path")
        == link_plan.get("expected_interop_bridge_module_artifact_relative_path")
        == provider_bridge_json.get("module_artifact_relative_path")
        == "module.interop-bridge.modulemap",
        "expected live package-loading runtime snapshot to preserve the emitted bridge modulemap path",
    )
    expect(
        bridge_payload.get("bridge_artifact_relative_path")
        == link_plan.get("expected_interop_bridge_artifact_relative_path")
        == provider_bridge_json.get("bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "expected live package-loading runtime snapshot to preserve the emitted bridge json path",
    )
    expect(
        packaging_payload.get("packaging_topology_ready") == 1
        and bridge_payload.get("cross_module_packaging_ready") == 1
        and link_plan.get("ready") is True,
        "expected compile artifacts and runtime snapshots to agree on package-loading readiness",
    )

    return CaseResult(
        case_id="live-package-loading-interop-runtime-implementation",
        probe="compile-artifact-plus-linked-runtime-snapshot-integration",
        fixture=INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        claim_class="runtime-linked-execution",
        passed=True,
        summary={
            "runtime_support_library_archive_relative_path": packaging_payload.get(
                "runtime_support_library_archive_relative_path"
            ),
            "bridge_header_artifact_relative_path": bridge_payload.get(
                "header_artifact_relative_path"
            ),
            "link_plan_ready": link_plan.get("ready"),
        },
    )

def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
