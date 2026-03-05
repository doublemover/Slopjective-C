#!/usr/bin/env python3
"""Fail-closed checker for M233-E005 conformance corpus and gate closeout edge-case and compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m233-e005-lane-e-conformance-corpus-gate-closeout-edge-case-and-compatibility-completion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m233_lane_e_conformance_corpus_and_gate_closeout_edge_case_and_compatibility_completion_e005_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m233"
    / "m233_e005_lane_e_conformance_corpus_and_gate_closeout_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m233/M233-E005/lane_e_conformance_corpus_gate_closeout_edge_case_and_compatibility_completion_summary.json"
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
        "M233-E005-DEP-E004-01",
        Path("docs/contracts/m233_lane_e_conformance_corpus_and_gate_closeout_core_feature_expansion_e004_expectations.md"),
    ),
    AssetCheck(
        "M233-E005-DEP-E004-02",
        Path("spec/planning/compiler/m233/m233_e004_lane_e_conformance_corpus_and_gate_closeout_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M233-E005-DEP-E004-03",
        Path("scripts/check_m233_e004_lane_e_conformance_corpus_and_gate_closeout_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M233-E005-DEP-E004-04",
        Path(
            "tests/tooling/"
            "test_check_m233_e004_lane_e_conformance_corpus_and_gate_closeout_core_feature_expansion_contract.py"
        ),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-E005-DOC-EXP-01",
        "# M233 Lane E Conformance Corpus and Gate Closeout Edge-Case and Compatibility Completion Expectations (E005)",
    ),
    SnippetCheck(
        "M233-E005-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-conformance-corpus-gate-closeout-edge-case-and-compatibility-completion/m233-e005-v1`",
    ),
    SnippetCheck(
        "M233-E005-DOC-EXP-03",
        "Issue `#5658` defines canonical lane-E edge-case and compatibility completion scope.",
    ),
    SnippetCheck(
        "M233-E005-DOC-EXP-04",
        "Dependencies: `M233-E004`, `M233-A004`, `M233-B005`, `M233-C006`, `M233-D008`",
    ),
    SnippetCheck(
        "M233-E005-DOC-EXP-05",
        "docs/contracts/m233_lane_e_conformance_corpus_and_gate_closeout_core_feature_expansion_e004_expectations.md",
    ),
    SnippetCheck(
        "M233-E005-DOC-EXP-06",
        "milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M233-E005-DOC-EXP-07",
        "`scripts/check_m233_e005_lane_e_conformance_corpus_and_gate_closeout_edge_case_and_compatibility_completion_contract.py`",
    ),
    SnippetCheck(
        "M233-E005-DOC-EXP-08",
        "`tests/tooling/test_check_m233_e005_lane_e_conformance_corpus_and_gate_closeout_edge_case_and_compatibility_completion_contract.py`",
    ),
    SnippetCheck(
        "M233-E005-DOC-EXP-09",
        "`tmp/reports/m233/M233-E005/lane_e_conformance_corpus_gate_closeout_edge_case_and_compatibility_completion_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M233-E005-DOC-PKT-01",
        "# M233-E005 Lane-E Conformance Corpus and Gate Closeout Edge-Case and Compatibility Completion Packet",
    ),
    SnippetCheck("M233-E005-DOC-PKT-02", "Packet: `M233-E005`"),
    SnippetCheck("M233-E005-DOC-PKT-03", "Issue: `#5658`"),
    SnippetCheck(
        "M233-E005-DOC-PKT-04",
        "Dependencies: `M233-E004`, `M233-A004`, `M233-B005`, `M233-C006`, `M233-D008`",
    ),
    SnippetCheck(
        "M233-E005-DOC-PKT-05",
        "docs/contracts/m233_lane_e_conformance_corpus_and_gate_closeout_edge_case_and_compatibility_completion_e005_expectations.md",
    ),
    SnippetCheck(
        "M233-E005-DOC-PKT-06",
        "scripts/check_m233_e005_lane_e_conformance_corpus_and_gate_closeout_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M233-E005-DOC-PKT-07",
        "tests/tooling/test_check_m233_e005_lane_e_conformance_corpus_and_gate_closeout_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M233-E005-DOC-PKT-08",
        "spec/planning/compiler/m233/m233_e004_lane_e_conformance_corpus_and_gate_closeout_core_feature_expansion_packet.md",
    ),
    SnippetCheck(
        "M233-E005-DOC-PKT-09",
        "milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M233-E005-DOC-PKT-10",
        "tmp/reports/m233/M233-E005/lane_e_conformance_corpus_gate_closeout_edge_case_and_compatibility_completion_summary.json",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M233-E005-PKG-01", '"compile:objc3c": '),
    SnippetCheck("M233-E005-PKG-02", '"proof:objc3c": '),
    SnippetCheck("M233-E005-PKG-03", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M233-E005-PKG-04", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
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
        findings.append(Finding(display_path(path), exists_check_id, f"required document is missing: {display_path(path)}"))
        return checks_total, findings
    if not path.is_file():
        findings.append(Finding(display_path(path), exists_check_id, f"required path is not a file: {display_path(path)}"))
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M233-E005-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M233-E005-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.package_json, "M233-E005-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in failures],
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
