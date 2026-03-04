#!/usr/bin/env python3
"""Fail-closed contract checker for M245-C014 lowering/IR portability release-candidate and replay dry-run."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m245-c014-lowering-ir-portability-contracts-release-candidate-and-replay-dry-run-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_c014_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_C013_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m245_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_c013_expectations.md"
)
DEFAULT_C013_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m245"
    / "m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_packet.md"
)
DEFAULT_C013_CHECKER = (
    ROOT
    / "scripts"
    / "check_m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_contract.py"
)
DEFAULT_C013_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_contract.py"
)
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m245/M245-C014/lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    lane_task: str
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
        "M245-C014-C013-01",
        "M245-C013",
        Path(
            "docs/contracts/m245_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_c013_expectations.md"
        ),
    ),
    AssetCheck(
        "M245-C014-C013-02",
        "M245-C013",
        Path(
            "spec/planning/compiler/m245/m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_packet.md"
        ),
    ),
    AssetCheck(
        "M245-C014-C013-03",
        "M245-C013",
        Path(
            "scripts/check_m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_contract.py"
        ),
    ),
    AssetCheck(
        "M245-C014-C013-04",
        "M245-C013",
        Path(
            "tests/tooling/test_check_m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_contract.py"
        ),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-C014-DOC-EXP-01",
        "# M245 Lowering/IR Portability Contracts Release-Candidate and Replay Dry-Run Expectations (C014)",
    ),
    SnippetCheck(
        "M245-C014-DOC-EXP-02",
        "Contract ID: `objc3c-lowering-ir-portability-contracts-release-candidate-and-replay-dry-run/m245-c014-v1`",
    ),
    SnippetCheck("M245-C014-DOC-EXP-03", "Dependencies: `M245-C013`"),
    SnippetCheck(
        "M245-C014-DOC-EXP-04",
        "Issue `#6649` defines canonical lane-C release-candidate and replay dry-run scope.",
    ),
    SnippetCheck("M245-C014-DOC-EXP-05", "Dependency token: `M245-C013`."),
    SnippetCheck(
        "M245-C014-DOC-EXP-06",
        "`scripts/check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M245-C014-DOC-EXP-07",
        "`tests/tooling/test_check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck("M245-C014-DOC-EXP-08", "release-candidate and replay dry-run"),
    SnippetCheck("M245-C014-DOC-EXP-09", "fail-closed snippet checks on owned lane-C"),
    SnippetCheck(
        "M245-C014-DOC-EXP-10",
        "python scripts/check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py --emit-json",
    ),
    SnippetCheck(
        "M245-C014-DOC-EXP-11",
        "tmp/reports/m245/M245-C014/lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-C014-DOC-PKT-01",
        "# M245-C014 Lowering/IR Portability Contracts Release-Candidate and Replay Dry-Run Packet",
    ),
    SnippetCheck("M245-C014-DOC-PKT-02", "Packet: `M245-C014`"),
    SnippetCheck("M245-C014-DOC-PKT-03", "Issue: `#6649`"),
    SnippetCheck("M245-C014-DOC-PKT-04", "Dependencies: `M245-C013`"),
    SnippetCheck("M245-C014-DOC-PKT-05", "Theme: `release-candidate and replay dry-run`"),
    SnippetCheck("M245-C014-DOC-PKT-06", "Freeze date: `2026-03-04`"),
    SnippetCheck(
        "M245-C014-DOC-PKT-07",
        "m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_packet.md",
    ),
    SnippetCheck(
        "M245-C014-DOC-PKT-08",
        "python scripts/check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py --emit-json",
    ),
    SnippetCheck("M245-C014-DOC-PKT-09", "release-candidate and replay dry-run"),
    SnippetCheck(
        "M245-C014-DOC-PKT-10",
        "tmp/reports/m245/M245-C014/lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract_summary.json",
    ),
)

C013_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-C014-C013-DOC-01",
        "Contract ID: `objc3c-lowering-ir-portability-contracts-docs-and-operator-runbook-synchronization/m245-c013-v1`",
    ),
    SnippetCheck("M245-C014-C013-DOC-02", "Dependencies: `M245-C012`"),
)

C013_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-C014-C013-PKT-01", "Packet: `M245-C013`"),
    SnippetCheck("M245-C014-C013-PKT-02", "Dependencies: `M245-C012`"),
)

C013_CHECKER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M245-C014-C013-CHK-01",
        'MODE = "m245-c013-lowering-ir-portability-contracts-docs-and-operator-runbook-synchronization-contract-v1"',
    ),
    SnippetCheck("M245-C014-C013-CHK-02", "DEFAULT_SUMMARY_OUT = Path("),
)

C013_TEST_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M245-C014-C013-TST-01", "def test_contract_passes_on_repository_sources"),
    SnippetCheck(
        "M245-C014-C013-TST-02",
        "def test_contract_fails_closed_when_packet_issue_anchor_drifts",
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
    parser.add_argument("--expectations-doc", type=Path, default=DEFAULT_EXPECTATIONS_DOC)
    parser.add_argument("--packet-doc", type=Path, default=DEFAULT_PACKET_DOC)
    parser.add_argument("--c013-expectations-doc", type=Path, default=DEFAULT_C013_EXPECTATIONS_DOC)
    parser.add_argument("--c013-packet-doc", type=Path, default=DEFAULT_C013_PACKET_DOC)
    parser.add_argument("--c013-checker", type=Path, default=DEFAULT_C013_CHECKER)
    parser.add_argument("--c013-test", type=Path, default=DEFAULT_C013_TEST)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists() or not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}",
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
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        text = path.read_text(encoding="utf-8")
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
                    detail=f"missing required snippet: {snippet_check.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    checks_total, findings = check_prerequisite_assets()

    for artifact_name, path, exists_check_id, snippets in (
        ("expectations_doc", args.expectations_doc, "M245-C014-DOC-EXP-00", EXPECTATIONS_SNIPPETS),
        ("packet_doc", args.packet_doc, "M245-C014-DOC-PKT-00", PACKET_SNIPPETS),
        (
            "c013_expectations_doc",
            args.c013_expectations_doc,
            "M245-C014-C013-DOC-00",
            C013_EXPECTATIONS_SNIPPETS,
        ),
        ("c013_packet_doc", args.c013_packet_doc, "M245-C014-C013-PKT-00", C013_PACKET_SNIPPETS),
        ("c013_checker", args.c013_checker, "M245-C014-C013-CHK-00", C013_CHECKER_SNIPPETS),
        ("c013_test", args.c013_test, "M245-C014-C013-TST-00", C013_TEST_SNIPPETS),
    ):
        count, new_findings = check_doc_contract(
            artifact_name=artifact_name,
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        findings.extend(new_findings)

    findings = sorted(findings, key=lambda finding: (finding.check_id, finding.artifact, finding.detail))
    summary_payload = {
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

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary_payload), end="")

    if not findings:
        if not args.emit_json:
            print(f"[ok] {MODE}: {summary_payload['checks_passed']}/{summary_payload['checks_total']} checks passed")
        return 0

    if not args.emit_json:
        print(f"{MODE}: contract drift detected ({len(findings)} failed check(s)).", file=sys.stderr)
        for finding in findings:
            print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
