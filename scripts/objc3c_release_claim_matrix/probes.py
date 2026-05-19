"""Live probe orchestration for release/runtime claim matrix publication."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import subprocess

from .commands import ensure_success, run
from .paths import (
    HELLO_FIXTURE,
    METADATA_FIXTURE,
    NATIVE_EXE,
    PUBLISHED_MATRIX_ARTIFACT_ROOT,
    REPORT_ROOT,
    RUNNER_EXE,
)


@dataclass(frozen=True)
class MatrixProbeArtifacts:
    native_report_path: Path
    native_publication_path: Path
    validation_path: Path
    runner_report_path: Path
    runner_publication_path: Path
    strict_reject: subprocess.CompletedProcess[str]
    yaml_reject: subprocess.CompletedProcess[str]


def ensure_native_build() -> None:
    ensure_summary = REPORT_ROOT / "ensure_build_summary.json"
    build = run(
        [
            "python",
            "scripts/ensure_objc3c_native_build.py",
            "--mode",
            "fast",
            "--reason",
            "release_claims-published-matrix",
            "--summary-out",
            str(ensure_summary),
        ]
    )
    ensure_success(build, "ensure_objc3c_native_build")


def run_matrix_probes() -> MatrixProbeArtifacts:
    native_dir = PUBLISHED_MATRIX_ARTIFACT_ROOT / "native"
    native_dir.mkdir(parents=True, exist_ok=True)
    native = run(
        [
            str(NATIVE_EXE),
            str(HELLO_FIXTURE),
            "--out-dir",
            str(native_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance",
            "--emit-objc3-conformance-format",
            "json",
        ]
    )
    ensure_success(native, "native CLI conformance emit")

    native_report_path = native_dir / "module.objc3-conformance-report.json"
    native_publication_path = native_dir / "module.objc3-conformance-publication.json"
    if not native_report_path.exists() or not native_publication_path.exists():
        raise SystemExit("native CLI did not publish the expected conformance report/publication sidecars")

    validate_dir = PUBLISHED_MATRIX_ARTIFACT_ROOT / "validate"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validate = run(
        [
            str(NATIVE_EXE),
            "--validate-objc3-conformance",
            str(native_report_path),
            "--out-dir",
            str(validate_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "json",
        ]
    )
    ensure_success(validate, "native CLI conformance validate")
    validation_path = validate_dir / "module.objc3-conformance-validation.json"
    if not validation_path.exists():
        raise SystemExit("native CLI validation artifact missing")

    runner_dir = PUBLISHED_MATRIX_ARTIFACT_ROOT / "frontend-runner"
    runner_dir.mkdir(parents=True, exist_ok=True)
    runner = run(
        [
            str(RUNNER_EXE),
            str(METADATA_FIXTURE),
            "--out-dir",
            str(runner_dir),
            "--emit-prefix",
            "module",
            "--no-emit-ir",
            "--no-emit-object",
        ]
    )
    ensure_success(runner, "frontend C API publication")
    runner_report_path = runner_dir / "module.objc3-conformance-report.json"
    runner_publication_path = runner_dir / "module.objc3-conformance-publication.json"
    if not runner_report_path.exists() or not runner_publication_path.exists():
        raise SystemExit("frontend runner did not publish the expected conformance sidecars")

    strict_dir = PUBLISHED_MATRIX_ARTIFACT_ROOT / "strict-reject"
    strict_dir.mkdir(parents=True, exist_ok=True)
    strict_reject = run(
        [
            str(NATIVE_EXE),
            str(HELLO_FIXTURE),
            "--out-dir",
            str(strict_dir),
            "--emit-prefix",
            "module",
            "--objc3-conformance-profile",
            "strict",
        ]
    )

    yaml_dir = PUBLISHED_MATRIX_ARTIFACT_ROOT / "yaml-reject"
    yaml_dir.mkdir(parents=True, exist_ok=True)
    yaml_reject = run(
        [
            str(NATIVE_EXE),
            str(HELLO_FIXTURE),
            "--out-dir",
            str(yaml_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance",
            "--emit-objc3-conformance-format",
            "yaml",
        ]
    )

    return MatrixProbeArtifacts(
        native_report_path=native_report_path,
        native_publication_path=native_publication_path,
        validation_path=validation_path,
        runner_report_path=runner_report_path,
        runner_publication_path=runner_publication_path,
        strict_reject=strict_reject,
        yaml_reject=yaml_reject,
    )
