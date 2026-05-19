from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import run_probe
from objc3c_runtime_acceptance.progress_format import repo_display_path


ROOT = Path(__file__).resolve().parents[3]
RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID = "objc3c.runtime.state.publication.surface.v1"
RUNTIME_STATE_PUBLICATION_SURFACE_KIND = "compile-manifest-plus-registration-manifest"
RUNTIME_BOOTSTRAP_REGISTRATION_SOURCE_SURFACE_CONTRACT_ID = "objc3c.runtime.bootstrap.registration.source.surface.v1"
RUNTIME_BOOTSTRAP_LOWERING_REGISTRATION_ARTIFACT_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.bootstrap.lowering.registration.artifact.surface.v1"
)


def build_runtime_state_publication_surface(public_runtime_abi_boundary: list[str]) -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
        "publication_surface_kind": RUNTIME_STATE_PUBLICATION_SURFACE_KIND,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "public_runtime_abi_boundary": public_runtime_abi_boundary,
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


def check_runtime_probe_helper_support_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "runtime-probe-helper-support"
    helper_probes = [
        ROOT / "tests" / "tooling" / "runtime" / "json_probe_writer_support_test.cpp",
        ROOT / "tests" / "tooling" / "runtime" / "runtime_snapshot_stabilizers_support_test.cpp",
        ROOT / "tests" / "tooling" / "runtime" / "dispatch_expectations_support_test.cpp",
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "runtime_probe_helper_output_equivalence_test.cpp",
    ]
    completed_probes: list[str] = []
    for helper_probe in helper_probes:
        helper_exe = case_dir / f"{helper_probe.stem}.exe"
        compile_probe(clangxx, helper_probe, helper_exe, [])
        run_probe(helper_exe)
        completed_probes.append(repo_display_path(helper_probe))
    return CaseResult(
        case_id="runtime-probe-helper-support",
        probe="tests/tooling/runtime/*_support_test.cpp",
        fixture=None,
        claim_class="runtime-probe-helper-tests",
        passed=True,
        summary={
            "kind": "fast-runtime-probe-helper-tests",
            "helper_probes": completed_probes,
            "bounded_mismatch_diagnostics": True,
            "representative_output_equivalence": [
                "json-field-writer-comma-and-null-output",
                "labeled-method-cache-state-output",
                "labeled-fast-path-method-cache-state-output",
                "labeled-dispatch-state-output",
            ],
        },
    )
