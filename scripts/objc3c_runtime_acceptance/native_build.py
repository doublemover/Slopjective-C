"""Native build and fixture compilation helpers for runtime acceptance."""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
from dataclasses import dataclass
from pathlib import Path
from time import perf_counter
from typing import Any

from .artifacts import ArtifactRegistryConfig
from .artifacts import RuntimeAcceptanceArtifactRegistry
from .assertions import expect
from .checksums import file_sha256_hex
from .checksums import optional_file_sha256_hex
from .checksums import replay_key_counter
from .checksums import sha256_text_hex
from .commands import run
from .progress import get_acceptance_progress
from .progress import repo_display_path
from .progress import round_seconds
from objc3c_tooling.probe_compile import find_clangxx
from objc3c_tooling.probe_compile import normal_user_manifest_link_args


ROOT = Path(__file__).resolve().parents[2]
BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
PWSH = shutil.which("pwsh") or shutil.which("powershell") or "pwsh"

COMPILE_PROVENANCE_CONTRACT_ID = "objc3c.native.compile.output.provenance.v1"
COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID = "objc3c.native.compile.output.truthfulness.v1"
DIRECT_COMPILE_BACKEND = "direct-native"
WRAPPER_COMPILE_BACKEND = "powershell-wrapper"
DEFAULT_COMPILE_BACKEND = os.environ.get(
    "OBJC3C_RUNTIME_ACCEPTANCE_COMPILE_BACKEND",
    DIRECT_COMPILE_BACKEND,
).strip().lower() or DIRECT_COMPILE_BACKEND

ACCEPTANCE_ARTIFACT_REGISTRY = RuntimeAcceptanceArtifactRegistry(
    ArtifactRegistryConfig(
        root=ROOT,
        native_exe=NATIVE_EXE,
        runtime_lib=RUNTIME_LIB,
        direct_compile_backend=DIRECT_COMPILE_BACKEND,
        default_compile_backend=DEFAULT_COMPILE_BACKEND,
        repo_display_path=repo_display_path,
        optional_file_sha256_hex=optional_file_sha256_hex,
        sha256_text_hex=sha256_text_hex,
    )
)


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
    lowering = manifest.get("lowering", {})
    property_synthesis = manifest.get("lowering_property_synthesis_ivar_binding", {})
    runtime_dispatch_symbol = ""
    if isinstance(lowering, dict):
        runtime_dispatch_symbol = str(lowering.get("runtime_dispatch_symbol", ""))
    if runtime_dispatch_symbol == "":
        runtime_dispatch_symbol = str(
            manifest.get("runtime_support_library_link_wiring_runtime_dispatch_symbol", "")
        )
    if runtime_dispatch_symbol == "":
        runtime_dispatch_symbol = str(
            manifest.get("runtime_link_host_link_runtime_dispatch_symbol", "")
        )
    if runtime_dispatch_symbol == "":
        raise RuntimeError(
            "compile output truthfulness check could not resolve the runtime dispatch symbol"
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

    escaped_dispatch_symbol = re.escape(runtime_dispatch_symbol)
    dispatch_declaration_count = len(
        re.findall(r"(?m)declare i32 @" + escaped_dispatch_symbol + r"\(", ll_text)
    )
    dispatch_call_count = len(
        re.findall(r"(?m)call i32 @" + escaped_dispatch_symbol + r"\(", ll_text)
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
        dispatch_declaration_count >= 1
        and property_descriptor_section_present
        and ivar_descriptor_section_present
        and property_descriptor_counts_match
        and ivar_descriptor_counts_match
        and synthesized_property_surface_matches
    )
    failures: list[str] = []
    if dispatch_declaration_count < 1:
        failures.append(
            f"missing LLVM declaration for runtime dispatch symbol '{runtime_dispatch_symbol}'"
        )
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
        "runtime_dispatch_symbol": runtime_dispatch_symbol,
        "runtime_dispatch_declaration_count": dispatch_declaration_count,
        "runtime_dispatch_call_count": dispatch_call_count,
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
    driver_script = Path(__file__).resolve()
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


def compile_command(
    fixture: Path,
    out_dir: Path,
    *,
    extra_args: list[str] | None = None,
    backend: str | None = None,
) -> tuple[list[str], str]:
    selected_backend = (backend or DEFAULT_COMPILE_BACKEND).strip().lower()
    if selected_backend in {"wrapper", WRAPPER_COMPILE_BACKEND}:
        return (
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
            ],
            WRAPPER_COMPILE_BACKEND,
        )
    if selected_backend in {"direct", DIRECT_COMPILE_BACKEND}:
        return (
            [
                str(NATIVE_EXE),
                str(fixture),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                *(extra_args or []),
            ],
            DIRECT_COMPILE_BACKEND,
        )
    raise RuntimeError(
        "unsupported OBJC3C_RUNTIME_ACCEPTANCE_COMPILE_BACKEND "
        f"{selected_backend!r}; expected direct-native or powershell-wrapper"
    )


def run_fixture_compile(
    fixture: Path,
    out_dir: Path,
    *,
    extra_args: list[str] | None = None,
    backend: str | None = None,
    write_provenance: bool = True,
    reuse_policy: str = "none",
) -> tuple[subprocess.CompletedProcess[str], str]:
    out_dir.mkdir(parents=True, exist_ok=True)
    progress = get_acceptance_progress()
    command, selected_backend = compile_command(
        fixture,
        out_dir,
        extra_args=extra_args,
        backend=backend,
    )
    if ACCEPTANCE_ARTIFACT_REGISTRY.try_reuse(
        fixture=fixture,
        out_dir=out_dir,
        extra_args=extra_args,
        backend=selected_backend,
        emit_prefix="module",
        reuse_policy=reuse_policy,
        progress=progress,
    ):
        return (
            subprocess.CompletedProcess(
                args=command,
                returncode=0,
                stdout="runtime acceptance artifact registry reused compile outputs\n",
                stderr="",
            ),
            selected_backend,
        )
    result = run(command)
    if (
        write_provenance
        and result.returncode == 0
        and selected_backend == DIRECT_COMPILE_BACKEND
    ):
        write_compile_output_provenance(
            compile_dir=out_dir,
            input_path=fixture,
            compile_backend=selected_backend,
        )
    if write_provenance and result.returncode == 0:
        ACCEPTANCE_ARTIFACT_REGISTRY.register(
            fixture=fixture,
            out_dir=out_dir,
            extra_args=extra_args,
            backend=selected_backend,
            emit_prefix="module",
            reuse_policy=reuse_policy,
            progress=progress,
        )
    return result, selected_backend


def ensure_native_binaries() -> None:
    native_source_entrypoint = ROOT / "native" / "objc3c" / "src" / "main.cpp"
    if not native_source_entrypoint.is_file():
        if NATIVE_EXE.is_file() and RUNTIME_LIB.is_file():
            return
        raise RuntimeError(
            "native source tree is not packaged and required runtime executable/library artifacts are missing"
        )
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


def compile_fixture_with_args(
    fixture: Path,
    out_dir: Path,
    extra_args: list[str] | None = None,
    *,
    reuse_policy: str = "none",
) -> Path:
    result, selected_backend = run_fixture_compile(
        fixture,
        out_dir,
        extra_args=extra_args,
        reuse_policy=reuse_policy,
    )
    if result.returncode != 0:
        raise RuntimeError(
            f"fixture compile failed for {fixture}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )
    obj_path = out_dir / "module.obj"
    if not obj_path.is_file():
        raise RuntimeError(f"fixture compile did not publish {obj_path}")
    provenance_path = out_dir / "module.compile-provenance.json"
    registration_manifest_path = out_dir / "module.runtime-registration-manifest.json"
    if not provenance_path.is_file():
        raise RuntimeError(f"fixture compile did not publish {provenance_path}")
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"fixture compile did not publish {registration_manifest_path}"
        )
    provenance = json.loads(provenance_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    if provenance.get("contract_id") != COMPILE_PROVENANCE_CONTRACT_ID:
        raise RuntimeError("compiled fixture did not publish the native compile provenance contract")
    if (
        selected_backend == DIRECT_COMPILE_BACKEND
        and provenance.get("compile_backend") != DIRECT_COMPILE_BACKEND
    ):
        raise RuntimeError("direct fixture compile did not publish the direct compile backend")
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


def compile_fixture_manifest_only(
    fixture: Path, out_dir: Path, extra_args: list[str] | None = None
) -> tuple[Path, subprocess.CompletedProcess[str]]:
    result, _ = run_fixture_compile(
        fixture,
        out_dir,
        extra_args=extra_args,
        reuse_policy="immutable-inspection",
    )
    manifest_path = out_dir / "module.manifest.json"
    if not manifest_path.is_file():
        raise RuntimeError(
            f"fixture compile did not publish {manifest_path} for {fixture}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )
    return manifest_path, result


def compile_fixture_outputs(fixture: Path, out_dir: Path) -> tuple[Path, Path, Path]:
    obj_path = compile_fixture_with_args(
        fixture,
        out_dir,
        reuse_policy="immutable-inspection",
    )
    ll_path = out_dir / "module.ll"
    manifest_path = out_dir / "module.manifest.json"
    if not ll_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {ll_path}")
    return obj_path, ll_path, manifest_path


def compile_fixture_outputs_with_args(
    fixture: Path, out_dir: Path, extra_args: list[str] | None = None
) -> tuple[Path, Path, Path]:
    obj_path = compile_fixture_with_args(
        fixture,
        out_dir,
        extra_args=extra_args,
        reuse_policy="immutable-inspection",
    )
    ll_path = out_dir / "module.ll"
    manifest_path = out_dir / "module.manifest.json"
    if not ll_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {ll_path}")
    return obj_path, ll_path, manifest_path


@dataclass(frozen=True)
class NegativeDiagnosticExpectation:
    key: str
    fixture: Path
    expected_snippets: list[str]
    expected_codes: list[str]
    extra_args: list[str] | None = None
    allow_missing_structured_diagnostics: bool = False


def compile_fixture_expect_failure(
    fixture: Path,
    out_dir: Path,
    *,
    expected_snippets: list[str],
    expected_codes: list[str],
    extra_args: list[str] | None = None,
    allow_missing_structured_diagnostics: bool = False,
) -> dict[str, Any]:
    result, _ = run_fixture_compile(
        fixture,
        out_dir,
        extra_args=extra_args,
        write_provenance=False,
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
    if diagnostics_text == "" and result.stderr:
        diagnostics_text = result.stderr
    diagnostics_payload = json.loads(diagnostics_json_path.read_text(encoding="utf-8"))
    diagnostics = diagnostics_payload.get("diagnostics", [])
    if allow_missing_structured_diagnostics:
        expect(
            isinstance(diagnostics, list),
            f"failed compile for {fixture} did not publish a diagnostics list",
        )
    else:
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
        "stderr": result.stderr,
        "diagnostics_path": repo_display_path(diagnostics_json_path),
    }


def compile_negative_diagnostic_batch(
    *,
    case_id: str,
    out_dir: Path,
    expectations: list[NegativeDiagnosticExpectation],
) -> dict[str, Any]:
    out_dir.mkdir(parents=True, exist_ok=True)
    started_at = perf_counter()
    results: list[dict[str, Any]] = []
    for expectation in expectations:
        fixture_started_at = perf_counter()
        negative_result = compile_fixture_expect_failure(
            expectation.fixture,
            out_dir / expectation.key,
            expected_snippets=expectation.expected_snippets,
            expected_codes=expectation.expected_codes,
            extra_args=expectation.extra_args,
            allow_missing_structured_diagnostics=(
                expectation.allow_missing_structured_diagnostics
            ),
        )
        results.append(
            {
                "key": expectation.key,
                "fixture": repo_display_path(expectation.fixture),
                "expected_codes": list(expectation.expected_codes),
                "diagnostic_codes": negative_result["diagnostic_codes"],
                "diagnostic_count": negative_result["diagnostic_count"],
                "diagnostics": negative_result["diagnostics_path"],
                "returncode": negative_result["returncode"],
                "duration_seconds": round_seconds(perf_counter() - fixture_started_at),
            }
        )
    return {
        "contract_id": "objc3c.runtime.acceptance.negative.diagnostics.batch.v1",
        "case_id": case_id,
        "batch_out_dir": repo_display_path(out_dir),
        "fixture_count": len(results),
        "total_seconds": round_seconds(perf_counter() - started_at),
        "results": results,
        "preserves_per_fixture_expected_diagnostic_codes": True,
        "preserves_per_fixture_expected_diagnostic_snippets": True,
        "fail_closed_on_unexpected_success": True,
        "fail_closed_on_missing_structured_diagnostics": True,
    }


def link_fixture_executable(clangxx: str, obj_path: Path, exe_path: Path) -> None:
    exe_path.parent.mkdir(parents=True, exist_ok=True)
    command = [
        clangxx,
        "-std=c++20",
        "-fms-runtime-lib=dll",
        *normal_user_manifest_link_args(),
        str(obj_path),
        str(RUNTIME_LIB),
        "-o",
        str(exe_path),
    ]
    result = run(command)
    if result.returncode != 0:
        raise RuntimeError(
            f"fixture link failed for {obj_path}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )


__all__ = [
    "ACCEPTANCE_ARTIFACT_REGISTRY",
    "COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID",
    "COMPILE_PROVENANCE_CONTRACT_ID",
    "DEFAULT_COMPILE_BACKEND",
    "DIRECT_COMPILE_BACKEND",
    "NATIVE_EXE",
    "NegativeDiagnosticExpectation",
    "RUNTIME_LIB",
    "WRAPPER_COMPILE_BACKEND",
    "compile_command",
    "compile_fixture",
    "compile_fixture_expect_failure",
    "compile_fixture_manifest_only",
    "compile_fixture_outputs",
    "compile_fixture_outputs_with_args",
    "compile_fixture_with_args",
    "compile_negative_diagnostic_batch",
    "compile_output_truthfulness",
    "ensure_native_binaries",
    "find_clangxx",
    "link_fixture_executable",
    "run_fixture_compile",
    "write_compile_output_provenance",
]
