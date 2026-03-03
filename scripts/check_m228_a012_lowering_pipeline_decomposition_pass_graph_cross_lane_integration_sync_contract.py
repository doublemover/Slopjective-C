#!/usr/bin/env python3
"""Fail-closed validator for M228-A012 cross-lane integration sync."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-a012-lowering-pipeline-pass-graph-cross-lane-integration-sync-contract-v1"

CONTRACT_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_a012_expectations.md"
)
CONTRACT_ID = "objc3c-lowering-pipeline-pass-graph-cross-lane-integration-sync/m228-a012-v1"

LANE_CONTRACTS: tuple[tuple[str, str, Path], ...] = (
    (
        "A011",
        "objc3c-lowering-pipeline-pass-graph-performance-quality-guardrails/m228-a011-v1",
        ROOT
        / "docs"
        / "contracts"
        / "m228_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_a011_expectations.md",
    ),
    (
        "B007",
        "objc3c-ownership-aware-lowering-behavior-diagnostics-hardening/m228-b007-v1",
        ROOT
        / "docs"
        / "contracts"
        / "m228_ownership_aware_lowering_behavior_diagnostics_hardening_b007_expectations.md",
    ),
    (
        "C005",
        "objc3c-ir-emission-completeness-edge-case-and-compatibility-completion/m228-c005-v1",
        ROOT
        / "docs"
        / "contracts"
        / "m228_ir_emission_completeness_edge_case_and_compatibility_completion_c005_expectations.md",
    ),
    (
        "D006",
        "objc3c-object-emission-link-path-reliability-edge-case-expansion-and-robustness/m228-d006-v1",
        ROOT
        / "docs"
        / "contracts"
        / "m228_object_emission_link_path_reliability_edge_case_expansion_and_robustness_d006_expectations.md",
    ),
    (
        "E006",
        "objc3c-lane-e-replay-proof-performance-closeout-gate-edge-case-expansion-and-robustness-contract/m228-e006-v1",
        ROOT
        / "docs"
        / "contracts"
        / "m228_lane_e_replay_proof_and_performance_closeout_gate_edge_case_expansion_and_robustness_e006_expectations.md",
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
        default=Path(
            "tmp/reports/m228/M228-A012/lowering_pipeline_pass_graph_cross_lane_integration_sync_contract_summary.json"
        ),
    )
    return parser.parse_args(argv)


def maybe_load_text(path: Path) -> str | None:
    if not path.exists() or not path.is_file():
        return None
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    contract_doc_text = maybe_load_text(CONTRACT_DOC)
    total_checks += 1
    if contract_doc_text is None:
        findings.append(
            Finding("contract_doc", "M228-A012-DOC-00", f"missing contract doc: {CONTRACT_DOC.as_posix()}")
        )
        contract_doc_text = ""
    else:
        passed_checks += 1

    total_checks += 1
    if f"Contract ID: `{CONTRACT_ID}`" in contract_doc_text:
        passed_checks += 1
    else:
        findings.append(Finding("contract_doc", "M228-A012-DOC-01", "missing A012 contract id"))

    for packet_id, contract_id, path in LANE_CONTRACTS:
        total_checks += 1
        if contract_id in contract_doc_text:
            passed_checks += 1
        else:
            findings.append(
                Finding(
                    "contract_doc",
                    f"M228-A012-LINK-{packet_id}",
                    f"missing lane link for {packet_id}: {contract_id}",
                )
            )

        lane_text = maybe_load_text(path)
        total_checks += 1
        if lane_text is None:
            findings.append(
                Finding(
                    f"lane_contract_{packet_id}",
                    f"M228-A012-LANE-{packet_id}-00",
                    f"missing lane contract doc: {path.as_posix()}",
                )
            )
            continue

        if f"Contract ID: `{contract_id}`" in lane_text:
            passed_checks += 1
        else:
            findings.append(
                Finding(
                    f"lane_contract_{packet_id}",
                    f"M228-A012-LANE-{packet_id}",
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
