#!/usr/bin/env python3
"""Fail-closed validator for M227-A012 semantic-pass cross-lane integration sync."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-a012-semantic-pass-cross-lane-integration-sync-contract-v1"

CONTRACT_DOC = ROOT / "docs" / "contracts" / "m227_semantic_pass_cross_lane_integration_sync_expectations.md"
PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_a012_semantic_pass_cross_lane_integration_sync_packet.md"
)
CONTRACT_ID = "objc3c-semantic-pass-cross-lane-integration-sync/m227-a012-v1"

LANE_CONTRACTS: tuple[tuple[str, str, Path], ...] = (
    (
        "A011",
        "objc3c-semantic-pass-performance-quality-guardrails/m227-a011-v1",
        ROOT / "docs" / "contracts" / "m227_semantic_pass_performance_quality_guardrails_expectations.md",
    ),
    (
        "B007",
        "objc3c-type-system-objc3-forms-diagnostics-hardening/m227-b007-v1",
        ROOT / "docs" / "contracts" / "m227_type_system_objc3_forms_diagnostics_hardening_b007_expectations.md",
    ),
    (
        "C002",
        "objc3c-typed-sema-to-lowering-modular-split-scaffold/m227-c002-v1",
        ROOT / "docs" / "contracts" / "m227_typed_sema_to_lowering_modular_split_c002_expectations.md",
    ),
    (
        "D001",
        "objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-v1",
        ROOT / "docs" / "contracts" / "m227_runtime_facing_type_metadata_semantics_expectations.md",
    ),
    (
        "E001",
        "objc3c-lane-e-semantic-conformance-quality-gate-contract/m227-e001-v1",
        ROOT / "docs" / "contracts" / "m227_lane_e_semantic_conformance_quality_gate_expectations.md",
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
        default=Path("tmp/reports/m227/M227-A012/semantic_pass_cross_lane_integration_sync_summary.json"),
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
        findings.append(Finding("contract_doc", "M227-A012-DOC-00", f"missing contract doc: {CONTRACT_DOC.as_posix()}"))
        contract_doc_text = ""
    else:
        passed_checks += 1

    packet_doc_text = maybe_load_text(PACKET_DOC)
    total_checks += 1
    if packet_doc_text is None:
        findings.append(Finding("packet_doc", "M227-A012-PKT-00", f"missing packet doc: {PACKET_DOC.as_posix()}"))
        packet_doc_text = ""
    else:
        passed_checks += 1

    total_checks += 1
    if f"Contract ID: `{CONTRACT_ID}`" in contract_doc_text:
        passed_checks += 1
    else:
        findings.append(Finding("contract_doc", "M227-A012-DOC-01", "missing A012 contract id"))

    total_checks += 1
    if "Packet: `M227-A012`" in packet_doc_text:
        passed_checks += 1
    else:
        findings.append(Finding("packet_doc", "M227-A012-PKT-01", "missing packet id"))

    for packet_id, contract_id, path in LANE_CONTRACTS:
        total_checks += 1
        if contract_id in contract_doc_text:
            passed_checks += 1
        else:
            findings.append(
                Finding("contract_doc", f"M227-A012-LINK-{packet_id}", f"missing lane link for {packet_id}: {contract_id}")
            )

        total_checks += 1
        if contract_id in packet_doc_text:
            passed_checks += 1
        else:
            findings.append(
                Finding("packet_doc", f"M227-A012-PKT-LINK-{packet_id}", f"missing packet lane link for {packet_id}: {contract_id}")
            )

        lane_text = maybe_load_text(path)
        total_checks += 1
        if lane_text is None:
            findings.append(
                Finding(
                    f"lane_contract_{packet_id}",
                    f"M227-A012-LANE-{packet_id}-00",
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
                    f"M227-A012-LANE-{packet_id}",
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
