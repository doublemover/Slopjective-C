"""Positive fixture compile output contracts for runtime acceptance."""

from __future__ import annotations

import json
import subprocess
from pathlib import Path

from .compile_backends import DIRECT_COMPILE_BACKEND
from .compile_backends import explicit_live_error_runtime_args
from .compile_truth import COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID
from .compile_truth import COMPILE_PROVENANCE_CONTRACT_ID
from .fixture_compile_runner import run_fixture_compile


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
        raise RuntimeError(
            "compiled fixture did not publish the native compile provenance contract"
        )
    if (
        selected_backend == DIRECT_COMPILE_BACKEND
        and provenance.get("compile_backend") != DIRECT_COMPILE_BACKEND
    ):
        raise RuntimeError(
            "direct fixture compile did not publish the direct compile backend"
        )
    truthfulness = provenance.get("compile_output_truthfulness")
    if (
        not isinstance(truthfulness, dict)
        or truthfulness.get("contract_id") != COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID
    ):
        raise RuntimeError(
            "compiled fixture did not publish the compile output truthfulness contract"
        )
    if truthfulness.get("truthful") is not True:
        raise RuntimeError("compiled fixture did not certify truthful compile output")
    if (
        registration_manifest.get("compile_output_provenance_artifact")
        != "module.compile-provenance.json"
    ):
        raise RuntimeError(
            "runtime registration manifest did not bind compile provenance artifact"
        )
    if registration_manifest.get("compile_output_truthful") is not True:
        raise RuntimeError(
            "runtime registration manifest did not certify truthful compile output"
        )
    if registration_manifest.get(
        "compile_output_artifact_set_digest_sha256"
    ) != provenance.get("artifact_set_digest_sha256"):
        raise RuntimeError(
            "runtime registration manifest compile output digest drifted from compile provenance"
        )
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


def compile_live_error_runtime_fixture_outputs(
    fixture: Path, out_dir: Path, extra_args: list[str] | None = None
) -> tuple[Path, Path, Path]:
    return compile_fixture_outputs_with_args(
        fixture,
        out_dir,
        explicit_live_error_runtime_args(extra_args),
    )


__all__ = [
    "compile_fixture",
    "compile_fixture_manifest_only",
    "compile_live_error_runtime_fixture_outputs",
    "compile_fixture_outputs",
    "compile_fixture_outputs_with_args",
    "compile_fixture_with_args",
]
