#!/usr/bin/env python3
"""Fail-closed validator for M226-A012 cross-lane integration sync."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-cross-lane-integration-sync-contract-a012-v1"

CONTRACT_DOC = ROOT / "docs" / "contracts" / "m226_cross_lane_integration_sync_a012_expectations.md"

LANE_CONTRACTS: tuple[tuple[str, str, Path], ...] = (
    (
        "A011",
        "objc3c-parser-performance-quality-guardrails-contract/m226-a011-v1",
        ROOT / "docs" / "contracts" / "m226_parser_performance_quality_guardrails_expectations.md",
    ),
    (
        "B003",
        "objc3c-parser-sema-core-handoff-contract/m226-b003-v1",
        ROOT / "docs" / "contracts" / "m226_parser_sema_core_handoff_b003_expectations.md",
    ),
    (
        "C003",
        "objc3c-parse-lowering-core-readiness-contract/m226-c003-v1",
        ROOT / "docs" / "contracts" / "m226_parse_lowering_core_readiness_c003_expectations.md",
    ),
    (
        "D003",
        "objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1",
        ROOT / "docs" / "contracts" / "m226_frontend_build_invocation_manifest_guard_d003_expectations.md",
    ),
    (
        "E003",
        "objc3c-lane-e-integration-gate-core-evidence-contract/m226-e003-v1",
        ROOT / "docs" / "contracts" / "m226_lane_e_integration_gate_e003_core_evidence_expectations.md",
    ),
)


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
        default=Path("tmp/reports/m226/M226-A012/cross_lane_integration_sync_summary.json"),
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

    contract_doc_text = load_text(CONTRACT_DOC, artifact="contract_doc")
    total_checks += 1
    if "Contract ID: `objc3c-cross-lane-integration-sync-contract/m226-a012-v1`" in contract_doc_text:
        passed_checks += 1
    else:
        findings.append(Finding("contract_doc", "M226-A012-DOC-01", "missing A012 contract id"))

    for packet_id, contract_id, path in LANE_CONTRACTS:
        total_checks += 1
        if contract_id in contract_doc_text:
            passed_checks += 1
        else:
            findings.append(
                Finding(
                    "contract_doc",
                    f"M226-A012-LINK-{packet_id}",
                    f"missing lane link for {packet_id}: {contract_id}",
                )
            )

        lane_text = load_text(path, artifact=f"lane_contract_{packet_id}")
        total_checks += 1
        if f"Contract ID: `{contract_id}`" in lane_text:
            passed_checks += 1
        else:
            findings.append(
                Finding(
                    f"lane_contract_{packet_id}",
                    f"M226-A012-LANE-{packet_id}",
                    f"missing expected contract id in {path.as_posix()}",
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
