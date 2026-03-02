#!/usr/bin/env python3
"""Fail-closed validator for M226-D008 frontend build/invocation recovery determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-frontend-build-invocation-recovery-determinism-hardening-contract-d008-v1"

ARTIFACTS: dict[str, Path] = {
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "compile_wrapper": ROOT / "scripts" / "objc3c_native_compile.ps1",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m226_frontend_build_invocation_recovery_determinism_hardening_d008_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "build_script": (
        ("M226-D008-BLD-01", "function Write-FrontendRecoveryDeterminismHardeningArtifact"),
        (
            "M226-D008-BLD-02",
            'contract_id = "objc3c-frontend-build-invocation-recovery-determinism-hardening/m226-d008-v1"',
        ),
        (
            "M226-D008-BLD-03",
            '$expectedDiagnosticsContractId = "objc3c-frontend-build-invocation-diagnostics-hardening/m226-d007-v1"',
        ),
        ("M226-D008-BLD-04", '"objc3c-frontend-build-invocation-edge-robustness/m226-d006-v1"'),
        ("M226-D008-BLD-05", 'entry_contract_id = "objc3c-native-cache-entry/m226-d008-v1"'),
        ("M226-D008-BLD-06", "required_entry_files = @("),
        ("M226-D008-BLD-07", "recovery_signals = @("),
        ("M226-D008-BLD-08", "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json"),
        ("M226-D008-BLD-09", 'Write-Output ("frontend_recovery_determinism_hardening="'),
    ),
    "compile_wrapper": (
        ("M226-D008-CMP-01", "function Resolve-FrontendRecoveryDeterminismHardeningPath"),
        ("M226-D008-CMP-02", "function Assert-FrontendRecoveryDeterminismHardening"),
        ("M226-D008-CMP-03", "function Try-RestoreCacheEntry"),
        (
            "M226-D008-CMP-04",
            '$expectedContractId = "objc3c-frontend-build-invocation-recovery-determinism-hardening/m226-d008-v1"',
        ),
        ("M226-D008-CMP-05", "frontend_recovery_determinism_hardening_relative_path"),
        ("M226-D008-CMP-06", "frontend recovery determinism hardening artifact missing at"),
        ("M226-D008-CMP-07", "frontend recovery determinism hardening cache_determinism metadata missing in"),
        ("M226-D008-CMP-08", "frontend recovery determinism hardening entry_contract_id mismatch in"),
        ("M226-D008-CMP-09", '$cacheEntryContractId = "objc3c-native-cache-entry/m226-d008-v1"'),
        ("M226-D008-CMP-10", "metadata.json"),
        ("M226-D008-CMP-11", "cache_recovery=metadata_missing"),
        ("M226-D008-CMP-12", "cache_recovery=metadata_digest_mismatch"),
        ("M226-D008-CMP-13", "cache_recovery=restore_failed"),
        (
            "M226-D008-CMP-14",
            "Resolve-FrontendRecoveryDeterminismHardeningPath -RepoRoot $repoRoot -BuildResult $buildResult",
        ),
    ),
    "contract_doc": (
        (
            "M226-D008-DOC-01",
            "Contract ID: `objc3c-frontend-build-invocation-recovery-determinism-hardening/m226-d008-v1`",
        ),
        (
            "M226-D008-DOC-02",
            "`tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json`",
        ),
        (
            "M226-D008-DOC-03",
            "`python scripts/check_m226_d008_frontend_build_invocation_recovery_determinism_hardening_contract.py`",
        ),
        (
            "M226-D008-DOC-04",
            "`python -m pytest tests/tooling/test_check_m226_d008_frontend_build_invocation_recovery_determinism_hardening_contract.py -q`",
        ),
        ("M226-D008-DOC-05", "`npm run build:objc3c-native`"),
        (
            "M226-D008-DOC-06",
            "`powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/reports/m226/M226-D008/smoke_out --emit-prefix module`",
        ),
        (
            "M226-D008-DOC-07",
            "`tmp/reports/m226/M226-D008/frontend_build_invocation_recovery_determinism_hardening_summary.json`",
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
            "tmp/reports/m226/M226-D008/frontend_build_invocation_recovery_determinism_hardening_summary.json"
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
            "Assert-FrontendRecoveryDeterminismHardening -RepoRoot $repoRoot -BuildResult $buildResult | Out-Null"
        )
        >= 2
    ):
        passed_checks += 1
    else:
        findings.append(
            Finding(
                "compile_wrapper",
                "M226-D008-CMP-15",
                "expected Assert-FrontendRecoveryDeterminismHardening invocation in both no-cache and cache-aware paths",
            )
        )

    total_checks += 1
    if (
        compile_wrapper_text.count(
            "Try-RestoreCacheEntry"
        )
        >= 2
    ):
        passed_checks += 1
    else:
        findings.append(
            Finding(
                "compile_wrapper",
                "M226-D008-CMP-16",
                "expected Try-RestoreCacheEntry definition and use in cache replay path",
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
