"""Compile output truthfulness and provenance contracts."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from .checksums import optional_file_sha256_hex
from .compile_backends import DIRECT_COMPILE_BACKEND
from .compile_truth_artifacts import collect_compile_artifact_set
from .compile_truth_dispatch import runtime_dispatch_truth_from_outputs
from .compile_truth_runtime_metadata import runtime_metadata_truth_from_outputs
from .paths import NATIVE_EXE
from .paths import RUNTIME_LIB
from .progress_format import repo_display_path


COMPILE_PROVENANCE_CONTRACT_ID = "objc3c.native.compile.output.provenance.v1"
COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID = "objc3c.native.compile.output.truthfulness.v1"


def compile_output_truthfulness(compile_dir: Path, emit_prefix: str = "module") -> dict[str, Any]:
    manifest_path = compile_dir / f"{emit_prefix}.manifest.json"
    registration_manifest_path = (
        compile_dir / f"{emit_prefix}.runtime-registration-manifest.json"
    )
    ll_path = compile_dir / f"{emit_prefix}.ll"
    for required_path in (manifest_path, registration_manifest_path, ll_path):
        if not required_path.is_file():
            raise RuntimeError(
                f"compile output truthfulness check missing required artifact {required_path}"
            )

    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    ll_text = ll_path.read_text(encoding="utf-8")
    runtime_dispatch_truth = runtime_dispatch_truth_from_outputs(
        manifest=manifest,
        ll_text=ll_text,
    )
    runtime_metadata_truth = runtime_metadata_truth_from_outputs(
        manifest=manifest,
        registration_manifest=registration_manifest,
        ll_text=ll_text,
    )
    truthful = (
        runtime_dispatch_truth.declaration_present
        and runtime_metadata_truth.truthful
    )
    failures = runtime_dispatch_truth.failures() + runtime_metadata_truth.failures()

    return {
        "contract_id": COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID,
        "llvm_ir_artifact": f"{emit_prefix}.ll",
        "manifest_artifact": f"{emit_prefix}.manifest.json",
        "registration_manifest_artifact": (
            f"{emit_prefix}.runtime-registration-manifest.json"
        ),
        "verification_model": (
            "direct-runtime-acceptance-cross-checks-manifest-and-runtime-registration-claims-against-emitted-llvm-ir"
        ),
        **runtime_dispatch_truth.payload_fields(),
        **runtime_metadata_truth.payload_fields(),
        "truthful": truthful,
        "failures": failures,
    }


def write_compile_output_provenance(
    *,
    compile_dir: Path,
    input_path: Path,
    emit_prefix: str = "module",
    compile_backend: str = DIRECT_COMPILE_BACKEND,
) -> Path:
    truthfulness = compile_output_truthfulness(compile_dir, emit_prefix)
    if truthfulness.get("truthful") is not True:
        failures = truthfulness.get("failures", [])
        failure_summary = "; ".join(str(failure) for failure in failures) or "unknown"
        raise RuntimeError(f"compile output truthfulness check failed: {failure_summary}")

    provenance_file_name = f"{emit_prefix}.compile-provenance.json"
    provenance_path = compile_dir / provenance_file_name
    registration_manifest_path = (
        compile_dir / f"{emit_prefix}.runtime-registration-manifest.json"
    )
    artifact_set = collect_compile_artifact_set(
        compile_dir=compile_dir,
        emit_prefix=emit_prefix,
        provenance_file_name=provenance_file_name,
    )
    driver_script = Path(__file__).resolve().with_name("fixture_compile_runner.py")
    payload = {
        "contract_id": COMPILE_PROVENANCE_CONTRACT_ID,
        "provenance_artifact": provenance_file_name,
        "manifest_artifact": f"{emit_prefix}.manifest.json",
        "registration_manifest_artifact": (
            f"{emit_prefix}.runtime-registration-manifest.json"
        ),
        "input_source": repo_display_path(input_path),
        "input_source_sha256": optional_file_sha256_hex(input_path),
        "compiler_binary": repo_display_path(NATIVE_EXE),
        "compiler_binary_sha256": optional_file_sha256_hex(NATIVE_EXE),
        "runtime_support_library": repo_display_path(RUNTIME_LIB),
        "runtime_support_library_sha256": optional_file_sha256_hex(RUNTIME_LIB),
        "compile_wrapper_script": repo_display_path(driver_script),
        "compile_wrapper_script_sha256": optional_file_sha256_hex(driver_script),
        "compile_backend": compile_backend,
        "direct_compile_backend": compile_backend == DIRECT_COMPILE_BACKEND,
        "replay_verification_model": (
            "artifact-set-digest-plus-per-file-sha256-over-real-emitted-compile-outputs"
        ),
        "compile_output_truthfulness": truthfulness,
        **artifact_set.provenance_payload_fields(),
    }
    provenance_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    if registration_manifest_path.is_file():
        registration_manifest = json.loads(
            registration_manifest_path.read_text(encoding="utf-8")
        )
        registration_manifest["compile_output_provenance_contract_id"] = (
            COMPILE_PROVENANCE_CONTRACT_ID
        )
        registration_manifest["compile_output_provenance_artifact"] = provenance_file_name
        registration_manifest["compile_output_truthfulness_contract_id"] = str(
            truthfulness["contract_id"]
        )
        registration_manifest["compile_output_truthful"] = bool(truthfulness["truthful"])
        registration_manifest["compile_output_truthfulness_runtime_dispatch_symbol"] = str(
            truthfulness["runtime_dispatch_symbol"]
        )
        registration_manifest[
            "compile_output_truthfulness_property_descriptor_count"
        ] = int(truthfulness["property_descriptor_definition_count"])
        registration_manifest["compile_output_truthfulness_ivar_descriptor_count"] = int(
            truthfulness["ivar_descriptor_definition_count"]
        )
        registration_manifest.update(
            artifact_set.registration_manifest_fields()
        )
        registration_manifest_path.write_text(
            json.dumps(registration_manifest, indent=2) + "\n",
            encoding="utf-8",
        )
    return provenance_path


__all__ = [
    "COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID",
    "COMPILE_PROVENANCE_CONTRACT_ID",
    "compile_output_truthfulness",
    "write_compile_output_provenance",
]
