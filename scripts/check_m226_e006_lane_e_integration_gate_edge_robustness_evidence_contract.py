#!/usr/bin/env python3
"""Fail-closed validator for M226-E006 lane-E edge robustness evidence."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-e006-lane-e-integration-gate-edge-robustness-evidence-contract-v1"

DEFAULT_CONTRACT_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m226_lane_e_integration_gate_e006_edge_robustness_evidence_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m226"
    / "m226_e006_lane_e_integration_gate_edge_robustness_evidence_packet.md"
)
DEFAULT_EVIDENCE_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m226"
    / "m226_e006_lane_e_integration_gate_edge_robustness_evidence_scaffold.md"
)
DEFAULT_FREEZE_DOC = ROOT / "spec" / "planning" / "compiler" / "m226" / "m226_lane_e_contract_freeze_20260302.md"
DEFAULT_SUMMARY_OUT = (
    Path("tmp/reports/m226/m226_e006_lane_e_integration_gate_edge_robustness_evidence_contract_summary.json")
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
        "M226-E006-ASSET-01",
        Path("docs/contracts/m226_lane_e_integration_gate_e005_edge_compatibility_evidence_expectations.md"),
    ),
    AssetCheck(
        "M226-E006-ASSET-02",
        Path("scripts/check_m226_e005_lane_e_integration_gate_edge_compatibility_evidence_contract.py"),
    ),
    AssetCheck(
        "M226-E006-ASSET-03",
        Path("tests/tooling/test_check_m226_e005_lane_e_integration_gate_edge_compatibility_evidence_contract.py"),
    ),
    AssetCheck(
        "M226-E006-ASSET-04",
        Path("docs/contracts/m226_parser_advanced_core_workpack_a015_expectations.md"),
    ),
    AssetCheck(
        "M226-E006-ASSET-05",
        Path("docs/contracts/m226_parser_sema_edge_robustness_b006_expectations.md"),
    ),
    AssetCheck(
        "M226-E006-ASSET-06",
        Path("scripts/check_m226_e006_lane_e_integration_gate_edge_robustness_evidence_contract.py"),
    ),
    AssetCheck(
        "M226-E006-ASSET-07",
        Path("tests/tooling/test_check_m226_e006_lane_e_integration_gate_edge_robustness_evidence_contract.py"),
    ),
)

CONTRACT_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M226-E006-DOC-CON-02",
        "# M226 Lane E Integration Gate Edge Robustness Evidence Expectations (E006)",
    ),
    SnippetCheck(
        "M226-E006-DOC-CON-03",
        "Contract ID: `objc3c-lane-e-integration-gate-edge-robustness-evidence-contract/m226-e006-v1`",
    ),
    SnippetCheck(
        "M226-E006-DOC-CON-04",
        "Required top-level keys: `packet`, `generated_at_utc`, `upstream_packets`, `artifacts`, `milestone_gate`, `feature_matrix`, `edge_compatibility`, `edge_robustness`.",
    ),
    SnippetCheck(
        "M226-E006-DOC-CON-05",
        "`edge_robustness` required keys: `parser_core`, `parser_sema`, `parse_lowering`, `build_invocation`, `integration_gate`.",
    ),
    SnippetCheck(
        "M226-E006-DOC-CON-06",
        "`generated_at_utc` uses RFC3339 UTC format with trailing `Z`.",
    ),
    SnippetCheck(
        "M226-E006-DOC-CON-07",
        "Index path is `tmp/reports/m226/e006/evidence_index.json`.",
    ),
    SnippetCheck(
        "M226-E006-DOC-CON-08",
        "- `M226-A015`, `M226-B006`, `M226-C006`, `M226-D006`, `M226-E005`.",
    ),
    SnippetCheck(
        "M226-E006-DOC-CON-09",
        "`docs/contracts/m226_parser_advanced_core_workpack_a015_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-CON-10",
        "`docs/contracts/m226_parser_sema_edge_robustness_b006_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-CON-11",
        "`docs/contracts/m226_lane_e_integration_gate_e005_edge_compatibility_evidence_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-CON-12",
        "`tmp/reports/m226/M226-C006/parse_lowering_edge_robustness_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-CON-13",
        "`tmp/reports/m226/M226-D006/frontend_build_invocation_edge_robustness_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-CON-14",
        "`python scripts/check_m226_e006_lane_e_integration_gate_edge_robustness_evidence_contract.py`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M226-E006-DOC-PKT-02",
        "# M226 Lane E Integration Gate Edge Robustness Evidence Packet (E006)",
    ),
    SnippetCheck("M226-E006-DOC-PKT-03", "Packet: `M226-E006`"),
    SnippetCheck(
        "M226-E006-DOC-PKT-04",
        "Upstream packet dependencies: `M226-E005`, `M226-A015`, `M226-B006`, `M226-C006`, `M226-D006`",
    ),
    SnippetCheck(
        "M226-E006-DOC-PKT-05",
        "| Upstream contract doc anchors |",
    ),
    SnippetCheck(
        "M226-E006-DOC-PKT-06",
        "`docs/contracts/m226_parser_advanced_core_workpack_a015_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-PKT-07",
        "`docs/contracts/m226_parser_sema_edge_robustness_b006_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-PKT-08",
        "`docs/contracts/m226_lane_e_integration_gate_e005_edge_compatibility_evidence_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-PKT-09",
        "| Upstream robustness artifact anchors |",
    ),
    SnippetCheck(
        "M226-E006-DOC-PKT-10",
        "`tmp/reports/m226/M226-A015/parser_advanced_core_workpack_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-PKT-11",
        "`tmp/reports/m226/M226-B006/parser_sema_edge_robustness_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-PKT-12",
        "`tmp/reports/m226/M226-C006/parse_lowering_edge_robustness_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-PKT-13",
        "`tmp/reports/m226/M226-D006/frontend_build_invocation_edge_robustness_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-PKT-14",
        "`tmp/reports/m226/m226_e005_lane_e_integration_gate_edge_compatibility_evidence_contract_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-PKT-15",
        "`scripts/check_m226_e006_lane_e_integration_gate_edge_robustness_evidence_contract.py`",
    ),
    SnippetCheck(
        "M226-E006-DOC-PKT-16",
        "`python -m pytest tests/tooling/test_check_m226_e006_lane_e_integration_gate_edge_robustness_evidence_contract.py -q`",
    ),
)

EVIDENCE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M226-E006-DOC-EVI-02",
        "# M226 Lane E Integration Gate Edge Robustness Evidence Scaffold (E006)",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-03",
        "Path: `tmp/reports/m226/m226_e006_lane_e_integration_gate_edge_robustness_evidence_contract_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-04",
        "Path: `tmp/reports/m226/e006/validation/pytest_check_m226_e006_lane_e_integration_gate_edge_robustness_evidence_contract.txt`",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-05",
        "Path: `tmp/reports/m226/e006/evidence_index.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-06",
        "Required top-level keys: `packet`, `generated_at_utc`, `upstream_packets`, `artifacts`, `milestone_gate`, `feature_matrix`, `edge_compatibility`, `edge_robustness`.",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-07",
        "- `milestone_gate` required keys: `milestone_id`, `gate_status`, `validated_by`.",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-08",
        "- `feature_matrix` required entries: `core_index`, `milestone_gate`, `validation_transcripts`, `edge_compatibility`, `edge_robustness`.",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-09",
        "- `edge_compatibility` required keys: `parser_sema`, `parse_lowering`, `build_invocation`, `replay_dry_run`.",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-10",
        "- `edge_robustness` required keys: `parser_core`, `parser_sema`, `parse_lowering`, `build_invocation`, `integration_gate`.",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-11",
        "- `generated_at_utc` format: RFC3339 UTC with trailing `Z`.",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-12",
        "- `E006-DOC-01`: `docs/contracts/m226_parser_advanced_core_workpack_a015_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-13",
        "- `E006-DOC-02`: `docs/contracts/m226_parser_sema_edge_robustness_b006_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-14",
        "- `E006-DOC-03`: `docs/contracts/m226_lane_e_integration_gate_e005_edge_compatibility_evidence_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-15",
        "- `E006-ART-01`: `tmp/reports/m226/m226_e005_lane_e_integration_gate_edge_compatibility_evidence_contract_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-16",
        "- `E006-ART-02`: `tmp/reports/m226/M226-A015/parser_advanced_core_workpack_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-17",
        "- `E006-ART-03`: `tmp/reports/m226/M226-B006/parser_sema_edge_robustness_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-18",
        "- `E006-ART-04`: `tmp/reports/m226/M226-C006/parse_lowering_edge_robustness_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-19",
        "- `E006-ART-05`: `tmp/reports/m226/M226-D006/frontend_build_invocation_edge_robustness_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-EVI-20",
        "- `E006-ART-06`: `tmp/reports/m226/e006/validation/pytest_check_m226_e006_lane_e_integration_gate_edge_robustness_evidence_contract.txt`",
    ),
)

FREEZE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M226-E006-DOC-FRZ-02", "# M226 Lane E Contract Freeze (2026-03-02)"),
    SnippetCheck(
        "M226-E006-DOC-FRZ-03",
        "| `M226-E006` | `objc3c-lane-e-integration-gate-edge-robustness-evidence-contract/m226-e006-v1` |",
    ),
    SnippetCheck("M226-E006-DOC-FRZ-04", "## Packet: `M226-E006`"),
    SnippetCheck(
        "M226-E006-DOC-FRZ-05",
        "`docs/contracts/m226_lane_e_integration_gate_e006_edge_robustness_evidence_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-FRZ-06",
        "`spec/planning/compiler/m226/m226_e006_lane_e_integration_gate_edge_robustness_evidence_packet.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-FRZ-07",
        "`spec/planning/compiler/m226/m226_e006_lane_e_integration_gate_edge_robustness_evidence_scaffold.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-FRZ-08",
        "`scripts/check_m226_e006_lane_e_integration_gate_edge_robustness_evidence_contract.py`",
    ),
    SnippetCheck(
        "M226-E006-DOC-FRZ-09",
        "`tests/tooling/test_check_m226_e006_lane_e_integration_gate_edge_robustness_evidence_contract.py`",
    ),
    SnippetCheck(
        "M226-E006-DOC-FRZ-10",
        "#### Upstream Robustness Anchors",
    ),
    SnippetCheck(
        "M226-E006-DOC-FRZ-11",
        "`docs/contracts/m226_parser_advanced_core_workpack_a015_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-FRZ-12",
        "`docs/contracts/m226_parser_sema_edge_robustness_b006_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-FRZ-13",
        "`docs/contracts/m226_lane_e_integration_gate_e005_edge_compatibility_evidence_expectations.md`",
    ),
    SnippetCheck(
        "M226-E006-DOC-FRZ-14",
        "`tmp/reports/m226/M226-C006/parse_lowering_edge_robustness_summary.json`",
    ),
    SnippetCheck(
        "M226-E006-DOC-FRZ-15",
        "`tmp/reports/m226/M226-D006/frontend_build_invocation_edge_robustness_summary.json`",
    ),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--contract-doc", type=Path, default=DEFAULT_CONTRACT_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--evidence-doc", type=Path, default=DEFAULT_EVIDENCE_DOC)
    parser.add_argument("--freeze-doc", type=Path, default=DEFAULT_FREEZE_DOC)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"required asset missing: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"required asset path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_doc_contract(
    *,
    artifact_name: str,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail="required document is not valid UTF-8",
            )
        )
        return checks_total, findings
    except OSError as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"unable to read required document: {exc}",
            )
        )
        return checks_total, findings

    for snippet_check in snippets:
        checks_total += 1
        if snippet_check.snippet not in text:
            findings.append(
                Finding(
                    artifact=artifact_name,
                    check_id=snippet_check.check_id,
                    detail=f"expected snippet missing: {snippet_check.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    checks_total, findings = check_prerequisite_assets()

    contract_checks, contract_findings = check_doc_contract(
        artifact_name="contract_doc",
        path=args.contract_doc,
        exists_check_id="M226-E006-DOC-CON-01",
        snippets=CONTRACT_SNIPPETS,
    )
    checks_total += contract_checks
    findings.extend(contract_findings)

    packet_checks, packet_findings = check_doc_contract(
        artifact_name="packet_doc",
        path=args.packet_doc,
        exists_check_id="M226-E006-DOC-PKT-01",
        snippets=PACKET_SNIPPETS,
    )
    checks_total += packet_checks
    findings.extend(packet_findings)

    evidence_checks, evidence_findings = check_doc_contract(
        artifact_name="evidence_doc",
        path=args.evidence_doc,
        exists_check_id="M226-E006-DOC-EVI-01",
        snippets=EVIDENCE_SNIPPETS,
    )
    checks_total += evidence_checks
    findings.extend(evidence_findings)

    freeze_checks, freeze_findings = check_doc_contract(
        artifact_name="freeze_doc",
        path=args.freeze_doc,
        exists_check_id="M226-E006-DOC-FRZ-01",
        snippets=FREEZE_SNIPPETS,
    )
    checks_total += freeze_checks
    findings.extend(freeze_findings)

    findings = sorted(findings, key=lambda finding: (finding.check_id, finding.artifact, finding.detail))
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(findings),
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }

    try:
        args.summary_out.parent.mkdir(parents=True, exist_ok=True)
        args.summary_out.write_text(canonical_json(summary), encoding="utf-8")
    except OSError as exc:
        print(
            "m226-e006-lane-e-integration-gate-edge-robustness-evidence-contract: "
            f"error: unable to write summary: {exc}",
            file=sys.stderr,
        )
        return 2

    if not findings:
        print("m226-e006-lane-e-integration-gate-edge-robustness-evidence-contract: OK")
        return 0

    print(
        "m226-e006-lane-e-integration-gate-edge-robustness-evidence-contract: contract drift detected "
        f"({len(findings)} failed check(s)).",
        file=sys.stderr,
    )
    for finding in findings:
        print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
