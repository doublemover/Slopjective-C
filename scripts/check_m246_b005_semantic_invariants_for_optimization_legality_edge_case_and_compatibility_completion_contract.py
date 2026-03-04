#!/usr/bin/env python3
"""Fail-closed checker for M246-B005 semantic invariants edge-case and compatibility completion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-b005-semantic-invariants-optimization-legality-edge-case-and-compatibility-completion-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_b005_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_b005_lane_b_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-B005/semantic_invariants_optimization_legality_edge_case_and_compatibility_completion_summary.json"
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
        "M246-B005-DEP-B004-01",
        Path("docs/contracts/m246_semantic_invariants_for_optimization_legality_core_feature_expansion_b004_expectations.md"),
    ),
    AssetCheck(
        "M246-B005-DEP-B004-02",
        Path("spec/planning/compiler/m246/m246_b004_semantic_invariants_for_optimization_legality_core_feature_expansion_packet.md"),
    ),
    AssetCheck(
        "M246-B005-DEP-B004-03",
        Path("scripts/check_m246_b004_semantic_invariants_for_optimization_legality_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M246-B005-DEP-B004-04",
        Path("tests/tooling/test_check_m246_b004_semantic_invariants_for_optimization_legality_core_feature_expansion_contract.py"),
    ),
    AssetCheck(
        "M246-B005-DEP-B004-05",
        Path("scripts/run_m246_b004_lane_b_readiness.py"),
    ),
    AssetCheck(
        "M246-B005-OWN-01",
        Path("docs/contracts/m246_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_b005_expectations.md"),
    ),
    AssetCheck(
        "M246-B005-OWN-02",
        Path("spec/planning/compiler/m246/m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_packet.md"),
    ),
    AssetCheck(
        "M246-B005-OWN-03",
        Path("scripts/check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M246-B005-OWN-04",
        Path("tests/tooling/test_check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M246-B005-OWN-05",
        Path("scripts/run_m246_b005_lane_b_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-B005-DOC-EXP-01",
        "# M246 Semantic Invariants for Optimization Legality Edge-Case and Compatibility Completion Expectations (B005)",
    ),
    SnippetCheck(
        "M246-B005-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-invariants-optimization-legality-edge-case-and-compatibility-completion/m246-b005-v1`",
    ),
    SnippetCheck("M246-B005-DOC-EXP-03", "## Issue Anchor"),
    SnippetCheck("M246-B005-DOC-EXP-04", "- Issue: `#5064`"),
    SnippetCheck(
        "M246-B005-DOC-EXP-05",
        "Issue `#5064` defines canonical lane-B edge-case and compatibility completion scope.",
    ),
    SnippetCheck("M246-B005-DOC-EXP-06", "Dependencies: `M246-B004`"),
    SnippetCheck(
        "M246-B005-DOC-EXP-07",
        "scripts/check_m246_b004_semantic_invariants_for_optimization_legality_core_feature_expansion_contract.py",
    ),
    SnippetCheck(
        "M246-B005-DOC-EXP-08",
        "tests/tooling/test_check_m246_b004_semantic_invariants_for_optimization_legality_core_feature_expansion_contract.py",
    ),
    SnippetCheck("M246-B005-DOC-EXP-09", "scripts/run_m246_b004_lane_b_readiness.py"),
    SnippetCheck(
        "M246-B005-DOC-EXP-10",
        "scripts/check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M246-B005-DOC-EXP-11",
        "tests/tooling/test_check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck("M246-B005-DOC-EXP-12", "scripts/run_m246_b005_lane_b_readiness.py"),
    SnippetCheck(
        "M246-B005-DOC-EXP-13",
        "milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M246-B005-DOC-EXP-14",
        "tmp/reports/m246/M246-B005/semantic_invariants_optimization_legality_edge_case_and_compatibility_completion_summary.json",
    ),
    SnippetCheck(
        "M246-B005-DOC-EXP-15",
        "deterministic sorted failures and `--emit-json` support.",
    ),
    SnippetCheck(
        "M246-B005-DOC-EXP-16",
        "`python scripts/check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py --emit-json --summary-out tmp/reports/m246/M246-B005/semantic_invariants_optimization_legality_edge_case_and_compatibility_completion_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-B005-DOC-PKT-01",
        "# M246-B005 Semantic Invariants for Optimization Legality Edge-Case and Compatibility Completion Packet",
    ),
    SnippetCheck("M246-B005-DOC-PKT-02", "Packet: `M246-B005`"),
    SnippetCheck("M246-B005-DOC-PKT-03", "Issue: `#5064`"),
    SnippetCheck("M246-B005-DOC-PKT-04", "Dependencies: `M246-B004`"),
    SnippetCheck("M246-B005-DOC-PKT-05", "## Issue Anchor"),
    SnippetCheck("M246-B005-DOC-PKT-06", "- Issue: `#5064`"),
    SnippetCheck(
        "M246-B005-DOC-PKT-07",
        "scripts/check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M246-B005-DOC-PKT-08",
        "tests/tooling/test_check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M246-B005-DOC-PKT-09",
        "scripts/run_m246_b005_lane_b_readiness.py",
    ),
    SnippetCheck(
        "M246-B005-DOC-PKT-10",
        "scripts/check_m246_b004_semantic_invariants_for_optimization_legality_core_feature_expansion_contract.py",
    ),
    SnippetCheck(
        "M246-B005-DOC-PKT-11",
        "tests/tooling/test_check_m246_b004_semantic_invariants_for_optimization_legality_core_feature_expansion_contract.py",
    ),
    SnippetCheck(
        "M246-B005-DOC-PKT-12",
        "scripts/run_m246_b004_lane_b_readiness.py",
    ),
    SnippetCheck(
        "M246-B005-DOC-PKT-13",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M246-B005-DOC-PKT-14",
        "tmp/reports/m246/M246-B005/semantic_invariants_optimization_legality_edge_case_and_compatibility_completion_summary.json",
    ),
    SnippetCheck(
        "M246-B005-DOC-PKT-15",
        "deterministic sorted failures and `--emit-json` support",
    ),
    SnippetCheck(
        "M246-B005-DOC-PKT-16",
        "`python scripts/check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py --emit-json --summary-out tmp/reports/m246/M246-B005/semantic_invariants_optimization_legality_edge_case_and_compatibility_completion_summary.json`",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-B005-RUN-01",
        "scripts/run_m246_b004_lane_b_readiness.py",
    ),
    SnippetCheck(
        "M246-B005-RUN-02",
        "scripts/check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck(
        "M246-B005-RUN-03",
        "tests/tooling/test_check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py",
    ),
    SnippetCheck("M246-B005-RUN-04", "[ok] M246-B005 lane-B readiness chain completed"),
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
    parser.add_argument("--readiness-script", type=Path, default=DEFAULT_READINESS_SCRIPT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--emit-json", action="store_true", help="Emit canonical summary JSON to stdout.")
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

    try:
        text = path.read_text(encoding="utf-8")
    except OSError as exc:
        findings.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"unable to read required document: {exc}",
            )
        )
        return checks_total, findings

    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(Finding(display_path(path), snippet.check_id, f"missing required snippet: {snippet.snippet}"))
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
        (args.expectations_doc, "M246-B005-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-B005-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.readiness_script, "M246-B005-RUN-EXISTS", READINESS_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    failures = sorted(failures, key=finding_sort_key)
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

    if args.emit_json:
        sys.stdout.write(canonical_json(summary_payload))

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
