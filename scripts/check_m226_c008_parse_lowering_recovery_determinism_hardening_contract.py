#!/usr/bin/env python3
"""Fail-closed validator for M226-C008 parse/lowering recovery determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-c008-parse-lowering-recovery-determinism-hardening-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "readiness_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parse_lowering_recovery_determinism_hardening_c008_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "readiness_surface_header": (
        ("M226-C008-RDY-01", "inline std::string BuildObjc3ParseRecoveryDeterminismHardeningKey("),
        ("M226-C008-RDY-02", "surface.parse_recovery_determinism_hardening_consistent ="),
        ("M226-C008-RDY-03", "surface.parse_recovery_determinism_hardening_key = BuildObjc3ParseRecoveryDeterminismHardeningKey("),
        ("M226-C008-RDY-04", "const bool parse_recovery_determinism_hardening_ready ="),
        ("M226-C008-RDY-05", "parse_recovery_determinism_hardening_ready;"),
        ("M226-C008-RDY-06", "surface.failure_reason = \"parse recovery/determinism hardening is inconsistent\";"),
    ),
    "frontend_types_header": (
        ("M226-C008-TYP-01", "bool parse_recovery_determinism_hardening_consistent = false;"),
        ("M226-C008-TYP-02", "std::string parse_recovery_determinism_hardening_key;"),
    ),
    "artifacts_source": (
        ("M226-C008-ART-01", '\\"parse_recovery_determinism_hardening_consistent\\": '),
        ("M226-C008-ART-02", '\\"parse_recovery_determinism_hardening_key\\":\\"'),
    ),
    "contract_doc": (
        (
            "M226-C008-DOC-01",
            "Contract ID: `objc3c-parse-lowering-recovery-determinism-hardening-contract/m226-c008-v1`",
        ),
        ("M226-C008-DOC-02", "parse_recovery_determinism_hardening_consistent"),
        ("M226-C008-DOC-03", "parse_recovery_determinism_hardening_key"),
        ("M226-C008-DOC-04", "python scripts/check_m226_c008_parse_lowering_recovery_determinism_hardening_contract.py"),
        (
            "M226-C008-DOC-05",
            "python -m pytest tests/tooling/test_check_m226_c008_parse_lowering_recovery_determinism_hardening_contract.py -q",
        ),
        ("M226-C008-DOC-06", "tmp/reports/m226/m226_c008_parse_lowering_recovery_determinism_hardening_contract_summary.json"),
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
        default=Path("tmp/reports/m226/m226_c008_parse_lowering_recovery_determinism_hardening_contract_summary.json"),
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

    summary = {
        "mode": MODE,
        "ok": not findings,
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

    if findings:
        for finding in findings:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
