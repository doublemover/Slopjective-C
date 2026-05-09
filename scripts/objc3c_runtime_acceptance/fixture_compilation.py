"""Fixture compile orchestration for runtime acceptance."""

from __future__ import annotations

import json
import subprocess
from dataclasses import dataclass
from pathlib import Path
from time import perf_counter
from typing import Any

from .expectation_matching import expect
from .compile_backends import DIRECT_COMPILE_BACKEND
from .compile_backends import compile_command
from .compile_truth import COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID
from .compile_truth import COMPILE_PROVENANCE_CONTRACT_ID
from .compile_truth import write_compile_output_provenance
from .progress_format import repo_display_path
from .progress_format import round_seconds
from .progress_state import get_acceptance_progress
from .process_execution import run
from .runtime_artifact_registry import ACCEPTANCE_ARTIFACT_REGISTRY


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


__all__ = [
    "NegativeDiagnosticExpectation",
    "compile_fixture",
    "compile_fixture_expect_failure",
    "compile_fixture_manifest_only",
    "compile_fixture_outputs",
    "compile_fixture_outputs_with_args",
    "compile_fixture_with_args",
    "compile_negative_diagnostic_batch",
    "run_fixture_compile",
]
