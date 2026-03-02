#!/usr/bin/env python3
"""Fail-closed validator for M226-D004 frontend build/invocation core feature expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-frontend-build-invocation-core-feature-expansion-contract-d004-v1"

ARTIFACTS: dict[str, Path] = {
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "compile_wrapper": ROOT / "scripts" / "objc3c_native_compile.ps1",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m226_frontend_build_invocation_core_feature_expansion_d004_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "build_script": (
        ("M226-D004-BLD-01", "function Write-FrontendCoreFeatureExpansionArtifact"),
        (
            "M226-D004-BLD-02",
            'contract_id = "objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1"',
        ),
        ("M226-D004-BLD-03", "depends_on_contract_ids = @("),
        ("M226-D004-BLD-04", '"objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1"'),
        ("M226-D004-BLD-05", '"objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1"'),
        ("M226-D004-BLD-06", 'allowed_ir_object_backends = @("clang", "llvm-direct")'),
        ("M226-D004-BLD-07", "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json"),
        ("M226-D004-BLD-08", 'Write-Output ("frontend_core_feature_expansion="'),
    ),
    "compile_wrapper": (
        ("M226-D004-CMP-01", "function Resolve-FrontendCoreFeatureExpansionPath"),
        ("M226-D004-CMP-02", "function Assert-FrontendCoreFeatureExpansion"),
        (
            "M226-D004-CMP-03",
            '$expectedContractId = "objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1"',
        ),
        ("M226-D004-CMP-04", '$defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json"'),
        ("M226-D004-CMP-05", '"objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1"'),
        ("M226-D004-CMP-06", '"objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1"'),
        ("M226-D004-CMP-07", "--objc3-ir-object-backend"),
        ("M226-D004-CMP-08", "--objc3-route-backend-from-capabilities"),
        ("M226-D004-CMP-09", "--llvm-capabilities-summary"),
        ("M226-D004-CMP-10", "frontend_core_feature_expansion_relative_path"),
    ),
    "contract_doc": (
        (
            "M226-D004-DOC-01",
            "Contract ID: `objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1`",
        ),
        ("M226-D004-DOC-02", "`tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json`"),
        (
            "M226-D004-DOC-03",
            "`python scripts/check_m226_d004_frontend_build_invocation_core_feature_expansion_contract.py`",
        ),
        (
            "M226-D004-DOC-04",
            "`python -m pytest tests/tooling/test_check_m226_d004_frontend_build_invocation_core_feature_expansion_contract.py -q`",
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
        default=Path("tmp/reports/m226/M226-D004/frontend_build_invocation_core_feature_expansion_summary.json"),
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
            "Assert-FrontendCoreFeatureExpansion -RepoRoot $repoRoot -BuildResult $buildResult -ParsedArgs $parsed"
        )
        >= 2
    ):
        passed_checks += 1
    else:
        findings.append(
            Finding(
                "compile_wrapper",
                "M226-D004-CMP-11",
                "expected Assert-FrontendCoreFeatureExpansion call in both no-cache and cache-aware compile paths",
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
