#!/usr/bin/env python3
"""Fail-closed prerequisite checker for the M226-E001 lane-E integration gate."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-e001-lane-e-integration-gate-contract-v1"

DEFAULT_EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m226_lane_e_integration_gate_expectations.md"
DEFAULT_FREEZE_DOC = (
    ROOT / "spec" / "planning" / "compiler" / "m226" / "m226_lane_e_contract_freeze_20260302.md"
)
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m226/m226_e001_lane_e_integration_gate_contract_summary.json")


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
        "M226-E001-A001-01",
        "M226-A001",
        Path("docs/contracts/m226_parser_architecture_expectations.md"),
    ),
    AssetCheck(
        "M226-E001-A001-02",
        "M226-A001",
        Path("scripts/check_m226_a001_parser_architecture_contract.py"),
    ),
    AssetCheck(
        "M226-E001-A001-03",
        "M226-A001",
        Path("tests/tooling/test_check_m226_a001_parser_architecture_contract.py"),
    ),
    AssetCheck(
        "M226-E001-A002-01",
        "M226-A002",
        Path("docs/contracts/m226_parser_modular_split_expectations.md"),
    ),
    AssetCheck(
        "M226-E001-A002-02",
        "M226-A002",
        Path("scripts/check_m226_a002_parser_modular_split_contract.py"),
    ),
    AssetCheck(
        "M226-E001-A002-03",
        "M226-A002",
        Path("tests/tooling/test_check_m226_a002_parser_modular_split_contract.py"),
    ),
    AssetCheck(
        "M226-E001-A003-01",
        "M226-A003",
        Path("docs/contracts/m226_parser_contract_snapshot_expectations.md"),
    ),
    AssetCheck(
        "M226-E001-A003-02",
        "M226-A003",
        Path("scripts/check_m226_a003_parser_contract_snapshot_contract.py"),
    ),
    AssetCheck(
        "M226-E001-A003-03",
        "M226-A003",
        Path("tests/tooling/test_check_m226_a003_parser_contract_snapshot_contract.py"),
    ),
    AssetCheck(
        "M226-E001-A004-01",
        "M226-A004",
        Path("docs/contracts/m226_parser_snapshot_surface_expansion_expectations.md"),
    ),
    AssetCheck(
        "M226-E001-A004-02",
        "M226-A004",
        Path("scripts/check_m226_a004_parser_snapshot_surface_expansion_contract.py"),
    ),
    AssetCheck(
        "M226-E001-A004-03",
        "M226-A004",
        Path("tests/tooling/test_check_m226_a004_parser_snapshot_surface_expansion_contract.py"),
    ),
    AssetCheck(
        "M226-E001-B001-01",
        "M226-B001",
        Path("docs/contracts/m226_parser_sema_handoff_expectations.md"),
    ),
    AssetCheck(
        "M226-E001-B001-02",
        "M226-B001",
        Path("scripts/check_m226_b001_parser_sema_handoff_contract.py"),
    ),
    AssetCheck(
        "M226-E001-B001-03",
        "M226-B001",
        Path("tests/tooling/test_check_m226_b001_parser_sema_handoff_contract.py"),
    ),
    AssetCheck(
        "M226-E001-C001-01",
        "M226-C001",
        Path("scripts/check_m142_frontend_lowering_parity_contract.py"),
    ),
    AssetCheck(
        "M226-E001-D001-01",
        "M226-D001",
        Path("docs/contracts/m226_frontend_build_invocation_expectations.md"),
    ),
    AssetCheck(
        "M226-E001-D001-02",
        "M226-D001",
        Path("scripts/check_m226_d001_frontend_build_invocation_contract.py"),
    ),
    AssetCheck(
        "M226-E001-D001-03",
        "M226-D001",
        Path("tests/tooling/test_check_m226_d001_frontend_build_invocation_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M226-E001-DOC-EXP-02", "# M226 Lane E Integration Gate Expectations (E001)"),
    SnippetCheck(
        "M226-E001-DOC-EXP-03",
        "Contract ID: `objc3c-lane-e-integration-gate-contract/m226-e001-v1`",
    ),
    SnippetCheck("M226-E001-DOC-EXP-04", "`M226-A001`"),
    SnippetCheck("M226-E001-DOC-EXP-05", "`M226-D001`"),
    SnippetCheck(
        "M226-E001-DOC-EXP-06",
        "`scripts/check_m142_frontend_lowering_parity_contract.py`",
    ),
    SnippetCheck(
        "M226-E001-DOC-EXP-07",
        "`tmp/reports/m226/m226_e001_lane_e_integration_gate_contract_summary.json`",
    ),
)

FREEZE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M226-E001-DOC-FRZ-02", "# M226 Lane E Contract Freeze (2026-03-02)"),
    SnippetCheck("M226-E001-DOC-FRZ-03", "Packet: `M226-E001`"),
    SnippetCheck("M226-E001-DOC-FRZ-04", "Freeze date: `2026-03-02`"),
    SnippetCheck("M226-E001-DOC-FRZ-05", "`M226-C001`"),
    SnippetCheck(
        "M226-E001-DOC-FRZ-06",
        "`scripts/check_m142_frontend_lowering_parity_contract.py`",
    ),
    SnippetCheck(
        "M226-E001-DOC-FRZ-07",
        "`python scripts/check_m226_e001_lane_e_integration_gate_contract.py`",
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
                    detail=f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite path is not a file: {asset.relative_path.as_posix()}",
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

    expectations_checks, expectations_findings = check_doc_contract(
        artifact_name="expectations_doc",
        path=args.expectations_doc,
        exists_check_id="M226-E001-DOC-EXP-01",
        snippets=EXPECTATIONS_SNIPPETS,
    )
    checks_total += expectations_checks
    findings.extend(expectations_findings)

    freeze_checks, freeze_findings = check_doc_contract(
        artifact_name="freeze_doc",
        path=args.freeze_doc,
        exists_check_id="M226-E001-DOC-FRZ-01",
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
        print(f"m226-e001-lane-e-integration-gate-contract: error: unable to write summary: {exc}", file=sys.stderr)
        return 2

    if not findings:
        print("m226-e001-lane-e-integration-gate-contract: OK")
        return 0

    print(
        "m226-e001-lane-e-integration-gate-contract: contract drift detected "
        f"({len(findings)} failed check(s)).",
        file=sys.stderr,
    )
    for finding in findings:
        print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
