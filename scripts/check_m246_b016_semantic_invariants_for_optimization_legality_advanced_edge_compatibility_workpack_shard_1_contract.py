#!/usr/bin/env python3
"""Fail-closed checker for M246-B016 semantic invariants advanced edge compatibility workpack (shard 1)."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-b016-semantic-invariants-optimization-legality-advanced-edge-compatibility-workpack-shard-1-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_semantic_invariants_for_optimization_legality_advanced_edge_compatibility_workpack_shard_1_b016_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_b016_semantic_invariants_for_optimization_legality_advanced_edge_compatibility_workpack_shard_1_packet.md"
)
DEFAULT_B015_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_semantic_invariants_for_optimization_legality_advanced_core_workpack_shard_1_b015_expectations.md"
)
DEFAULT_B015_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_b015_semantic_invariants_for_optimization_legality_advanced_core_workpack_shard_1_packet.md"
)
DEFAULT_B015_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_b015_semantic_invariants_for_optimization_legality_advanced_core_workpack_shard_1_contract.py"
)
DEFAULT_B015_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_b015_semantic_invariants_for_optimization_legality_advanced_core_workpack_shard_1_contract.py"
)
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m246_b016_lane_b_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-B016/semantic_invariants_optimization_legality_advanced_edge_compatibility_workpack_shard_1_summary.json"
)


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-B016-DOC-EXP-01",
        "# M246 Semantic Invariants for Optimization Legality Advanced Edge Compatibility Workpack (Shard 1) Expectations (B016)",
    ),
    SnippetCheck(
        "M246-B016-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-invariants-optimization-legality-advanced-edge-compatibility-workpack-shard-1/m246-b016-v1`",
    ),
    SnippetCheck(
        "M246-B016-DOC-EXP-03",
        "Issue `#5075` defines canonical lane-B advanced edge compatibility workpack (shard 1) scope.",
    ),
    SnippetCheck("M246-B016-DOC-EXP-04", "Dependencies: `M246-B015`"),
    SnippetCheck(
        "M246-B016-DOC-EXP-05",
        "scripts/check_m246_b015_semantic_invariants_for_optimization_legality_advanced_core_workpack_shard_1_contract.py",
    ),
    SnippetCheck(
        "M246-B016-DOC-EXP-06",
        "tests/tooling/test_check_m246_b015_semantic_invariants_for_optimization_legality_advanced_core_workpack_shard_1_contract.py",
    ),
    SnippetCheck("M246-B016-DOC-EXP-07", "scripts/run_m246_b015_lane_b_readiness.py"),
    SnippetCheck(
        "M246-B016-DOC-EXP-08",
        "scripts/check_m246_b016_semantic_invariants_for_optimization_legality_advanced_edge_compatibility_workpack_shard_1_contract.py",
    ),
    SnippetCheck(
        "M246-B016-DOC-EXP-09",
        "tests/tooling/test_check_m246_b016_semantic_invariants_for_optimization_legality_advanced_edge_compatibility_workpack_shard_1_contract.py",
    ),
    SnippetCheck("M246-B016-DOC-EXP-10", "scripts/run_m246_b016_lane_b_readiness.py"),
    SnippetCheck(
        "M246-B016-DOC-EXP-11",
        "advanced edge compatibility workpack (shard 1) dependency anchors remain explicit, deterministic, and traceable",
    ),
    SnippetCheck(
        "M246-B016-DOC-EXP-12",
        "tmp/reports/m246/M246-B016/semantic_invariants_optimization_legality_advanced_edge_compatibility_workpack_shard_1_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-B016-DOC-PKT-01",
        "# M246-B016 Semantic Invariants for Optimization Legality Advanced Edge Compatibility Workpack (Shard 1) Packet",
    ),
    SnippetCheck("M246-B016-DOC-PKT-02", "Packet: `M246-B016`"),
    SnippetCheck("M246-B016-DOC-PKT-03", "Issue: `#5075`"),
    SnippetCheck("M246-B016-DOC-PKT-04", "Dependencies: `M246-B015`"),
    SnippetCheck("M246-B016-DOC-PKT-05", "Theme: `advanced edge compatibility workpack (shard 1)`"),
    SnippetCheck(
        "M246-B016-DOC-PKT-06",
        "scripts/check_m246_b016_semantic_invariants_for_optimization_legality_advanced_edge_compatibility_workpack_shard_1_contract.py",
    ),
    SnippetCheck(
        "M246-B016-DOC-PKT-07",
        "tests/tooling/test_check_m246_b016_semantic_invariants_for_optimization_legality_advanced_edge_compatibility_workpack_shard_1_contract.py",
    ),
    SnippetCheck("M246-B016-DOC-PKT-08", "scripts/run_m246_b016_lane_b_readiness.py"),
    SnippetCheck(
        "M246-B016-DOC-PKT-09",
        "scripts/check_m246_b015_semantic_invariants_for_optimization_legality_advanced_core_workpack_shard_1_contract.py",
    ),
    SnippetCheck(
        "M246-B016-DOC-PKT-10",
        "tests/tooling/test_check_m246_b015_semantic_invariants_for_optimization_legality_advanced_core_workpack_shard_1_contract.py",
    ),
    SnippetCheck("M246-B016-DOC-PKT-11", "scripts/run_m246_b015_lane_b_readiness.py"),
    SnippetCheck(
        "M246-B016-DOC-PKT-12",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M246-B016-DOC-PKT-13",
        "tmp/reports/m246/M246-B016/semantic_invariants_optimization_legality_advanced_edge_compatibility_workpack_shard_1_summary.json",
    ),
)

B015_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-B016-B015-DOC-01",
        "Contract ID: `objc3c-semantic-invariants-optimization-legality-advanced-core-workpack-shard-1/m246-b015-v1`",
    ),
    SnippetCheck(
        "M246-B016-B015-DOC-02",
        "Issue `#5074` defines canonical lane-B advanced core workpack (shard 1) scope.",
    ),
    SnippetCheck("M246-B016-B015-DOC-03", "Dependencies: `M246-B014`"),
    SnippetCheck(
        "M246-B016-B015-DOC-04",
        "scripts/check_m246_b015_semantic_invariants_for_optimization_legality_advanced_core_workpack_shard_1_contract.py",
    ),
)

B015_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-B016-B015-PKT-01", "Packet: `M246-B015`"),
    SnippetCheck("M246-B016-B015-PKT-02", "Issue: `#5074`"),
    SnippetCheck("M246-B016-B015-PKT-03", "Dependencies: `M246-B014`"),
    SnippetCheck("M246-B016-B015-PKT-04", "Theme: `advanced core workpack (shard 1)`"),
)

READINESS_RUNNER_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-B016-RUN-01", '"""Run M246-B016 lane-B readiness checks without deep npm nesting."""'),
    SnippetCheck("M246-B016-RUN-02", "scripts/run_m246_b015_lane_b_readiness.py"),
    SnippetCheck(
        "M246-B016-RUN-03",
        "scripts/check_m246_b016_semantic_invariants_for_optimization_legality_advanced_edge_compatibility_workpack_shard_1_contract.py",
    ),
    SnippetCheck(
        "M246-B016-RUN-04",
        "tests/tooling/test_check_m246_b016_semantic_invariants_for_optimization_legality_advanced_edge_compatibility_workpack_shard_1_contract.py",
    ),
    SnippetCheck("M246-B016-RUN-05", "[ok] M246-B016 lane-B readiness chain completed"),
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
    parser.add_argument("--b015-expectations-doc", type=Path, default=DEFAULT_B015_EXPECTATIONS_DOC)
    parser.add_argument("--b015-packet-doc", type=Path, default=DEFAULT_B015_PACKET_DOC)
    parser.add_argument("--b015-checker", type=Path, default=DEFAULT_B015_CHECKER)
    parser.add_argument("--b015-test", type=Path, default=DEFAULT_B015_TEST)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


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
                display_path(path),
                exists_check_id,
                f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings
    if not path.is_file():
        findings.append(
            Finding(
                display_path(path),
                exists_check_id,
                f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, findings

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    display_path(path),
                    snippet.check_id,
                    f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M246-B016-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-B016-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.b015_expectations_doc, "M246-B016-B015-DOC-EXISTS", B015_EXPECTATIONS_SNIPPETS),
        (args.b015_packet_doc, "M246-B016-B015-PKT-EXISTS", B015_PACKET_SNIPPETS),
        (args.readiness_runner, "M246-B016-RUN-EXISTS", READINESS_RUNNER_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.b015_checker, "M246-B016-DEP-B015-ARG-01"),
        (args.b015_test, "M246-B016-DEP-B015-ARG-02"),
    ):
        checks_total += 1
        if not path.exists():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            failures.append(
                Finding(
                    display_path(path),
                    check_id,
                    f"required dependency path is not a file: {display_path(path)}",
                )
            )

    failures = sorted(failures, key=lambda failure: (failure.artifact, failure.check_id, failure.detail))
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













