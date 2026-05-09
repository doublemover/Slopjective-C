"""Interop Packaging runtime acceptance domain."""

from __future__ import annotations

from typing import Any

from ..case_result import CaseResult
from objc3c_runtime_acceptance.domains.interop_packaging_cases import (
    check_imported_runtime_packaging_replay_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_source_cases import (
    check_cross_module_runtime_package_interop_source_surface_case,
    check_textual_binary_interface_parity_source_surface_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_semantic_cases import (
    check_mixed_image_compatibility_interop_semantics_case,
    check_c_cpp_swift_bridge_compatibility_semantics_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_diagnostic_cases import (
    check_import_version_feature_claim_diagnostics_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_artifact_cases import (
    check_runtime_packaging_bridge_loader_artifact_surface_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_lowering_cases import (
    check_mixed_image_package_lowering_bridge_emission_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_replay_cases import (
    check_cross_language_replay_import_surface_preservation_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_runtime_cases import (
    check_runtime_package_loader_bridge_abi_case,
    check_live_package_loading_interop_runtime_implementation_case,
)
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

def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
