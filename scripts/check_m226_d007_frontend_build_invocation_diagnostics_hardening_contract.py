#!/usr/bin/env python3
"""Fail-closed validator for M226-D007 frontend build/invocation diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-frontend-build-invocation-diagnostics-hardening-contract-d007-v1"

ARTIFACTS: dict[str, Path] = {
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "compile_wrapper": ROOT / "scripts" / "objc3c_native_compile.ps1",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m226_frontend_build_invocation_diagnostics_hardening_d007_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "build_script": (
        ("M226-D007-BLD-01", "function Write-FrontendDiagnosticsHardeningArtifact"),
        (
            "M226-D007-BLD-02",
            'contract_id = "objc3c-frontend-build-invocation-diagnostics-hardening/m226-d007-v1"',
        ),
        (
            "M226-D007-BLD-03",
            '$expectedEdgeRobustnessContractId = "objc3c-frontend-build-invocation-edge-robustness/m226-d006-v1"',
        ),
        (
            "M226-D007-BLD-04",
            '"objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1"',
        ),
        ("M226-D007-BLD-05", "wrapper_diagnostics = [ordered]@{"),
        ("M226-D007-BLD-06", "required_error_messages = @("),
        ("M226-D007-BLD-07", "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json"),
        ("M226-D007-BLD-08", 'Write-Output ("frontend_diagnostics_hardening="'),
    ),
    "compile_wrapper": (
        ("M226-D007-CMP-01", "function Resolve-FrontendDiagnosticsHardeningPath"),
        ("M226-D007-CMP-02", "function Assert-FrontendDiagnosticsHardening"),
        (
            "M226-D007-CMP-03",
            '$expectedContractId = "objc3c-frontend-build-invocation-diagnostics-hardening/m226-d007-v1"',
        ),
        ("M226-D007-CMP-04", "frontend diagnostics hardening artifact missing at"),
        ("M226-D007-CMP-05", "frontend diagnostics hardening wrapper_diagnostics metadata missing in"),
        ("M226-D007-CMP-06", "frontend diagnostics hardening fail_closed_exit_code must be 2 in"),
        ("M226-D007-CMP-07", "missing value for --out-dir"),
        ("M226-D007-CMP-08", "empty value for --out-dir"),
        ("M226-D007-CMP-09", "missing value for --emit-prefix"),
        ("M226-D007-CMP-10", "empty value for --emit-prefix"),
        ("M226-D007-CMP-11", "missing value for --clang"),
        ("M226-D007-CMP-12", "empty value for --clang"),
        ("M226-D007-CMP-13", "frontend_diagnostics_hardening_relative_path"),
        (
            "M226-D007-CMP-14",
            "Resolve-FrontendDiagnosticsHardeningPath -RepoRoot $repoRoot -BuildResult $buildResult",
        ),
        ("M226-D007-CMP-15", "frontend diagnostics hardening missing required_error_messages entry"),
    ),
    "contract_doc": (
        (
            "M226-D007-DOC-01",
            "Contract ID: `objc3c-frontend-build-invocation-diagnostics-hardening/m226-d007-v1`",
        ),
        (
            "M226-D007-DOC-02",
            "`tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json`",
        ),
        (
            "M226-D007-DOC-03",
            "`python scripts/check_m226_d007_frontend_build_invocation_diagnostics_hardening_contract.py`",
        ),
        (
            "M226-D007-DOC-04",
            "`python -m pytest tests/tooling/test_check_m226_d007_frontend_build_invocation_diagnostics_hardening_contract.py -q`",
        ),
        ("M226-D007-DOC-05", "`npm run build:objc3c-native:full`"),
        (
            "M226-D007-DOC-06",
            "`powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/reports/m226/M226-D007/smoke_out --emit-prefix module`",
        ),
        (
            "M226-D007-DOC-07",
            "`tmp/reports/m226/M226-D007/frontend_build_invocation_diagnostics_hardening_summary.json`",
        ),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m226/M226-D007/frontend_build_invocation_diagnostics_hardening_summary.json"
        ),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

    compile_wrapper_text = load_text(ARTIFACTS["compile_wrapper"], artifact="compile_wrapper")

    total_checks += 1
    if (
        compile_wrapper_text.count(
            "Assert-FrontendDiagnosticsHardening -RepoRoot $repoRoot -BuildResult $buildResult | Out-Null"
        )
        >= 2
    ):
        passed_checks += 1
    else:
        findings.append(
            Finding(
                "compile_wrapper",
                "M226-D007-CMP-16",
                "expected Assert-FrontendDiagnosticsHardening invocation in both no-cache and cache-aware paths",
            )
        )

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

