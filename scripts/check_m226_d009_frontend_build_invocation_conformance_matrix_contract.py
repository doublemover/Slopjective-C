#!/usr/bin/env python3
"""Fail-closed validator for M226-D009 frontend build/invocation conformance matrix."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-frontend-build-invocation-conformance-matrix-contract-d009-v1"

ARTIFACTS: dict[str, Path] = {
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "compile_wrapper": ROOT / "scripts" / "objc3c_native_compile.ps1",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m226_frontend_build_invocation_conformance_matrix_d009_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "build_script": (
        ("M226-D009-BLD-01", "function Write-FrontendConformanceMatrixArtifact"),
        (
            "M226-D009-BLD-02",
            'contract_id = "objc3c-frontend-build-invocation-conformance-matrix/m226-d009-v1"',
        ),
        (
            "M226-D009-BLD-03",
            '$expectedRecoveryContractId = "objc3c-frontend-build-invocation-recovery-determinism-hardening/m226-d008-v1"',
        ),
        ("M226-D009-BLD-04", '$profileKey = "{0}|{1}|manual|{2}" -f $cacheMode, $backendMode, $summaryMode'),
        ("M226-D009-BLD-05", 'case_id = ("D009-C{0:D3}" -f $caseOrdinal)'),
        ("M226-D009-BLD-06", 'case_id = "D009-R001"'),
        ("M226-D009-BLD-07", "acceptance_profile_count = $acceptRows.Count"),
        ("M226-D009-BLD-08", "rejection_profile_count = $rejectRows.Count"),
        ("M226-D009-BLD-09", "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json"),
        ("M226-D009-BLD-10", 'Write-Output ("frontend_conformance_matrix="'),
    ),
    "compile_wrapper": (
        ("M226-D009-CMP-01", "function Resolve-FrontendConformanceMatrixPath"),
        ("M226-D009-CMP-02", "function Assert-FrontendConformanceMatrix"),
        (
            "M226-D009-CMP-03",
            '$expectedContractId = "objc3c-frontend-build-invocation-conformance-matrix/m226-d009-v1"',
        ),
        ("M226-D009-CMP-04", "frontend conformance matrix artifact missing at"),
        ("M226-D009-CMP-05", "frontend conformance matrix acceptance_profile_count must be positive in"),
        ("M226-D009-CMP-06", "frontend conformance matrix missing acceptance profile"),
        (
            "M226-D009-CMP-07",
            "frontend conformance matrix has no acceptance row for invocation profile",
        ),
        ("M226-D009-CMP-08", "frontend_conformance_matrix_relative_path"),
        (
            "M226-D009-CMP-09",
            "Resolve-FrontendConformanceMatrixPath -RepoRoot $RepoRoot -BuildResult $BuildResult",
        ),
        ("M226-D009-CMP-10", 'if ($routeEnabled) {'),
        ("M226-D009-CMP-11", '$invocationProfileKey = "{0}|{1}|{2}|{3}" -f $cacheMode, $backendMode, $routingMode, $summaryMode'),
        ("M226-D009-CMP-12", "frontend conformance matrix missing rejection diagnostic"),
    ),
    "contract_doc": (
        (
            "M226-D009-DOC-01",
            "Contract ID: `objc3c-frontend-build-invocation-conformance-matrix/m226-d009-v1`",
        ),
        (
            "M226-D009-DOC-02",
            "`tmp/artifacts/objc3c-native/frontend_conformance_matrix.json`",
        ),
        ("M226-D009-DOC-03", "| `D009-C001` | `no-cache|default|manual|none` | Accept |"),
        (
            "M226-D009-DOC-04",
            "| `D009-R001` | `any|any|capability-route|none` | Reject (`--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary`) |",
        ),
        (
            "M226-D009-DOC-05",
            "`python scripts/check_m226_d009_frontend_build_invocation_conformance_matrix_contract.py`",
        ),
        (
            "M226-D009-DOC-06",
            "`python -m pytest tests/tooling/test_check_m226_d009_frontend_build_invocation_conformance_matrix_contract.py -q`",
        ),
        ("M226-D009-DOC-07", "`npm run build:objc3c-native`"),
        (
            "M226-D009-DOC-08",
            "`tmp/reports/m226/M226-D009/frontend_build_invocation_conformance_matrix_summary.json`",
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
        default=Path("tmp/reports/m226/M226-D009/frontend_build_invocation_conformance_matrix_summary.json"),
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
    if compile_wrapper_text.count("Assert-FrontendConformanceMatrix `") >= 2:
        passed_checks += 1
    else:
        findings.append(
            Finding(
                "compile_wrapper",
                "M226-D009-CMP-13",
                "expected Assert-FrontendConformanceMatrix invocation in both no-cache and cache-aware paths",
            )
        )

    total_checks += 1
    if compile_wrapper_text.count("Resolve-FrontendConformanceMatrixPath") >= 3:
        passed_checks += 1
    else:
        findings.append(
            Finding(
                "compile_wrapper",
                "M226-D009-CMP-14",
                "expected Resolve-FrontendConformanceMatrixPath coverage for function, artifact checks, and cache preflight path",
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
