"""Compile output truthfulness and provenance contracts."""

from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any

from .checksums import file_sha256_hex
from .checksums import optional_file_sha256_hex
from .checksums import replay_key_counter
from .checksums import sha256_text_hex
from .compile_backends import DIRECT_COMPILE_BACKEND
from .compile_truth_dispatch import runtime_dispatch_truth_from_outputs
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
    property_synthesis = manifest.get("lowering_property_synthesis_ivar_binding", {})
    runtime_dispatch_truth = runtime_dispatch_truth_from_outputs(
        manifest=manifest,
        ll_text=ll_text,
    )

    property_descriptor_count_expected = int(
        registration_manifest.get("property_descriptor_count", 0)
    )
    ivar_descriptor_count_expected = int(
        registration_manifest.get("ivar_descriptor_count", 0)
    )
    property_synthesis_sites_expected = 0
    if isinstance(property_synthesis, dict):
        property_synthesis_sites_expected = replay_key_counter(
            str(property_synthesis.get("replay_key", "")),
            "property_synthesis_sites",
        )

    property_descriptor_definition_count = len(
        re.findall(r"(?m)^@__objc3_meta_property_[0-9]+ = ", ll_text)
    )
    ivar_descriptor_definition_count = len(
        re.findall(r"(?m)^@__objc3_meta_ivar_[0-9]+ = ", ll_text)
    )
    property_descriptor_section_present = (
        len(re.findall(r"(?m)^@__objc3_sec_property_descriptors = ", ll_text)) >= 1
    )
    ivar_descriptor_section_present = (
        len(re.findall(r"(?m)^@__objc3_sec_ivar_descriptors = ", ll_text)) >= 1
    )
    current_property_helper_call_count = (
        len(re.findall(r"(?m)call i32 @objc3_runtime_read_current_property_i32\(", ll_text))
        + len(
            re.findall(
                r"(?m)call void @objc3_runtime_write_current_property_i32\(",
                ll_text,
            )
        )
        + len(
            re.findall(
                r"(?m)call i32 @objc3_runtime_exchange_current_property_i32\(",
                ll_text,
            )
        )
    )
    synthesized_accessor_definition_count = (
        len(re.findall(r"(?m)^define i32 @objc3_method_.*_instance_.*\(", ll_text))
        + len(re.findall(r"(?m)^define i1 @objc3_method_.*_instance_.*\(", ll_text))
        + len(re.findall(r"(?m)^define void @objc3_method_.*_instance_.*\(", ll_text))
    )

    property_descriptor_counts_match = (
        property_descriptor_definition_count == property_descriptor_count_expected
    )
    ivar_descriptor_counts_match = (
        ivar_descriptor_definition_count == ivar_descriptor_count_expected
    )
    synthesized_property_surface_matches = (
        property_synthesis_sites_expected == 0
        or (
            property_descriptor_count_expected > 0
            and (
                (
                    current_property_helper_call_count > 0
                    and synthesized_accessor_definition_count
                    >= property_synthesis_sites_expected
                )
                or current_property_helper_call_count == 0
            )
        )
    )
    truthful = (
        runtime_dispatch_truth.declaration_present
        and property_descriptor_section_present
        and ivar_descriptor_section_present
        and property_descriptor_counts_match
        and ivar_descriptor_counts_match
        and synthesized_property_surface_matches
    )
    failures = runtime_dispatch_truth.failures()
    if not property_descriptor_section_present:
        failures.append("missing property descriptor aggregate section in emitted LLVM IR")
    if not ivar_descriptor_section_present:
        failures.append("missing ivar descriptor aggregate section in emitted LLVM IR")
    if not property_descriptor_counts_match:
        failures.append(
            "property descriptor count mismatch: "
            f"registration manifest={property_descriptor_count_expected} "
            f"emitted LLVM IR={property_descriptor_definition_count}"
        )
    if not ivar_descriptor_counts_match:
        failures.append(
            "ivar descriptor count mismatch: "
            f"registration manifest={ivar_descriptor_count_expected} "
            f"emitted LLVM IR={ivar_descriptor_definition_count}"
        )
    if not synthesized_property_surface_matches:
        failures.append(
            "synthesized property lowering replay claims do not match emitted "
            "runtime-backed accessor/helper surface"
        )

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
        "property_descriptor_count_expected": property_descriptor_count_expected,
        "property_descriptor_definition_count": property_descriptor_definition_count,
        "property_descriptor_section_present": property_descriptor_section_present,
        "ivar_descriptor_count_expected": ivar_descriptor_count_expected,
        "ivar_descriptor_definition_count": ivar_descriptor_definition_count,
        "ivar_descriptor_section_present": ivar_descriptor_section_present,
        "property_synthesis_sites_expected": property_synthesis_sites_expected,
        "synthesized_accessor_definition_count": synthesized_accessor_definition_count,
        "current_property_helper_call_count": current_property_helper_call_count,
        "property_descriptor_counts_match": property_descriptor_counts_match,
        "ivar_descriptor_counts_match": ivar_descriptor_counts_match,
        "synthesized_property_surface_matches": synthesized_property_surface_matches,
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
    artifact_entries: list[dict[str, Any]] = []
    for artifact in sorted(compile_dir.iterdir(), key=lambda entry: entry.name):
        if not artifact.is_file():
            continue
        if artifact.name in {
            provenance_file_name,
            f"{emit_prefix}.runtime-registration-manifest.json",
        }:
            continue
        if not (
            artifact.name.lower() == emit_prefix.lower()
            or artifact.name.lower().startswith(f"{emit_prefix}.".lower())
            or artifact.name.lower().startswith(f"{emit_prefix}-".lower())
        ):
            continue
        artifact_entries.append(
            {
                "path": artifact.name,
                "byte_count": artifact.stat().st_size,
                "sha256": file_sha256_hex(artifact),
            }
        )
    artifact_digest_lines = [
        f"{entry['path']}|{entry['byte_count']}|{entry['sha256']}"
        for entry in artifact_entries
    ]
    artifact_set_digest = sha256_text_hex("\n".join(artifact_digest_lines))
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
        "artifact_count": len(artifact_entries),
        "artifact_set_digest_sha256": artifact_set_digest,
        "emitted_artifacts": artifact_entries,
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
        registration_manifest["compile_output_artifact_count"] = len(artifact_entries)
        registration_manifest["compile_output_artifact_set_digest_sha256"] = (
            artifact_set_digest
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
