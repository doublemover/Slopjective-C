#!/usr/bin/env python3
"""Compile and run the live objc3 runtime acceptance workload."""

from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from time import perf_counter
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-runtime-acceptance"
REPORT_ROOT = ROOT / "tmp" / "reports" / "runtime" / "acceptance"
BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PWSH = shutil.which("pwsh") or shutil.which("powershell") or "pwsh"
RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID = "objc3c.runtime.state.publication.surface.v1"
RUNTIME_STATE_PUBLICATION_SURFACE_KIND = "compile-manifest-plus-registration-manifest"
RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID = "objc3c.runtime.bootstrap.registration.source.surface.v1"
RUNTIME_BOOTSTRAP_LOWERING_REGISTRATION_ARTIFACT_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.bootstrap.lowering.registration.artifact.surface.v1"
)
RUNTIME_MULTI_IMAGE_STARTUP_ORDERING_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.multi.image.startup.ordering.source.surface.v1"
)
RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.object.model.realization.source.surface.v1"
)
RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.property.ivar.storage.accessor.source.surface.v1"
)
RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.property.atomicity.synthesis.reflection.source.surface.v1"
)
RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.realization.lowering.reflection.artifact.surface.v1"
)
RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1"
)
RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.cross.module.realized.metadata.replay.preservation.surface.v1"
)
RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.object.model.abi.query.surface.v1"
)
RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.realization.lookup.reflection.implementation.surface.v1"
)
RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.reflection.query.surface.v1"
)
RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.realization.lookup.semantics.v1"
)
RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.class.metaclass.protocol.realization.v1"
)
RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.category.attachment.merged.dispatch.surface.v1"
)
RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.reflection.visibility.coherence.diagnostics.surface.v1"
)
RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID = "objc3c.runtime.acceptance.suite.surface.v1"
RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID = "objc3c.runtime.installation.abi.surface.v1"
RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID = "objc3c.runtime.loader.lifecycle.surface.v1"
RUNTIME_DUPLICATE_INSTALL_DIAGNOSTIC_MODEL = (
    "duplicate-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state"
)
RUNTIME_OUT_OF_ORDER_INSTALL_DIAGNOSTIC_MODEL = (
    "out-of-order-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state"
)
RUNTIME_PUBLIC_HEADER_PATH = "native/objc3c/src/runtime/objc3_runtime.h"
RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH = (
    "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
)
INSTALLATION_LIFECYCLE_FIXTURE = (
    "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3"
)
INSTALLATION_LIFECYCLE_PROBE = (
    "tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp"
)
IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/m258_d002_runtime_packaging_provider.objc3"
)
IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/m258_d002_runtime_packaging_consumer.objc3"
)
IMPORTED_RUNTIME_PACKAGING_PROBE = (
    "tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp"
)
REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE = (
    "tests/tooling/runtime/m259_d002_realization_lookup_reflection_runtime_probe.cpp"
)
RUNTIME_ACCEPTANCE_COMMAND = "python scripts/check_objc3c_runtime_acceptance.py"
VALIDATE_RUNTIME_ARCHITECTURE_COMMAND = (
    "python scripts/objc3c_public_workflow_runner.py validate-runtime-architecture"
)
PUBLIC_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_register_image",
    "objc3_runtime_lookup_selector",
    "objc3_runtime_dispatch_i32",
    "objc3_runtime_reset_for_testing",
]
RUNTIME_INSTALLATION_ABI_BOUNDARY = [
    "objc3_runtime_register_image",
    "objc3_runtime_copy_registration_state_for_testing",
    "objc3_runtime_reset_for_testing",
]
RUNTIME_LOADER_TESTING_BOUNDARY = [
    "objc3_runtime_stage_registration_table_for_bootstrap",
    "objc3_runtime_copy_image_walk_state_for_testing",
    "objc3_runtime_replay_registered_images_for_testing",
    "objc3_runtime_copy_reset_replay_state_for_testing",
]
COMPILE_PROVENANCE_CONTRACT_ID = "objc3c.native.compile.output.provenance.v1"
COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID = "objc3c.native.compile.output.truthfulness.v1"


def run(command: list[str], *, cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        cwd=str(cwd or ROOT),
        text=True,
        capture_output=True,
        check=False,
    )


def ensure_native_binaries() -> None:
    if NATIVE_EXE.is_file() and RUNTIME_LIB.is_file():
        return
    result = run(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(BUILD_PS1),
            "-ExecutionMode",
            "binaries-only",
        ]
    )
    if result.returncode != 0:
        raise RuntimeError(
            "native build failed:\nSTDOUT:\n"
            + result.stdout
            + "\nSTDERR:\n"
            + result.stderr
        )
    if not NATIVE_EXE.is_file() or not RUNTIME_LIB.is_file():
        raise RuntimeError("native build completed without publishing the runtime executable/library")


def find_clangxx() -> str:
    llvm_root = os.environ.get("LLVM_ROOT")
    if llvm_root:
        candidate = Path(llvm_root) / "bin" / "clang++.exe"
        if candidate.is_file():
            return str(candidate)
    candidate = shutil.which("clang++")
    if candidate:
        return candidate
    raise RuntimeError("clang++ not found; set LLVM_ROOT or ensure clang++ is on PATH")


def compile_fixture_with_args(
    fixture: Path, out_dir: Path, extra_args: list[str] | None = None
) -> Path:
    out_dir.mkdir(parents=True, exist_ok=True)
    result = run(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(COMPILE_PS1),
            str(fixture),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
            *(extra_args or []),
        ]
    )
    if result.returncode != 0:
        raise RuntimeError(
            f"fixture compile failed for {fixture}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )
    obj_path = out_dir / "module.obj"
    manifest_path = out_dir / "module.manifest.json"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    registration_descriptor_path = out_dir / "module.runtime-registration-descriptor.json"
    ll_path = out_dir / "module.ll"
    provenance_path = out_dir / "module.compile-provenance.json"
    if not obj_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {obj_path}")
    if not manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {manifest_path}")
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")
    if not registration_descriptor_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_descriptor_path}")
    if not ll_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {ll_path}")
    if not provenance_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {provenance_path}")

    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    provenance = json.loads(provenance_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))
    registration_descriptor = json.loads(registration_descriptor_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    semantic_surface = pipeline.get("semantic_surface", {}) if isinstance(pipeline, dict) else {}
    bootstrap_source_surface = manifest.get("runtime_bootstrap_registration_source_surface")
    bootstrap_lowering_registration_artifact_surface = manifest.get(
        "runtime_bootstrap_lowering_registration_artifact_surface"
    )
    multi_image_startup_ordering_source_surface = manifest.get(
        "runtime_multi_image_startup_ordering_source_surface"
    )
    object_model_realization_source_surface = manifest.get(
        "runtime_object_model_realization_source_surface"
    )
    property_ivar_storage_accessor_source_surface = manifest.get(
        "runtime_property_ivar_storage_accessor_source_surface"
    )
    property_atomicity_synthesis_reflection_source_surface = manifest.get(
        "runtime_property_atomicity_synthesis_reflection_source_surface"
    )
    realization_lowering_reflection_artifact_surface = manifest.get(
        "runtime_realization_lowering_reflection_artifact_surface"
    )
    dispatch_table_reflection_record_lowering_surface = manifest.get(
        "runtime_dispatch_table_reflection_record_lowering_surface"
    )
    object_model_abi_query_surface = manifest.get("runtime_object_model_abi_query_surface")
    realization_lookup_reflection_implementation_surface = manifest.get(
        "runtime_realization_lookup_reflection_implementation_surface"
    )
    reflection_query_surface = manifest.get("runtime_reflection_query_surface")
    realization_lookup_semantics_surface = manifest.get(
        "runtime_realization_lookup_semantics_surface"
    )
    class_metaclass_protocol_realization_surface = manifest.get(
        "runtime_class_metaclass_protocol_realization_surface"
    )
    category_attachment_merged_dispatch_surface = manifest.get(
        "runtime_category_attachment_merged_dispatch_surface"
    )
    reflection_visibility_coherence_diagnostics_surface = manifest.get(
        "runtime_reflection_visibility_coherence_diagnostics_surface"
    )
    registration_descriptor_frontend_closure = semantic_surface.get(
        "objc_runtime_registration_descriptor_frontend_closure",
        manifest.get("objc_runtime_registration_descriptor_frontend_closure", {}),
    )
    registration_descriptor_image_root_source_surface = semantic_surface.get(
        "objc_runtime_registration_descriptor_image_root_source_surface",
        manifest.get("objc_runtime_registration_descriptor_image_root_source_surface", {}),
    )
    translation_unit_registration_manifest = semantic_surface.get(
        "objc_runtime_translation_unit_registration_manifest",
        manifest.get("objc_runtime_translation_unit_registration_manifest", {}),
    )
    runtime_bootstrap_lowering = semantic_surface.get(
        "objc_runtime_bootstrap_lowering_contract",
        manifest.get(
            "objc_runtime_bootstrap_lowering_contract",
            manifest.get("objc_runtime_bootstrap_lowering", {}),
        ),
    )
    bootstrap_legality_semantics = semantic_surface.get(
        "objc_runtime_bootstrap_legality_semantics",
        manifest.get("objc_runtime_bootstrap_legality_semantics", {}),
    )
    bootstrap_failure_restart_semantics = semantic_surface.get(
        "objc_runtime_bootstrap_failure_restart_semantics",
        manifest.get("objc_runtime_bootstrap_failure_restart_semantics", {}),
    )
    runtime_bootstrap_semantics = semantic_surface.get(
        "objc_runtime_startup_bootstrap_semantics",
        manifest.get("objc_runtime_startup_bootstrap_semantics", {}),
    )
    bootstrap_api_contract = semantic_surface.get(
        "objc_runtime_bootstrap_api_contract",
        manifest.get("objc_runtime_bootstrap_api_contract", {}),
    )
    bootstrap_reset_contract = semantic_surface.get(
        "objc_runtime_bootstrap_reset_contract",
        manifest.get("objc_runtime_bootstrap_reset_contract", {}),
    )
    bootstrap_registrar_contract = semantic_surface.get(
        "objc_runtime_bootstrap_registrar_contract",
        manifest.get("objc_runtime_bootstrap_registrar_contract", {}),
    )
    bootstrap_archive_static_link_replay_corpus = semantic_surface.get(
        "objc_runtime_bootstrap_archive_static_link_replay_corpus",
        manifest.get("objc_runtime_bootstrap_archive_static_link_replay_corpus", {}),
    )
    publication_surface = manifest.get("runtime_state_publication_surface")
    runtime_installation_abi_surface = manifest.get("runtime_installation_abi_surface")
    runtime_loader_lifecycle_surface = manifest.get("runtime_loader_lifecycle_surface")
    if not isinstance(publication_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_state_publication_surface")
    if publication_surface.get("contract_id") != RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID:
        raise RuntimeError("compiled fixture manifest published the wrong runtime_state_publication_surface contract")
    if publication_surface.get("publication_surface_kind") != RUNTIME_STATE_PUBLICATION_SURFACE_KIND:
        raise RuntimeError("compiled fixture manifest published the wrong runtime_state_publication_surface kind")
    if publication_surface.get("compile_manifest_artifact") != "module.manifest.json":
        raise RuntimeError("runtime_state_publication_surface drifted from the compile manifest artifact path")
    if publication_surface.get("registration_manifest_artifact") != "module.runtime-registration-manifest.json":
        raise RuntimeError("runtime_state_publication_surface drifted from the runtime registration manifest artifact path")
    if publication_surface.get("object_artifact") != "module.obj":
        raise RuntimeError("runtime_state_publication_surface drifted from the emitted object artifact path")
    if publication_surface.get("backend_artifact") != "module.ll":
        raise RuntimeError("runtime_state_publication_surface drifted from the emitted LLVM IR artifact path")
    if publication_surface.get("runtime_support_library_archive_relative_path") != registration_manifest.get("runtime_support_library_archive_relative_path"):
        raise RuntimeError("runtime_state_publication_surface drifted from the runtime registration manifest archive path")
    if publication_surface.get("registration_entrypoint_symbol") != registration_manifest.get("registration_entrypoint_symbol"):
        raise RuntimeError("runtime_state_publication_surface drifted from the runtime registration entrypoint symbol")
    if publication_surface.get("runtime_state_snapshot_symbol") != registration_manifest.get("runtime_state_snapshot_symbol"):
        raise RuntimeError("runtime_state_publication_surface drifted from the runtime state snapshot symbol")
    if publication_surface.get("public_runtime_abi_boundary") != PUBLIC_RUNTIME_ABI_BOUNDARY:
        raise RuntimeError("runtime_state_publication_surface drifted from the public runtime ABI boundary")
    for field in (
        "class_descriptor_count",
        "protocol_descriptor_count",
        "category_descriptor_count",
        "property_descriptor_count",
        "ivar_descriptor_count",
        "total_descriptor_count",
    ):
        if publication_surface.get(field) != registration_manifest.get(field):
            raise RuntimeError(f"runtime_state_publication_surface drifted from registration manifest field {field}")
    if publication_surface.get("publication_requires_coupled_registration_manifest") is not True:
        raise RuntimeError("runtime_state_publication_surface must require the coupled runtime registration manifest")
    if publication_surface.get("publication_requires_real_compile_output") is not True:
        raise RuntimeError("runtime_state_publication_surface must require real compile output")
    if not isinstance(bootstrap_source_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_bootstrap_registration_source_surface")
    if bootstrap_source_surface.get("contract_id") != RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID:
        raise RuntimeError("compiled fixture manifest published the wrong runtime_bootstrap_registration_source_surface contract")
    if bootstrap_source_surface.get("compile_manifest_artifact") != "module.manifest.json":
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the compile manifest artifact path")
    if bootstrap_source_surface.get("registration_manifest_artifact") != "module.runtime-registration-manifest.json":
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the runtime registration manifest artifact path")
    if bootstrap_source_surface.get("registration_descriptor_artifact") != "module.runtime-registration-descriptor.json":
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the runtime registration descriptor artifact path")
    if bootstrap_source_surface.get("object_artifact") != "module.obj":
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the emitted object artifact path")
    if bootstrap_source_surface.get("backend_artifact") != "module.ll":
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the emitted LLVM IR artifact path")
    if bootstrap_source_surface.get("source_surface_contract_id") != registration_descriptor_image_root_source_surface.get("contract_id"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the registration-descriptor image-root source contract")
    if bootstrap_source_surface.get("frontend_closure_contract_id") != registration_descriptor_frontend_closure.get("contract_id"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the registration-descriptor frontend closure contract")
    if bootstrap_source_surface.get("registration_manifest_contract_id") != translation_unit_registration_manifest.get("contract_id"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the registration manifest contract")
    if bootstrap_source_surface.get("bootstrap_lowering_contract_id") != runtime_bootstrap_lowering.get("contract_id"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the bootstrap lowering contract")
    if bootstrap_source_surface.get("runtime_support_library_archive_relative_path") != registration_manifest.get("runtime_support_library_archive_relative_path"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the runtime support library archive path")
    if bootstrap_source_surface.get("registration_descriptor_identifier") != registration_descriptor_frontend_closure.get("registration_descriptor_identifier"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the registration descriptor identifier")
    if bootstrap_source_surface.get("image_root_identifier") != registration_descriptor_frontend_closure.get("image_root_identifier"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the image root identifier")
    if bootstrap_source_surface.get("registration_entrypoint_symbol") != registration_manifest.get("registration_entrypoint_symbol"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the registration entrypoint symbol")
    if bootstrap_source_surface.get("constructor_root_symbol") != registration_manifest.get("constructor_root_symbol"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the constructor root symbol")
    if bootstrap_source_surface.get("translation_unit_identity_key") != registration_manifest.get("translation_unit_identity_key"):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the translation unit identity key")
    if (
        bootstrap_source_surface.get("translation_unit_registration_order_ordinal")
        != registration_manifest.get("translation_unit_registration_order_ordinal")
    ):
        raise RuntimeError("runtime_bootstrap_registration_source_surface drifted from the translation unit registration order ordinal")
    if bootstrap_source_surface.get("requires_coupled_registration_descriptor_artifact") is not True:
        raise RuntimeError("runtime_bootstrap_registration_source_surface must require the coupled runtime registration descriptor artifact")
    if bootstrap_source_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError("runtime_bootstrap_registration_source_surface must require the coupled runtime registration manifest")
    if bootstrap_source_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError("runtime_bootstrap_registration_source_surface must require real compile output")
    if not isinstance(bootstrap_lowering_registration_artifact_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_bootstrap_lowering_registration_artifact_surface"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get("contract_id")
        != RUNTIME_BOOTSTRAP_LOWERING_REGISTRATION_ARTIFACT_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_bootstrap_lowering_registration_artifact_surface contract"
        )
    if bootstrap_lowering_registration_artifact_surface.get("compile_manifest_artifact") != "module.manifest.json":
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the compile manifest artifact path"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get("registration_manifest_artifact")
        != "module.runtime-registration-manifest.json"
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get("registration_descriptor_artifact")
        != "module.runtime-registration-descriptor.json"
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the runtime registration descriptor artifact path"
        )
    if bootstrap_lowering_registration_artifact_surface.get("object_artifact") != "module.obj":
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the emitted object artifact path"
        )
    if bootstrap_lowering_registration_artifact_surface.get("backend_artifact") != "module.ll":
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the emitted LLVM IR artifact path"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get("bootstrap_lowering_contract_id")
        != runtime_bootstrap_lowering.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the bootstrap lowering contract"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get("registration_manifest_contract_id")
        != translation_unit_registration_manifest.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the registration manifest contract"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get("bootstrap_semantics_contract_id")
        != runtime_bootstrap_semantics.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the bootstrap semantics contract"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get(
            "registration_descriptor_frontend_closure_contract_id"
        )
        != registration_descriptor_frontend_closure.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the registration-descriptor frontend closure contract"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get(
            "runtime_support_library_archive_relative_path"
        )
        != registration_manifest.get("runtime_support_library_archive_relative_path")
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the runtime support library archive path"
        )
    for surface_field, lowering_field in (
        ("constructor_root_symbol", "constructor_root_symbol"),
        ("init_stub_symbol_prefix", "constructor_init_stub_symbol_prefix"),
        ("registration_table_symbol_prefix", "registration_table_symbol_prefix"),
        ("image_local_init_state_symbol_prefix", "image_local_init_state_symbol_prefix"),
        ("registration_entrypoint_symbol", "registration_entrypoint_symbol"),
        ("global_ctor_list_model", "global_ctor_list_model"),
        ("registration_table_layout_model", "registration_table_layout_model"),
        ("image_local_initialization_model", "image_local_initialization_model"),
        ("registration_table_abi_version", "registration_table_abi_version"),
        ("registration_table_pointer_field_count", "registration_table_pointer_field_count"),
        ("constructor_root_emission_state", "constructor_root_emission_state"),
        ("init_stub_emission_state", "init_stub_emission_state"),
        ("registration_table_emission_state", "registration_table_emission_state"),
        ("bootstrap_ir_materialization_landed", "bootstrap_ir_materialization_landed"),
        ("image_local_initialization_landed", "image_local_initialization_landed"),
    ):
        if (
            bootstrap_lowering_registration_artifact_surface.get(surface_field)
            != runtime_bootstrap_lowering.get(lowering_field)
        ):
            raise RuntimeError(
                "runtime_bootstrap_lowering_registration_artifact_surface drifted "
                f"from bootstrap lowering field {lowering_field}"
            )
    if (
        bootstrap_lowering_registration_artifact_surface.get(
            "requires_coupled_registration_descriptor_artifact"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface must require the coupled runtime registration descriptor artifact"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface must require the coupled runtime registration manifest"
        )
    if bootstrap_lowering_registration_artifact_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface must require real compile output"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get(
            "lowered_registration_descriptor_fields"
        )
        != [
            "constructor_init_stub_symbol",
            "bootstrap_registration_table_symbol",
            "bootstrap_image_local_init_state_symbol",
            "bootstrap_registration_table_layout_model",
            "bootstrap_image_local_initialization_model",
            "bootstrap_registration_table_abi_version",
            "bootstrap_registration_table_pointer_field_count",
            "translation_unit_registration_order_ordinal",
        ]
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the lowered registration descriptor field set"
        )
    if (
        bootstrap_lowering_registration_artifact_surface.get(
            "loader_table_ir_proof_fields"
        )
        != [
            "constructor_root_symbol",
            "constructor_init_stub_symbol",
            "bootstrap_registration_table_symbol",
            "bootstrap_image_local_init_state_symbol",
            "translation_unit_registration_order_ordinal",
        ]
    ):
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface drifted from the loader-table IR proof field set"
        )
    if bootstrap_lowering_registration_artifact_surface.get("requires_emitted_loader_table_ir") is not True:
        raise RuntimeError(
            "runtime_bootstrap_lowering_registration_artifact_surface must require emitted loader-table IR"
        )
    if registration_descriptor.get("bootstrap_lowering_contract_id") != runtime_bootstrap_lowering.get(
        "contract_id"
    ):
        raise RuntimeError(
            "runtime registration descriptor drifted from the bootstrap lowering contract"
        )
    for descriptor_field, lowering_field in (
        ("bootstrap_registration_table_layout_model", "registration_table_layout_model"),
        ("bootstrap_image_local_initialization_model", "image_local_initialization_model"),
        ("bootstrap_constructor_root_emission_state", "constructor_root_emission_state"),
        ("bootstrap_init_stub_emission_state", "init_stub_emission_state"),
        ("bootstrap_registration_table_emission_state", "registration_table_emission_state"),
        ("bootstrap_registration_table_abi_version", "registration_table_abi_version"),
        ("bootstrap_registration_table_pointer_field_count", "registration_table_pointer_field_count"),
    ):
        if registration_descriptor.get(descriptor_field) != runtime_bootstrap_lowering.get(
            lowering_field
        ):
            raise RuntimeError(
                f"runtime registration descriptor drifted from bootstrap lowering field {lowering_field}"
            )
    for descriptor_field, manifest_field in (
        ("constructor_root_symbol", "constructor_root_symbol"),
        ("registration_entrypoint_symbol", "registration_entrypoint_symbol"),
        ("constructor_init_stub_symbol", "constructor_init_stub_symbol"),
        ("bootstrap_registration_table_symbol", "bootstrap_registration_table_symbol"),
        ("bootstrap_image_local_init_state_symbol", "bootstrap_image_local_init_state_symbol"),
        ("translation_unit_registration_order_ordinal", "translation_unit_registration_order_ordinal"),
    ):
        if registration_descriptor.get(descriptor_field) != registration_manifest.get(manifest_field):
            raise RuntimeError(
                f"runtime registration descriptor drifted from registration manifest field {manifest_field}"
            )
    if registration_descriptor.get("ready_for_loader_table_lowering") is not True:
        raise RuntimeError(
            "runtime registration descriptor must be ready for loader-table lowering"
        )
    for symbol_field in (
        "constructor_root_symbol",
        "constructor_init_stub_symbol",
        "bootstrap_registration_table_symbol",
        "bootstrap_image_local_init_state_symbol",
    ):
        symbol = registration_descriptor.get(symbol_field)
        if not isinstance(symbol, str) or not symbol:
            raise RuntimeError(f"runtime registration descriptor omitted {symbol_field}")
        if f"@{symbol}" not in ll_text:
            raise RuntimeError(
                f"emitted LLVM IR did not lower runtime bootstrap symbol {symbol_field}"
            )
    ctor_loader_table_edge = (
        f"ptr @{registration_descriptor['constructor_root_symbol']}, ptr "
        f"@{registration_descriptor['bootstrap_registration_table_symbol']}"
    )
    if ctor_loader_table_edge not in ll_text:
        raise RuntimeError(
            "emitted LLVM IR did not lower the constructor-root to loader-table global ctor edge"
        )
    if not isinstance(multi_image_startup_ordering_source_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_multi_image_startup_ordering_source_surface")
    if (
        multi_image_startup_ordering_source_surface.get("contract_id")
        != RUNTIME_MULTI_IMAGE_STARTUP_ORDERING_SOURCE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_multi_image_startup_ordering_source_surface contract"
        )
    if multi_image_startup_ordering_source_surface.get("compile_manifest_artifact") != "module.manifest.json":
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the compile manifest artifact path"
        )
    if multi_image_startup_ordering_source_surface.get("registration_manifest_artifact") != "module.runtime-registration-manifest.json":
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the runtime registration manifest artifact path"
        )
    if multi_image_startup_ordering_source_surface.get("registration_descriptor_artifact") != "module.runtime-registration-descriptor.json":
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the runtime registration descriptor artifact path"
        )
    if (
        multi_image_startup_ordering_source_surface.get("bootstrap_legality_semantics_contract_id")
        != bootstrap_legality_semantics.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the bootstrap legality semantics contract"
        )
    if (
        multi_image_startup_ordering_source_surface.get("bootstrap_failure_restart_contract_id")
        != bootstrap_failure_restart_semantics.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the bootstrap failure restart contract"
        )
    if (
        multi_image_startup_ordering_source_surface.get("bootstrap_api_contract_id")
        != bootstrap_api_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the bootstrap API contract"
        )
    if (
        multi_image_startup_ordering_source_surface.get("bootstrap_reset_contract_id")
        != bootstrap_reset_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the bootstrap reset contract"
        )
    if (
        multi_image_startup_ordering_source_surface.get("bootstrap_registrar_contract_id")
        != bootstrap_registrar_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the bootstrap registrar contract"
        )
    if (
        multi_image_startup_ordering_source_surface.get("archive_static_link_replay_corpus_contract_id")
        != bootstrap_archive_static_link_replay_corpus.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the archive static-link replay corpus contract"
        )
    if multi_image_startup_ordering_source_surface.get("public_header_path") != RUNTIME_PUBLIC_HEADER_PATH:
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the public runtime header path"
        )
    if multi_image_startup_ordering_source_surface.get("internal_header_path") != RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH:
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the bootstrap internal header path"
        )
    if (
        multi_image_startup_ordering_source_surface.get("registration_entrypoint_symbol")
        != registration_manifest.get("registration_entrypoint_symbol")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the registration entrypoint symbol"
        )
    if (
        multi_image_startup_ordering_source_surface.get("state_snapshot_symbol")
        != bootstrap_api_contract.get("state_snapshot_symbol")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the runtime state snapshot symbol"
        )
    if (
        multi_image_startup_ordering_source_surface.get("reset_for_testing_symbol")
        != bootstrap_api_contract.get("reset_for_testing_symbol")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the reset-for-testing symbol"
        )
    if (
        multi_image_startup_ordering_source_surface.get("replay_registered_images_symbol")
        != bootstrap_failure_restart_semantics.get("replay_registered_images_symbol")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the replay-registered-images symbol"
        )
    if (
        multi_image_startup_ordering_source_surface.get("reset_replay_state_snapshot_symbol")
        != bootstrap_failure_restart_semantics.get("reset_replay_state_snapshot_symbol")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the reset replay state snapshot symbol"
        )
    if (
        multi_image_startup_ordering_source_surface.get("translation_unit_identity_key")
        != registration_manifest.get("translation_unit_identity_key")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the translation unit identity key"
        )
    if (
        multi_image_startup_ordering_source_surface.get("translation_unit_registration_order_ordinal")
        != registration_manifest.get("translation_unit_registration_order_ordinal")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the translation unit registration order ordinal"
        )
    if (
        multi_image_startup_ordering_source_surface.get("duplicate_registration_status_code")
        != runtime_bootstrap_semantics.get("duplicate_registration_status_code")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the duplicate registration status code"
        )
    if (
        multi_image_startup_ordering_source_surface.get("out_of_order_status_code")
        != runtime_bootstrap_semantics.get("out_of_order_status_code")
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the out-of-order registration status code"
        )
    if (
        multi_image_startup_ordering_source_surface.get("duplicate_install_diagnostic_model")
        != RUNTIME_DUPLICATE_INSTALL_DIAGNOSTIC_MODEL
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the duplicate-install diagnostic model"
        )
    if (
        multi_image_startup_ordering_source_surface.get("out_of_order_install_diagnostic_model")
        != RUNTIME_OUT_OF_ORDER_INSTALL_DIAGNOSTIC_MODEL
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the out-of-order-install diagnostic model"
        )
    if (
        multi_image_startup_ordering_source_surface.get("last_rejected_module_name_field")
        != "last_rejected_module_name"
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the rejected module-name field"
        )
    if (
        multi_image_startup_ordering_source_surface.get("last_rejected_translation_unit_identity_key_field")
        != "last_rejected_translation_unit_identity_key"
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the rejected translation-unit identity field"
        )
    if runtime_bootstrap_semantics.get("invalid_registration_roots_status_code") != -4:
        raise RuntimeError(
            "objc_runtime_startup_bootstrap_semantics drifted from the invalid registration roots status code"
        )
    if bootstrap_failure_restart_semantics.get("invalid_registration_roots_status_code") != -4:
        raise RuntimeError(
            "objc_runtime_bootstrap_failure_restart_semantics drifted from the invalid registration roots status code"
        )
    if (
        multi_image_startup_ordering_source_surface.get("next_expected_registration_order_field")
        != "next_expected_registration_order_ordinal"
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the next-expected registration order field"
        )
    if (
        multi_image_startup_ordering_source_surface.get("last_successful_registration_order_field")
        != "last_successful_registration_order_ordinal"
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the last-successful registration order field"
        )
    if (
        multi_image_startup_ordering_source_surface.get("last_rejected_registration_order_field")
        != "last_rejected_registration_order_ordinal"
    ):
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface drifted from the last-rejected registration order field"
        )
    if multi_image_startup_ordering_source_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface must require a linked runtime probe"
        )
    if multi_image_startup_ordering_source_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_multi_image_startup_ordering_source_surface must require real compile output"
        )
    if not isinstance(object_model_realization_source_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_object_model_realization_source_surface")
    if (
        object_model_realization_source_surface.get("contract_id")
        != RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_object_model_realization_source_surface contract"
        )
    if object_model_realization_source_surface.get("compile_manifest_artifact") != manifest_path.name:
        raise RuntimeError(
            "runtime_object_model_realization_source_surface drifted from the compile manifest artifact path"
        )
    if (
        object_model_realization_source_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_object_model_realization_source_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        object_model_realization_source_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_object_model_realization_source_surface drifted from the runtime registration descriptor artifact path"
        )
    if object_model_realization_source_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_object_model_realization_source_surface drifted from the emitted object artifact path"
        )
    if object_model_realization_source_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_object_model_realization_source_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_object_model_contract_fields = {
        "executable_realization_records_contract_id": "objc3c.executable.realization.records.v1",
        "runtime_class_realization_contract_id": "objc3c.runtime.class.realization.freeze.v1",
        "runtime_metaclass_graph_contract_id": "objc3c.runtime.metaclass.graph.root.class.baseline.v1",
        "runtime_category_attachment_protocol_conformance_contract_id": (
            "objc3c.runtime.category.attachment.protocol.conformance.v1"
        ),
        "canonical_runnable_object_support_contract_id": (
            "objc3c.runtime.canonical.runnable.object.sample.support.v1"
        ),
        "runtime_support_library_archive_relative_path": (
            registration_manifest.get("runtime_support_library_archive_relative_path")
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "registration_entrypoint_symbol": registration_manifest.get("registration_entrypoint_symbol"),
        "selector_lookup_symbol": "objc3_runtime_lookup_selector",
        "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
        "realized_class_graph_snapshot_symbol": "objc3_runtime_copy_realized_class_graph_state_for_testing",
        "realized_class_entry_snapshot_symbol": "objc3_runtime_copy_realized_class_entry_for_testing",
        "protocol_conformance_query_symbol": "objc3_runtime_copy_protocol_conformance_query_for_testing",
    }
    for field, expected_value in expected_object_model_contract_fields.items():
        if object_model_realization_source_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_object_model_realization_source_surface drifted from {field}"
            )
    if object_model_realization_source_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError(
            "runtime_object_model_realization_source_surface must require the coupled runtime registration manifest"
        )
    if object_model_realization_source_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_object_model_realization_source_surface must require real compile output"
        )
    if object_model_realization_source_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_object_model_realization_source_surface must require a linked runtime probe"
        )
    if not isinstance(property_ivar_storage_accessor_source_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_property_ivar_storage_accessor_source_surface"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("contract_id")
        != RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_property_ivar_storage_accessor_source_surface contract"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from the compile manifest artifact path"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from the runtime registration descriptor artifact path"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("object_artifact")
        != obj_path.name
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from the emitted object artifact path"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("backend_artifact")
        != ll_path.name
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_property_ivar_storage_accessor_source_fields = {
        "property_ast_anchor": "Objc3PropertyDecl",
        "ivar_ast_anchor": "Objc3PropertyDecl.ivar_binding_symbol",
        "source_closure_contract_id": "objc3c.executable.property.ivar.source.closure.v1",
        "source_model_completion_contract_id": (
            "objc3c.executable.property.ivar.source.model.completion.v1"
        ),
        "source_semantics_contract_id": "objc3c.executable.property.ivar.semantics.v1",
        "source_surface_model": (
            "property-ivar-storage-accessor-runtime-source-surface-freezes-ast-sema-ir-pipeline-and-runtime-codepaths-before-lowering-or-runtime-semantic-expansion"
        ),
        "layout_model": (
            "property-ivar-source-model-computes-deterministic-layout-slots-sizes-and-alignment-before-runtime-storage-realization"
        ),
        "attribute_model": (
            "property-attribute-and-effective-accessor-source-model-publishes-deterministic-ownership-and-selector-profiles"
        ),
        "synthesis_semantics_model": (
            "non-category-class-interface-properties-own-authoritative-default-ivar-and-synthesized-binding-identities-across-implementation-redeclaration-boundaries"
        ),
        "default_ivar_binding_resolution_model": (
            "matched-class-implementations-resolve-interface-declared-properties-through-authoritative-default-ivar-bindings-with-or-without-implementation-redeclaration"
        ),
        "accessor_semantics_model": (
            "readonly-and-attribute-driven-accessor-selectors-resolve-to-one-declaration-level-profile-before-body-emission"
        ),
        "accessor_selector_uniqueness_model": (
            "effective-getter-and-setter-selectors-must-be-unique-within-each-property-container-before-runtime-accessor-binding"
        ),
        "ownership_atomicity_interaction_model": (
            "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land"
        ),
        "storage_semantics_model": (
            "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation"
        ),
        "layout_init_order_field": "Objc3PropertyDecl.executable_ivar_init_order_index",
        "layout_destroy_order_field": "Objc3PropertyDecl.executable_ivar_destroy_order_index",
        "synthesizes_accessors_field": (
            "Objc3RuntimeMetadataPropertySourceRecord.synthesizes_executable_accessors"
        ),
        "getter_runtime_helper_field": (
            "Objc3RuntimeMetadataPropertySourceRecord.getter_storage_runtime_helper_symbol"
        ),
        "setter_runtime_helper_field": (
            "Objc3RuntimeMetadataPropertySourceRecord.setter_storage_runtime_helper_symbol"
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        ),
        "dispatch_accessor_runtime_abi_surface_contract_id": (
            "objc3c.runtime.dispatch_accessor.abi.surface.v1"
        ),
        "accessor_storage_lowering_metadata_model": (
            "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path"
        ),
        "accessor_storage_lowering_helper_selection_model": (
            "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers"
        ),
        "compatibility_semantics_model": (
            "protocol-and-inheritance-compatibility-compare-declaration-level-attribute-accessor-ownership-profiles-not-storage-local-layout-symbols"
        ),
        "ast_source_path": "native/objc3c/src/ast/objc3_ast.h",
        "sema_source_path": "native/objc3c/src/sema/objc3_semantic_passes.cpp",
        "ir_emitter_source_path": "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        "frontend_artifacts_source_path": (
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
        ),
        "runtime_source_path": "native/objc3c/src/runtime/objc3_runtime.cpp",
    }
    for field, expected_value in expected_property_ivar_storage_accessor_source_fields.items():
        if property_ivar_storage_accessor_source_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_property_ivar_storage_accessor_source_surface drifted from {field}"
            )
    if (
        property_ivar_storage_accessor_source_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface must require the coupled runtime registration manifest"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("requires_real_compile_output")
        is not True
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface must require real compile output"
        )
    if (
        property_ivar_storage_accessor_source_surface.get("requires_linked_runtime_probe")
        is not True
    ):
        raise RuntimeError(
            "runtime_property_ivar_storage_accessor_source_surface must require a linked runtime probe"
        )
    if not isinstance(property_atomicity_synthesis_reflection_source_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_property_atomicity_synthesis_reflection_source_surface"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get("contract_id")
        != RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_property_atomicity_synthesis_reflection_source_surface contract"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from the compile manifest artifact path"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from the runtime registration descriptor artifact path"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get("object_artifact")
        != obj_path.name
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from the emitted object artifact path"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get("backend_artifact")
        != ll_path.name
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_property_atomicity_synthesis_reflection_fields = {
        "property_storage_source_surface_contract_id": (
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID
        ),
        "property_metadata_reflection_contract_id": "objc3c.runtime.property.metadata.reflection.v1",
        "runtime_backed_object_ownership_attribute_surface_contract_id": (
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        ),
        "atomic_modifier_field": "Objc3PropertyDecl.is_atomic",
        "nonatomic_modifier_field": "Objc3PropertyDecl.is_nonatomic",
        "atomicity_conflict_field": "Objc3PropertyDecl.has_atomicity_conflict",
        "property_attribute_profile_field": "Objc3PropertyDecl.property_attribute_profile",
        "reflection_attribute_profile_field": (
            "objc3_runtime_property_entry_snapshot.property_attribute_profile"
        ),
        "ast_source_path": "native/objc3c/src/ast/objc3_ast.h",
        "sema_source_path": "native/objc3c/src/sema/objc3_semantic_passes.cpp",
        "sema_pass_manager_source_path": "native/objc3c/src/sema/objc3_sema_pass_manager.cpp",
        "frontend_pipeline_source_path": "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
        "frontend_artifacts_source_path": (
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
        ),
        "runtime_internal_header_path": (
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
        ),
        "runtime_source_path": "native/objc3c/src/runtime/objc3_runtime.cpp",
        "source_surface_model": (
            "property-atomicity-synthesis-reflection-source-surface-freezes-atomicity-flags-conflict-state-attribute-profiles-and-private-reflection-codepaths-before-runtime-managed-atomic-storage-semantics-land"
        ),
        "atomicity_fail_closed_model": (
            "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land"
        ),
        "reflection_boundary_model": (
            "property-attribute-profiles-remain-the-authoritative-reflection-carrier-for-atomicity-and-synthesis-state-on-the-private-property-query-boundary"
        ),
    }
    for field, expected_value in expected_property_atomicity_synthesis_reflection_fields.items():
        if property_atomicity_synthesis_reflection_source_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_property_atomicity_synthesis_reflection_source_surface drifted from {field}"
            )
    if (
        property_atomicity_synthesis_reflection_source_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface must require the coupled runtime registration manifest"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get(
            "requires_real_compile_output"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface must require real compile output"
        )
    if (
        property_atomicity_synthesis_reflection_source_surface.get(
            "requires_linked_runtime_probe"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_property_atomicity_synthesis_reflection_source_surface must require a linked runtime probe"
        )
    if not isinstance(realization_lowering_reflection_artifact_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_realization_lowering_reflection_artifact_surface"
        )
    if (
        realization_lowering_reflection_artifact_surface.get("contract_id")
        != RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_realization_lowering_reflection_artifact_surface contract"
        )
    if (
        realization_lowering_reflection_artifact_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface drifted from the compile manifest artifact path"
        )
    if (
        realization_lowering_reflection_artifact_surface.get(
            "registration_manifest_artifact"
        )
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        realization_lowering_reflection_artifact_surface.get(
            "registration_descriptor_artifact"
        )
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface drifted from the runtime registration descriptor artifact path"
        )
    if realization_lowering_reflection_artifact_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface drifted from the emitted object artifact path"
        )
    if realization_lowering_reflection_artifact_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_realization_lowering_reflection_artifact_fields = {
        "runtime_object_model_realization_source_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        ),
        "executable_realization_records_contract_id": (
            "objc3c.executable.realization.records.v1"
        ),
        "property_metadata_reflection_contract_id": (
            "objc3c.runtime.property.metadata.reflection.v1"
        ),
        "runtime_backed_object_ownership_attribute_surface_contract_id": (
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        ),
        "runtime_support_library_archive_relative_path": (
            registration_manifest.get("runtime_support_library_archive_relative_path")
        ),
        "registration_entrypoint_symbol": registration_manifest.get("registration_entrypoint_symbol"),
        "selector_lookup_symbol": "objc3_runtime_lookup_selector",
        "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "lowering_artifact_boundary_model": (
            "compile-manifest-registration-descriptor-object-and-llvm-ir-co-publish-realization-lowering-and-reflection-artifacts"
        ),
        "reflection_artifact_handoff_model": (
            "property-metadata-and-ownership-artifacts-remain-coupled-to-lowered-dispatch-accessor-and-executable-realization-record-outputs"
        ),
    }
    for field, expected_value in expected_realization_lowering_reflection_artifact_fields.items():
        if realization_lowering_reflection_artifact_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_realization_lowering_reflection_artifact_surface drifted from {field}"
            )
    if (
        realization_lowering_reflection_artifact_surface.get(
            "requires_coupled_registration_descriptor_artifact"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface must require the coupled runtime registration descriptor artifact"
        )
    if (
        realization_lowering_reflection_artifact_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface must require the coupled runtime registration manifest"
        )
    if (
        realization_lowering_reflection_artifact_surface.get(
            "requires_real_compile_output"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface must require real compile output"
        )
    if (
        realization_lowering_reflection_artifact_surface.get(
            "requires_compile_output_truthfulness"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lowering_reflection_artifact_surface must require compile output truthfulness"
        )
    if not isinstance(dispatch_table_reflection_record_lowering_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_dispatch_table_reflection_record_lowering_surface"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get("contract_id")
        != RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_dispatch_table_reflection_record_lowering_surface contract"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface drifted from the compile manifest artifact path"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface drifted from the runtime registration descriptor artifact path"
        )
    if dispatch_table_reflection_record_lowering_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface drifted from the emitted object artifact path"
        )
    if dispatch_table_reflection_record_lowering_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface drifted from the emitted LLVM IR artifact path"
        )
    dispatch_lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expected_dispatch_table_reflection_record_lowering_fields = {
        "runtime_object_model_realization_source_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "runtime_state_publication_surface_contract_id": (
            RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        ),
        "method_dispatch_and_selector_thunk_lowering_contract_id": (
            "objc3c.method.dispatch.selector.thunk.lowering.v1"
        ),
        "executable_realization_records_contract_id": (
            "objc3c.executable.realization.records.v1"
        ),
        "runtime_support_library_archive_relative_path": (
            registration_manifest.get("runtime_support_library_archive_relative_path")
        ),
        "registration_entrypoint_symbol": registration_manifest.get("registration_entrypoint_symbol"),
        "selector_lookup_symbol": "objc3_runtime_lookup_selector",
        "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
        "selector_pool_section_root_symbol": "@__objc3_sec_selector_pool",
        "class_aggregate_symbol": "__objc3_sec_class_descriptors",
        "protocol_aggregate_symbol": "__objc3_sec_protocol_descriptors",
        "category_aggregate_symbol": "__objc3_sec_category_descriptors",
        "property_aggregate_symbol": "__objc3_sec_property_descriptors",
        "ivar_aggregate_symbol": "__objc3_sec_ivar_descriptors",
        "message_send_sites": dispatch_lowering_surface.get("message_send_sites"),
        "class_descriptor_count": registration_manifest.get("class_descriptor_count"),
        "protocol_descriptor_count": registration_manifest.get("protocol_descriptor_count"),
        "category_descriptor_count": registration_manifest.get("category_descriptor_count"),
        "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
        "ivar_descriptor_count": registration_manifest.get("ivar_descriptor_count"),
        "dispatch_table_lowering_model": (
            "selector-pool-backed-dispatch-thunks-and-runtime-dispatch-sites-co-publish-stable-selector-table-roots-in-llvm-ir-and-manifest-artifacts"
        ),
        "reflection_record_lowering_model": (
            "realization-records-and-runtime-metadata-section-aggregates-co-publish-class-protocol-category-property-and-ivar-record-roots-in-emitted-artifacts"
        ),
    }
    for field, expected_value in expected_dispatch_table_reflection_record_lowering_fields.items():
        if dispatch_table_reflection_record_lowering_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_dispatch_table_reflection_record_lowering_surface drifted from {field}"
            )
    if (
        dispatch_table_reflection_record_lowering_surface.get(
            "requires_coupled_registration_descriptor_artifact"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface must require the coupled runtime registration descriptor artifact"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface must require the coupled runtime registration manifest"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get("requires_real_compile_output")
        is not True
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface must require real compile output"
        )
    if (
        dispatch_table_reflection_record_lowering_surface.get(
            "requires_compile_output_truthfulness"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_dispatch_table_reflection_record_lowering_surface must require compile output truthfulness"
        )
    if not isinstance(object_model_abi_query_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_object_model_abi_query_surface"
        )
    if (
        object_model_abi_query_surface.get("contract_id")
        != RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_object_model_abi_query_surface contract"
        )
    if object_model_abi_query_surface.get("compile_manifest_artifact") != manifest_path.name:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the compile manifest artifact path"
        )
    if (
        object_model_abi_query_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        object_model_abi_query_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the runtime registration descriptor artifact path"
        )
    if object_model_abi_query_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the emitted object artifact path"
        )
    if object_model_abi_query_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_object_model_abi_query_surface_fields = {
        "runtime_object_model_realization_source_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "runtime_realization_lowering_reflection_artifact_surface_contract_id": (
            RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID
        ),
        "runtime_dispatch_table_reflection_record_lowering_surface_contract_id": (
            RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID
        ),
        "runtime_cross_module_realized_metadata_replay_preservation_surface_contract_id": (
            RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID
        ),
        "runtime_reflection_query_surface_contract_id": (
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID
        ),
        "runtime_realization_lookup_semantics_surface_contract_id": (
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "runtime_class_metaclass_protocol_realization_surface_contract_id": (
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID
        ),
        "runtime_category_attachment_merged_dispatch_surface_contract_id": (
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID
        ),
        "runtime_reflection_visibility_coherence_diagnostics_surface_contract_id": (
            RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "object_model_query_boundary_model": (
            "public-runtime-header-plus-private-testing-snapshots-freeze-the-object-model-lookup-and-reflection-query-surface-without-widening-the-public-abi"
        ),
    }
    for field, expected_value in expected_object_model_abi_query_surface_fields.items():
        if object_model_abi_query_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_object_model_abi_query_surface drifted from {field}"
            )
    if object_model_abi_query_surface.get("public_runtime_abi_boundary") != PUBLIC_RUNTIME_ABI_BOUNDARY:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the public runtime ABI boundary"
        )
    if object_model_abi_query_surface.get("private_object_model_query_boundary") != [
        "objc3_runtime_copy_realized_class_graph_state_for_testing",
        "objc3_runtime_copy_realized_class_entry_for_testing",
        "objc3_runtime_copy_property_registry_state_for_testing",
        "objc3_runtime_copy_property_entry_for_testing",
        "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "objc3_runtime_copy_selector_lookup_table_state_for_testing",
        "objc3_runtime_copy_selector_lookup_entry_for_testing",
        "objc3_runtime_copy_method_cache_state_for_testing",
        "objc3_runtime_copy_method_cache_entry_for_testing",
        "objc3_runtime_copy_dispatch_state_for_testing",
    ]:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface drifted from the private object-model query boundary"
        )
    if object_model_abi_query_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface must require the coupled runtime registration manifest"
        )
    if object_model_abi_query_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface must require real compile output"
        )
    if object_model_abi_query_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_object_model_abi_query_surface must require a linked runtime probe"
        )
    if not isinstance(realization_lookup_reflection_implementation_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_realization_lookup_reflection_implementation_surface"
        )
    if (
        realization_lookup_reflection_implementation_surface.get("contract_id")
        != RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_realization_lookup_reflection_implementation_surface contract"
        )
    if (
        realization_lookup_reflection_implementation_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface drifted from the compile manifest artifact path"
        )
    if (
        realization_lookup_reflection_implementation_surface.get(
            "registration_manifest_artifact"
        )
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        realization_lookup_reflection_implementation_surface.get(
            "registration_descriptor_artifact"
        )
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface drifted from the runtime registration descriptor artifact path"
        )
    if (
        realization_lookup_reflection_implementation_surface.get("object_artifact")
        != obj_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface drifted from the emitted object artifact path"
        )
    if (
        realization_lookup_reflection_implementation_surface.get("backend_artifact")
        != ll_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_realization_lookup_reflection_implementation_fields = {
        "runtime_object_model_abi_query_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID
        ),
        "runtime_reflection_query_surface_contract_id": (
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID
        ),
        "runtime_realization_lookup_semantics_surface_contract_id": (
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "runtime_class_metaclass_protocol_realization_surface_contract_id": (
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID
        ),
        "runtime_category_attachment_merged_dispatch_surface_contract_id": (
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID
        ),
        "runtime_reflection_visibility_coherence_diagnostics_surface_contract_id": (
            RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID
        ),
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "object_model_query_state_snapshot_symbol": (
            "objc3_runtime_copy_object_model_query_state_for_testing"
        ),
        "realized_class_entry_snapshot_symbol": (
            "objc3_runtime_copy_realized_class_entry_for_testing"
        ),
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "protocol_conformance_query_symbol": (
            "objc3_runtime_copy_protocol_conformance_query_for_testing"
        ),
        "selector_lookup_table_state_snapshot_symbol": (
            "objc3_runtime_copy_selector_lookup_table_state_for_testing"
        ),
        "selector_lookup_entry_snapshot_symbol": (
            "objc3_runtime_copy_selector_lookup_entry_for_testing"
        ),
        "method_cache_state_snapshot_symbol": (
            "objc3_runtime_copy_method_cache_state_for_testing"
        ),
        "method_cache_entry_snapshot_symbol": (
            "objc3_runtime_copy_method_cache_entry_for_testing"
        ),
        "dispatch_state_snapshot_symbol": "objc3_runtime_copy_dispatch_state_for_testing",
        "realization_lookup_reflection_implementation_model": (
            "runtime-owned-class-property-protocol-and-method-cache-query-snapshots-publish-coherent-last-query-state-and-live-object-model-counts"
        ),
    }
    for field, expected_value in (
        expected_realization_lookup_reflection_implementation_fields.items()
    ):
        if realization_lookup_reflection_implementation_surface.get(field) != expected_value:
            raise RuntimeError(
                "runtime_realization_lookup_reflection_implementation_surface drifted "
                f"from {field}"
            )
    if (
        realization_lookup_reflection_implementation_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface must require the coupled runtime registration manifest"
        )
    if (
        realization_lookup_reflection_implementation_surface.get(
            "requires_real_compile_output"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface must require real compile output"
        )
    if (
        realization_lookup_reflection_implementation_surface.get(
            "requires_linked_runtime_probe"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_realization_lookup_reflection_implementation_surface must require a linked runtime probe"
        )
    if not isinstance(reflection_query_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_reflection_query_surface")
    if (
        reflection_query_surface.get("contract_id")
        != RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_reflection_query_surface contract"
        )
    if reflection_query_surface.get("compile_manifest_artifact") != manifest_path.name:
        raise RuntimeError(
            "runtime_reflection_query_surface drifted from the compile manifest artifact path"
        )
    if reflection_query_surface.get("registration_manifest_artifact") != registration_manifest_path.name:
        raise RuntimeError(
            "runtime_reflection_query_surface drifted from the runtime registration manifest artifact path"
        )
    if reflection_query_surface.get("registration_descriptor_artifact") != registration_descriptor_path.name:
        raise RuntimeError(
            "runtime_reflection_query_surface drifted from the runtime registration descriptor artifact path"
        )
    if reflection_query_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError("runtime_reflection_query_surface drifted from the emitted object artifact path")
    if reflection_query_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError("runtime_reflection_query_surface drifted from the emitted LLVM IR artifact path")
    expected_reflection_surface_fields = {
        "object_model_realization_source_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "dispatch_accessor_runtime_abi_surface_contract_id": "objc3c.runtime.dispatch_accessor.abi.surface.v1",
        "property_metadata_reflection_contract_id": "objc3c.runtime.property.metadata.reflection.v1",
        "runtime_backed_object_ownership_attribute_surface_contract_id": (
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "query_api_boundary_model": (
            "private-testing-snapshots-over-runtime-owned-realized-class-property-and-protocol-metadata-with-no-public-reflection-abi"
        ),
    }
    for field, expected_value in expected_reflection_surface_fields.items():
        if reflection_query_surface.get(field) != expected_value:
            raise RuntimeError(f"runtime_reflection_query_surface drifted from {field}")
    if reflection_query_surface.get("private_query_symbols") != [
        "objc3_runtime_copy_realized_class_graph_state_for_testing",
        "objc3_runtime_copy_realized_class_entry_for_testing",
        "objc3_runtime_copy_property_registry_state_for_testing",
        "objc3_runtime_copy_property_entry_for_testing",
        "objc3_runtime_copy_protocol_conformance_query_for_testing",
    ]:
        raise RuntimeError("runtime_reflection_query_surface drifted from the private query symbol boundary")
    if reflection_query_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError(
            "runtime_reflection_query_surface must require the coupled runtime registration manifest"
        )
    if reflection_query_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_reflection_query_surface must require real compile output"
        )
    if reflection_query_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_reflection_query_surface must require a linked runtime probe"
        )
    if not isinstance(realization_lookup_semantics_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_realization_lookup_semantics_surface"
        )
    if (
        realization_lookup_semantics_surface.get("contract_id")
        != RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_realization_lookup_semantics_surface contract"
        )
    if realization_lookup_semantics_surface.get("compile_manifest_artifact") != manifest_path.name:
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface drifted from the compile manifest artifact path"
        )
    if (
        realization_lookup_semantics_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        realization_lookup_semantics_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface drifted from the runtime registration descriptor artifact path"
        )
    if realization_lookup_semantics_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface drifted from the emitted object artifact path"
        )
    if realization_lookup_semantics_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_realization_lookup_semantics_fields = {
        "runtime_object_model_realization_source_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "runtime_reflection_query_surface_contract_id": (
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID
        ),
        "dispatch_accessor_runtime_abi_surface_contract_id": (
            "objc3c.runtime.dispatch_accessor.abi.surface.v1"
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "registration_entrypoint_symbol": registration_manifest.get("registration_entrypoint_symbol"),
        "selector_lookup_symbol": "objc3_runtime_lookup_selector",
        "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
        "selector_lookup_table_state_snapshot_symbol": (
            "objc3_runtime_copy_selector_lookup_table_state_for_testing"
        ),
        "selector_lookup_entry_snapshot_symbol": "objc3_runtime_copy_selector_lookup_entry_for_testing",
        "method_cache_state_snapshot_symbol": "objc3_runtime_copy_method_cache_state_for_testing",
        "method_cache_entry_snapshot_symbol": "objc3_runtime_copy_method_cache_entry_for_testing",
        "realized_class_entry_snapshot_symbol": "objc3_runtime_copy_realized_class_entry_for_testing",
        "protocol_conformance_query_symbol": "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "lookup_resolution_order_model": (
            "seeded-cache-then-live-class-chain-then-attached-category-and-protocol-checks-then-deterministic-fallback"
        ),
        "selector_materialization_model": (
            "metadata-selectors-materialized-at-registration-and-dynamic-misses-interned-at-first-lookup"
        ),
        "unresolved_selector_behavior_model": (
            "negative-cache-entry-preserved-and-deterministic-fallback-returned"
        ),
    }
    for field, expected_value in expected_realization_lookup_semantics_fields.items():
        if realization_lookup_semantics_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_realization_lookup_semantics_surface drifted from {field}"
            )
    if realization_lookup_semantics_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface must require the coupled runtime registration manifest"
        )
    if realization_lookup_semantics_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface must require real compile output"
        )
    if realization_lookup_semantics_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_realization_lookup_semantics_surface must require a linked runtime probe"
        )
    if not isinstance(class_metaclass_protocol_realization_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_class_metaclass_protocol_realization_surface"
        )
    if (
        class_metaclass_protocol_realization_surface.get("contract_id")
        != RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_class_metaclass_protocol_realization_surface contract"
        )
    if class_metaclass_protocol_realization_surface.get("compile_manifest_artifact") != manifest_path.name:
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface drifted from the compile manifest artifact path"
        )
    if (
        class_metaclass_protocol_realization_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        class_metaclass_protocol_realization_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface drifted from the runtime registration descriptor artifact path"
        )
    if class_metaclass_protocol_realization_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface drifted from the emitted object artifact path"
        )
    if class_metaclass_protocol_realization_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_class_metaclass_protocol_fields = {
        "runtime_object_model_realization_source_surface_contract_id": (
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "runtime_reflection_query_surface_contract_id": (
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID
        ),
        "runtime_realization_lookup_semantics_surface_contract_id": (
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "registration_entrypoint_symbol": registration_manifest.get("registration_entrypoint_symbol"),
        "realized_class_graph_snapshot_symbol": "objc3_runtime_copy_realized_class_graph_state_for_testing",
        "realized_class_entry_snapshot_symbol": "objc3_runtime_copy_realized_class_entry_for_testing",
        "protocol_conformance_query_symbol": "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "class_realization_model": (
            "registration-installs-runtime-backed-class-records-before-live-dispatch-and-reflection"
        ),
        "metaclass_lineage_model": (
            "realized-class-entries-publish-stable-class-metaclass-superclass-and-super-metaclass-owner-identities"
        ),
        "protocol_conformance_model": (
            "realized-class-entries-and-runtime-conformance-queries-publish-direct-and-attached-protocol-conformance"
        ),
    }
    for field, expected_value in expected_class_metaclass_protocol_fields.items():
        if class_metaclass_protocol_realization_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_class_metaclass_protocol_realization_surface drifted from {field}"
            )
    if class_metaclass_protocol_realization_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface must require the coupled runtime registration manifest"
        )
    if class_metaclass_protocol_realization_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface must require real compile output"
        )
    if class_metaclass_protocol_realization_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_class_metaclass_protocol_realization_surface must require a linked runtime probe"
        )
    if not isinstance(category_attachment_merged_dispatch_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_category_attachment_merged_dispatch_surface"
        )
    if (
        category_attachment_merged_dispatch_surface.get("contract_id")
        != RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_category_attachment_merged_dispatch_surface contract"
        )
    if category_attachment_merged_dispatch_surface.get("compile_manifest_artifact") != manifest_path.name:
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface drifted from the compile manifest artifact path"
        )
    if (
        category_attachment_merged_dispatch_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        category_attachment_merged_dispatch_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface drifted from the runtime registration descriptor artifact path"
        )
    if category_attachment_merged_dispatch_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface drifted from the emitted object artifact path"
        )
    if category_attachment_merged_dispatch_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_category_attachment_merged_dispatch_fields = {
        "runtime_category_attachment_protocol_conformance_contract_id": (
            "objc3c.runtime.category.attachment.protocol.conformance.v1"
        ),
        "runtime_realization_lookup_semantics_surface_contract_id": (
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID
        ),
        "runtime_class_metaclass_protocol_realization_surface_contract_id": (
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "registration_entrypoint_symbol": registration_manifest.get("registration_entrypoint_symbol"),
        "selector_lookup_symbol": "objc3_runtime_lookup_selector",
        "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
        "realized_class_graph_snapshot_symbol": "objc3_runtime_copy_realized_class_graph_state_for_testing",
        "realized_class_entry_snapshot_symbol": "objc3_runtime_copy_realized_class_entry_for_testing",
        "protocol_conformance_query_symbol": "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "method_cache_state_snapshot_symbol": "objc3_runtime_copy_method_cache_state_for_testing",
        "method_cache_entry_snapshot_symbol": "objc3_runtime_copy_method_cache_entry_for_testing",
        "category_attachment_model": (
            "registration-attaches-category-owned-instance-and-protocol-members-onto-live-realized-classes-before-dispatch"
        ),
        "merged_dispatch_resolution_model": (
            "attached-category-implementations-override-base-class-instance-lookup-before-superclass-and-protocol-fallback"
        ),
        "attached_protocol_visibility_model": (
            "attached-categories-publish-owner-and-name-through-realized-class-entries-and-protocol-conformance-queries"
        ),
    }
    for field, expected_value in expected_category_attachment_merged_dispatch_fields.items():
        if category_attachment_merged_dispatch_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_category_attachment_merged_dispatch_surface drifted from {field}"
            )
    if category_attachment_merged_dispatch_surface.get("requires_coupled_registration_manifest") is not True:
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface must require the coupled runtime registration manifest"
        )
    if category_attachment_merged_dispatch_surface.get("requires_real_compile_output") is not True:
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface must require real compile output"
        )
    if category_attachment_merged_dispatch_surface.get("requires_linked_runtime_probe") is not True:
        raise RuntimeError(
            "runtime_category_attachment_merged_dispatch_surface must require a linked runtime probe"
        )
    if not isinstance(reflection_visibility_coherence_diagnostics_surface, dict):
        raise RuntimeError(
            "compiled fixture manifest did not publish runtime_reflection_visibility_coherence_diagnostics_surface"
        )
    if (
        reflection_visibility_coherence_diagnostics_surface.get("contract_id")
        != RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_reflection_visibility_coherence_diagnostics_surface contract"
        )
    if (
        reflection_visibility_coherence_diagnostics_surface.get("compile_manifest_artifact")
        != manifest_path.name
    ):
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface drifted from the compile manifest artifact path"
        )
    if (
        reflection_visibility_coherence_diagnostics_surface.get("registration_manifest_artifact")
        != registration_manifest_path.name
    ):
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface drifted from the runtime registration manifest artifact path"
        )
    if (
        reflection_visibility_coherence_diagnostics_surface.get("registration_descriptor_artifact")
        != registration_descriptor_path.name
    ):
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface drifted from the runtime registration descriptor artifact path"
        )
    if reflection_visibility_coherence_diagnostics_surface.get("object_artifact") != obj_path.name:
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface drifted from the emitted object artifact path"
        )
    if reflection_visibility_coherence_diagnostics_surface.get("backend_artifact") != ll_path.name:
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface drifted from the emitted LLVM IR artifact path"
        )
    expected_reflection_visibility_coherence_diagnostics_fields = {
        "runtime_reflection_query_surface_contract_id": (
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID
        ),
        "runtime_category_attachment_merged_dispatch_surface_contract_id": (
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID
        ),
        "dispatch_accessor_runtime_abi_surface_contract_id": (
            "objc3c.runtime.dispatch_accessor.abi.surface.v1"
        ),
        "property_metadata_reflection_contract_id": (
            "objc3c.runtime.property.metadata.reflection.v1"
        ),
        "runtime_backed_object_ownership_attribute_surface_contract_id": (
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "realized_class_entry_snapshot_symbol": "objc3_runtime_copy_realized_class_entry_for_testing",
        "protocol_conformance_query_symbol": "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "method_cache_state_snapshot_symbol": "objc3_runtime_copy_method_cache_state_for_testing",
        "reflection_visibility_boundary_model": (
            "private-testing-snapshots-remain-the-only-reflection-visibility-surface-and-publish-runtime-owned-class-property-and-protocol-state"
        ),
        "fail_closed_lookup_diagnostic_model": (
            "missing-class-and-property-lookups-publish-found-zero-without-mutating-property-registry-or-realized-class-state"
        ),
        "runtime_coherence_diagnostic_model": (
            "reflected-property-selectors-owner-identities-slot-layout-and-ownership-profiles-must-match-live-dispatch-realized-class-and-attached-protocol-state"
        ),
    }
    for field, expected_value in expected_reflection_visibility_coherence_diagnostics_fields.items():
        if reflection_visibility_coherence_diagnostics_surface.get(field) != expected_value:
            raise RuntimeError(
                f"runtime_reflection_visibility_coherence_diagnostics_surface drifted from {field}"
            )
    if (
        reflection_visibility_coherence_diagnostics_surface.get(
            "requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface must require the coupled runtime registration manifest"
        )
    if (
        reflection_visibility_coherence_diagnostics_surface.get(
            "requires_real_compile_output"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface must require real compile output"
        )
    if (
        reflection_visibility_coherence_diagnostics_surface.get(
            "requires_linked_runtime_probe"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_reflection_visibility_coherence_diagnostics_surface must require a linked runtime probe"
        )
    if not isinstance(runtime_installation_abi_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_installation_abi_surface")
    if (
        runtime_installation_abi_surface.get("contract_id")
        != RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_installation_abi_surface contract"
        )
    if runtime_installation_abi_surface.get("public_header_path") != RUNTIME_PUBLIC_HEADER_PATH:
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the public runtime header path"
        )
    if (
        runtime_installation_abi_surface.get("internal_header_path")
        != RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the bootstrap internal header path"
        )
    if (
        runtime_installation_abi_surface.get("bootstrap_api_contract_id")
        != bootstrap_api_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the bootstrap API contract"
        )
    if (
        runtime_installation_abi_surface.get("bootstrap_reset_contract_id")
        != bootstrap_reset_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the bootstrap reset contract"
        )
    if (
        runtime_installation_abi_surface.get("bootstrap_registrar_contract_id")
        != bootstrap_registrar_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the bootstrap registrar contract"
        )
    if (
        runtime_installation_abi_surface.get("public_installation_abi_boundary")
        != RUNTIME_INSTALLATION_ABI_BOUNDARY
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the public installation ABI boundary"
        )
    if (
        runtime_installation_abi_surface.get("private_loader_testing_boundary")
        != RUNTIME_LOADER_TESTING_BOUNDARY
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface drifted from the private loader testing boundary"
        )
    if (
        runtime_installation_abi_surface.get(
            "installation_requires_coupled_registration_manifest"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface must require the coupled registration manifest"
        )
    if (
        runtime_installation_abi_surface.get(
            "register_image_consumes_staged_registration_table_once"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface must preserve single-consumption staged registration"
        )
    if (
        runtime_installation_abi_surface.get("deterministic_reset_replay_supported")
        is not True
    ):
        raise RuntimeError(
            "runtime_installation_abi_surface must preserve deterministic reset/replay support"
        )
    if not isinstance(runtime_loader_lifecycle_surface, dict):
        raise RuntimeError("compiled fixture manifest did not publish runtime_loader_lifecycle_surface")
    if (
        runtime_loader_lifecycle_surface.get("contract_id")
        != RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture manifest published the wrong runtime_loader_lifecycle_surface contract"
        )
    if (
        runtime_loader_lifecycle_surface.get(
            "runtime_installation_abi_surface_contract_id"
        )
        != RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the installation ABI contract id"
        )
    if (
        runtime_loader_lifecycle_surface.get("bootstrap_semantics_contract_id")
        != runtime_bootstrap_semantics.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the bootstrap semantics contract"
        )
    if (
        runtime_loader_lifecycle_surface.get("bootstrap_reset_contract_id")
        != bootstrap_reset_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the bootstrap reset contract"
        )
    if (
        runtime_loader_lifecycle_surface.get("bootstrap_registrar_contract_id")
        != bootstrap_registrar_contract.get("contract_id")
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the bootstrap registrar contract"
        )
    if (
        runtime_loader_lifecycle_surface.get("authoritative_probe_path")
        != INSTALLATION_LIFECYCLE_PROBE
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the authoritative lifecycle probe path"
        )
    if (
        runtime_loader_lifecycle_surface.get("loader_testing_boundary_symbols")
        != RUNTIME_LOADER_TESTING_BOUNDARY
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the loader testing boundary symbols"
        )
    if runtime_loader_lifecycle_surface.get("lifecycle_phases") != [
        "startup-installed-runtime-state",
        "duplicate-registration-rejected-without-state-advance",
        "out-of-order-registration-rejected-without-state-advance",
        "invalid-anchor-root-rejected-without-state-advance",
        "invalid-discovery-root-rejected-without-state-advance",
        "reset-retained-bootstrap-catalog",
        "replay-restored-installed-runtime-state",
    ]:
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the lifecycle phase set"
        )
    if runtime_loader_lifecycle_surface.get("rejected_registration_status_codes") != {
        "duplicate_translation_unit_identity_key": -2,
        "out_of_order_registration": -3,
        "invalid_registration_roots": -4,
    }:
        raise RuntimeError(
            "runtime_loader_lifecycle_surface drifted from the rejected-registration status set"
        )
    if (
        runtime_loader_lifecycle_surface.get("retained_bootstrap_catalog_required")
        is not True
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface must require retained bootstrap catalog support"
        )
    if (
        runtime_loader_lifecycle_surface.get("deterministic_replay_required")
        is not True
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface must require deterministic replay"
        )
    if (
        runtime_loader_lifecycle_surface.get(
            "requires_linked_fixture_or_loader_retained_roots"
        )
        is not True
    ):
        raise RuntimeError(
            "runtime_loader_lifecycle_surface must require linked fixture or loader-retained roots"
        )

    if provenance.get("contract_id") != COMPILE_PROVENANCE_CONTRACT_ID:
        raise RuntimeError("compiled fixture did not publish the native compile provenance contract")
    truthfulness = provenance.get("compile_output_truthfulness")
    if not isinstance(truthfulness, dict) or truthfulness.get("contract_id") != COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID:
        raise RuntimeError("compiled fixture did not publish the compile output truthfulness contract")
    if truthfulness.get("truthful") is not True:
        raise RuntimeError("compiled fixture did not certify truthful compile output")
    if registration_manifest.get("compile_output_provenance_artifact") != "module.compile-provenance.json":
        raise RuntimeError("runtime registration manifest did not bind compile provenance artifact")
    if registration_manifest.get("compile_output_truthful") is not True:
        raise RuntimeError("runtime registration manifest did not certify truthful compile output")
    if registration_manifest.get("compile_output_artifact_set_digest_sha256") != provenance.get("artifact_set_digest_sha256"):
        raise RuntimeError("runtime registration manifest compile output digest drifted from compile provenance")
    return obj_path


def compile_fixture(fixture: Path, out_dir: Path) -> Path:
    return compile_fixture_with_args(fixture, out_dir)


def compile_fixture_outputs(fixture: Path, out_dir: Path) -> tuple[Path, Path, Path]:
    obj_path = compile_fixture(fixture, out_dir)
    ll_path = out_dir / "module.ll"
    manifest_path = out_dir / "module.manifest.json"
    if not ll_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {ll_path}")
    return obj_path, ll_path, manifest_path


def compile_fixture_expect_failure(
    fixture: Path,
    out_dir: Path,
    *,
    expected_snippets: list[str],
    expected_codes: list[str],
) -> dict[str, Any]:
    out_dir.mkdir(parents=True, exist_ok=True)
    result = run(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(COMPILE_PS1),
            str(fixture),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ]
    )
    if result.returncode == 0:
        raise RuntimeError(f"fixture compile unexpectedly succeeded for {fixture}")
    diagnostics_txt_path = out_dir / "module.diagnostics.txt"
    diagnostics_json_path = out_dir / "module.diagnostics.json"
    if not diagnostics_txt_path.is_file():
        raise RuntimeError(f"failed compile for {fixture} did not publish {diagnostics_txt_path}")
    if not diagnostics_json_path.is_file():
        raise RuntimeError(f"failed compile for {fixture} did not publish {diagnostics_json_path}")
    diagnostics_text = diagnostics_txt_path.read_text(encoding="utf-8")
    diagnostics_payload = json.loads(diagnostics_json_path.read_text(encoding="utf-8"))
    diagnostics = diagnostics_payload.get("diagnostics", [])
    expect(
        isinstance(diagnostics, list) and diagnostics,
        f"failed compile for {fixture} did not publish structured diagnostics",
    )
    for snippet in expected_snippets:
        expect(
            snippet in diagnostics_text,
            f"failed compile for {fixture} did not publish expected diagnostic snippet: {snippet}",
        )
    observed_codes = {
        diagnostic.get("code")
        for diagnostic in diagnostics
        if isinstance(diagnostic, dict) and isinstance(diagnostic.get("code"), str)
    }
    for expected_code in expected_codes:
        expect(
            expected_code in observed_codes,
            f"failed compile for {fixture} did not publish expected diagnostic code {expected_code}",
        )
    return {
        "returncode": result.returncode,
        "diagnostic_count": len(diagnostics),
        "diagnostic_codes": sorted(observed_codes),
        "diagnostics_path": str(diagnostics_json_path.relative_to(ROOT)).replace("\\", "/"),
    }


def compile_probe(clangxx: str, probe: Path, exe_path: Path, extra_objects: list[Path]) -> None:
    compile_probe_with_args(clangxx, probe, exe_path, extra_objects, [])


def compile_probe_with_args(
    clangxx: str,
    probe: Path,
    exe_path: Path,
    extra_objects: list[Path],
    extra_args: list[str],
) -> None:
    exe_path.parent.mkdir(parents=True, exist_ok=True)
    command = [
        clangxx,
        "-std=c++20",
        "-fms-runtime-lib=dll",
        "-I",
        str(ROOT / "native" / "objc3c" / "src"),
        *extra_args,
        str(probe),
        *[str(path) for path in extra_objects],
        str(RUNTIME_LIB),
        "-o",
        str(exe_path),
    ]
    result = run(command)
    if result.returncode != 0:
        raise RuntimeError(
            f"probe link failed for {probe}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )


def run_probe(exe_path: Path) -> subprocess.CompletedProcess[str]:
    result = run([str(exe_path)])
    if result.returncode != 0:
        raise RuntimeError(
            f"probe execution failed for {exe_path} (exit={result.returncode}):\n"
            f"STDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )
    return result


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def parse_json_output(result: subprocess.CompletedProcess[str], label: str) -> dict[str, Any]:
    stdout = result.stdout.strip()
    if not stdout:
        raise RuntimeError(f"{label} produced no JSON output")
    try:
        payload = json.loads(stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"{label} produced invalid JSON: {exc}\nstdout:\n{stdout}") from exc
    if not isinstance(payload, dict):
        raise RuntimeError(f"{label} did not produce a JSON object")
    return payload


def parse_key_value_output(
    result: subprocess.CompletedProcess[str], label: str
) -> dict[str, Any]:
    stdout = result.stdout.strip()
    if not stdout:
        raise RuntimeError(f"{label} produced no key/value output")
    payload: dict[str, Any] = {}
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if not line:
            continue
        if "=" not in line:
            raise RuntimeError(f"{label} produced malformed line: {line}")
        key, value = line.split("=", 1)
        key = key.strip()
        value = value.strip()
        if value and (value.lstrip("-").isdigit()):
            payload[key] = int(value)
        else:
            payload[key] = value
    if not payload:
        raise RuntimeError(f"{label} produced no parseable key/value output")
    return payload


@dataclass(frozen=True)
class CaseResult:
    case_id: str
    probe: str
    fixture: str | None
    claim_class: str
    passed: bool
    summary: dict[str, Any]


def build_claim_boundary() -> dict[str, Any]:
    return {
        "contract_id": "objc3c.runtime.execution.claim.boundary.v1",
        "authoritative_claim_classes": {
            "linked-runtime-probe": {
                "requires_runtime_library_or_emitted_object": True,
                "requires_executable_probe": True,
                "requires_runtime_backed_execution_or_snapshot": True,
            },
            "compile-coupled-inspection": {
                "requires_real_compile": True,
                "requires_compile_output_truthfulness": True,
                "requires_coupled_registration_manifest": True,
            },
        },
        "non_authoritative_inputs": [
            "hand-authored llvm ir without matching compile output",
            "sidecar-only manifests or reports with no coupled object/probe path",
            "compatibility shims without a coupled emitted object and runtime probe",
            "comment-only or placeholder-only capability claims",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
    }


def build_runtime_state_publication_surface() -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
        "publication_surface_kind": RUNTIME_STATE_PUBLICATION_SURFACE_KIND,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "publication_requires_coupled_registration_manifest": True,
        "publication_requires_real_compile_output": True,
    }


def build_runtime_bootstrap_registration_source_surface() -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "requires_coupled_registration_descriptor_artifact": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
    }


def build_runtime_bootstrap_lowering_registration_artifact_surface() -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_BOOTSTRAP_LOWERING_REGISTRATION_ARTIFACT_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "composed_source_inputs": [
            "objc_runtime_bootstrap_lowering_contract",
            "objc_runtime_translation_unit_registration_manifest",
            "objc_runtime_startup_bootstrap_semantics",
            "objc_runtime_registration_descriptor_frontend_closure",
        ],
        "emitted_symbol_fields": [
            "constructor_root_symbol",
            "init_stub_symbol_prefix",
            "registration_table_symbol_prefix",
            "image_local_init_state_symbol_prefix",
            "registration_entrypoint_symbol",
        ],
        "emitted_table_fields": [
            "registration_table_layout_model",
            "registration_table_abi_version",
            "registration_table_pointer_field_count",
        ],
        "emission_state_fields": [
            "constructor_root_emission_state",
            "init_stub_emission_state",
            "registration_table_emission_state",
            "bootstrap_ir_materialization_landed",
            "image_local_initialization_landed",
        ],
        "lowered_registration_descriptor_fields": [
            "constructor_init_stub_symbol",
            "bootstrap_registration_table_symbol",
            "bootstrap_image_local_init_state_symbol",
            "bootstrap_registration_table_layout_model",
            "bootstrap_image_local_initialization_model",
            "bootstrap_registration_table_abi_version",
            "bootstrap_registration_table_pointer_field_count",
            "translation_unit_registration_order_ordinal",
        ],
        "loader_table_ir_proof_fields": [
            "constructor_root_symbol",
            "constructor_init_stub_symbol",
            "bootstrap_registration_table_symbol",
            "bootstrap_image_local_init_state_symbol",
            "translation_unit_registration_order_ordinal",
        ],
        "requires_coupled_registration_descriptor_artifact": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_emitted_loader_table_ir": True,
    }


def build_runtime_multi_image_startup_ordering_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    installation_lifecycle = next(
        (result for result in results if result.case_id == "installation-lifecycle"),
        None,
    )
    latest_summary = installation_lifecycle.summary if installation_lifecycle is not None else {}
    return {
        "contract_id": RUNTIME_MULTI_IMAGE_STARTUP_ORDERING_SOURCE_SURFACE_CONTRACT_ID,
        "runtime_installation_abi_surface_contract_id": RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID,
        "runtime_loader_lifecycle_surface_contract_id": RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID,
        "authoritative_case_ids": (
            ["installation-lifecycle"] if installation_lifecycle is not None else []
        ),
        "fixture_path": INSTALLATION_LIFECYCLE_FIXTURE,
        "probe_path": INSTALLATION_LIFECYCLE_PROBE,
        "runtime_public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "runtime_bootstrap_internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "validation_commands": [
            RUNTIME_ACCEPTANCE_COMMAND,
            VALIDATE_RUNTIME_ARCHITECTURE_COMMAND,
        ],
        "runtime_symbols": [
            "objc3_runtime_register_image",
            "objc3_runtime_copy_registration_state_for_testing",
            "objc3_runtime_reset_for_testing",
            "objc3_runtime_replay_registered_images_for_testing",
            "objc3_runtime_copy_reset_replay_state_for_testing",
        ],
        "diagnostic_status_codes": {
            "duplicate_registration": -2,
            "out_of_order_registration": -3,
        },
        "diagnostic_models": {
            "duplicate_install": RUNTIME_DUPLICATE_INSTALL_DIAGNOSTIC_MODEL,
            "out_of_order_install": RUNTIME_OUT_OF_ORDER_INSTALL_DIAGNOSTIC_MODEL,
        },
        "rejected_registration_fields": [
            "last_rejected_module_name",
            "last_rejected_translation_unit_identity_key",
            "last_rejected_registration_order_ordinal",
        ],
        "ordering_snapshot_fields": [
            "next_expected_registration_order_ordinal",
            "last_successful_registration_order_ordinal",
            "last_rejected_registration_order_ordinal",
        ],
        "measured_summary_fields": [
            "fixture_compile_ms",
            "probe_link_ms",
            "probe_run_ms",
            "case_total_ms",
        ],
        "latest_installation_lifecycle_measurements": latest_summary,
        "latest_duplicate_install_diagnostic": {
            "status": latest_summary.get("duplicate_status"),
            "rejected_module_name": latest_summary.get("duplicate_rejected_module_name"),
            "rejected_translation_unit_identity_key": latest_summary.get(
                "duplicate_rejected_translation_unit_identity_key"
            ),
            "rejected_registration_order_ordinal": latest_summary.get(
                "duplicate_rejected_registration_order_ordinal"
            ),
        },
        "latest_out_of_order_install_diagnostic": {
            "status": latest_summary.get("out_of_order_status"),
            "rejected_module_name": latest_summary.get("out_of_order_rejected_module_name"),
            "rejected_translation_unit_identity_key": latest_summary.get(
                "out_of_order_rejected_translation_unit_identity_key"
            ),
            "rejected_registration_order_ordinal": latest_summary.get(
                "out_of_order_rejected_registration_order_ordinal"
            ),
        },
    }


def build_runtime_object_model_realization_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "imported-runtime-packaging-replay",
            "canonical-dispatch",
            "canonical-sample-set",
            "dispatch-fast-path",
        }
    ]
    return {
        "contract_id": RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.executable.realization.records.v1",
            "objc3c.runtime.class.realization.freeze.v1",
            "objc3c.runtime.metaclass.graph.root.class.baseline.v1",
            "objc3c.runtime.category.attachment.protocol.conformance.v1",
            "objc3c.runtime.canonical.runnable.object.sample.support.v1",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_object_model_query_boundary": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3",
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_property_ivar_storage_accessor_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "accessor-storage-lowering-metadata-surface",
            "property-ivar-ordering-semantics",
            "property-reflection-accessor-compatibility-diagnostics",
            "property-synthesis-storage-binding-semantics",
            "storage-legality-semantics",
            "synthesized-accessor-codegen",
            "synthesized-accessor-runtime",
            "property-layout",
            "property-execution",
            "property-reflection",
            "storage-ownership-reflection",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.executable.property.ivar.source.closure.v1",
            "objc3c.executable.property.ivar.source.model.completion.v1",
            "objc3c.executable.property.ivar.semantics.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_source_fields": [
            "Objc3PropertyDecl.ivar_binding_symbol",
            "Objc3PropertyDecl.executable_synthesized_binding_kind",
            "Objc3PropertyDecl.executable_synthesized_binding_symbol",
            "Objc3PropertyDecl.property_attribute_profile",
            "Objc3PropertyDecl.effective_getter_selector",
            "Objc3PropertyDecl.effective_setter_available",
            "Objc3PropertyDecl.effective_setter_selector",
            "Objc3PropertyDecl.accessor_ownership_profile",
            "Objc3PropertyDecl.executable_ivar_layout_symbol",
            "Objc3PropertyDecl.executable_ivar_layout_slot_index",
            "Objc3PropertyDecl.executable_ivar_layout_size_bytes",
            "Objc3PropertyDecl.executable_ivar_layout_alignment_bytes",
            "Objc3PropertyDecl.executable_ivar_init_order_index",
            "Objc3PropertyDecl.executable_ivar_destroy_order_index",
            "Objc3RuntimeMetadataPropertySourceRecord.synthesizes_executable_accessors",
            "Objc3RuntimeMetadataPropertySourceRecord.getter_storage_runtime_helper_symbol",
            "Objc3RuntimeMetadataPropertySourceRecord.setter_storage_runtime_helper_symbol",
            "Objc3ExecutableMetadataPropertyGraphNode.synthesizes_executable_accessors",
            "Objc3ExecutableMetadataPropertyGraphNode.getter_storage_runtime_helper_symbol",
            "Objc3ExecutableMetadataPropertyGraphNode.setter_storage_runtime_helper_symbol",
        ],
        "semantic_boundary_model": (
            "property-ivar-storage-accessor-source-surface-freezes-ast-sema-ir-pipeline-and-runtime-codepaths-before-lowering-or-runtime-semantic-expansion"
        ),
        "source_models": [
            "property-ivar-source-model-computes-deterministic-layout-slots-sizes-and-alignment-before-runtime-storage-realization",
            "property-attribute-and-effective-accessor-source-model-publishes-deterministic-ownership-and-selector-profiles",
            "non-category-class-interface-properties-own-authoritative-default-ivar-and-synthesized-binding-identities-across-implementation-redeclaration-boundaries",
            "matched-class-implementations-resolve-interface-declared-properties-through-authoritative-default-ivar-bindings-with-or-without-implementation-redeclaration",
            "readonly-and-attribute-driven-accessor-selectors-resolve-to-one-declaration-level-profile-before-body-emission",
            "effective-getter-and-setter-selectors-must-be-unique-within-each-property-container-before-runtime-accessor-binding",
            "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land",
            "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation",
            "protocol-and-inheritance-compatibility-compare-declaration-level-attribute-accessor-ownership-profiles-not-storage-local-layout-symbols",
            "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path",
            "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/m280_b004_property_accessor_selector_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/m280_b004_property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/m257_c003_synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/m257_d001_property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-lowering-owned-storage-or-accessor-semantics-invention",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_property_atomicity_synthesis_reflection_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "property-reflection-accessor-compatibility-diagnostics",
            "storage-legality-semantics",
            "property-reflection",
            "property-execution",
            "storage-ownership-reflection",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": (
            RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID
        ),
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.runtime.property.metadata.reflection.v1",
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/sema/objc3_sema_pass_manager.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_source_fields": [
            "Objc3PropertyDecl.is_atomic",
            "Objc3PropertyDecl.is_nonatomic",
            "Objc3PropertyDecl.has_atomicity_conflict",
            "Objc3PropertyDecl.property_attribute_profile",
            "objc3_runtime_property_entry_snapshot.property_attribute_profile",
        ],
        "source_surface_model": (
            "property-atomicity-synthesis-reflection-source-surface-freezes-atomicity-flags-conflict-state-attribute-profiles-and-private-reflection-codepaths-before-runtime-managed-atomic-storage-semantics-land"
        ),
        "atomicity_fail_closed_model": (
            "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land"
        ),
        "reflection_boundary_model": (
            "property-attribute-profiles-remain-the-authoritative-reflection-carrier-for-atomicity-and-synthesis-state-on-the-private-property-query-boundary"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/m257_property_atomic_ownership_negative.objc3",
            "tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/m280_b004_property_accessor_selector_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/m280_b004_property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-atomic-property-runtime-abi-widening",
            "no-runtime-managed-atomic-storage-semantics-before-lane-b-and-lane-d-implementation",
            "no-milestone-specific-scaffolding",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_realization_lowering_reflection_artifact_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-dispatch",
            "canonical-sample-set",
            "property-reflection",
            "property-execution",
            "storage-ownership-reflection",
        }
    ]
    return {
        "contract_id": RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
            "<emit-prefix>.compile-provenance.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
            "objc3c.executable.realization.records.v1",
            "objc3c.runtime.property.metadata.reflection.v1",
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        ],
        "lowering_artifact_boundary_model": (
            "compile-manifest-registration-descriptor-object-and-llvm-ir-co-publish-realization-lowering-and-reflection-artifacts"
        ),
        "reflection_artifact_handoff_model": (
            "property-metadata-and-ownership-artifacts-remain-coupled-to-lowered-dispatch-accessor-and-executable-realization-record-outputs"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_reflection_artifact_query_boundary": [
            "objc3_runtime_copy_property_registry_state_for_testing",
            "objc3_runtime_copy_property_entry_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "requires_coupled_registration_descriptor_artifact": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_compile_output_truthfulness": True,
    }


def build_runtime_dispatch_table_reflection_record_lowering_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-dispatch",
            "canonical-sample-set",
            "dispatch-fast-path",
            "property-execution",
        }
    ]
    return {
        "contract_id": RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
            "<emit-prefix>.compile-provenance.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
            "objc3c.method.dispatch.selector.thunk.lowering.v1",
            "objc3c.executable.realization.records.v1",
        ],
        "dispatch_table_lowering_model": (
            "selector-pool-backed-dispatch-thunks-and-runtime-dispatch-sites-co-publish-stable-selector-table-roots-in-llvm-ir-and-manifest-artifacts"
        ),
        "reflection_record_lowering_model": (
            "realization-records-and-runtime-metadata-section-aggregates-co-publish-class-protocol-category-property-and-ivar-record-roots-in-emitted-artifacts"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3",
            "tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp",
            "tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp",
        ],
        "requires_coupled_registration_descriptor_artifact": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_compile_output_truthfulness": True,
    }


def build_runtime_cross_module_realized_metadata_replay_preservation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id == "imported-runtime-packaging-replay"
    ]
    return {
        "contract_id": RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.runtime-import-surface.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.cross-module-runtime-link-plan.json",
            "<emit-prefix>.cross-module-runtime-linker-options.rsp",
        ],
        "source_contract_ids": [
            "objc3c.cross.module.runtime.packaging.link.plan.v1",
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
            RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        ],
        "realized_metadata_replay_preservation_model": (
            "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
        ],
        "authoritative_probe_paths": [IMPORTED_RUNTIME_PACKAGING_PROBE],
        "requires_runtime_import_surface_artifact": True,
        "requires_cross_module_link_plan_artifact": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_object_model_abi_query_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "imported-runtime-packaging-replay",
            "canonical-dispatch",
            "canonical-sample-set",
            "realization-lookup-reflection-runtime",
            "dispatch-fast-path",
            "property-reflection",
            "property-execution",
            "storage-ownership-reflection",
        }
    ]
    return {
        "contract_id": RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
            RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
            RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        ],
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_object_model_query_boundary": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_property_registry_state_for_testing",
            "objc3_runtime_copy_property_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
            "objc3_runtime_copy_selector_lookup_table_state_for_testing",
            "objc3_runtime_copy_selector_lookup_entry_for_testing",
            "objc3_runtime_copy_method_cache_state_for_testing",
            "objc3_runtime_copy_method_cache_entry_for_testing",
            "objc3_runtime_copy_dispatch_state_for_testing",
        ],
        "object_model_query_boundary_model": (
            "public-runtime-header-plus-private-testing-snapshots-freeze-the-object-model-lookup-and-reflection-query-surface-without-widening-the-public-abi"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3",
            "tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp",
            REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
            "tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_realization_lookup_reflection_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "realization-lookup-reflection-runtime",
            "canonical-sample-set",
            "dispatch-fast-path",
        }
    ]
    return {
        "contract_id": RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        ],
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "object_model_query_state_snapshot_symbol": (
            "objc3_runtime_copy_object_model_query_state_for_testing"
        ),
        "realized_class_entry_snapshot_symbol": (
            "objc3_runtime_copy_realized_class_entry_for_testing"
        ),
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "protocol_conformance_query_symbol": (
            "objc3_runtime_copy_protocol_conformance_query_for_testing"
        ),
        "selector_lookup_table_state_snapshot_symbol": (
            "objc3_runtime_copy_selector_lookup_table_state_for_testing"
        ),
        "selector_lookup_entry_snapshot_symbol": (
            "objc3_runtime_copy_selector_lookup_entry_for_testing"
        ),
        "method_cache_state_snapshot_symbol": (
            "objc3_runtime_copy_method_cache_state_for_testing"
        ),
        "method_cache_entry_snapshot_symbol": (
            "objc3_runtime_copy_method_cache_entry_for_testing"
        ),
        "dispatch_state_snapshot_symbol": "objc3_runtime_copy_dispatch_state_for_testing",
        "realization_lookup_reflection_implementation_model": (
            "runtime-owned-class-property-protocol-and-method-cache-query-snapshots-publish-coherent-last-query-state-and-live-object-model-counts"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3",
        ],
        "authoritative_probe_paths": [
            REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
            "tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_reflection_query_surface(results: list[CaseResult]) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-sample-set",
            "property-reflection",
            "storage-ownership-reflection",
        }
    ]
    return {
        "contract_id": RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            "objc3c.runtime.dispatch_accessor.abi.surface.v1",
            "objc3c.runtime.property.metadata.reflection.v1",
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        ],
        "query_api_boundary_model": (
            "private-testing-snapshots-over-runtime-owned-realized-class-property-and-protocol-metadata-with-no-public-reflection-abi"
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_query_symbols": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_property_registry_state_for_testing",
            "objc3_runtime_copy_property_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "no_public_reflection_abi": True,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_realization_lookup_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-dispatch",
            "canonical-sample-set",
            "dispatch-fast-path",
        }
    ]
    return {
        "contract_id": RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            "objc3c.runtime.dispatch_accessor.abi.surface.v1",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_lookup_query_boundary": [
            "objc3_runtime_copy_selector_lookup_table_state_for_testing",
            "objc3_runtime_copy_selector_lookup_entry_for_testing",
            "objc3_runtime_copy_method_cache_state_for_testing",
            "objc3_runtime_copy_method_cache_entry_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
        ],
        "lookup_resolution_order_model": (
            "seeded-cache-then-live-class-chain-then-attached-category-and-protocol-checks-then-deterministic-fallback"
        ),
        "selector_materialization_model": (
            "metadata-selectors-materialized-at-registration-and-dynamic-misses-interned-at-first-lookup"
        ),
        "unresolved_selector_behavior_model": (
            "negative-cache-entry-preserved-and-deterministic-fallback-returned"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_class_metaclass_protocol_realization_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "imported-runtime-packaging-replay",
            "canonical-dispatch",
        }
    ]
    return {
        "contract_id": RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_realization_query_boundary": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
        ],
        "class_realization_model": (
            "registration-installs-runtime-backed-class-records-before-live-dispatch-and-reflection"
        ),
        "metaclass_lineage_model": (
            "realized-class-entries-publish-stable-class-metaclass-superclass-and-super-metaclass-owner-identities"
        ),
        "protocol_conformance_model": (
            "realized-class-entries-and-runtime-conformance-queries-publish-direct-and-attached-protocol-conformance"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_category_attachment_merged_dispatch_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "imported-runtime-packaging-replay",
            "canonical-dispatch",
            "canonical-sample-set",
        }
    ]
    return {
        "contract_id": RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            "objc3c.runtime.category.attachment.protocol.conformance.v1",
            RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
            RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_category_query_boundary": [
            "objc3_runtime_copy_realized_class_graph_state_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
            "objc3_runtime_copy_method_cache_state_for_testing",
            "objc3_runtime_copy_method_cache_entry_for_testing",
        ],
        "category_attachment_model": (
            "registration-attaches-category-owned-instance-and-protocol-members-onto-live-realized-classes-before-dispatch"
        ),
        "merged_dispatch_resolution_model": (
            "attached-category-implementations-override-base-class-instance-lookup-before-superclass-and-protocol-fallback"
        ),
        "attached_protocol_visibility_model": (
            "attached-categories-publish-owner-and-name-through-realized-class-entries-and-protocol-conformance-queries"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
            IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
            "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
            "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3",
        ],
        "authoritative_probe_paths": [
            IMPORTED_RUNTIME_PACKAGING_PROBE,
            "tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
            "tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_reflection_visibility_coherence_diagnostics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {
            "canonical-sample-set",
            "property-reflection-accessor-compatibility-diagnostics",
            "property-reflection",
            "property-execution",
            "storage-ownership-reflection",
        }
    ]
    return {
        "contract_id": RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.runtime-registration-descriptor.json",
        ],
        "source_contract_ids": [
            RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
            RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
            "objc3c.runtime.dispatch_accessor.abi.surface.v1",
            "objc3c.runtime.property.metadata.reflection.v1",
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1",
        ],
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_coherence_query_boundary": [
            "objc3_runtime_copy_property_registry_state_for_testing",
            "objc3_runtime_copy_property_entry_for_testing",
            "objc3_runtime_copy_realized_class_entry_for_testing",
            "objc3_runtime_copy_protocol_conformance_query_for_testing",
            "objc3_runtime_copy_method_cache_state_for_testing",
        ],
        "reflection_visibility_boundary_model": (
            "private-testing-snapshots-remain-the-only-reflection-visibility-surface-and-publish-runtime-owned-class-property-and-protocol-state"
        ),
        "fail_closed_lookup_diagnostic_model": (
            "missing-class-and-property-lookups-publish-found-zero-without-mutating-property-registry-or-realized-class-state"
        ),
        "runtime_coherence_diagnostic_model": (
            "reflected-property-selectors-owner-identities-slot-layout-and-ownership-profiles-must-match-live-dispatch-realized-class-and-attached-protocol-state"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3",
            "tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3",
            "tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/m280_b004_property_accessor_selector_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/m280_b004_property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_acceptance_suite_surface(results: list[CaseResult], report_path: Path) -> dict[str, Any]:
    compile_coupled_case_ids = [result.case_id for result in results if result.fixture is not None]
    linked_runtime_probe_case_ids = [
        result.case_id for result in results if result.claim_class == "linked-runtime-probe"
    ]
    compile_coupled_inspection_case_ids = [
        result.case_id for result in results if result.claim_class == "compile-coupled-inspection"
    ]
    return {
        "contract_id": RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID,
        "suite_path": "scripts/check_objc3c_runtime_acceptance.py",
        "report_path": str(report_path.relative_to(ROOT)).replace("\\", "/"),
        "consumes_runtime_state_publication_surface_contract_id": RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
        "authoritative_claim_classes": [
            "linked-runtime-probe",
            "compile-coupled-inspection",
        ],
        "linked_runtime_probe_case_ids": linked_runtime_probe_case_ids,
        "compile_coupled_case_ids": compile_coupled_case_ids,
        "compile_coupled_inspection_case_ids": compile_coupled_inspection_case_ids,
        "compile_output_provenance_contract_id": COMPILE_PROVENANCE_CONTRACT_ID,
        "compile_output_truthfulness_contract_id": COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID,
        "coupled_artifact_requirements": [
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.compile-provenance.json",
        ],
    }


def build_runtime_installation_abi_surface() -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID,
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "bootstrap_api_contract_id": "objc3c.runtime.bootstrap.api.freeze.v1",
        "bootstrap_reset_contract_id": "objc3c.runtime.bootstrap.reset.replay.v1",
        "bootstrap_registrar_contract_id": "objc3c.runtime.bootstrap.registrar.image.walk.v1",
        "public_installation_abi_boundary": RUNTIME_INSTALLATION_ABI_BOUNDARY,
        "private_loader_testing_boundary": RUNTIME_LOADER_TESTING_BOUNDARY,
        "installation_requires_coupled_registration_manifest": True,
        "register_image_consumes_staged_registration_table_once": True,
        "deterministic_reset_replay_supported": True,
    }


def build_runtime_loader_lifecycle_surface(results: list[CaseResult]) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id for result in results if result.case_id == "installation-lifecycle"
    ]
    return {
        "contract_id": RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID,
        "runtime_installation_abi_surface_contract_id": RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID,
        "bootstrap_semantics_contract_id": "objc3c.runtime.startup.bootstrap.semantics.v1",
        "bootstrap_reset_contract_id": "objc3c.runtime.bootstrap.reset.replay.v1",
        "bootstrap_registrar_contract_id": "objc3c.runtime.bootstrap.registrar.image.walk.v1",
        "authoritative_probe_path": INSTALLATION_LIFECYCLE_PROBE,
        "authoritative_case_ids": authoritative_case_ids,
        "loader_testing_boundary_symbols": RUNTIME_LOADER_TESTING_BOUNDARY,
        "lifecycle_phases": [
            "startup-installed-runtime-state",
            "duplicate-registration-rejected-without-state-advance",
            "out-of-order-registration-rejected-without-state-advance",
            "invalid-anchor-root-rejected-without-state-advance",
            "invalid-discovery-root-rejected-without-state-advance",
            "reset-retained-bootstrap-catalog",
            "replay-restored-installed-runtime-state",
        ],
        "rejected_registration_status_codes": {
            "duplicate_translation_unit_identity_key": -2,
            "out_of_order_registration": -3,
            "invalid_registration_roots": -4,
        },
        "retained_bootstrap_catalog_required": True,
        "deterministic_replay_required": True,
        "requires_linked_fixture_or_loader_retained_roots": True,
    }


def check_runtime_library_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "runtime-library"
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_library_probe.cpp"
    exe_path = case_dir / "runtime_library_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    run_probe(exe_path)
    return CaseResult(
        case_id="runtime-library",
        probe="tests/tooling/runtime/runtime_library_probe.cpp",
        fixture=None,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={"kind": "standalone-runtime-probe"},
    )


def check_installation_lifecycle_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_started = perf_counter()
    case_dir = run_dir / "installation-lifecycle"
    fixture = ROOT / Path(INSTALLATION_LIFECYCLE_FIXTURE)
    probe = ROOT / Path(INSTALLATION_LIFECYCLE_PROBE)
    compile_started = perf_counter()
    obj_path = compile_fixture(fixture, case_dir / "compile")
    fixture_compile_ms = int((perf_counter() - compile_started) * 1000)
    registration_descriptor = json.loads(
        (case_dir / "compile" / "module.runtime-registration-descriptor.json").read_text(
            encoding="utf-8"
        )
    )
    probe_fixture_config = case_dir / "runtime_installation_loader_lifecycle_probe_fixture_config.h"
    probe_fixture_config.write_text(
        "\n".join(
            [
                "#pragma once",
                f"#define OBJC3_RUNTIME_FIXTURE_MODULE_NAME {json.dumps(registration_descriptor['registration_descriptor_identifier'].removesuffix('_registration_descriptor'))}",
                f"#define OBJC3_RUNTIME_FIXTURE_TRANSLATION_UNIT_IDENTITY_KEY {json.dumps(registration_descriptor['translation_unit_identity_key'])}",
                f"#define OBJC3_RUNTIME_FIXTURE_REGISTRATION_ORDER_ORDINAL {registration_descriptor['translation_unit_registration_order_ordinal']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_CLASS_DESCRIPTOR_COUNT {registration_descriptor['class_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_PROTOCOL_DESCRIPTOR_COUNT {registration_descriptor['protocol_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_CATEGORY_DESCRIPTOR_COUNT {registration_descriptor['category_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_PROPERTY_DESCRIPTOR_COUNT {registration_descriptor['property_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_IVAR_DESCRIPTOR_COUNT {registration_descriptor['ivar_descriptor_count']}ULL",
                "",
            ]
        ),
        encoding="utf-8",
    )
    exe_path = case_dir / "runtime_installation_loader_lifecycle_probe.exe"
    probe_link_started = perf_counter()
    compile_probe_with_args(
        clangxx,
        probe,
        exe_path,
        [obj_path],
        [
            "-include",
            str(probe_fixture_config),
        ],
    )
    probe_link_ms = int((perf_counter() - probe_link_started) * 1000)
    probe_run_started = perf_counter()
    payload = parse_json_output(run_probe(exe_path), "runtime installation loader lifecycle probe")
    probe_run_ms = int((perf_counter() - probe_run_started) * 1000)
    case_total_ms = int((perf_counter() - case_started) * 1000)

    expect(payload.get("startup_registration_copy_status") == 0, "expected startup registration snapshot copy to succeed")
    expect(payload.get("startup_image_walk_copy_status") == 0, "expected startup image walk snapshot copy to succeed")
    expect(payload.get("startup_reset_replay_copy_status") == 0, "expected startup reset/replay snapshot copy to succeed")
    expect(payload.get("startup_registered_image_count") == 1, "expected startup runtime installation state to contain one registered image")
    expect(payload.get("startup_next_expected_registration_order_ordinal") == 2, "expected startup installation state to advance the next registration ordinal")
    expect(payload.get("startup_walked_image_count") == 1, "expected startup image walk state to publish one walked image")
    expect(payload.get("startup_last_discovery_root_entry_count", 0) > 0, "expected startup image walk state to publish a non-empty discovery root")
    expect(payload.get("startup_last_registration_used_staged_table") == 1, "expected startup installation state to consume the staged registration table")
    expect(payload.get("startup_retained_bootstrap_image_count") == 1, "expected startup reset/replay state to retain one bootstrap image")

    startup_module_name = payload.get("startup_last_registered_module_name")
    startup_identity_key = payload.get("startup_last_registered_translation_unit_identity_key")
    expect(isinstance(startup_module_name, str) and startup_module_name != "", "expected startup installation state to publish a registered module name")
    expect(isinstance(startup_identity_key, str) and startup_identity_key != "", "expected startup installation state to publish a registered translation unit identity key")
    expect(payload.get("duplicate_status") == -2, "expected duplicate registration to fail with duplicate translation-unit identity status")
    expect(payload.get("after_duplicate_registration_copy_status") == 0, "expected duplicate rejection registration snapshot copy to succeed")
    expect(payload.get("after_duplicate_image_walk_copy_status") == 0, "expected duplicate rejection image walk snapshot copy to succeed")
    expect(payload.get("after_duplicate_registered_image_count") == 1, "expected duplicate rejection to leave installed image count unchanged")
    expect(payload.get("after_duplicate_next_expected_registration_order_ordinal") == 2, "expected duplicate rejection to preserve the next expected registration ordinal")
    expect(payload.get("after_duplicate_last_successful_registration_order_ordinal") == 1, "expected duplicate rejection to preserve the last successful registration ordinal")
    expect(payload.get("after_duplicate_last_registration_status") == -2, "expected duplicate rejection snapshot to publish duplicate registration status")
    expect(payload.get("after_duplicate_last_rejected_module_name") == startup_module_name, "expected duplicate rejection snapshot to publish the rejected module name")
    expect(payload.get("after_duplicate_last_rejected_translation_unit_identity_key") == startup_identity_key, "expected duplicate rejection snapshot to publish the rejected translation unit identity key")
    expect(payload.get("after_duplicate_last_rejected_registration_order_ordinal") == 1, "expected duplicate rejection snapshot to publish the rejected registration ordinal")
    expect(payload.get("after_duplicate_walked_image_count") == 1, "expected duplicate rejection to leave image walk state unchanged")
    expect(payload.get("out_of_order_status") == -3, "expected out-of-order registration to fail with out-of-order status")
    expect(payload.get("after_out_of_order_registration_copy_status") == 0, "expected out-of-order rejection registration snapshot copy to succeed")
    expect(payload.get("after_out_of_order_image_walk_copy_status") == 0, "expected out-of-order rejection image walk snapshot copy to succeed")
    expect(payload.get("after_out_of_order_registered_image_count") == 1, "expected out-of-order rejection to leave installed image count unchanged")
    expect(payload.get("after_out_of_order_next_expected_registration_order_ordinal") == 2, "expected out-of-order rejection to preserve the next expected registration ordinal")
    expect(payload.get("after_out_of_order_last_successful_registration_order_ordinal") == 1, "expected out-of-order rejection to preserve the last successful registration ordinal")
    expect(payload.get("after_out_of_order_last_registration_status") == -3, "expected out-of-order rejection snapshot to publish out-of-order status")
    expect(payload.get("after_out_of_order_last_rejected_module_name") == "out-of-order-module", "expected out-of-order rejection snapshot to publish the rejected module name")
    expect(
        payload.get("after_out_of_order_last_rejected_translation_unit_identity_key")
        == startup_identity_key + "-out-of-order",
        "expected out-of-order rejection snapshot to publish the rejected translation unit identity key",
    )
    expect(payload.get("after_out_of_order_last_rejected_registration_order_ordinal") == 3, "expected out-of-order rejection snapshot to publish the rejected registration ordinal")
    expect(payload.get("after_out_of_order_walked_image_count") == 1, "expected out-of-order rejection to leave image walk state unchanged")

    expect(payload.get("post_reset_registration_copy_status") == 0, "expected post-reset registration snapshot copy to succeed")
    expect(payload.get("post_reset_reset_replay_copy_status") == 0, "expected post-reset reset/replay snapshot copy to succeed")
    expect(payload.get("post_reset_registered_image_count") == 0, "expected reset to clear installed runtime images")
    expect(payload.get("post_reset_next_expected_registration_order_ordinal") == 1, "expected reset to restore the initial registration ordinal")
    expect(payload.get("post_reset_retained_bootstrap_image_count") == 1, "expected reset to retain one bootstrap image for replay")
    expect(payload.get("post_reset_last_reset_cleared_image_local_init_state_count") == 1, "expected reset to clear one image-local initialization state record")
    expect(payload.get("post_reset_invalid_anchor_status") == -4, "expected mismatched linker-anchor registration to fail with invalid registration roots status")
    expect(payload.get("after_invalid_anchor_registration_copy_status") == 0, "expected invalid-anchor rejection registration snapshot copy to succeed")
    expect(payload.get("after_invalid_anchor_image_walk_copy_status") == 0, "expected invalid-anchor rejection image walk snapshot copy to succeed")
    expect(payload.get("after_invalid_anchor_registered_image_count") == 0, "expected invalid-anchor rejection to preserve the cleared installed image count")
    expect(payload.get("after_invalid_anchor_next_expected_registration_order_ordinal") == 1, "expected invalid-anchor rejection to preserve the reset registration ordinal")
    expect(payload.get("after_invalid_anchor_last_registration_status") == -4, "expected invalid-anchor rejection snapshot to publish invalid registration roots status")
    expect(payload.get("after_invalid_anchor_walked_image_count") == 0, "expected invalid-anchor rejection to leave image walk state empty")
    expect(payload.get("after_invalid_anchor_last_linker_anchor_matches_discovery_root") == 0, "expected invalid-anchor rejection to leave linker-anchor/discovery-root proof unset")
    expect(payload.get("post_reset_invalid_discovery_root_status") == -4, "expected malformed discovery-root registration to fail with invalid registration roots status")
    expect(payload.get("after_invalid_discovery_root_registration_copy_status") == 0, "expected invalid-discovery-root rejection registration snapshot copy to succeed")
    expect(payload.get("after_invalid_discovery_root_image_walk_copy_status") == 0, "expected invalid-discovery-root rejection image walk snapshot copy to succeed")
    expect(payload.get("after_invalid_discovery_root_registered_image_count") == 0, "expected invalid-discovery-root rejection to preserve the cleared installed image count")
    expect(payload.get("after_invalid_discovery_root_next_expected_registration_order_ordinal") == 1, "expected invalid-discovery-root rejection to preserve the reset registration ordinal")
    expect(payload.get("after_invalid_discovery_root_last_registration_status") == -4, "expected invalid-discovery-root rejection snapshot to publish invalid registration roots status")
    expect(payload.get("after_invalid_discovery_root_walked_image_count") == 0, "expected invalid-discovery-root rejection to leave image walk state empty")
    expect(payload.get("after_invalid_discovery_root_last_linker_anchor_matches_discovery_root") == 0, "expected invalid-discovery-root rejection to leave linker-anchor/discovery-root proof unset")

    expect(payload.get("replay_status") == 0, "expected replay_registered_images_for_testing to succeed")
    expect(payload.get("post_replay_registration_copy_status") == 0, "expected post-replay registration snapshot copy to succeed")
    expect(payload.get("post_replay_image_walk_copy_status") == 0, "expected post-replay image walk snapshot copy to succeed")
    expect(payload.get("post_replay_reset_replay_copy_status") == 0, "expected post-replay reset/replay snapshot copy to succeed")
    expect(payload.get("post_replay_registered_image_count") == 1, "expected replay to restore one registered runtime image")
    expect(payload.get("post_replay_next_expected_registration_order_ordinal") == 2, "expected replay to restore the next registration ordinal")
    expect(payload.get("post_replay_walked_image_count") == 1, "expected replay to restore one walked image")
    expect(payload.get("post_replay_last_discovery_root_entry_count") == payload.get("startup_last_discovery_root_entry_count"), "expected replay to preserve discovery root entry count")
    expect(payload.get("post_replay_last_registration_used_staged_table") == 1, "expected replay registration to consume the staged registration table")
    expect(payload.get("post_replay_retained_bootstrap_image_count") == 1, "expected replay to preserve the retained bootstrap catalog")
    expect(payload.get("post_replay_last_replayed_image_count") == 1, "expected replay state to publish one replayed image")
    expect(payload.get("post_replay_replay_generation", 0) >= 1, "expected replay state to advance the replay generation")
    expect(payload.get("post_replay_last_replay_status") == 0, "expected replay state to publish a successful replay status")
    expect(payload.get("post_replay_last_registered_module_name") == startup_module_name, "expected replay to restore the registered module name")
    expect(payload.get("post_replay_last_walked_module_name") == startup_module_name, "expected replay image walk state to publish the registered module name")
    expect(payload.get("post_replay_last_replayed_module_name") == startup_module_name, "expected replay state to publish the replayed module name")
    expect(payload.get("post_replay_last_registered_translation_unit_identity_key") == startup_identity_key, "expected replay to restore the registered translation unit identity key")
    expect(payload.get("post_replay_last_walked_translation_unit_identity_key") == startup_identity_key, "expected replay image walk state to publish the translation unit identity key")
    expect(payload.get("post_replay_last_replayed_translation_unit_identity_key") == startup_identity_key, "expected replay state to publish the replayed translation unit identity key")

    return CaseResult(
        case_id="installation-lifecycle",
        probe="tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp",
        fixture="tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "fixture_compile_ms": fixture_compile_ms,
            "probe_link_ms": probe_link_ms,
            "probe_run_ms": probe_run_ms,
            "case_total_ms": case_total_ms,
            "duplicate_status": payload["duplicate_status"],
            "duplicate_rejected_module_name": payload["after_duplicate_last_rejected_module_name"],
            "duplicate_rejected_translation_unit_identity_key": payload[
                "after_duplicate_last_rejected_translation_unit_identity_key"
            ],
            "duplicate_rejected_registration_order_ordinal": payload[
                "after_duplicate_last_rejected_registration_order_ordinal"
            ],
            "out_of_order_status": payload["out_of_order_status"],
            "out_of_order_rejected_module_name": payload[
                "after_out_of_order_last_rejected_module_name"
            ],
            "out_of_order_rejected_translation_unit_identity_key": payload[
                "after_out_of_order_last_rejected_translation_unit_identity_key"
            ],
            "out_of_order_rejected_registration_order_ordinal": payload[
                "after_out_of_order_last_rejected_registration_order_ordinal"
            ],
            "invalid_anchor_status": payload["post_reset_invalid_anchor_status"],
            "invalid_discovery_root_status": payload["post_reset_invalid_discovery_root_status"],
            "startup_registered_image_count": payload["startup_registered_image_count"],
            "post_reset_registered_image_count": payload["post_reset_registered_image_count"],
            "post_replay_registered_image_count": payload["post_replay_registered_image_count"],
            "retained_bootstrap_image_count": payload["post_replay_retained_bootstrap_image_count"],
            "replay_generation": payload["post_replay_replay_generation"],
        },
    )


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
        link_plan.get("direct_import_input_count") == 1,
        "expected cross-module link plan to preserve one direct imported runtime surface",
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

    exe_path = case_dir / "m258_e002_import_module_execution_matrix_probe.exe"
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
        and imported_provider_class_value != 0,
        "expected imported provider class dispatch to succeed",
    )
    expect(
        isinstance(imported_provider_protocol_value, int)
        and imported_provider_protocol_value != 0,
        "expected imported provider protocol dispatch to succeed",
    )
    expect(
        isinstance(local_consumer_class_value, int)
        and local_consumer_class_value != 0,
        "expected local consumer class dispatch to succeed",
    )
    expect(
        len(
            {
                imported_provider_class_value,
                imported_provider_protocol_value,
                local_consumer_class_value,
            }
        )
        == 3,
        "expected imported and local selector dispatch results to remain distinct",
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
    expect(payload.get("method_cache_live_dispatch_count") == 0, "expected imported-runtime startup to avoid live dispatch fast-path entries")
    expect(payload.get("method_cache_fallback_dispatch_count") == 3, "expected imported-runtime startup to publish three metadata-backed fallback dispatches")
    expect(payload.get("method_cache_last_selector") == "localClassValue", "expected imported-runtime startup to publish the last resolved selector")
    expect(payload.get("method_cache_last_resolved_class_name") is None, "expected imported-runtime startup to keep the method-cache class name unset for metadata-backed fallback dispatch")
    expect(payload.get("method_cache_last_resolved_owner_identity") is None, "expected imported-runtime startup to keep the method-cache owner identity unset for metadata-backed fallback dispatch")
    expect(payload.get("provider_method_status") == 0 and payload.get("provider_method_found") == 1 and payload.get("provider_method_resolved") == 0, "expected provider class method metadata to remain an unresolved fallback entry at startup")
    expect(payload.get("provider_method_owner_identity") is None, "expected provider class fallback method metadata to avoid a resolved owner identity at startup")
    expect(payload.get("imported_protocol_method_status") == 0 and payload.get("imported_protocol_method_found") == 1 and payload.get("imported_protocol_method_resolved") == 0, "expected imported protocol method metadata to remain an unresolved fallback entry at startup")
    expect(payload.get("imported_protocol_method_owner_identity") is None, "expected imported protocol fallback metadata to avoid a resolved owner identity at startup")
    expect(payload.get("local_method_status") == 0 and payload.get("local_method_found") == 1 and payload.get("local_method_resolved") == 0, "expected local class method metadata to remain an unresolved fallback entry at startup")
    expect(payload.get("local_method_owner_identity") is None, "expected local class fallback metadata to avoid a resolved owner identity at startup")
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
        == imported_provider_class_value,
        "expected imported provider class dispatch to survive replay",
    )
    expect(
        payload.get("post_replay_imported_provider_protocol_value")
        == imported_provider_protocol_value,
        "expected imported provider protocol dispatch to survive replay",
    )
    expect(
        payload.get("post_replay_local_consumer_class_value")
        == local_consumer_class_value,
        "expected local consumer class dispatch to survive replay",
    )
    expect(payload.get("post_replay_selector_table_status") == 0, "expected replay selector-table snapshot copy to succeed")
    expect(payload.get("post_replay_selector_table_entry_count") == 3, "expected replay to restore three selector entries")
    expect(payload.get("post_replay_selector_metadata_backed_selector_count") == 3, "expected replay to restore three metadata-backed selectors")
    expect(payload.get("post_replay_provider_selector_status") == 0 and payload.get("post_replay_provider_selector_found") == 1, "expected provider selector metadata to survive replay")
    expect(payload.get("post_replay_imported_protocol_selector_status") == 0 and payload.get("post_replay_imported_protocol_selector_found") == 1, "expected imported protocol selector metadata to survive replay")
    expect(payload.get("post_replay_local_selector_status") == 0 and payload.get("post_replay_local_selector_found") == 1, "expected local selector metadata to survive replay")
    expect(payload.get("post_replay_method_cache_state_status") == 0, "expected replay method-cache snapshot copy to succeed")
    expect(payload.get("post_replay_method_cache_entry_count") == 3, "expected replay to restore three method-cache entries")
    expect(payload.get("post_replay_method_cache_live_dispatch_count") == 0, "expected replay to keep method-cache entries on the fallback path")
    expect(payload.get("post_replay_method_cache_fallback_dispatch_count") == 3, "expected replay to restore three metadata-backed fallback dispatches")
    expect(payload.get("post_replay_method_cache_last_selector") == "localClassValue", "expected replay to preserve the last resolved selector")
    expect(payload.get("post_replay_method_cache_last_resolved_class_name") is None, "expected replay to keep the method-cache class name unset for metadata-backed fallback dispatch")
    expect(payload.get("post_replay_method_cache_last_resolved_owner_identity") is None, "expected replay to keep the method-cache owner identity unset for metadata-backed fallback dispatch")
    expect(payload.get("post_replay_provider_method_status") == 0 and payload.get("post_replay_provider_method_found") == 1 and payload.get("post_replay_provider_method_resolved") == 0, "expected provider class fallback metadata to survive replay")
    expect(payload.get("post_replay_provider_method_owner_identity") is None, "expected provider class fallback metadata to avoid a resolved owner identity after replay")
    expect(payload.get("post_replay_imported_protocol_method_status") == 0 and payload.get("post_replay_imported_protocol_method_found") == 1 and payload.get("post_replay_imported_protocol_method_resolved") == 0, "expected imported protocol fallback metadata to survive replay")
    expect(payload.get("post_replay_imported_protocol_method_owner_identity") is None, "expected imported protocol fallback metadata to avoid a resolved owner identity after replay")
    expect(payload.get("post_replay_local_method_status") == 0 and payload.get("post_replay_local_method_found") == 1 and payload.get("post_replay_local_method_resolved") == 0, "expected local class fallback metadata to survive replay")
    expect(payload.get("post_replay_local_method_owner_identity") is None, "expected local class fallback metadata to avoid a resolved owner identity after replay")

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


def check_canonical_dispatch_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "canonical-dispatch"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "runtime_canonical_runnable_object_runtime_library.objc3"
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_canonical_runnable_object_probe.cpp"
    exe_path = case_dir / "runtime_canonical_runnable_object_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "canonical dispatch probe")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")

    expect(payload.get("traced_value") == 13, "expected category-backed tracedValue dispatch to return 13")
    expect(payload.get("inherited_value") == 7, "expected superclass inheritedValue dispatch to return 7")
    expect(payload.get("class_value") == 11, "expected class method dispatch to return 11")
    expect(payload.get("alloc_value", 0) != 0, "expected alloc dispatch to return a realized instance receiver")
    expect(payload.get("init_value") == payload.get("alloc_value"), "expected init to preserve the allocated receiver")
    expect(payload.get("new_value", 0) != 0, "expected new dispatch to materialize an instance receiver")
    expect(payload.get("ignored_value") == payload.get("ignored_expected"),
           "expected unresolved selector dispatch to return the deterministic fallback value")
    expect(payload.get("ignored_cached_value") == payload.get("ignored_expected"),
           "expected cached unresolved selector dispatch to preserve the deterministic fallback value")

    worker_query = payload.get("worker_query", {})
    tracer_query = payload.get("tracer_query", {})
    graph_state = payload.get("graph_state", {})
    widget_entry = payload.get("widget_entry", {})
    method_state = payload.get("method_state", {})
    inherited_state = payload.get("inherited_state", {})
    traced_state = payload.get("traced_state", {})
    class_state = payload.get("class_state", {})
    ignored_state = payload.get("ignored_state", {})
    ignored_cached_state = payload.get("ignored_cached_state", {})
    selector_handles = payload.get("selector_handles", {})
    selector_table_state = payload.get("selector_table_state", {})
    traced_selector_entry = payload.get("traced_selector_entry", {})
    inherited_selector_entry = payload.get("inherited_selector_entry", {})
    class_selector_entry = payload.get("class_selector_entry", {})
    ignored_selector_entry = payload.get("ignored_selector_entry", {})
    traced_entry = payload.get("traced_entry", {})
    inherited_entry = payload.get("inherited_entry", {})
    class_entry = payload.get("class_entry", {})
    ignored_entry = payload.get("ignored_entry", {})
    alloc_entry = payload.get("alloc_entry", {})
    init_entry = payload.get("init_entry", {})
    new_entry = payload.get("new_entry", {})

    expect(worker_query.get("conforms") == 1, "expected Widget to conform to Worker at runtime")
    expect(tracer_query.get("conforms") == 1, "expected Widget category attachment to satisfy Tracer at runtime")
    expect(
        graph_state.get("realized_class_count") == 2
        and graph_state.get("root_class_count") == 1
        and graph_state.get("metaclass_edge_count") == 1
        and graph_state.get("receiver_class_binding_count") == 2
        and graph_state.get("attached_category_count") == 1
        and graph_state.get("protocol_conformance_edge_count") == 2
        and graph_state.get("last_realized_class_name") == "Widget"
        and graph_state.get("last_realized_class_owner_identity") == "class:Widget"
        and graph_state.get("last_realized_metaclass_owner_identity") == "metaclass:Widget"
        and graph_state.get("last_attached_category_owner_identity") == "category:Widget(Tracing)"
        and graph_state.get("last_attached_category_name") == "Tracing",
        "expected canonical dispatch to publish the realized Widget class graph with stable class, metaclass, and attached-category lineage",
    )
    expect(
        widget_entry.get("found") == 1
        and widget_entry.get("is_root_class") == 0
        and widget_entry.get("implementation_backed") == 1
        and widget_entry.get("attached_category_count") == 1
        and widget_entry.get("direct_protocol_count") == 1
        and widget_entry.get("attached_protocol_count") == 1
        and widget_entry.get("class_name") == "Widget"
        and widget_entry.get("class_owner_identity") == "class:Widget"
        and widget_entry.get("metaclass_owner_identity") == "metaclass:Widget"
        and widget_entry.get("super_class_owner_identity") == "class:Base"
        and widget_entry.get("super_metaclass_owner_identity") == "metaclass:Base"
        and widget_entry.get("last_attached_category_owner_identity") == "category:Widget(Tracing)"
        and widget_entry.get("last_attached_category_name") == "Tracing",
        "expected canonical dispatch to publish stable Widget class, metaclass, superclass, attached-category, and protocol realization facts",
    )
    expect(
        tracer_query.get("matched_protocol_owner_identity") == "protocol:Tracer"
        and tracer_query.get("matched_attachment_owner_identity") == "category:Widget(Tracing)",
        "expected Tracer conformance to resolve through the attached Widget(Tracing) category",
    )
    expect(method_state.get("live_dispatch_count", 0) >= 6, "expected live dispatch count to cover alloc/init/new/traced/inherited/class")
    expect(method_state.get("fallback_dispatch_count", 0) == 2, "expected canonical dispatch workload to publish both unresolved fallback calls")
    expect(method_state.get("last_selector_stable_id", 0) == ignored_entry.get("selector_stable_id", 0),
           "expected last dispatch selector stable id to match the negative-cache ignoredValue selector")
    expect(selector_handles.get("alloc", 0) != 0 and selector_handles.get("tracedValue", 0) != 0,
           "expected canonical dispatch selectors to be interned in the runtime selector pool")
    expect(alloc_entry.get("selector_stable_id", 0) == selector_handles.get("alloc", 0),
           "expected alloc cache entry to be keyed by the selector pool stable id")
    expect(init_entry.get("selector_stable_id", 0) == selector_handles.get("init", 0),
           "expected init cache entry to be keyed by the selector pool stable id")
    expect(new_entry.get("selector_stable_id", 0) == selector_handles.get("new", 0),
           "expected new cache entry to be keyed by the selector pool stable id")
    expect(traced_entry.get("selector_stable_id", 0) == selector_handles.get("tracedValue", 0),
           "expected tracedValue cache entry to be keyed by the selector pool stable id")
    expect(inherited_entry.get("selector_stable_id", 0) == selector_handles.get("inheritedValue", 0),
           "expected inheritedValue cache entry to be keyed by the selector pool stable id")
    expect(class_entry.get("selector_stable_id", 0) == selector_handles.get("classValue", 0),
           "expected classValue cache entry to be keyed by the selector pool stable id")
    expect(traced_entry.get("resolved") == 1, "expected tracedValue cache entry to resolve live")
    expect(inherited_entry.get("resolved") == 1, "expected inheritedValue cache entry to resolve live")
    expect(class_entry.get("resolved") == 1, "expected classValue cache entry to resolve live")
    expect(selector_table_state.get("metadata_backed_selector_count", 0) >= 4,
           "expected canonical dispatch selector materialization to keep metadata-backed selectors interned")
    expect(selector_table_state.get("dynamic_selector_count", 0) >= 1,
           "expected unresolved selector dispatch to intern a dynamic selector entry")
    expect(selector_table_state.get("last_materialized_selector") == "ignoredValue",
           "expected ignoredValue to be the last materialized selector after the fallback probe")
    expect(selector_table_state.get("last_materialized_from_metadata") == 0,
           "expected ignoredValue to be recorded as a dynamic selector lookup")
    expect(
        traced_selector_entry.get("found") == 1
        and traced_selector_entry.get("metadata_backed") == 1
        and traced_selector_entry.get("canonical_selector") == "tracedValue",
        "expected tracedValue to remain metadata-backed in the selector table",
    )
    expect(
        inherited_selector_entry.get("found") == 1
        and inherited_selector_entry.get("metadata_backed") == 1
        and inherited_selector_entry.get("canonical_selector") == "inheritedValue",
        "expected inheritedValue to remain metadata-backed in the selector table",
    )
    expect(
        class_selector_entry.get("found") == 1
        and class_selector_entry.get("metadata_backed") == 1
        and class_selector_entry.get("canonical_selector") == "classValue",
        "expected classValue to remain metadata-backed in the selector table",
    )
    expect(
        ignored_selector_entry.get("found") == 1
        and ignored_selector_entry.get("metadata_backed") == 0
        and ignored_selector_entry.get("canonical_selector") == "ignoredValue",
        "expected ignoredValue to materialize as a dynamic selector-table entry",
    )
    expect(
        inherited_state.get("last_dispatch_used_cache") == 0
        and inherited_state.get("last_dispatch_resolved_live_method") == 1
        and inherited_state.get("last_dispatch_fell_back") == 0
        and inherited_state.get("last_selector_stable_id") == selector_handles.get("inheritedValue", 0)
        and inherited_state.get("last_normalized_receiver_identity") == 1042
        and inherited_state.get("last_category_probe_count") == 1
        and inherited_state.get("last_protocol_probe_count") == 3,
        "expected inheritedValue to miss cache first, resolve live, and preserve category/protocol probe counts",
    )
    expect(
        traced_state.get("last_dispatch_used_cache") == 0
        and traced_state.get("last_dispatch_resolved_live_method") == 1
        and traced_state.get("last_dispatch_fell_back") == 0
        and traced_state.get("last_selector_stable_id") == selector_handles.get("tracedValue", 0)
        and traced_state.get("last_normalized_receiver_identity") == 1042
        and traced_state.get("last_category_probe_count") == 1
        and traced_state.get("last_protocol_probe_count") == 0,
        "expected tracedValue to resolve live through the attached category without protocol fallback probes",
    )
    expect(
        class_state.get("last_dispatch_used_cache") == 0
        and class_state.get("last_dispatch_resolved_live_method") == 1
        and class_state.get("last_dispatch_fell_back") == 0
        and class_state.get("last_selector_stable_id") == selector_handles.get("classValue", 0)
        and class_state.get("last_normalized_receiver_identity") == 1043,
        "expected classValue to resolve live through the metaclass path",
    )
    expect(
        ignored_state.get("last_dispatch_used_cache") == 0
        and ignored_state.get("last_dispatch_resolved_live_method") == 0
        and ignored_state.get("last_dispatch_fell_back") == 1
        and ignored_state.get("last_selector_stable_id") == ignored_entry.get("selector_stable_id", 0)
        and ignored_state.get("last_normalized_receiver_identity") == 1042
        and ignored_state.get("last_category_probe_count") == 1
        and ignored_state.get("last_protocol_probe_count") == 3,
        "expected the first ignoredValue dispatch to materialize a negative cache entry and fall back deterministically",
    )
    expect(
        ignored_cached_state.get("last_dispatch_used_cache") == 1
        and ignored_cached_state.get("last_dispatch_resolved_live_method") == 0
        and ignored_cached_state.get("last_dispatch_fell_back") == 1
        and ignored_cached_state.get("last_selector_stable_id") == ignored_entry.get("selector_stable_id", 0)
        and ignored_cached_state.get("last_normalized_receiver_identity") == 1042
        and ignored_cached_state.get("last_category_probe_count") == 1
        and ignored_cached_state.get("last_protocol_probe_count") == 3,
        "expected the second ignoredValue dispatch to reuse the negative cache entry and preserve probe counts",
    )
    expect(
        traced_entry.get("resolved_owner_identity") == "implementation:Widget(Tracing)::instance_method:tracedValue",
        "expected tracedValue cache entry to preserve the category implementation owner",
    )
    expect(
        inherited_entry.get("normalized_receiver_identity") == 1042
        and inherited_entry.get("category_probe_count") == 1
        and inherited_entry.get("protocol_probe_count") == 3
        and inherited_entry.get("resolved_class_name") == "Base"
        and inherited_entry.get("resolved_owner_identity") == "implementation:Base::instance_method:inheritedValue",
        "expected inheritedValue cache entry to preserve the instance-family lookup result through Base",
    )
    expect(
        class_entry.get("dispatch_family_is_class") == 1
        and class_entry.get("normalized_receiver_identity") == 1043
        and class_entry.get("resolved_class_name") == "Widget"
        and class_entry.get("resolved_owner_identity") == "implementation:Widget::class_method:classValue",
        "expected classValue cache entry to preserve the metaclass lookup result",
    )
    expect(
        ignored_entry.get("found") == 1
        and ignored_entry.get("resolved") == 0
        and ignored_entry.get("dispatch_family_is_class") == 0
        and ignored_entry.get("normalized_receiver_identity") == 1042
        and ignored_entry.get("category_probe_count") == 1
        and ignored_entry.get("protocol_probe_count") == 3
        and ignored_entry.get("selector") == "ignoredValue",
        "expected ignoredValue to preserve an unresolved negative cache entry on the canonical instance receiver",
    )
    dispatch_table_reflection_record_lowering_surface = manifest.get(
        "runtime_dispatch_table_reflection_record_lowering_surface", {}
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("contract_id")
        == RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "expected canonical dispatch compile manifest to publish dispatch-table/reflection-record lowering surface",
    )
    expect(
        "; executable_realization_records = contract=objc3c.executable.realization.records.v1"
        in ll_text,
        "expected canonical dispatch LLVM IR to publish executable realization records",
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("selector_pool_section_root_symbol")
        == "@__objc3_sec_selector_pool",
        "expected canonical dispatch lowering surface to preserve the selector pool section root",
    )

    return CaseResult(
        case_id="canonical-dispatch",
        probe="tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
        fixture="tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "traced_value": payload["traced_value"],
            "inherited_value": payload["inherited_value"],
            "class_value": payload["class_value"],
            "live_dispatch_count": method_state["live_dispatch_count"],
            "attached_category_count": payload.get("graph_state", {}).get("attached_category_count"),
            "ignored_fallback": payload["ignored_expected"],
        },
    )


def check_canonical_sample_set_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "canonical-sample-set"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m259_a002_canonical_runnable_sample_set.objc3"
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    probe = ROOT / "tests" / "tooling" / "runtime" / "m259_a002_canonical_runnable_sample_set_probe.cpp"
    exe_path = case_dir / "m259_a002_canonical_runnable_sample_set_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "canonical runnable sample set probe")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")

    widget_entry = payload.get("widget_entry", {})
    worker_query = payload.get("worker_query", {})
    tracer_query = payload.get("tracer_query", {})
    count_property = payload.get("count_property", {})
    value_property = payload.get("value_property", {})
    token_property = payload.get("token_property", {})

    expect(widget_entry.get("found") == 1, "expected Widget to realize successfully for the canonical sample set")
    expect(widget_entry.get("base_identity") == 1041, "expected Widget base identity 1041 for the canonical sample set")
    expect(widget_entry.get("runtime_property_accessor_count") == 4, "expected Widget to expose four runtime property accessors")
    expect(widget_entry.get("runtime_instance_size_bytes") == 24, "expected Widget instance size to remain 24 bytes")
    expect(
        widget_entry.get("last_attached_category_owner_identity") == "category:Widget(Tracing)",
        "expected Widget to preserve the attached Tracing category owner",
    )

    expect(registration_manifest.get("class_descriptor_count") == 4, "expected canonical sample set to publish four class descriptors")
    expect(registration_manifest.get("protocol_descriptor_count") == 2, "expected canonical sample set to publish two protocol descriptors")
    expect(registration_manifest.get("category_descriptor_count") == 2, "expected canonical sample set to publish two category descriptors")
    expect(registration_manifest.get("property_descriptor_count") == 8, "expected canonical sample set to publish eight property descriptors")
    expect(registration_manifest.get("ivar_descriptor_count") == 4, "expected canonical sample set to publish four ivar descriptors")
    expect(
        registration_manifest.get("compile_output_truthfulness_property_descriptor_count") == 8,
        "expected compile-output truthfulness to certify eight property descriptors for the canonical sample set",
    )
    expect(
        registration_manifest.get("compile_output_truthfulness_ivar_descriptor_count") == 4,
        "expected compile-output truthfulness to certify four ivar descriptors for the canonical sample set",
    )

    expect(payload.get("init_value", 0) != 0, "expected alloc/init to return a non-zero canonical sample-set receiver")
    expect(payload.get("traced_value") == 13, "expected tracedValue to return 13 for the canonical sample set")
    expect(payload.get("inherited_value") == 7, "expected inheritedValue to return 7 for the canonical sample set")
    expect(payload.get("class_value") == 11, "expected classValue to return 11 for the canonical sample set")
    expect(payload.get("shared_value") == 19, "expected shared to return 19 for the canonical sample set")
    expect(payload.get("count_value") == 37, "expected count to reload 37 for the canonical sample set")
    expect(payload.get("enabled_value") == 1, "expected enabled to reload 1 for the canonical sample set")
    expect(payload.get("current_value") == 55, "expected currentValue to reload 55 for the canonical sample set")
    expect(payload.get("token_value") == 0, "expected tokenValue to remain 0 for the canonical sample set")

    expect(worker_query.get("conforms") == 1, "expected Widget to conform to Worker in the canonical sample set")
    expect(
        worker_query.get("matched_protocol_owner_identity") in {"protocol:Worker", "protocol:Tracer"},
        "expected Worker query to resolve through Worker or inherited Tracer",
    )
    expect(worker_query.get("matched_attachment_owner_identity") is None, "did not expect Worker query to require an attachment-owner match")
    expect(tracer_query.get("conforms") == 1, "expected Widget to conform to Tracer in the canonical sample set")
    expect(tracer_query.get("matched_protocol_owner_identity") == "protocol:Tracer", "expected Tracer query to resolve through protocol:Tracer")
    expect(
        tracer_query.get("matched_attachment_owner_identity") == "category:Widget(Tracing)",
        "expected Tracer query to resolve through the attached category owner",
    )

    expect(
        count_property.get("found") == 1
        and count_property.get("slot_index") == 0
        and count_property.get("offset_bytes") == 0
        and count_property.get("size_bytes") == 4
        and count_property.get("getter_owner_identity") == "implementation:Widget::instance_method:count"
        and count_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setCount:",
        "expected count property reflection to preserve slot/layout/accessor facts",
    )
    expect(
        value_property.get("found") == 1
        and value_property.get("slot_index") == 2
        and value_property.get("offset_bytes") == 8
        and value_property.get("size_bytes") == 8
        and value_property.get("getter_owner_identity") == "implementation:Widget::instance_method:currentValue"
        and value_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setCurrentValue:",
        "expected currentValue property reflection to preserve slot/layout/accessor facts",
    )
    expect(
        token_property.get("found") == 1
        and token_property.get("slot_index") == 3
        and token_property.get("offset_bytes") == 16
        and token_property.get("setter_available") == 0
        and token_property.get("getter_owner_identity") == "implementation:Widget::instance_method:tokenValue"
        and token_property.get("setter_owner_identity") is None,
        "expected tokenValue property reflection to preserve readonly slot/layout/accessor facts",
    )
    dispatch_table_reflection_record_lowering_surface = manifest.get(
        "runtime_dispatch_table_reflection_record_lowering_surface", {}
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("contract_id")
        == RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "expected canonical sample set compile manifest to publish dispatch-table/reflection-record lowering surface",
    )
    expect(
        "; executable_realization_records = contract=objc3c.executable.realization.records.v1"
        in ll_text,
        "expected canonical sample set LLVM IR to publish executable realization records",
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("class_aggregate_symbol")
        == "__objc3_sec_class_descriptors",
        "expected canonical sample set lowering surface to preserve the class aggregate root symbol",
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("property_aggregate_symbol")
        == "__objc3_sec_property_descriptors",
        "expected canonical sample set lowering surface to preserve the property aggregate root symbol",
    )

    return CaseResult(
        case_id="canonical-sample-set",
        probe="tests/tooling/runtime/m259_a002_canonical_runnable_sample_set_probe.cpp",
        fixture="tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "widget_base_identity": widget_entry.get("base_identity"),
            "traced_value": payload["traced_value"],
            "inherited_value": payload["inherited_value"],
            "class_value": payload["class_value"],
            "shared_value": payload["shared_value"],
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
        },
    )


def check_realization_lookup_reflection_runtime_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "realization-lookup-reflection-runtime"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m259_a002_canonical_runnable_sample_set.objc3"
    )
    obj_path, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = (
        case_dir / "compile" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )

    probe = ROOT / REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE
    exe_path = case_dir / "m259_d002_realization_lookup_reflection_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(
        run_probe(exe_path), "realization lookup reflection runtime probe"
    )
    aggregate = payload.get("aggregate", {})
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

    expect(payload.get("widget_found") == 1, "expected Widget class lookup to succeed")
    expect(payload.get("traced_value") == 13, "expected tracedValue to return 13")
    expect(payload.get("count_value") == 41, "expected count to reload the written value")
    expect(
        payload.get("count_property_found") == 1,
        "expected count property reflection lookup to succeed",
    )
    expect(payload.get("tracer_conforms") == 1, "expected Widget to conform to Tracer")
    expect(
        aggregate.get("realized_class_count") == 2,
        "expected aggregate realized class count to report the two live instance-class nodes",
    )
    expect(
        aggregate.get("reflectable_property_count") == 4,
        "expected aggregate reflectable property count to report the four live Widget/Base property accessors",
    )
    expect(
        aggregate.get("attached_category_count") == 1,
        "expected aggregate attached category count to report the single live attached category",
    )
    expect(
        aggregate.get("protocol_conformance_edge_count", 0) >= 2,
        "expected aggregate protocol conformance edge count to stay live",
    )
    expect(
        aggregate.get("method_cache_entry_count", 0) >= 4,
        "expected aggregate method cache entry count to reflect the executed dispatches",
    )
    expect(
        aggregate.get("last_class_query_found") == 1
        and aggregate.get("last_queried_class_name") == "Widget"
        and aggregate.get("last_resolved_class_name") == "Widget",
        "expected aggregate state to preserve the last class lookup",
    )
    expect(
        aggregate.get("last_property_query_found") == 1
        and aggregate.get("last_property_query_inherited") == 0
        and aggregate.get("last_queried_property_name") == "count"
        and aggregate.get("last_resolved_property_class_name") == "Widget",
        "expected aggregate state to preserve the last property lookup",
    )
    expect(
        aggregate.get("last_protocol_query_class_found") == 1
        and aggregate.get("last_protocol_query_protocol_found") == 1
        and aggregate.get("last_protocol_query_conforms") == 1
        and aggregate.get("last_queried_protocol_class_name") == "Widget"
        and aggregate.get("last_queried_protocol_name") == "Tracer",
        "expected aggregate state to preserve the last protocol-conformance query",
    )
    expect(
        bool(aggregate.get("last_resolved_class_owner_identity"))
        and bool(aggregate.get("last_resolved_property_owner_identity"))
        and bool(aggregate.get("last_matched_protocol_owner_identity")),
        "expected aggregate state to preserve the last resolved owner identities",
    )
    expect(
        registration_manifest.get("class_descriptor_count") == 4
        and registration_manifest.get("property_descriptor_count") == 8
        and registration_manifest.get("category_descriptor_count") == 2,
        "expected canonical sample-set registration manifests to keep the broader emitted descriptor counts",
    )
    implementation_surface = manifest.get(
        "runtime_realization_lookup_reflection_implementation_surface", {}
    )
    expect(
        implementation_surface.get("contract_id")
        == RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "expected compile manifest to publish the realization lookup/reflection implementation surface",
    )

    return CaseResult(
        case_id="realization-lookup-reflection-runtime",
        probe=REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
        fixture="tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "realized_class_count": aggregate["realized_class_count"],
            "reflectable_property_count": aggregate["reflectable_property_count"],
            "method_cache_entry_count": aggregate["method_cache_entry_count"],
            "last_queried_protocol_name": aggregate["last_queried_protocol_name"],
        },
    )


def check_live_dispatch_fast_path_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "dispatch-fast-path"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m272_d002_live_dispatch_fast_path_positive.objc3"
    )
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "m272_d002_live_dispatch_fast_path_probe.cpp"
    exe_path = case_dir / "m272_d002_live_dispatch_fast_path_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_key_value_output(run_probe(exe_path), "dispatch fast-path probe")
    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))

    expect(payload.get("baseline_status") == 0, "expected baseline method-cache snapshot to succeed")
    expect(payload.get("dynamic_entry_status") == 0, "expected dynamic fast-path entry lookup to succeed")
    expect(payload.get("explicit_entry_status") == 0, "expected explicit fast-path entry lookup to succeed")
    expect(payload.get("fallback_entry_status") == 0, "expected fallback method-cache entry lookup to succeed")
    expect(payload.get("implicit_value") == 3, "expected implicit direct call to remain direct")
    expect(payload.get("explicit_value") == 5, "expected explicit direct call to remain direct")
    expect(payload.get("mixed_first") == 12 and payload.get("mixed_second") == 12,
           "expected mixed dispatch fixture to execute through the live runtime")
    expect(payload.get("fallback_first") == payload.get("fallback_expected") == payload.get("fallback_second"),
           "expected fallback dispatch to stay deterministic across cache miss/hit")
    expect(payload.get("baseline_cache_entry_count") == 4,
           "expected realized dispatch runtime to seed four method-cache entries")
    expect(payload.get("baseline_fast_path_seed_count") == 4,
           "expected realized dispatch runtime to publish seeded fast-path entries")
    expect(payload.get("dynamic_entry_found") == 1 and payload.get("dynamic_entry_resolved") == 1,
           "expected dynamicEscape entry to resolve live")
    expect(payload.get("dynamic_entry_fast_path_seeded") == 1,
           "expected dynamicEscape entry to be seeded for fast-path dispatch")
    expect(payload.get("dynamic_entry_effective_direct_dispatch") == 0,
           "expected dynamicEscape entry to stay runtime-dispatched")
    expect(payload.get("dynamic_entry_fast_path_reason") == "class-final",
           "expected dynamicEscape fast-path reason to remain class-final")
    expect(payload.get("explicit_entry_found") == 1 and payload.get("explicit_entry_resolved") == 1,
           "expected explicitDirect entry to resolve live")
    expect(payload.get("explicit_entry_fast_path_seeded") == 1,
           "expected explicitDirect entry to be seeded for direct dispatch")
    expect(payload.get("explicit_entry_effective_direct_dispatch") == 1,
           "expected explicitDirect entry to preserve direct dispatch semantics")
    expect(payload.get("explicit_entry_fast_path_reason") == "direct",
           "expected explicitDirect fast-path reason to remain direct")
    expect(payload.get("mixed_first_state_last_dispatch_used_cache") == 1,
           "expected first mixed dispatch runtime call to hit the seeded cache")
    expect(payload.get("mixed_first_state_last_dispatch_used_fast_path") == 1,
           "expected first mixed dispatch runtime call to use the seeded fast path")
    expect(payload.get("mixed_first_state_last_dispatch_resolved_live_method") == 1,
           "expected first mixed dispatch runtime call to resolve a live method")
    expect(payload.get("mixed_first_state_last_dispatch_fell_back") == 0,
           "did not expect first mixed dispatch runtime call to fall back")
    expect(payload.get("mixed_first_state_last_selector") == "dynamicEscape",
           "expected first mixed dispatch runtime call to target dynamicEscape")
    expect(payload.get("mixed_first_dispatch_state_status") == 0,
           "expected first mixed dispatch runtime call to publish dispatch state")
    expect(payload.get("mixed_first_dispatch_state_last_dispatch_path") == "cache-hit-fast-path",
           "expected first mixed dispatch runtime call to report the cache-hit fast path")
    expect(payload.get("mixed_first_dispatch_state_last_implementation_kind") == "emitted-method-body",
           "expected first mixed dispatch runtime call to execute an emitted method body")
    expect(payload.get("mixed_first_dispatch_state_last_effective_direct_dispatch") == 0,
           "expected first mixed dispatch runtime call to remain runtime-dispatched")
    expect(payload.get("mixed_first_dispatch_state_last_used_builtin") == 0,
           "expected first mixed dispatch runtime call to avoid builtin dispatch")
    expect(payload.get("mixed_second_state_last_dispatch_used_cache") == 1,
           "expected repeated mixed dispatch runtime call to remain cached")
    expect(payload.get("mixed_second_state_last_dispatch_used_fast_path") == 1,
           "expected repeated mixed dispatch runtime call to remain on the fast path")
    expect(payload.get("mixed_second_state_last_dispatch_fell_back") == 0,
           "did not expect repeated mixed dispatch runtime call to fall back")
    expect(payload.get("mixed_second_dispatch_state_status") == 0,
           "expected repeated mixed dispatch runtime call to publish dispatch state")
    expect(payload.get("mixed_second_dispatch_state_last_dispatch_path") == "cache-hit-fast-path",
           "expected repeated mixed dispatch runtime call to stay on the cache-hit fast path")
    expect(payload.get("mixed_second_dispatch_state_last_implementation_kind") == "emitted-method-body",
           "expected repeated mixed dispatch runtime call to execute an emitted method body")
    expect(payload.get("mixed_second_dispatch_state_last_effective_direct_dispatch") == 0,
           "expected repeated mixed dispatch runtime call to remain runtime-dispatched")
    expect(payload.get("mixed_second_dispatch_state_last_used_builtin") == 0,
           "expected repeated mixed dispatch runtime call to avoid builtin dispatch")
    expect(payload.get("fallback_first_state_last_dispatch_used_cache") == 0,
           "expected first missingDispatch: call to miss the cache")
    expect(payload.get("fallback_first_state_last_dispatch_used_fast_path") == 0,
           "expected first missingDispatch: call to avoid the fast path")
    expect(payload.get("fallback_first_state_last_dispatch_resolved_live_method") == 0,
           "did not expect first missingDispatch: call to resolve live")
    expect(payload.get("fallback_first_state_last_dispatch_fell_back") == 1,
           "expected first missingDispatch: call to fall back")
    expect(payload.get("fallback_first_dispatch_state_status") == 0,
           "expected first missingDispatch: call to publish dispatch state")
    expect(payload.get("fallback_first_dispatch_state_last_dispatch_path") == "slow-path-fallback",
           "expected first missingDispatch: call to report slow-path fallback dispatch")
    expect(payload.get("fallback_first_dispatch_state_last_implementation_kind") == "fallback-formula",
           "expected first missingDispatch: call to execute deterministic fallback formula")
    expect(payload.get("fallback_second_state_last_dispatch_used_cache") == 1,
           "expected repeated missingDispatch: call to hit the fallback cache entry")
    expect(payload.get("fallback_second_state_last_dispatch_used_fast_path") == 0,
           "expected repeated missingDispatch: call to stay off the fast path")
    expect(payload.get("fallback_second_state_last_dispatch_fell_back") == 1,
           "expected repeated missingDispatch: call to remain a fallback dispatch")
    expect(payload.get("fallback_second_dispatch_state_status") == 0,
           "expected repeated missingDispatch: call to publish dispatch state")
    expect(payload.get("fallback_second_dispatch_state_last_dispatch_path") == "cache-hit-fallback",
           "expected repeated missingDispatch: call to report cached fallback dispatch")
    expect(payload.get("fallback_second_dispatch_state_last_implementation_kind") == "fallback-formula",
           "expected repeated missingDispatch: call to execute deterministic fallback formula")
    expect(
        "; method_dispatch_and_selector_thunk_lowering_surface = "
        "contract_id=objc3c.method.dispatch.selector.thunk.lowering.v1"
        in ll_text,
        "expected LLVM IR to publish authoritative method dispatch and selector thunk lowering surface",
    )
    expect("direct_dispatch_call_sites=5" in ll_text,
           "expected mixed dispatch fixture to emit five direct dispatch calls")
    expect("runtime_dispatch_call_sites=1" in ll_text,
           "expected mixed dispatch fixture to emit one live runtime dispatch call")
    expect("selector_pool_gep_sites=1" in ll_text,
           "expected mixed dispatch fixture to materialize one selector thunk gep")
    expect("selector_pool_count=4" in ll_text,
           "expected mixed dispatch fixture to publish four pooled selectors")
    expect("dynamic_opt_out_sites=2" in ll_text,
           "expected mixed dispatch fixture to preserve two objc_dynamic opt-out sites")
    expect("call i32 @objc3_method_PolicyBox_class_implicitDirect()" in ll_text,
           "expected implicit direct calls to lower as exact direct LLVM calls")
    expect("call i32 @objc3_method_PolicyBox_class_explicitDirect()" in ll_text,
           "expected explicit direct calls to lower as exact direct LLVM calls")
    expect("call i32 @objc3_method_PolicyBox_class_callers()" in ll_text,
           "expected runFixture to preserve direct class-method dispatch to callers")
    expect("call i32 @objc3_runtime_dispatch_i32(" in ll_text,
           "expected dynamicEscape lowering to retain the live runtime dispatch call")
    expect("@__objc3_sec_selector_pool" in ll_text,
           "expected mixed dispatch fixture to emit the selector pool section root")
    lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expect(isinstance(lowering_surface, dict),
           "expected compile manifest to publish the live lowering surface")
    expect(lowering_surface.get("runtime_dispatch_symbol_matches_lowering") is True,
           "expected compile manifest lowering surface to keep dispatch symbols aligned")
    expect(lowering_surface.get("message_send_sites") == 6,
           "expected compile manifest lowering surface to publish six message send sites")
    runtime_abi_surface = manifest.get("dispatch_accessor_runtime_abi_surface", {})
    expect(isinstance(runtime_abi_surface, dict),
           "expected compile manifest to publish dispatch/accessor runtime ABI surface")
    expect(runtime_abi_surface.get("contract_id") == "objc3c.runtime.dispatch_accessor.abi.surface.v1",
           "expected dispatch/accessor runtime ABI surface contract id in compile manifest")
    expect(runtime_abi_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
           "expected runtime ABI surface to publish canonical runtime dispatch symbol")
    expect(runtime_abi_surface.get("dispatch_state_snapshot_symbol") == "objc3_runtime_copy_dispatch_state_for_testing",
           "expected runtime ABI surface to publish dispatch state snapshot helper")
    expect(runtime_abi_surface.get("method_cache_state_snapshot_symbol") == "objc3_runtime_copy_method_cache_state_for_testing",
           "expected runtime ABI surface to publish method cache state snapshot helper")
    expect(runtime_abi_surface.get("property_registry_state_snapshot_symbol") == "objc3_runtime_copy_property_registry_state_for_testing",
           "expected runtime ABI surface to publish property registry snapshot helper")
    expect(runtime_abi_surface.get("arc_debug_state_snapshot_symbol") == "objc3_runtime_copy_arc_debug_state_for_testing",
           "expected runtime ABI surface to publish ARC debug snapshot helper")
    expect(runtime_abi_surface.get("bind_current_property_context_symbol") == "objc3_runtime_bind_current_property_context_for_testing",
           "expected runtime ABI surface to publish property context bind helper")
    expect(runtime_abi_surface.get("clear_current_property_context_symbol") == "objc3_runtime_clear_current_property_context_for_testing",
           "expected runtime ABI surface to publish property context clear helper")
    expect(runtime_abi_surface.get("private_testing_surface_only") is True,
           "expected runtime ABI surface to remain on the private testing boundary")
    expect(runtime_abi_surface.get("deterministic") is True,
           "expected runtime ABI surface to report deterministic handoff")
    registration_runtime_abi_surface = registration_manifest.get("dispatch_accessor_runtime_abi_surface", {})
    expect(isinstance(registration_runtime_abi_surface, dict),
           "expected runtime registration manifest to publish dispatch/accessor runtime ABI surface")
    expect(registration_runtime_abi_surface.get("contract_id") == "objc3c.runtime.dispatch_accessor.abi.surface.v1",
           "expected dispatch/accessor runtime ABI surface contract id in runtime registration manifest")
    expect(registration_runtime_abi_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
           "expected runtime registration manifest to publish canonical runtime dispatch symbol")
    expect(registration_runtime_abi_surface.get("dispatch_state_snapshot_symbol") == "objc3_runtime_copy_dispatch_state_for_testing",
           "expected runtime registration manifest to publish dispatch state snapshot helper")
    expect(registration_runtime_abi_surface.get("current_property_read_symbol") == "objc3_runtime_read_current_property_i32",
           "expected runtime registration manifest to publish current-property read helper")
    expect(registration_runtime_abi_surface.get("current_property_exchange_symbol") == "objc3_runtime_exchange_current_property_i32",
           "expected runtime registration manifest to publish current-property exchange helper")
    expect(registration_runtime_abi_surface.get("weak_current_property_load_symbol") == "objc3_runtime_load_weak_current_property_i32",
           "expected runtime registration manifest to publish weak current-property load helper")
    expect(registration_runtime_abi_surface.get("autorelease_symbol") == "objc3_runtime_autorelease_i32",
           "expected runtime registration manifest to publish autorelease helper")
    expect(registration_runtime_abi_surface.get("private_testing_surface_only") is True,
           "expected runtime registration manifest ABI surface to remain private-testing only")
    expect(registration_runtime_abi_surface.get("deterministic") is True,
           "expected runtime registration manifest ABI surface to report deterministic handoff")

    return CaseResult(
        case_id="dispatch-fast-path",
        probe="tests/tooling/runtime/m272_d002_live_dispatch_fast_path_probe.cpp",
        fixture="tests/tooling/fixtures/native/m272_d002_live_dispatch_fast_path_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "llvm_ir": str(ll_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "registration_manifest": str(registration_manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "baseline_cache_entry_count": payload.get("baseline_cache_entry_count"),
            "baseline_fast_path_seed_count": payload.get("baseline_fast_path_seed_count"),
            "mixed_first_dispatch_path": payload.get("mixed_first_dispatch_state_last_dispatch_path"),
            "mixed_first_implementation_kind": payload.get("mixed_first_dispatch_state_last_implementation_kind"),
            "mixed_second_dispatch_path": payload.get("mixed_second_dispatch_state_last_dispatch_path"),
            "mixed_second_implementation_kind": payload.get("mixed_second_dispatch_state_last_implementation_kind"),
            "fallback_first_dispatch_path": payload.get("fallback_first_dispatch_state_last_dispatch_path"),
            "fallback_first_implementation_kind": payload.get("fallback_first_dispatch_state_last_implementation_kind"),
            "fallback_second_dispatch_path": payload.get("fallback_second_dispatch_state_last_dispatch_path"),
            "fallback_second_implementation_kind": payload.get("fallback_second_dispatch_state_last_implementation_kind"),
            "mixed_first_live_dispatch_count": payload.get("mixed_first_state_live_dispatch_count"),
            "mixed_second_live_dispatch_count": payload.get("mixed_second_state_live_dispatch_count"),
            "fallback_first_fallback_dispatch_count": payload.get("fallback_first_state_fallback_dispatch_count"),
            "fallback_second_fallback_dispatch_count": payload.get("fallback_second_state_fallback_dispatch_count"),
        },
    )


def check_property_reflection_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-reflection"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_d003_property_metadata_reflection_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_property_metadata_reflection_probe.cpp"
    exe_path = case_dir / "runtime_property_metadata_reflection_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property reflection probe")

    widget_entry = payload.get("widget_entry", {})
    token_property = payload.get("token_property", {})
    value_property = payload.get("value_property", {})
    count_property = payload.get("count_property", {})
    missing_property = payload.get("missing_property", {})
    missing_class_property = payload.get("missing_class_property", {})
    registry_after_count = payload.get("registry_state_after_count", {})

    expect(widget_entry.get("found") == 1, "expected Widget realized class entry to be present")
    expect(token_property.get("found") == 1, "expected token property to be reflectable")
    expect(token_property.get("setter_available") == 0, "expected readonly token property to have no setter")
    expect(token_property.get("has_runtime_getter") == 1, "expected token property getter to be runtime-backed")
    expect(value_property.get("found") == 1, "expected value property to be reflectable")
    expect(value_property.get("setter_available") == 1, "expected value property to expose a setter")
    expect(value_property.get("has_runtime_getter") == 1 and value_property.get("has_runtime_setter") == 1,
           "expected value property getter/setter to be runtime-backed")
    expect(count_property.get("found") == 1, "expected count property to be reflectable")
    expect(count_property.get("has_runtime_getter") == 1 and count_property.get("has_runtime_setter") == 1,
           "expected count property getter/setter to be runtime-backed")
    expect(registry_after_count.get("slot_backed_property_count", 0) >= 3,
           "expected slot-backed property registry to include the three Widget properties")
    expect(missing_property.get("found") == 0, "expected missing property lookup to fail closed")
    expect(missing_class_property.get("found") == 0, "expected missing class property lookup to fail closed")

    return CaseResult(
        case_id="property-reflection",
        probe="tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
        fixture="tests/tooling/fixtures/native/m257_d003_property_metadata_reflection_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "reflectable_property_count": registry_after_count.get("reflectable_property_count"),
            "slot_backed_property_count": registry_after_count.get("slot_backed_property_count"),
            "value_property_setter_available": value_property.get("setter_available"),
            "count_property_runtime_setter": count_property.get("has_runtime_setter"),
        },
    )


def check_property_execution_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-execution"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_property_ivar_execution_matrix_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "m257_e002_property_ivar_execution_matrix_probe.cpp"
    exe_path = case_dir / "m257_e002_property_ivar_execution_matrix_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property execution probe")

    widget_entry = payload.get("widget_entry", {})
    registry_state = payload.get("registry_state", {})
    count_property = payload.get("count_property", {})
    enabled_property = payload.get("enabled_property", {})
    value_property = payload.get("value_property", {})
    token_property = payload.get("token_property", {})
    count_method = payload.get("count_method", {})
    enabled_method = payload.get("enabled_method", {})
    value_method = payload.get("value_method", {})
    token_method = payload.get("token_method", {})
    set_count_dispatch = payload.get("set_count_dispatch", {})
    count_dispatch = payload.get("count_dispatch", {})
    set_enabled_dispatch = payload.get("set_enabled_dispatch", {})
    enabled_dispatch = payload.get("enabled_dispatch", {})
    set_value_dispatch = payload.get("set_value_dispatch", {})
    value_dispatch = payload.get("value_dispatch", {})
    token_dispatch = payload.get("token_dispatch", {})

    expect(payload.get("widget_instance", 0) != 0, "expected alloc to materialize a Widget instance")
    expect(payload.get("count_value") == 37, "expected synthesized count getter to return the stored value")
    expect(payload.get("enabled_value") == 1, "expected synthesized enabled getter to return the stored value")
    expect(payload.get("value_result") == 55, "expected synthesized strong property getter to return the stored value")
    expect(widget_entry.get("found") == 1, "expected Widget to be realized during property execution")
    expect(widget_entry.get("runtime_property_accessor_count", 0) >= 4,
           "expected Widget to publish runtime-backed synthesized accessors")
    expect(registry_state.get("slot_backed_property_count", 0) >= 4,
           "expected property execution fixture to register four slot-backed properties")
    expect(count_property.get("has_runtime_getter") == 1 and count_property.get("has_runtime_setter") == 1,
           "expected count property to execute through runtime-backed synthesized accessors")
    expect(enabled_property.get("has_runtime_getter") == 1 and enabled_property.get("has_runtime_setter") == 1,
           "expected enabled property to execute through runtime-backed synthesized accessors")
    expect(value_property.get("has_runtime_getter") == 1 and value_property.get("has_runtime_setter") == 1,
           "expected value property to execute through runtime-backed synthesized accessors")
    expect(token_property.get("has_runtime_getter") == 1 and token_property.get("setter_available") == 0,
           "expected readonly token property to expose only the synthesized getter")
    expect(count_property.get("property_name") == "count",
           "expected count property reflection to stay coherent")
    expect(count_property.get("effective_getter_selector") == "count",
           "expected count getter selector reflection to stay coherent")
    expect(count_property.get("effective_setter_selector") == "setCount:",
           "expected count setter selector reflection to stay coherent")
    expect(enabled_property.get("effective_getter_selector") == "enabled",
           "expected enabled getter selector reflection to stay coherent")
    expect(enabled_property.get("effective_setter_selector") == "setEnabled:",
           "expected enabled setter selector reflection to stay coherent")
    expect(value_property.get("effective_getter_selector") == "currentValue",
           "expected value getter selector reflection to stay coherent")
    expect(value_property.get("effective_setter_selector") == "setCurrentValue:",
           "expected value setter selector reflection to stay coherent")
    expect(token_property.get("effective_getter_selector") == "tokenValue",
           "expected token getter selector reflection to stay coherent")
    expect(count_property.get("getter_owner_identity"), "expected count getter owner identity to be published")
    expect(count_property.get("setter_owner_identity"), "expected count setter owner identity to be published")
    expect(enabled_property.get("getter_owner_identity"), "expected enabled getter owner identity to be published")
    expect(enabled_property.get("setter_owner_identity"), "expected enabled setter owner identity to be published")
    expect(value_property.get("getter_owner_identity"), "expected value getter owner identity to be published")
    expect(value_property.get("setter_owner_identity"), "expected value setter owner identity to be published")
    expect(token_property.get("getter_owner_identity"), "expected token getter owner identity to be published")
    expect(token_property.get("setter_owner_identity") is None,
           "did not expect readonly token property to publish a setter owner identity")
    expect(count_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected count property base identity to match the realized Widget class")
    expect(enabled_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected enabled property base identity to match the realized Widget class")
    expect(value_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected value property base identity to match the realized Widget class")
    expect(token_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected token property base identity to match the realized Widget class")
    expect(registry_state.get("last_resolved_class_name") == "Widget",
           "expected property registry to resolve Widget during live accessor execution")
    expect(registry_state.get("last_resolved_owner_identity"),
           "expected property registry to publish the resolved owner identity")
    expect(count_method.get("resolved") == 1 and count_method.get("parameter_count") == 0,
           "expected count getter dispatch to resolve live through the runtime cache")
    expect(enabled_method.get("resolved") == 1 and enabled_method.get("parameter_count") == 0,
           "expected enabled getter dispatch to resolve live through the runtime cache")
    expect(value_method.get("resolved") == 1 and value_method.get("parameter_count") == 0,
           "expected currentValue getter dispatch to resolve live through the runtime cache")
    expect(token_method.get("resolved") == 1 and token_method.get("parameter_count") == 0,
           "expected tokenValue getter dispatch to resolve live through the runtime cache")
    expect(count_method.get("resolved_owner_identity") == count_property.get("getter_owner_identity"),
           "expected count getter cache ownership to match reflected property ownership")
    expect(enabled_method.get("resolved_owner_identity") == enabled_property.get("getter_owner_identity"),
           "expected enabled getter cache ownership to match reflected property ownership")
    expect(value_method.get("resolved_owner_identity") == value_property.get("getter_owner_identity"),
           "expected currentValue getter cache ownership to match reflected property ownership")
    expect(token_method.get("resolved_owner_identity") == token_property.get("getter_owner_identity"),
           "expected tokenValue getter cache ownership to match reflected property ownership")
    expect(set_count_dispatch.get("last_dispatch_path") == "slow-path-live",
           "expected setCount: to execute through live synthesized accessor resolution")
    expect(set_count_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setCount: to execute through the runtime property-setter builtin")
    expect(set_count_dispatch.get("last_property_name") == count_property.get("property_name"),
           "expected setCount: dispatch property name to match reflected property metadata")
    expect(set_count_dispatch.get("last_property_base_identity") == count_property.get("base_identity"),
           "expected setCount: dispatch base identity to match reflected property metadata")
    expect(set_count_dispatch.get("last_property_slot_index") == count_property.get("slot_index"),
           "expected setCount: dispatch slot index to match reflected property metadata")
    expect(set_count_dispatch.get("last_selector") == count_property.get("effective_setter_selector"),
           "expected setCount: dispatch selector to match reflected property metadata")
    expect(set_count_dispatch.get("last_resolved_owner_identity") == count_property.get("setter_owner_identity"),
           "expected setCount: dispatch ownership to match reflected property metadata")
    expect(set_count_dispatch.get("last_used_builtin") == 1 and set_count_dispatch.get("last_effective_direct_dispatch") == 0,
           "expected setCount: to remain builtin-backed and runtime-dispatched")
    expect(set_count_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setCount: dispatch to report one setter parameter")
    expect(count_dispatch.get("last_dispatch_path") == "slow-path-live",
           "expected count getter to execute through live synthesized accessor resolution")
    expect(count_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected count getter to execute through the runtime property-getter builtin")
    expect(count_dispatch.get("last_property_name") == count_property.get("property_name"),
           "expected count getter dispatch property name to match reflected property metadata")
    expect(count_dispatch.get("last_property_base_identity") == count_property.get("base_identity"),
           "expected count getter dispatch base identity to match reflected property metadata")
    expect(count_dispatch.get("last_property_slot_index") == count_property.get("slot_index"),
           "expected count getter dispatch slot index to match reflected property metadata")
    expect(count_dispatch.get("last_selector") == count_property.get("effective_getter_selector"),
           "expected count getter dispatch selector to match reflected property metadata")
    expect(count_dispatch.get("last_resolved_owner_identity") == count_property.get("getter_owner_identity"),
           "expected count getter dispatch ownership to match reflected property metadata")
    expect(count_dispatch.get("last_used_builtin") == 1 and count_dispatch.get("last_effective_direct_dispatch") == 0,
           "expected count getter to remain builtin-backed and runtime-dispatched")
    expect(count_dispatch.get("last_resolved_parameter_count") == 0,
           "expected count getter dispatch to report zero getter parameters")
    expect(set_enabled_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setEnabled: to execute through the runtime property-setter builtin")
    expect(set_enabled_dispatch.get("last_property_name") == enabled_property.get("property_name"),
           "expected setEnabled: dispatch property name to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_property_base_identity") == enabled_property.get("base_identity"),
           "expected setEnabled: dispatch base identity to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_property_slot_index") == enabled_property.get("slot_index"),
           "expected setEnabled: dispatch slot index to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_selector") == enabled_property.get("effective_setter_selector"),
           "expected setEnabled: dispatch selector to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_resolved_owner_identity") == enabled_property.get("setter_owner_identity"),
           "expected setEnabled: dispatch ownership to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_used_builtin") == 1 and set_enabled_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setEnabled: to remain builtin-backed and report one setter parameter")
    expect(enabled_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected enabled getter to execute through the runtime property-getter builtin")
    expect(enabled_dispatch.get("last_property_name") == enabled_property.get("property_name"),
           "expected enabled getter dispatch property name to match reflected property metadata")
    expect(enabled_dispatch.get("last_property_base_identity") == enabled_property.get("base_identity"),
           "expected enabled getter dispatch base identity to match reflected property metadata")
    expect(enabled_dispatch.get("last_property_slot_index") == enabled_property.get("slot_index"),
           "expected enabled getter dispatch slot index to match reflected property metadata")
    expect(enabled_dispatch.get("last_selector") == enabled_property.get("effective_getter_selector"),
           "expected enabled getter dispatch selector to match reflected property metadata")
    expect(enabled_dispatch.get("last_resolved_owner_identity") == enabled_property.get("getter_owner_identity"),
           "expected enabled getter dispatch ownership to match reflected property metadata")
    expect(enabled_dispatch.get("last_used_builtin") == 1 and enabled_dispatch.get("last_resolved_parameter_count") == 0,
           "expected enabled getter to remain builtin-backed and report zero getter parameters")
    expect(set_value_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setCurrentValue: to execute through the runtime property-setter builtin")
    expect(set_value_dispatch.get("last_property_name") == value_property.get("property_name"),
           "expected setCurrentValue: dispatch property name to match reflected property metadata")
    expect(set_value_dispatch.get("last_property_base_identity") == value_property.get("base_identity"),
           "expected setCurrentValue: dispatch base identity to match reflected property metadata")
    expect(set_value_dispatch.get("last_property_slot_index") == value_property.get("slot_index"),
           "expected setCurrentValue: dispatch slot index to match reflected property metadata")
    expect(set_value_dispatch.get("last_selector") == value_property.get("effective_setter_selector"),
           "expected setCurrentValue: dispatch selector to match reflected property metadata")
    expect(set_value_dispatch.get("last_resolved_owner_identity") == value_property.get("setter_owner_identity"),
           "expected setCurrentValue: dispatch ownership to match reflected property metadata")
    expect(set_value_dispatch.get("last_used_builtin") == 1 and set_value_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setCurrentValue: to remain builtin-backed and report one setter parameter")
    expect(value_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected currentValue getter to execute through the runtime property-getter builtin")
    expect(value_dispatch.get("last_property_name") == value_property.get("property_name"),
           "expected currentValue getter dispatch property name to match reflected property metadata")
    expect(value_dispatch.get("last_property_base_identity") == value_property.get("base_identity"),
           "expected currentValue getter dispatch base identity to match reflected property metadata")
    expect(value_dispatch.get("last_property_slot_index") == value_property.get("slot_index"),
           "expected currentValue getter dispatch slot index to match reflected property metadata")
    expect(value_dispatch.get("last_selector") == value_property.get("effective_getter_selector"),
           "expected currentValue getter dispatch selector to match reflected property metadata")
    expect(value_dispatch.get("last_resolved_owner_identity") == value_property.get("getter_owner_identity"),
           "expected currentValue getter dispatch ownership to match reflected property metadata")
    expect(value_dispatch.get("last_used_builtin") == 1 and value_dispatch.get("last_resolved_parameter_count") == 0,
           "expected currentValue getter to remain builtin-backed and report zero getter parameters")
    expect(token_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected tokenValue getter to execute through the runtime property-getter builtin")
    expect(token_dispatch.get("last_property_name") == token_property.get("property_name"),
           "expected tokenValue getter dispatch property name to match reflected property metadata")
    expect(token_dispatch.get("last_property_base_identity") == token_property.get("base_identity"),
           "expected tokenValue getter dispatch base identity to match reflected property metadata")
    expect(token_dispatch.get("last_property_slot_index") == token_property.get("slot_index"),
           "expected tokenValue getter dispatch slot index to match reflected property metadata")
    expect(token_dispatch.get("last_selector") == token_property.get("effective_getter_selector"),
           "expected tokenValue getter dispatch selector to match reflected property metadata")
    expect(token_dispatch.get("last_resolved_owner_identity") == token_property.get("getter_owner_identity"),
           "expected tokenValue getter dispatch ownership to match reflected property metadata")
    expect(token_dispatch.get("last_used_builtin") == 1 and token_dispatch.get("last_resolved_parameter_count") == 0,
           "expected tokenValue getter to remain builtin-backed and report zero getter parameters")
    return CaseResult(
        case_id="property-execution",
        probe="tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp",
        fixture="tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "count_value": payload.get("count_value"),
            "enabled_value": payload.get("enabled_value"),
            "value_result": payload.get("value_result"),
            "runtime_property_accessor_count": widget_entry.get("runtime_property_accessor_count"),
            "slot_backed_property_count": registry_state.get("slot_backed_property_count"),
            "count_dispatch_kind": count_dispatch.get("last_implementation_kind"),
            "value_dispatch_kind": value_dispatch.get("last_implementation_kind"),
            "token_dispatch_kind": token_dispatch.get("last_implementation_kind"),
        },
    )


def check_arc_property_helper_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "arc-property-helper-abi"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m262_arc_property_interaction_positive.objc3"
    )
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "m262_d003_arc_debug_instrumentation_probe.cpp"
    exe_path = case_dir / "m262_d003_arc_debug_instrumentation_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "arc property helper probe")

    inside = payload.get("inside", {})
    after = payload.get("after", {})

    expect(payload.get("parent", 0) != 0 and payload.get("child", 0) != 0,
           "expected ArcBox runtime helper probe to allocate live receivers")
    expect(payload.get("bind_current_status") == 0,
           "expected strong-property current context binding to succeed")
    expect(payload.get("bind_weak_status") == 0,
           "expected weak-property current context binding to succeed")
    expect(payload.get("rebind_current_status") == 0 and payload.get("rebind_weak_status") == 0,
           "expected current-property helper rebinds to succeed")
    expect(payload.get("getter_value") == payload.get("child"),
           "expected current-property getter helper to read the stored child value")
    expect(payload.get("weak_set_result") == payload.get("child"),
           "expected weak-property helper write to preserve the child value")
    expect(payload.get("weak_inside_pool") == payload.get("child"),
           "expected weak-property helper read inside the pool to preserve the child value")
    expect(payload.get("weak_after_pool") == payload.get("child"),
           "expected weak-property helper read after pool pop to stay coherent")
    expect(payload.get("strong_set_result") == 0,
           "expected first strong-property exchange to replace an empty slot")
    expect(payload.get("clear_strong_result") == payload.get("child"),
           "expected clearing the strong property to return the previous child value")
    expect(inside.get("current_property_read_count", 0) >= 2,
           "expected live current-property reads to execute through the runtime helper ABI")
    expect(inside.get("current_property_write_count", 0) >= 1,
           "expected live current-property writes to execute through the runtime helper ABI")
    expect(inside.get("current_property_exchange_count", 0) >= 2,
           "expected strong ownership accessors to execute through exchange helper traffic")
    expect(inside.get("weak_current_property_load_count", 0) >= 1,
           "expected weak-property loads to execute through the runtime helper ABI")
    expect(inside.get("weak_current_property_store_count", 0) >= 1,
           "expected weak-property stores to execute through the runtime helper ABI")
    expect(inside.get("last_property_receiver") == payload.get("parent"),
           "expected helper ABI debug state to preserve the bound receiver")
    expect(inside.get("last_property_name") == "weakValue",
           "expected helper ABI debug state to report the bound weak property")
    expect(inside.get("last_property_owner_identity") == "implementation:ArcBox",
           "expected helper ABI debug state to report the ArcBox owner identity")
    expect(after.get("autoreleasepool_pop_count", 0) >= 1,
           "expected helper ABI probe to pop an autorelease pool")
    expect(after.get("release_call_count", 0) >= 3,
           "expected helper ABI probe to release the child, retained value, and parent")

    return CaseResult(
        case_id="arc-property-helper-abi",
        probe="tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp",
        fixture="tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "parent": payload.get("parent"),
            "child": payload.get("child"),
            "getter_value": payload.get("getter_value"),
            "weak_after_pool": payload.get("weak_after_pool"),
            "inside_current_property_exchange_count": inside.get("current_property_exchange_count"),
            "after_release_call_count": after.get("release_call_count"),
        },
    )


def check_storage_ownership_reflection_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "storage-ownership-reflection"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m260_runtime_backed_storage_ownership_reflection_positive.objc3"
    )
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    probe = ROOT / "tests" / "tooling" / "runtime" / "m260_runtime_backed_storage_ownership_reflection_probe.cpp"
    exe_path = case_dir / "m260_runtime_backed_storage_ownership_reflection_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "storage ownership reflection probe")

    ll_text = ll_path.read_text(encoding="utf-8")
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))
    box_entry = payload.get("box_entry", {})

    expect(box_entry.get("found") == 1, "expected Box to be realized for storage ownership reflection")
    expect(box_entry.get("runtime_property_accessor_count", 0) >= 5,
           "expected Box to publish five runtime-backed storage accessors")
    expect(box_entry.get("runtime_instance_size_bytes", 0) >= 40,
           "expected Box instance layout to reserve five object-backed storage slots")
    expect(registration_manifest.get("property_descriptor_count") == 10,
           "expected storage ownership fixture to publish ten property descriptors")
    expect(registration_manifest.get("ivar_descriptor_count") == 5,
           "expected storage ownership fixture to publish five ivar layout descriptors")
    expect(registration_manifest.get("compile_output_truthfulness_property_descriptor_count") == 10,
           "expected compile-output truthfulness to certify ten property descriptors")
    expect(registration_manifest.get("compile_output_truthfulness_ivar_descriptor_count") == 5,
           "expected compile-output truthfulness to certify five ivar descriptors")
    expect(
        "; runtime_backed_object_ownership_attribute_surface = "
        "contract=objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        in ll_text,
        "expected LLVM IR to publish the runtime-backed object ownership attribute surface",
    )
    expect("property_attribute_profiles=10" in ll_text,
           "expected LLVM IR ownership surface to publish ten property-attribute profiles")
    expect("ownership_lifetime_profiles=10" in ll_text,
           "expected LLVM IR ownership surface to publish ten ownership lifetime profiles")
    expect("ownership_runtime_hook_profiles=6" in ll_text,
           "expected LLVM IR ownership surface to publish six runtime hook profiles")
    expect("accessor_ownership_profiles=10" in ll_text,
           "expected LLVM IR ownership surface to publish ten accessor ownership profiles")

    expected_properties = {
        "current_value_property": {
            "property_name": "currentValue",
            "slot_index": 0,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;strong=1;weak=0;unowned=0;assign=0;attributes=nonatomic,strong",
            "ownership_lifetime_profile": "strong-owned",
            "ownership_runtime_hook_profile": None,
            "accessor_ownership_profile": "getter=currentValue;setter_available=1;setter=setCurrentValue:;ownership_lifetime=strong-owned;runtime_hook=",
            "effective_getter_selector": "currentValue",
            "effective_setter_selector": "setCurrentValue:",
        },
        "copied_value_property": {
            "property_name": "copiedValue",
            "slot_index": 1,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=1;strong=0;weak=0;unowned=0;assign=0;attributes=copy,nonatomic",
            "ownership_lifetime_profile": "strong-owned",
            "ownership_runtime_hook_profile": None,
            "accessor_ownership_profile": "getter=copiedValue;setter_available=1;setter=setCopiedValue:;ownership_lifetime=strong-owned;runtime_hook=",
            "effective_getter_selector": "copiedValue",
            "effective_setter_selector": "setCopiedValue:",
        },
        "weak_value_property": {
            "property_name": "weakValue",
            "slot_index": 2,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;strong=0;weak=1;unowned=0;assign=0;attributes=nonatomic,weak",
            "ownership_lifetime_profile": "weak",
            "ownership_runtime_hook_profile": "objc-weak-side-table",
            "accessor_ownership_profile": "getter=weakValue;setter_available=1;setter=setWeakValue:;ownership_lifetime=weak;runtime_hook=objc-weak-side-table",
            "effective_getter_selector": "weakValue",
            "effective_setter_selector": "setWeakValue:",
        },
        "borrowed_value_property": {
            "property_name": "borrowedValue",
            "slot_index": 3,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;strong=0;weak=0;unowned=0;assign=1;attributes=assign",
            "ownership_lifetime_profile": "unowned-unsafe",
            "ownership_runtime_hook_profile": "objc-unowned-unsafe-direct",
            "accessor_ownership_profile": "getter=borrowedValue;setter_available=1;setter=setBorrowedValue:;ownership_lifetime=unowned-unsafe;runtime_hook=objc-unowned-unsafe-direct",
            "effective_getter_selector": "borrowedValue",
            "effective_setter_selector": "setBorrowedValue:",
        },
        "guarded_value_property": {
            "property_name": "guardedValue",
            "slot_index": 4,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;strong=0;weak=0;unowned=1;assign=0;attributes=unowned",
            "ownership_lifetime_profile": "unowned-safe",
            "ownership_runtime_hook_profile": "objc-unowned-safe-guard",
            "accessor_ownership_profile": "getter=guardedValue;setter_available=1;setter=setGuardedValue:;ownership_lifetime=unowned-safe;runtime_hook=objc-unowned-safe-guard",
            "effective_getter_selector": "guardedValue",
            "effective_setter_selector": "setGuardedValue:",
        },
    }

    for payload_key, expected in expected_properties.items():
        prop = payload.get(payload_key, {})
        expect(prop.get("found") == 1, f"expected {expected['property_name']} to be reflectable")
        expect(prop.get("has_runtime_getter") == 1 and prop.get("has_runtime_setter") == 1,
               f"expected {expected['property_name']} to execute through runtime-backed accessors")
        expect(prop.get("base_identity") == box_entry.get("base_identity"),
               f"expected {expected['property_name']} to share Box base identity")
        expect(prop.get("slot_index") == expected["slot_index"],
               f"expected {expected['property_name']} to keep slot index {expected['slot_index']}")
        expect(prop.get("size_bytes") == 8 and prop.get("alignment_bytes") == 8,
               f"expected {expected['property_name']} to preserve 8-byte object storage layout")
        expect(prop.get("property_name") == expected["property_name"],
               f"expected runtime property name for {expected['property_name']}")
        expect(prop.get("effective_getter_selector") == expected["effective_getter_selector"],
               f"expected getter selector for {expected['property_name']}")
        expect(prop.get("effective_setter_selector") == expected["effective_setter_selector"],
               f"expected setter selector for {expected['property_name']}")
        expect(prop.get("property_attribute_profile") == expected["property_attribute_profile"],
               f"expected property attribute profile for {expected['property_name']}")
        expect(prop.get("ownership_lifetime_profile") == expected["ownership_lifetime_profile"],
               f"expected ownership lifetime profile for {expected['property_name']}")
        expected_runtime_hook = expected["ownership_runtime_hook_profile"]
        if expected_runtime_hook is None:
            expect(prop.get("ownership_runtime_hook_profile") in (None, ""),
                   f"expected no runtime hook profile for {expected['property_name']}")
        else:
            expect(prop.get("ownership_runtime_hook_profile") == expected_runtime_hook,
                   f"expected runtime hook profile for {expected['property_name']}")
        expect(prop.get("accessor_ownership_profile") == expected["accessor_ownership_profile"],
               f"expected accessor ownership profile for {expected['property_name']}")
        expect(prop.get("getter_owner_identity"), f"expected getter owner identity for {expected['property_name']}")
        expect(prop.get("setter_owner_identity"), f"expected setter owner identity for {expected['property_name']}")

    return CaseResult(
        case_id="storage-ownership-reflection",
        probe="tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp",
        fixture="tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "runtime_property_accessor_count": box_entry.get("runtime_property_accessor_count"),
            "runtime_instance_size_bytes": box_entry.get("runtime_instance_size_bytes"),
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
            "ivar_descriptor_count": registration_manifest.get("ivar_descriptor_count"),
            "guarded_runtime_hook_profile": payload.get("guarded_value_property", {}).get("ownership_runtime_hook_profile"),
            "weak_runtime_hook_profile": payload.get("weak_value_property", {}).get("ownership_runtime_hook_profile"),
        },
    )


def check_storage_legality_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "storage-legality-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m260_runtime_backed_storage_ownership_legality_positive.objc3"
    )
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "positive")
    registration_manifest_path = (
        case_dir / "positive" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")

    expect(
        registration_manifest.get("property_descriptor_count") == 10,
        "expected storage legality positive fixture to publish ten property descriptors",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 5,
        "expected storage legality positive fixture to publish five ivar descriptors",
    )
    expect(
        manifest.get("runtime_property_ivar_storage_accessor_source_surface", {}).get(
            "contract_id"
        )
        == RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "expected storage legality positive fixture to publish the property/ivar/storage/accessor source surface",
    )
    expect(
        manifest.get(
            "runtime_property_atomicity_synthesis_reflection_source_surface", {}
        ).get("contract_id")
        == RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
        "expected storage legality positive fixture to publish the property atomicity/synthesis/reflection source surface",
    )
    for needle, label in (
        ("runtime_backed_storage_ownership_legality", "runtime-backed storage ownership legality"),
        ("property_attribute_profiles=10", "ten property-attribute profiles"),
        ("accessor_ownership_profiles=10", "ten accessor ownership profiles"),
    ):
        expect(
            needle in ll_text,
            f"expected storage legality positive fixture to publish {label} in LLVM IR",
        )

    atomic_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m257_property_atomic_ownership_negative.objc3",
        case_dir / "negative-atomic-ownership",
        expected_snippets=[
            "atomic ownership-aware property 'value' in interface 'Widget' is unsupported until executable accessor storage semantics land"
        ],
        expected_codes=["O3S206"],
    )
    weak_mismatch_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m260_runtime_backed_storage_ownership_weak_mismatch_negative.objc3",
        case_dir / "negative-weak-mismatch",
        expected_snippets=[
            "property ownership qualifier '__weak' conflicts with @property ownership modifier 'assign'"
        ],
        expected_codes=["O3S206"],
    )
    unowned_mismatch_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m260_runtime_backed_storage_ownership_unowned_mismatch_negative.objc3",
        case_dir / "negative-unowned-mismatch",
        expected_snippets=[
            "property ownership qualifier '__unsafe_unretained' conflicts with @property ownership modifier 'unowned'"
        ],
        expected_codes=["O3S206"],
    )

    return CaseResult(
        case_id="storage-legality-semantics",
        probe="compile-manifest-and-diagnostics",
        fixture="tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_legality_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
            "ivar_descriptor_count": registration_manifest.get("ivar_descriptor_count"),
            "atomic_negative_diagnostic_count": atomic_negative["diagnostic_count"],
            "weak_mismatch_diagnostic_count": weak_mismatch_negative["diagnostic_count"],
            "unowned_mismatch_diagnostic_count": unowned_mismatch_negative["diagnostic_count"],
        },
    )


def check_property_synthesis_storage_binding_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-synthesis-storage-binding-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m257_property_synthesis_default_ivar_binding_no_redeclaration.objc3"
    )
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "positive")
    registration_manifest_path = (
        case_dir / "positive" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")
    lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expect(
        isinstance(lowering_surface, dict),
        "expected property synthesis/storage-binding positive fixture to publish the lowering surface",
    )
    expect(
        lowering_surface.get("property_synthesis_sites") == 2,
        "expected no-redeclaration property synthesis fixture to publish two synthesis sites",
    )
    expect(
        lowering_surface.get("property_synthesis_default_ivar_bindings") == 2,
        "expected no-redeclaration property synthesis fixture to publish two default ivar bindings",
    )
    expect(
        lowering_surface.get("interface_owned_property_synthesis_sites") == 2,
        "expected no-redeclaration property synthesis fixture to publish two interface-owned synthesis sites",
    )
    expect(
        lowering_surface.get("implementation_property_redeclaration_sites") == 0,
        "expected no-redeclaration property synthesis fixture to publish zero implementation redeclaration sites",
    )
    expect(
        lowering_surface.get("ivar_binding_resolved") == 2,
        "expected no-redeclaration property synthesis fixture to publish two resolved ivar bindings",
    )
    expect(
        registration_manifest.get("property_descriptor_count") == 2,
        "expected no-redeclaration property synthesis fixture to publish two property descriptors",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 2,
        "expected no-redeclaration property synthesis fixture to publish two ivar descriptors",
    )
    replay_key = manifest.get("lowering_property_synthesis_ivar_binding", {}).get(
        "replay_key", ""
    )
    for snippet, label in (
        (
            "interface_owned_property_synthesis_sites=2",
            "two interface-owned synthesis sites in the replay key",
        ),
        (
            "implementation_property_redeclaration_sites=0",
            "zero implementation redeclaration sites in the replay key",
        ),
        (
            "define void @objc3_method_Widget_instance_setCurrentValue_(i32 %arg0)",
            "the synthesized setter definition",
        ),
        (
            "call i32 @objc3_runtime_exchange_current_property_i32(i32 %objc3_property_retained)",
            "the runtime-backed setter exchange path",
        ),
    ):
        expect(
            snippet in (replay_key if "sites=" in snippet else ll_text),
            f"expected no-redeclaration property synthesis fixture to publish {label}",
        )

    incompatible_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m257_property_synthesis_default_ivar_binding_incompatible_redeclaration.objc3",
        case_dir / "negative-incompatible-redeclaration",
        expected_snippets=[
            "type mismatch: property synthesis for 'token' in implementation 'Widget' drifted from the interface default ivar binding",
            "type mismatch: incompatible property signature for 'token' in implementation 'Widget'",
        ],
        expected_codes=["O3S206"],
    )

    return CaseResult(
        case_id="property-synthesis-storage-binding-semantics",
        probe="compile-manifest-and-diagnostics",
        fixture="tests/tooling/fixtures/native/m257_property_synthesis_default_ivar_binding_no_redeclaration.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_synthesis_sites": lowering_surface.get("property_synthesis_sites"),
            "interface_owned_property_synthesis_sites": lowering_surface.get(
                "interface_owned_property_synthesis_sites"
            ),
            "implementation_property_redeclaration_sites": lowering_surface.get(
                "implementation_property_redeclaration_sites"
            ),
            "negative_incompatible_redeclaration_diagnostic_count": incompatible_negative[
                "diagnostic_count"
            ],
        },
    )


def check_property_reflection_accessor_compatibility_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "property-reflection-accessor-compatibility-diagnostics"
    accessor_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m280_b004_property_accessor_selector_compatibility_negative.objc3",
        case_dir / "accessor-selector-mismatch",
        expected_snippets=[
            "type mismatch: effective getter selector profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
        ],
        expected_codes=["O3S206"],
    )
    reflection_negative = compile_fixture_expect_failure(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m280_b004_property_reflection_attribute_compatibility_negative.objc3",
        case_dir / "reflection-attribute-mismatch",
        expected_snippets=[
            "type mismatch: reflected property attribute and ownership profile for property 'value' in implementation 'Widget' drifted from the interface declaration",
        ],
        expected_codes=["O3S206"],
    )

    return CaseResult(
        case_id="property-reflection-accessor-compatibility-diagnostics",
        probe="compile-diagnostics",
        fixture="tests/tooling/fixtures/native/m280_b004_property_accessor_selector_compatibility_negative.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "accessor_selector_negative_diagnostic_count": accessor_negative[
                "diagnostic_count"
            ],
            "reflection_attribute_negative_diagnostic_count": reflection_negative[
                "diagnostic_count"
            ],
        },
    )


def check_property_ivar_ordering_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-ivar-ordering-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "m257_property_ivar_source_model_completion_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

    surface = manifest.get("runtime_property_ivar_storage_accessor_source_surface", {})
    expect(
        surface.get("layout_init_order_field")
        == "Objc3PropertyDecl.executable_ivar_init_order_index",
        "expected property/ivar storage source surface to publish the init-order field",
    )
    expect(
        surface.get("layout_destroy_order_field")
        == "Objc3PropertyDecl.executable_ivar_destroy_order_index",
        "expected property/ivar storage source surface to publish the destruction-order field",
    )
    expect(
        surface.get("storage_semantics_model")
        == "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation",
        "expected property/ivar storage source surface to publish the init/destroy ordering model",
    )

    property_records = manifest.get("runtime_metadata_source_records", {}).get(
        "properties", []
    )
    ivar_records = manifest.get("runtime_metadata_source_records", {}).get(
        "ivars", []
    )
    expect(
        isinstance(property_records, list) and property_records,
        "expected property ordering fixture to publish property source records",
    )
    expect(
        isinstance(ivar_records, list) and ivar_records,
        "expected property ordering fixture to publish ivar source records",
    )

    property_index = {
        (record.get("owner_kind"), record.get("owner_name"), record.get("property_name")): record
        for record in property_records
        if isinstance(record, dict)
    }
    ivar_index = {
        (record.get("owner_kind"), record.get("owner_name"), record.get("property_name")): record
        for record in ivar_records
        if isinstance(record, dict)
    }
    expected_property_records = (
        ("class-interface", "Widget", "token", 0, 2),
        ("class-interface", "Widget", "value", 1, 1),
        ("class-interface", "Widget", "count", 2, 0),
        ("class-implementation", "Widget", "token", 0, 2),
        ("class-implementation", "Widget", "value", 1, 1),
        ("class-implementation", "Widget", "count", 2, 0),
    )
    for owner_kind, owner_name, property_name, init_index, destroy_index in expected_property_records:
        record = property_index.get((owner_kind, owner_name, property_name), {})
        expect(
            record.get("executable_ivar_layout_slot_index") == init_index,
            f"expected {owner_kind} {owner_name}.{property_name} to preserve slot index {init_index}",
        )
        expect(
            record.get("executable_ivar_init_order_index") == init_index,
            f"expected {owner_kind} {owner_name}.{property_name} to preserve init order {init_index}",
        )
        expect(
            record.get("executable_ivar_destroy_order_index") == destroy_index,
            f"expected {owner_kind} {owner_name}.{property_name} to preserve destruction order {destroy_index}",
        )

    expected_ivar_records = (
        ("class-interface", "Widget", "token", 0, 2),
        ("class-interface", "Widget", "value", 1, 1),
        ("class-interface", "Widget", "count", 2, 0),
    )
    for owner_kind, owner_name, property_name, init_index, destroy_index in expected_ivar_records:
        record = ivar_index.get((owner_kind, owner_name, property_name), {})
        expect(
            record.get("executable_ivar_init_order_index") == init_index,
            f"expected ivar record for {owner_name}.{property_name} to preserve init order {init_index}",
        )
        expect(
            record.get("executable_ivar_destroy_order_index") == destroy_index,
            f"expected ivar record for {owner_name}.{property_name} to preserve destruction order {destroy_index}",
        )

    return CaseResult(
        case_id="property-ivar-ordering-semantics",
        probe="compile-manifest-source-records",
        fixture="tests/tooling/fixtures/native/m257_property_ivar_source_model_completion_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "property_record_count": len(property_records),
            "ivar_record_count": len(ivar_records),
            "token_destroy_order_index": property_index.get(
                ("class-interface", "Widget", "token"), {}
            ).get("executable_ivar_destroy_order_index"),
            "count_init_order_index": property_index.get(
                ("class-interface", "Widget", "count"), {}
            ).get("executable_ivar_init_order_index"),
        },
    )


def check_synthesized_accessor_runtime_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "synthesized-accessor-runtime"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_synthesized_accessor_property_lowering_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "m257_c003_synthesized_accessor_probe.cpp"
    exe_path = case_dir / "m257_c003_synthesized_accessor_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "synthesized accessor runtime probe")

    registration_state = payload.get("registration_state", {})
    selector_state = payload.get("selector_table_state", {})
    count_entry = payload.get("count_entry", {})
    set_count_entry = payload.get("set_count_entry", {})
    enabled_entry = payload.get("enabled_entry", {})
    set_enabled_entry = payload.get("set_enabled_entry", {})
    value_entry = payload.get("value_entry", {})
    set_value_entry = payload.get("set_value_entry", {})

    expect(payload.get("widget_instance", 0) > 0, "expected synthesized-accessor runtime probe to allocate a positive Widget receiver")
    expect(payload.get("set_count_result") == 0, "expected synthesized-accessor count setter dispatch to return zero")
    expect(payload.get("count_value") == 37, "expected synthesized-accessor count getter to reload 37")
    expect(payload.get("set_enabled_result") == 0, "expected synthesized-accessor enabled setter dispatch to return zero")
    expect(payload.get("enabled_value") == 1, "expected synthesized-accessor enabled getter to reload 1")
    expect(payload.get("set_value_result") == 0, "expected synthesized-accessor value setter dispatch to return zero")
    expect(payload.get("value_result") == 55, "expected synthesized-accessor value getter to reload 55")

    expect(registration_state.get("registered_image_count", 0) >= 1, "expected synthesized-accessor runtime probe to report at least one registered image")
    expect(registration_state.get("registered_descriptor_total", 0) >= 1, "expected synthesized-accessor runtime probe to report a non-zero descriptor total")
    expect(selector_state.get("selector_table_entry_count", 0) >= 6, "expected synthesized-accessor runtime probe to materialize the accessor selector surface")
    expect(selector_state.get("metadata_backed_selector_count", 0) >= 6, "expected synthesized-accessor runtime probe to preserve metadata-backed selectors")

    expected_entries = (
        (count_entry, "count", 0, "implementation:Widget::instance_method:count"),
        (set_count_entry, "setCount:", 1, "implementation:Widget::instance_method:setCount:"),
        (enabled_entry, "enabled", 0, "implementation:Widget::instance_method:enabled"),
        (set_enabled_entry, "setEnabled:", 1, "implementation:Widget::instance_method:setEnabled:"),
        (value_entry, "value", 0, "implementation:Widget::instance_method:value"),
        (set_value_entry, "setValue:", 1, "implementation:Widget::instance_method:setValue:"),
    )
    for entry, selector, parameter_count, owner_identity in expected_entries:
        expect(entry.get("found") == 1 and entry.get("resolved") == 1, f"expected {selector} cache entry to resolve live")
        expect(entry.get("selector") == selector, f"expected {selector} cache entry to preserve selector spelling")
        expect(entry.get("parameter_count") == parameter_count, f"expected {selector} cache entry to preserve parameter count {parameter_count}")
        expect(entry.get("resolved_class_name") == "Widget", f"expected {selector} cache entry to resolve against Widget")
        expect(entry.get("resolved_owner_identity") == owner_identity, f"expected {selector} cache entry to preserve owner identity")

    return CaseResult(
        case_id="synthesized-accessor-runtime",
        probe="tests/tooling/runtime/m257_c003_synthesized_accessor_probe.cpp",
        fixture="tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "widget_instance": payload["widget_instance"],
            "count_value": payload["count_value"],
            "enabled_value": payload["enabled_value"],
            "value_result": payload["value_result"],
            "selector_table_entry_count": selector_state.get("selector_table_entry_count"),
        },
    )


def check_accessor_storage_lowering_metadata_surface_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "accessor-storage-lowering-metadata"

    def load_compile_artifacts(fixture_name: str, output_name: str) -> tuple[dict[str, Any], str]:
        fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / fixture_name
        _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / output_name)
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        return manifest, ll_path.read_text(encoding="utf-8")

    def find_property_entry(entries: list[dict[str, Any]], owner_kind: str, owner_name: str, property_name: str) -> dict[str, Any]:
        for entry in entries:
            if (
                entry.get("owner_kind") == owner_kind
                and entry.get("owner_name") == owner_name
                and entry.get("property_name") == property_name
            ):
                return entry
        raise RuntimeError(
            f"missing property entry for {owner_kind}:{owner_name}:{property_name}"
        )

    def expect_property_lowering(
        manifest: dict[str, Any],
        owner_kind: str,
        owner_name: str,
        property_name: str,
        synthesizes_executable_accessors: bool,
        getter_helper_symbol: str,
        setter_helper_symbol: str,
    ) -> None:
        runtime_metadata_records = manifest.get("runtime_metadata_source_records", {})
        property_records = runtime_metadata_records.get("properties", [])
        expect(
            isinstance(property_records, list),
            "expected runtime metadata source records to publish property entries",
        )
        property_record = find_property_entry(
            property_records, owner_kind, owner_name, property_name
        )
        expect(
            property_record.get("synthesizes_executable_accessors")
            is synthesizes_executable_accessors,
            f"expected runtime metadata property record to preserve synthesized-accessor lowering truth for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_record.get("getter_storage_runtime_helper_symbol")
            == getter_helper_symbol,
            f"expected runtime metadata property record to preserve getter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_record.get("setter_storage_runtime_helper_symbol")
            == setter_helper_symbol,
            f"expected runtime metadata property record to preserve setter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )

        source_graph = manifest.get("objc_executable_metadata_source_graph")
        if not isinstance(source_graph, dict):
            source_graph = manifest.get("executable_metadata_source_graph")
        if not isinstance(source_graph, dict):
            source_graph = (
                manifest.get("frontend", {})
                .get("pipeline", {})
                .get("semantic_surface", {})
                .get("objc_executable_metadata_source_graph", {})
            )
        property_nodes = source_graph.get("property_node_entries", [])
        expect(
            isinstance(property_nodes, list),
            "expected executable metadata source graph to publish property nodes",
        )
        property_node = find_property_entry(
            property_nodes, owner_kind, owner_name, property_name
        )
        expect(
            property_node.get("synthesizes_executable_accessors")
            is synthesizes_executable_accessors,
            f"expected executable metadata property node to preserve synthesized-accessor lowering truth for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_node.get("getter_storage_runtime_helper_symbol")
            == getter_helper_symbol,
            f"expected executable metadata property node to preserve getter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )
        expect(
            property_node.get("setter_storage_runtime_helper_symbol")
            == setter_helper_symbol,
            f"expected executable metadata property node to preserve setter helper lowering for {owner_kind}:{owner_name}:{property_name}",
        )

    synthesized_manifest, synthesized_ll = load_compile_artifacts(
        "m257_synthesized_accessor_property_lowering_positive.objc3",
        "synthesized-accessors",
    )
    synthesized_lowering_surface = synthesized_manifest.get(
        "dispatch_and_synthesized_accessor_lowering_surface", {}
    )
    expect(
        isinstance(synthesized_lowering_surface, dict),
        "expected synthesized accessor lowering metadata fixture to publish the lowering surface",
    )
    expect(
        synthesized_lowering_surface.get("contract_id")
        == "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        "expected synthesized accessor lowering metadata fixture to preserve the lowering surface contract id",
    )
    expect(
        synthesized_lowering_surface.get(
            "runtime_property_ivar_storage_accessor_source_surface_contract_id"
        )
        == RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        "expected lowering surface to point back at the runtime property/ivar storage source surface",
    )
    expect(
        synthesized_lowering_surface.get(
            "dispatch_accessor_runtime_abi_surface_contract_id"
        )
        == "objc3c.runtime.dispatch_accessor.abi.surface.v1",
        "expected lowering surface to point at the dispatch/accessor runtime ABI surface",
    )
    expect(
        synthesized_lowering_surface.get("accessor_storage_lowering_metadata_model")
        == "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path",
        "expected lowering surface to publish the accessor-storage metadata model",
    )
    expect(
        synthesized_lowering_surface.get(
            "accessor_storage_lowering_helper_selection_model"
        )
        == "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers",
        "expected lowering surface to publish the helper-selection model",
    )
    expect(
        synthesized_lowering_surface.get("synthesized_accessor_owner_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered property owners",
    )
    expect(
        synthesized_lowering_surface.get("synthesized_getter_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered getters",
    )
    expect(
        synthesized_lowering_surface.get("synthesized_setter_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three lowered setters",
    )
    expect(
        synthesized_lowering_surface.get("current_property_read_entries") == 3,
        "expected synthesized accessor lowering metadata fixture to publish three current-property read entries",
    )
    expect(
        synthesized_lowering_surface.get("current_property_write_entries") == 2,
        "expected synthesized accessor lowering metadata fixture to publish two current-property write entries",
    )
    expect(
        synthesized_lowering_surface.get("current_property_exchange_entries") == 1,
        "expected synthesized accessor lowering metadata fixture to publish one current-property exchange entry",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_load_entries") == 0,
        "expected synthesized accessor lowering metadata fixture to publish zero weak-load entries",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_store_entries") == 0,
        "expected synthesized accessor lowering metadata fixture to publish zero weak-store entries",
    )
    expect(
        synthesized_lowering_surface.get("current_property_read_symbol")
        == "objc3_runtime_read_current_property_i32",
        "expected lowering surface to publish the canonical current-property read symbol",
    )
    expect(
        synthesized_lowering_surface.get("current_property_write_symbol")
        == "objc3_runtime_write_current_property_i32",
        "expected lowering surface to publish the canonical current-property write symbol",
    )
    expect(
        synthesized_lowering_surface.get("current_property_exchange_symbol")
        == "objc3_runtime_exchange_current_property_i32",
        "expected lowering surface to publish the canonical current-property exchange symbol",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_load_symbol")
        == "objc3_runtime_load_weak_current_property_i32",
        "expected lowering surface to publish the canonical weak current-property load symbol",
    )
    expect(
        synthesized_lowering_surface.get("weak_current_property_store_symbol")
        == "objc3_runtime_store_weak_current_property_i32",
        "expected lowering surface to publish the canonical weak current-property store symbol",
    )
    expect(
        "getter_definitions=3" in synthesized_ll
        and "setter_definitions=3" in synthesized_ll
        and "read_current_property_calls=3" in synthesized_ll
        and "write_current_property_calls=2" in synthesized_ll
        and "exchange_current_property_calls=1" in synthesized_ll,
        "expected synthesized accessor lowering metadata fixture LLVM IR to agree with the published lowering counts",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-interface",
        "Widget",
        "count",
        False,
        "",
        "",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-implementation",
        "Widget",
        "count",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_write_current_property_i32",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-implementation",
        "Widget",
        "enabled",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_write_current_property_i32",
    )
    expect_property_lowering(
        synthesized_manifest,
        "class-implementation",
        "Widget",
        "value",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_exchange_current_property_i32",
    )

    arc_manifest, arc_ll = load_compile_artifacts(
        "m262_arc_property_interaction_positive.objc3",
        "arc-accessors",
    )
    arc_lowering_surface = arc_manifest.get(
        "dispatch_and_synthesized_accessor_lowering_surface", {}
    )
    expect(
        arc_lowering_surface.get("synthesized_accessor_owner_entries") == 2,
        "expected ARC property interaction fixture to publish two lowered property owners",
    )
    expect(
        arc_lowering_surface.get("synthesized_getter_entries") == 2
        and arc_lowering_surface.get("synthesized_setter_entries") == 2,
        "expected ARC property interaction fixture to publish two lowered getters and setters",
    )
    expect(
        arc_lowering_surface.get("current_property_read_entries") == 1,
        "expected ARC property interaction fixture to publish one plain/strong getter read entry",
    )
    expect(
        arc_lowering_surface.get("current_property_write_entries") == 0,
        "expected ARC property interaction fixture to publish zero plain write entries",
    )
    expect(
        arc_lowering_surface.get("current_property_exchange_entries") == 1,
        "expected ARC property interaction fixture to publish one strong exchange entry",
    )
    expect(
        arc_lowering_surface.get("weak_current_property_load_entries") == 1,
        "expected ARC property interaction fixture to publish one weak-load entry",
    )
    expect(
        arc_lowering_surface.get("weak_current_property_store_entries") == 1,
        "expected ARC property interaction fixture to publish one weak-store entry",
    )
    expect(
        "exchange_current_property_calls=1" in arc_ll
        and "weak_load_current_property_calls=1" in arc_ll
        and "weak_store_current_property_calls=1" in arc_ll,
        "expected ARC property interaction fixture LLVM IR to agree with the published helper-lowering counts",
    )
    expect_property_lowering(
        arc_manifest,
        "class-interface",
        "ArcBox",
        "currentValue",
        False,
        "",
        "",
    )
    expect_property_lowering(
        arc_manifest,
        "class-implementation",
        "ArcBox",
        "currentValue",
        True,
        "objc3_runtime_read_current_property_i32",
        "objc3_runtime_exchange_current_property_i32",
    )
    expect_property_lowering(
        arc_manifest,
        "class-implementation",
        "ArcBox",
        "weakValue",
        True,
        "objc3_runtime_load_weak_current_property_i32",
        "objc3_runtime_store_weak_current_property_i32",
    )

    return CaseResult(
        case_id="accessor-storage-lowering-metadata-surface",
        probe="compile-manifest-and-executable-metadata-surface",
        fixture="tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "synthesized_accessor_owner_entries": synthesized_lowering_surface.get(
                "synthesized_accessor_owner_entries"
            ),
            "synthesized_getter_entries": synthesized_lowering_surface.get(
                "synthesized_getter_entries"
            ),
            "synthesized_setter_entries": synthesized_lowering_surface.get(
                "synthesized_setter_entries"
            ),
            "strong_exchange_entries": synthesized_lowering_surface.get(
                "current_property_exchange_entries"
            ),
            "weak_store_entries": arc_lowering_surface.get(
                "weak_current_property_store_entries"
            ),
        },
    )


def check_property_layout_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-layout"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_synthesized_accessor_property_lowering_positive.objc3"
    obj_path, ll_path, _ = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "m257_d001_property_layout_runtime_probe.cpp"
    exe_path = case_dir / "m257_d001_property_layout_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property layout runtime probe")

    ll_text = ll_path.read_text(encoding="utf-8")
    registration_state = payload.get("registration_state", {})
    selector_state = payload.get("selector_table_state", {})
    count_entry = payload.get("count_entry", {})
    set_count_entry = payload.get("set_count_entry", {})

    expect(
        "; runtime_property_layout_consumption = "
        "contract=objc3c.runtime.property.layout.consumption.freeze.v1"
        in ll_text,
        "expected LLVM IR to publish the runtime property/layout consumption surface",
    )
    expect("synthesized_accessor_entries=6" in ll_text, "expected property-layout fixture to preserve six synthesized accessors")
    expect("property_descriptor_entries=" in ll_text, "expected property-layout fixture to publish property descriptor inventory")
    expect("ivar_layout_owner_entries=" in ll_text, "expected property-layout fixture to publish ivar layout owner inventory")

    first_alloc = int(payload.get("first_alloc", 0))
    second_alloc = int(payload.get("second_alloc", 0))
    same_instance_mode = first_alloc == 1025 and second_alloc == 1025
    distinct_instance_mode = first_alloc > 0 and second_alloc > 0 and first_alloc != second_alloc

    expect(first_alloc > 0, "expected first alloc to materialize a positive Widget instance identity")
    expect(
        same_instance_mode or distinct_instance_mode,
        "expected property-layout probe to preserve either the historical single-instance mode or the forward-compatible distinct-instance mode",
    )
    expect(payload.get("set_count_result") == 0, "expected count setter dispatch to return zero")
    expect(payload.get("count_value_first") == 37, "expected count getter to observe the written value on the first alloc")
    expect(
        payload.get("count_value_second") == (37 if same_instance_mode else 0),
        "expected second alloc to preserve historical shared storage or observe the forward-compatible distinct-instance default",
    )
    expect(payload.get("set_enabled_result") == 0, "expected enabled setter dispatch to return zero")
    expect(
        payload.get("enabled_value_second") == (1 if same_instance_mode else 0),
        "expected second alloc to preserve historical shared bool storage or observe the forward-compatible distinct-instance default",
    )
    expect(payload.get("set_value_result") == 0, "expected value setter dispatch to return zero")
    expect(
        payload.get("value_result_second") == (55 if same_instance_mode else 0),
        "expected second alloc to preserve historical shared scalar storage or observe the forward-compatible distinct-instance default",
    )

    expect(registration_state.get("registered_image_count", 0) >= 1, "expected property-layout runtime to report at least one registered image")
    expect(registration_state.get("registered_descriptor_total", 0) >= 1, "expected property-layout runtime to report a non-zero descriptor total")
    expect(selector_state.get("selector_table_entry_count", 0) >= 6, "expected property-layout runtime to materialize the synthesized accessor selector surface")

    expect(count_entry.get("found") == 1 and count_entry.get("resolved") == 1, "expected count getter cache entry to resolve")
    expect(count_entry.get("parameter_count") == 0, "expected count getter cache entry to preserve zero parameters")
    expect(
        str(count_entry.get("resolved_owner_identity", "")).endswith("implementation:Widget::instance_method:count"),
        "expected count getter cache entry to preserve the synthesized owner identity",
    )

    expect(set_count_entry.get("found") == 1 and set_count_entry.get("resolved") == 1, "expected setCount setter cache entry to resolve")
    expect(set_count_entry.get("parameter_count") == 1, "expected setCount setter cache entry to preserve one parameter")
    expect(
        str(set_count_entry.get("resolved_owner_identity", "")).endswith("implementation:Widget::instance_method:setCount:"),
        "expected setCount setter cache entry to preserve the synthesized owner identity",
    )

    allocation_mode = "shared-instance-freeze" if same_instance_mode else "distinct-instance-forward-compatible"
    return CaseResult(
        case_id="property-layout",
        probe="tests/tooling/runtime/m257_d001_property_layout_runtime_probe.cpp",
        fixture="tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "allocation_mode": allocation_mode,
            "first_alloc": first_alloc,
            "second_alloc": second_alloc,
            "count_value_first": payload["count_value_first"],
            "count_value_second": payload["count_value_second"],
            "selector_table_entry_count": selector_state.get("selector_table_entry_count"),
        },
    )


def check_synthesized_accessor_codegen_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-codegen"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "m257_synthesized_accessor_property_lowering_positive.objc3"
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))

    required_ir_snippets = {
        "count getter": "define i32 @objc3_method_Widget_instance_count()",
        "count setter": "define void @objc3_method_Widget_instance_setCount_(i32 %arg0)",
        "enabled getter": "define i1 @objc3_method_Widget_instance_enabled()",
        "enabled setter": "define void @objc3_method_Widget_instance_setEnabled_(i1 %arg0)",
        "value getter": "define i32 @objc3_method_Widget_instance_value()",
        "value setter": "define void @objc3_method_Widget_instance_setValue_(i32 %arg0)",
        "getter runtime read": "call i32 @objc3_runtime_read_current_property_i32()",
        "setter runtime write": "call void @objc3_runtime_write_current_property_i32(i32 %arg0)",
        "bool setter coercion": "%objc3_property_value = zext i1 %arg0 to i32",
        "strong getter retain": "%objc3_property_retained = call i32 @objc3_runtime_retain_i32(i32 %objc3_property_slot)",
        "strong getter autorelease": "%objc3_property_autoreleased = call i32 @objc3_runtime_autorelease_i32(i32 %objc3_property_retained)",
        "strong setter exchange": "%objc3_property_previous = call i32 @objc3_runtime_exchange_current_property_i32(i32 %objc3_property_retained)",
        "strong setter release": "%objc3_property_release = call i32 @objc3_runtime_release_i32(i32 %objc3_property_previous)",
        "count descriptor getter binding": "ptr @objc3_method_Widget_instance_count, ptr @objc3_method_Widget_instance_setCount_",
        "enabled descriptor getter binding": "ptr @objc3_method_Widget_instance_enabled, ptr @objc3_method_Widget_instance_setEnabled_",
        "value descriptor getter binding": "ptr @objc3_method_Widget_instance_value, ptr @objc3_method_Widget_instance_setValue_",
    }
    for label, snippet in required_ir_snippets.items():
        expect(snippet in ll_text, f"expected synthesized accessor codegen to emit {label}")

    synthesis_summary = manifest.get("lowering_property_synthesis_ivar_binding", {})
    expect(isinstance(synthesis_summary, dict), "expected property synthesis lowering summary in compile manifest")
    expect(synthesis_summary.get("deterministic_handoff") is True,
           "expected property synthesis lowering summary to report deterministic handoff")
    replay_key = synthesis_summary.get("replay_key", "")
    expect("property_synthesis_sites=3" in replay_key,
           "expected property synthesis replay key to record the three synthesized properties")
    expect("property_synthesis_default_ivar_bindings=3" in replay_key,
           "expected property synthesis replay key to record the default ivar bindings")
    expect(registration_manifest.get("property_descriptor_count", 0) >= 6,
           "expected runtime registration manifest to publish synthesized property descriptors")
    lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expect(isinstance(lowering_surface, dict),
           "expected authoritative dispatch and synthesized-accessor lowering surface in compile manifest")
    expect(lowering_surface.get("contract_id") ==
           "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
           "expected lowering surface contract id for dispatch and synthesized accessors")
    expect(lowering_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
           "expected lowering surface to publish canonical runtime dispatch symbol")
    expect(lowering_surface.get("runtime_dispatch_symbol_matches_lowering") is True,
           "expected lowering surface to bind lowering, shim, and runtime library dispatch symbols together")
    expect(lowering_surface.get("property_synthesis_sites") == 3,
           "expected lowering surface to publish three synthesized properties")
    expect(lowering_surface.get("property_synthesis_default_ivar_bindings") == 3,
           "expected lowering surface to publish three default ivar bindings")
    expect(lowering_surface.get("property_descriptor_count") == registration_manifest.get("property_descriptor_count"),
           "expected lowering surface property descriptor count to match runtime registration manifest")
    expect(lowering_surface.get("ivar_descriptor_count") == registration_manifest.get("ivar_descriptor_count"),
           "expected lowering surface ivar descriptor count to match runtime registration manifest")
    expect(lowering_surface.get("deterministic_handoff") is True,
           "expected lowering surface to report deterministic handoff")
    expect(
        "; dispatch_and_synthesized_accessor_lowering_surface = "
        "contract_id=objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        in ll_text,
        "expected LLVM IR banner to publish dispatch and synthesized-accessor lowering surface",
    )
    expect("property_synthesis_sites=3" in ll_text,
           "expected LLVM IR banner to report three synthesized properties")
    expect("property_descriptor_count=6" in ll_text,
           "expected LLVM IR banner to report synthesized property descriptor count")
    expect("member_table_emission_ready=true" in ll_text,
           "expected LLVM IR banner to report member table emission readiness")
    expect(
        "; synthesized_getter_setter_llvm_ir_generation_surface = "
        "contract_id=objc3c.synthesized.getter.setter.llvm.ir.generation.v1"
        in ll_text,
        "expected LLVM IR to publish synthesized getter/setter generation surface",
    )
    expect("getter_definitions=3" in ll_text,
           "expected synthesized accessor fixture to emit three getter definitions")
    expect("setter_definitions=3" in ll_text,
           "expected synthesized accessor fixture to emit three setter definitions")
    expect("read_current_property_calls=3" in ll_text,
           "expected synthesized accessor fixture to emit three current-property reads")
    expect("write_current_property_calls=2" in ll_text,
           "expected synthesized accessor fixture to emit two current-property writes")
    expect("exchange_current_property_calls=1" in ll_text,
           "expected synthesized accessor fixture to emit one strong current-property exchange")
    expect("retain_calls=2" in ll_text,
           "expected synthesized accessor fixture to emit two retain helper calls")
    expect("release_calls=1" in ll_text,
           "expected synthesized accessor fixture to emit one release helper call")
    expect("autorelease_calls=1" in ll_text,
           "expected synthesized accessor fixture to emit one autorelease helper call")

    return CaseResult(
        case_id="property-codegen",
        probe="real-compile-llvm-inspection",
        fixture="tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "llvm_ir": str(ll_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "registration_manifest": str(registration_manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
        },
    )


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    run_dir = TMP_ROOT / run_id
    report_path = REPORT_ROOT / "summary.json"
    run_dir.mkdir(parents=True, exist_ok=True)
    report_path.parent.mkdir(parents=True, exist_ok=True)

    ensure_native_binaries()
    clangxx = find_clangxx()

    results = [
        check_runtime_library_case(clangxx, run_dir),
        check_installation_lifecycle_case(clangxx, run_dir),
        check_imported_runtime_packaging_replay_case(clangxx, run_dir),
        check_canonical_dispatch_case(clangxx, run_dir),
        check_canonical_sample_set_case(clangxx, run_dir),
        check_realization_lookup_reflection_runtime_case(clangxx, run_dir),
        check_live_dispatch_fast_path_case(clangxx, run_dir),
        check_storage_ownership_reflection_case(clangxx, run_dir),
        check_property_ivar_ordering_semantics_case(run_dir),
        check_accessor_storage_lowering_metadata_surface_case(run_dir),
        check_property_reflection_accessor_compatibility_diagnostics_case(run_dir),
        check_property_synthesis_storage_binding_semantics_case(run_dir),
        check_storage_legality_semantics_case(run_dir),
        check_synthesized_accessor_codegen_case(run_dir),
        check_synthesized_accessor_runtime_case(clangxx, run_dir),
        check_property_layout_case(clangxx, run_dir),
        check_property_execution_case(clangxx, run_dir),
        check_property_reflection_case(clangxx, run_dir),
        check_arc_property_helper_case(clangxx, run_dir),
    ]

    summary = {
        "status": "PASS",
        "run_dir": str(run_dir.relative_to(ROOT)).replace("\\", "/"),
        "clangxx": clangxx,
        "runtime_library": str(RUNTIME_LIB.relative_to(ROOT)).replace("\\", "/"),
        "case_count": len(results),
        "cases": [
            {
                "case_id": result.case_id,
                "probe": result.probe,
                "fixture": result.fixture,
                "claim_class": result.claim_class,
                "passed": result.passed,
                "summary": result.summary,
            }
            for result in results
        ],
        "claim_boundary": build_claim_boundary(),
        "runtime_state_publication_surface": build_runtime_state_publication_surface(),
        "runtime_bootstrap_registration_source_surface": build_runtime_bootstrap_registration_source_surface(),
        "runtime_bootstrap_lowering_registration_artifact_surface": (
            build_runtime_bootstrap_lowering_registration_artifact_surface()
        ),
        "runtime_multi_image_startup_ordering_source_surface": (
            build_runtime_multi_image_startup_ordering_source_surface(results)
        ),
        "runtime_object_model_realization_source_surface": (
            build_runtime_object_model_realization_source_surface(results)
        ),
        "runtime_property_ivar_storage_accessor_source_surface": (
            build_runtime_property_ivar_storage_accessor_source_surface(results)
        ),
        "runtime_property_atomicity_synthesis_reflection_source_surface": (
            build_runtime_property_atomicity_synthesis_reflection_source_surface(results)
        ),
        "runtime_realization_lowering_reflection_artifact_surface": (
            build_runtime_realization_lowering_reflection_artifact_surface(results)
        ),
        "runtime_dispatch_table_reflection_record_lowering_surface": (
            build_runtime_dispatch_table_reflection_record_lowering_surface(results)
        ),
        "runtime_cross_module_realized_metadata_replay_preservation_surface": (
            build_runtime_cross_module_realized_metadata_replay_preservation_surface(results)
        ),
        "runtime_object_model_abi_query_surface": (
            build_runtime_object_model_abi_query_surface(results)
        ),
        "runtime_realization_lookup_reflection_implementation_surface": (
            build_runtime_realization_lookup_reflection_implementation_surface(results)
        ),
        "runtime_reflection_query_surface": build_runtime_reflection_query_surface(results),
        "runtime_realization_lookup_semantics_surface": (
            build_runtime_realization_lookup_semantics_surface(results)
        ),
        "runtime_class_metaclass_protocol_realization_surface": (
            build_runtime_class_metaclass_protocol_realization_surface(results)
        ),
        "runtime_category_attachment_merged_dispatch_surface": (
            build_runtime_category_attachment_merged_dispatch_surface(results)
        ),
        "runtime_reflection_visibility_coherence_diagnostics_surface": (
            build_runtime_reflection_visibility_coherence_diagnostics_surface(results)
        ),
        "acceptance_suite_surface": build_acceptance_suite_surface(results, report_path),
        "runtime_installation_abi_surface": build_runtime_installation_abi_surface(),
        "runtime_loader_lifecycle_surface": build_runtime_loader_lifecycle_surface(results),
        "dispatch_accessor_runtime_abi_surface": {
            "contract_id": "objc3c.runtime.dispatch_accessor.abi.surface.v1",
            "proof_cases": [
                "canonical-sample-set",
                "dispatch-fast-path",
                "synthesized-accessor-runtime",
                "property-layout",
                "property-execution",
                "arc-property-helper-abi",
            ],
            "runtime_dispatch_symbol": "objc3_runtime_dispatch_i32",
            "dispatch_state_snapshot_symbol": "objc3_runtime_copy_dispatch_state_for_testing",
            "method_cache_state_snapshot_symbol": "objc3_runtime_copy_method_cache_state_for_testing",
            "property_registry_state_snapshot_symbol": "objc3_runtime_copy_property_registry_state_for_testing",
            "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
            "arc_debug_state_snapshot_symbol": "objc3_runtime_copy_arc_debug_state_for_testing",
            "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
            "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
            "current_property_exchange_symbol": "objc3_runtime_exchange_current_property_i32",
            "weak_current_property_load_symbol": "objc3_runtime_load_weak_current_property_i32",
            "weak_current_property_store_symbol": "objc3_runtime_store_weak_current_property_i32",
            "retain_symbol": "objc3_runtime_retain_i32",
            "release_symbol": "objc3_runtime_release_i32",
            "autorelease_symbol": "objc3_runtime_autorelease_i32",
            "private_testing_surface_only": True,
            "deterministic": True,
        },
    }
    report_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"runtime-acceptance: PASS ({report_path})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
