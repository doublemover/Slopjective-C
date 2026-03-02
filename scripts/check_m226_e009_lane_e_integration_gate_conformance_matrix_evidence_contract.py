#!/usr/bin/env python3
"""Fail-closed validator for M226-E009 lane-E conformance matrix evidence."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-e009-lane-e-integration-gate-conformance-matrix-evidence-contract-v1"

DEFAULT_CONTRACT_DOC = (
    ROOT / "docs" / "contracts" / "m226_lane_e_integration_gate_e009_conformance_matrix_evidence_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m226"
    / "m226_e009_lane_e_integration_gate_conformance_matrix_evidence_packet.md"
)
DEFAULT_EVIDENCE_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m226"
    / "m226_e009_lane_e_integration_gate_conformance_matrix_evidence_scaffold.md"
)
DEFAULT_FREEZE_DOC = ROOT / "spec" / "planning" / "compiler" / "m226" / "m226_lane_e_contract_freeze_20260302.md"
DEFAULT_SUMMARY_OUT = (
    Path("tmp/reports/m226/m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract_summary.json")
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    relative_path: Path


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M226-E009-ASSET-01",
        Path("docs/contracts/m226_lane_e_integration_gate_e008_recovery_determinism_evidence_expectations.md"),
    ),
    AssetCheck(
        "M226-E009-ASSET-02",
        Path("docs/contracts/m226_parser_conformance_matrix_expectations.md"),
    ),
    AssetCheck(
        "M226-E009-ASSET-03",
        Path("docs/contracts/m226_parser_sema_conformance_matrix_b009_expectations.md"),
    ),
    AssetCheck(
        "M226-E009-ASSET-04",
        Path("docs/contracts/m226_parse_lowering_conformance_matrix_c009_expectations.md"),
    ),
    AssetCheck(
        "M226-E009-ASSET-05",
        Path("docs/contracts/m226_frontend_build_invocation_conformance_matrix_d009_expectations.md"),
    ),
    AssetCheck(
        "M226-E009-ASSET-06",
        Path("scripts/check_m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract.py"),
    ),
    AssetCheck(
        "M226-E009-ASSET-07",
        Path("tests/tooling/test_check_m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract.py"),
    ),
)

CONTRACT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M226-E009-DOC-CON-01",
        "# M226 Lane E Integration Gate Conformance Matrix Evidence Expectations (E009)",
    ),
    SnippetCheck(
        "M226-E009-DOC-CON-02",
        "Contract ID: `objc3c-lane-e-integration-gate-conformance-matrix-evidence-contract/m226-e009-v1`",
    ),
    SnippetCheck(
        "M226-E009-DOC-CON-03",
        "Required top-level keys: `packet`, `generated_at_utc`, `upstream_packets`, `artifacts`, `milestone_gate`, `feature_matrix`, `conformance_matrix`.",
    ),
    SnippetCheck(
        "M226-E009-DOC-CON-04",
        "`conformance_matrix` required keys: `parser`, `parser_sema`, `parse_lowering`, `build_invocation`, `integration_gate`.",
    ),
    SnippetCheck(
        "M226-E009-DOC-CON-05",
        "- `M226-E008`, `M226-A009`, `M226-B009`, `M226-C009`, `M226-D009`.",
    ),
    SnippetCheck(
        "M226-E009-DOC-CON-06",
        "`docs/contracts/m226_parse_lowering_conformance_matrix_c009_expectations.md`",
    ),
    SnippetCheck(
        "M226-E009-DOC-CON-07",
        "`tmp/reports/m226/m226_c009_parse_lowering_conformance_matrix_contract_summary.json`",
    ),
    SnippetCheck(
        "M226-E009-DOC-CON-08",
        "`python scripts/check_m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract.py`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M226-E009-DOC-PKT-01", "# M226 Lane E Integration Gate Conformance Matrix Evidence Packet (E009)"),
    SnippetCheck("M226-E009-DOC-PKT-02", "Packet: `M226-E009`"),
    SnippetCheck(
        "M226-E009-DOC-PKT-03",
        "Upstream packet dependencies: `M226-E008`, `M226-A009`, `M226-B009`, `M226-C009`, `M226-D009`",
    ),
    SnippetCheck("M226-E009-DOC-PKT-04", "| Upstream conformance artifact anchors |"),
    SnippetCheck(
        "M226-E009-DOC-PKT-05",
        "`tmp/reports/m226/M226-A009/parser_conformance_matrix_summary.json`",
    ),
    SnippetCheck(
        "M226-E009-DOC-PKT-06",
        "`tmp/reports/m226/M226-D009/frontend_build_invocation_conformance_matrix_summary.json`",
    ),
    SnippetCheck(
        "M226-E009-DOC-PKT-07",
        "`scripts/check_m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract.py`",
    ),
)

EVIDENCE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M226-E009-DOC-EVI-01",
        "# M226 Lane E Integration Gate Conformance Matrix Evidence Scaffold (E009)",
    ),
    SnippetCheck(
        "M226-E009-DOC-EVI-02",
        "Path: `tmp/reports/m226/m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract_summary.json`",
    ),
    SnippetCheck(
        "M226-E009-DOC-EVI-03",
        "Path: `tmp/reports/m226/e009/evidence_index.json`",
    ),
    SnippetCheck(
        "M226-E009-DOC-EVI-04",
        "- Required top-level keys: `packet`, `generated_at_utc`, `upstream_packets`, `artifacts`, `milestone_gate`, `feature_matrix`, `conformance_matrix`.",
    ),
    SnippetCheck(
        "M226-E009-DOC-EVI-05",
        "- `conformance_matrix` required keys: `parser`, `parser_sema`, `parse_lowering`, `build_invocation`, `integration_gate`.",
    ),
    SnippetCheck(
        "M226-E009-DOC-EVI-06",
        "- `E009-DOC-01`: `docs/contracts/m226_parser_conformance_matrix_expectations.md`",
    ),
    SnippetCheck(
        "M226-E009-DOC-EVI-07",
        "- `E009-ART-05`: `tmp/reports/m226/M226-D009/frontend_build_invocation_conformance_matrix_summary.json`",
    ),
)

FREEZE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M226-E009-DOC-FRZ-01",
        "| `M226-E009` | `objc3c-lane-e-integration-gate-conformance-matrix-evidence-contract/m226-e009-v1` |",
    ),
    SnippetCheck("M226-E009-DOC-FRZ-02", "## Packet: `M226-E009`"),
    SnippetCheck(
        "M226-E009-DOC-FRZ-03",
        "`docs/contracts/m226_lane_e_integration_gate_e009_conformance_matrix_evidence_expectations.md`",
    ),
    SnippetCheck(
        "M226-E009-DOC-FRZ-04",
        "`scripts/check_m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract.py`",
    ),
    SnippetCheck(
        "M226-E009-DOC-FRZ-05",
        "`tests/tooling/test_check_m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract.py`",
    ),
    SnippetCheck(
        "M226-E009-DOC-FRZ-06",
        "#### Upstream Conformance Matrix Anchors",
    ),
    SnippetCheck(
        "M226-E009-DOC-FRZ-07",
        "`tmp/reports/m226/M226-A009/parser_conformance_matrix_summary.json`",
    ),
    SnippetCheck(
        "M226-E009-DOC-FRZ-08",
        "`tmp/reports/m226/m226_e008_lane_e_integration_gate_recovery_determinism_evidence_contract_summary.json`",
    ),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--contract-doc", type=Path, default=DEFAULT_CONTRACT_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--evidence-doc", type=Path, default=DEFAULT_EVIDENCE_DOC)
    parser.add_argument("--freeze-doc", type=Path, default=DEFAULT_FREEZE_DOC)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def append_snippet_findings(
    *,
    artifact: str,
    text: str,
    checks: Sequence[SnippetCheck],
    findings: list[Finding],
) -> tuple[int, int]:
    total = 0
    passed = 0
    for check in checks:
        total += 1
        if check.snippet in text:
            passed += 1
        else:
            findings.append(Finding(artifact, check.check_id, f"expected snippet missing: {check.snippet}"))
    return total, passed


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute_path = ROOT / asset.relative_path
        if absolute_path.exists() and absolute_path.is_file():
            checks_passed += 1
        else:
            findings.append(
                Finding(
                    "prerequisite_assets",
                    asset.check_id,
                    f"required asset missing: {asset.relative_path.as_posix()}",
                )
            )

    contract_text = load_text(args.contract_doc, artifact="contract_doc")
    packet_text = load_text(args.packet_doc, artifact="packet_doc")
    evidence_text = load_text(args.evidence_doc, artifact="evidence_doc")
    freeze_text = load_text(args.freeze_doc, artifact="freeze_doc")

    total, passed = append_snippet_findings(
        artifact="contract_doc",
        text=contract_text,
        checks=CONTRACT_SNIPPETS,
        findings=findings,
    )
    checks_total += total
    checks_passed += passed

    total, passed = append_snippet_findings(
        artifact="packet_doc",
        text=packet_text,
        checks=PACKET_SNIPPETS,
        findings=findings,
    )
    checks_total += total
    checks_passed += passed

    total, passed = append_snippet_findings(
        artifact="evidence_doc",
        text=evidence_text,
        checks=EVIDENCE_SNIPPETS,
        findings=findings,
    )
    checks_total += total
    checks_passed += passed

    total, passed = append_snippet_findings(
        artifact="freeze_doc",
        text=freeze_text,
        checks=FREEZE_SNIPPETS,
        findings=findings,
    )
    checks_total += total
    checks_passed += passed

    summary_payload = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
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
    args.summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    if not findings:
        return 0

    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
