#!/usr/bin/env python3
"""Fail-closed validator for M226-E013 lane-E docs/runbook synchronization."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-lane-e-docs-runbook-synchronization-contract-e013-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT / "docs" / "contracts" / "m226_lane_e_docs_runbook_synchronization_e013_expectations.md",
    "core_surface_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_final_readiness_gate_core_feature_implementation_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        ("M226-E013-DOC-01", "Contract ID: `objc3c-lane-e-docs-runbook-synchronization/m226-e013-v1`"),
        ("M226-E013-DOC-02", "`python scripts/check_m226_e013_lane_e_docs_runbook_synchronization_contract.py`"),
        ("M226-E013-DOC-03", "`python -m pytest tests/tooling/test_check_m226_e013_lane_e_docs_runbook_synchronization_contract.py -q`"),
    ),
    "core_surface_header": (
        ("M226-E013-SUR-01", "BuildObjc3FinalReadinessGateDocsRunbookSyncKey("),
        ("M226-E013-SUR-02", "surface.docs_runbook_sync_consistent ="),
        ("M226-E013-SUR-03", "surface.docs_runbook_sync_ready ="),
        ("M226-E013-SUR-04", "surface.docs_runbook_sync_key ="),
    ),
    "frontend_types": (
        ("M226-E013-TYP-01", "bool docs_runbook_sync_consistent = false;"),
        ("M226-E013-TYP-02", "bool docs_runbook_sync_ready = false;"),
        ("M226-E013-TYP-03", "std::string docs_runbook_sync_key;"),
    ),
    "architecture_doc": (
        ("M226-E013-ARC-01", "readiness docs/runbook synchronization guardrails (`docs_runbook_sync_*`)"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M226-E013-FORB-01", "surface.docs_runbook_sync_ready = true;"),
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
        default=Path("tmp/reports/m226/M226-E013/lane_e_docs_runbook_synchronization_summary.json"),
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
        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                passed_checks += 1

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
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


