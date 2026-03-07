#!/usr/bin/env python3
"""Fail-closed checker for M237-E006 block conformance and replay gate contract freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m237-e001-block-conformance-and-replay-gate-contract-architecture-freeze-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m237_block_conformance_and_replay_gate_edge_case_expansion_and_robustness_e006_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m237"
    / "m237_e006_block_conformance_and_replay_gate_edge_case_expansion_and_robustness_packet.md"
)
DEFAULT_SUMMARY_OUT = Path("tmp/reports/m237/M237-E006/local_check_summary.json")


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
        "M237-E006-DEP-A001-01",
        Path("docs/contracts/m237_block_syntax_and_capture_declarations_contract_and_architecture_freeze_a001_expectations.md"),
    ),
    AssetCheck(
        "M237-E006-DEP-A001-02",
        Path("spec/planning/compiler/m237/m237_a001_block_syntax_and_capture_declarations_contract_and_architecture_freeze_packet.md"),
    ),
    AssetCheck(
        "M237-E006-DEP-A001-03",
        Path("scripts/check_m237_a001_block_syntax_and_capture_declarations_contract.py"),
    ),
    AssetCheck(
        "M237-E006-DEP-A001-04",
        Path("tests/tooling/test_check_m237_a001_block_syntax_and_capture_declarations_contract.py"),
    ),
    AssetCheck(
        "M237-E006-DEP-B001-01",
        Path("docs/contracts/m237_block_semantic_capture_and_lifetime_rules_contract_and_architecture_freeze_b001_expectations.md"),
    ),
    AssetCheck(
        "M237-E006-DEP-B001-02",
        Path("spec/planning/compiler/m237/m237_b001_block_semantic_capture_and_lifetime_rules_contract_and_architecture_freeze_packet.md"),
    ),
    AssetCheck(
        "M237-E006-DEP-B001-03",
        Path("scripts/check_m237_b001_block_semantic_capture_and_lifetime_rules_contract.py"),
    ),
    AssetCheck(
        "M237-E006-DEP-B001-04",
        Path("tests/tooling/test_check_m237_b001_block_semantic_capture_and_lifetime_rules_contract.py"),
    ),
    AssetCheck(
        "M237-E006-DEP-C001-01",
        Path("docs/contracts/m237_block_lowering_and_invoke_emission_contract_and_architecture_freeze_c001_expectations.md"),
    ),
    AssetCheck(
        "M237-E006-DEP-C001-02",
        Path("spec/planning/compiler/m237/m237_c001_block_lowering_and_invoke_emission_contract_and_architecture_freeze_packet.md"),
    ),
    AssetCheck(
        "M237-E006-DEP-C001-03",
        Path("scripts/check_m237_c001_block_lowering_and_invoke_emission_contract.py"),
    ),
    AssetCheck(
        "M237-E006-DEP-C001-04",
        Path("tests/tooling/test_check_m237_c001_block_lowering_and_invoke_emission_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M237-E006-DOC-EXP-01",
        "# M237 Block conformance and replay gate Edge-case Expansion and Robustness Expectations (E001)",
    ),
    SnippetCheck(
        "M237-E006-DOC-EXP-02",
        "Contract ID: `objc3c-block-conformance-and-replay-gate-contract-architecture-freeze/m237-e001-v1`",
    ),
    SnippetCheck("M237-E006-DOC-EXP-03", "Issue: `#5840`"),
    SnippetCheck(
        "M237-E006-DOC-EXP-04",
        "Dependencies: `M237-A001`, `M237-B001`, `M237-C001`",
    ),
    SnippetCheck(
        "M237-E006-DOC-EXP-05",
        "currently-closed early lane steps",
    ),
    SnippetCheck("M237-E006-DOC-EXP-06", "| `M237-A001` |"),
    SnippetCheck("M237-E006-DOC-EXP-07", "| `M237-B001` |"),
    SnippetCheck("M237-E006-DOC-EXP-08", "| `M237-C001` |"),
    SnippetCheck(
        "M237-E006-DOC-EXP-09",
        "scripts/check_m237_e006_block_conformance_and_replay_gate_contract.py",
    ),
    SnippetCheck(
        "M237-E006-DOC-EXP-10",
        "tests/tooling/test_check_m237_e006_block_conformance_and_replay_gate_contract.py",
    ),
    SnippetCheck(
        "M237-E006-DOC-EXP-11",
        "tmp/reports/m237/M237-E006/local_check_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M237-E006-DOC-PKT-01",
        "# M237-E006 Block conformance and replay gate Edge-case Expansion and Robustness Packet",
    ),
    SnippetCheck("M237-E006-DOC-PKT-02", "Packet: `M237-E006`"),
    SnippetCheck("M237-E006-DOC-PKT-03", "Issue: `#5840`"),
    SnippetCheck(
        "M237-E006-DOC-PKT-04",
        "Dependencies: `M237-A001`, `M237-B001`, `M237-C001`",
    ),
    SnippetCheck(
        "M237-E006-DOC-PKT-05",
        "currently-closed early lane steps",
    ),
    SnippetCheck(
        "M237-E006-DOC-PKT-06",
        "docs/contracts/m237_block_conformance_and_replay_gate_edge_case_expansion_and_robustness_e006_expectations.md",
    ),
    SnippetCheck(
        "M237-E006-DOC-PKT-07",
        "scripts/check_m237_e006_block_conformance_and_replay_gate_contract.py",
    ),
    SnippetCheck(
        "M237-E006-DOC-PKT-08",
        "tests/tooling/test_check_m237_e006_block_conformance_and_replay_gate_contract.py",
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
                    detail=f"missing prerequisite asset: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_doc_contract(
    *,
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
                detail=f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, findings


def finding_sort_key(finding: Finding) -> tuple[str, str, str]:
    return (finding.artifact, finding.check_id, finding.detail)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M237-E006-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M237-E006-DOC-PKT-EXISTS", PACKET_SNIPPETS),
    ):
        check_count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += check_count
        failures.extend(findings)

    failures.sort(key=finding_sort_key)
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in failures
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))







