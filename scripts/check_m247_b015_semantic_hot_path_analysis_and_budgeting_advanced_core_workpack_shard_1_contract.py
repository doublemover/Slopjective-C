#!/usr/bin/env python3
"""Fail-closed checker for M247-B015 semantic hot-path advanced core workpack shard 1."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-b015-semantic-hot-path-analysis-and-budgeting-advanced-core-workpack-shard-1-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_b015_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_b015_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_packet.md"
)
DEFAULT_B014_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_b014_expectations.md"
)
DEFAULT_B014_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_b014_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_B014_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_b014_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_B014_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_b014_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_B014_READINESS_RUNNER = ROOT / "scripts" / "run_m247_b014_lane_b_readiness.py"
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m247_b015_lane_b_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-B015/semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_contract_summary.json"
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
        "M247-B015-DOC-EXP-01",
        "# M247 Semantic Hot-Path Analysis and Budgeting Advanced Core Workpack (Shard 1) Expectations (B015)",
    ),
    SnippetCheck(
        "M247-B015-DOC-EXP-02",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-advanced-core-workpack-shard-1/m247-b015-v1`",
    ),
    SnippetCheck("M247-B015-DOC-EXP-03", "Dependencies: `M247-B014`"),
    SnippetCheck(
        "M247-B015-DOC-EXP-04",
        "Issue `#6738` defines canonical lane-B advanced core workpack (shard 1) scope.",
    ),
    SnippetCheck(
        "M247-B015-DOC-EXP-05",
        "`scripts/check_m247_b014_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M247-B015-DOC-EXP-06",
        "advanced-core-workpack command sequencing and advanced-core-workpack-shard-1-key",
    ),
    SnippetCheck(
        "M247-B015-DOC-EXP-07",
        "`check:objc3c:m247-b015-semantic-hot-path-analysis-and-budgeting-advanced-core-workpack-shard-1-contract`",
    ),
    SnippetCheck(
        "M247-B015-DOC-EXP-08",
        "`test:tooling:m247-b015-semantic-hot-path-analysis-and-budgeting-advanced-core-workpack-shard-1-contract`",
    ),
    SnippetCheck("M247-B015-DOC-EXP-09", "`check:objc3c:m247-b015-lane-b-readiness`"),
    SnippetCheck(
        "M247-B015-DOC-EXP-10",
        "`python scripts/run_m247_b014_lane_b_readiness.py`",
    ),
    SnippetCheck(
        "M247-B015-DOC-EXP-11",
        "`python scripts/check_m247_b015_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_contract.py`",
    ),
    SnippetCheck(
        "M247-B015-DOC-EXP-12",
        "`tmp/reports/m247/M247-B015/semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_contract_summary.json`",
    ),
    SnippetCheck(
        "M247-B015-DOC-EXP-13",
        "optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck("M247-B015-DOC-EXP-14", "`test:objc3c:sema-pass-manager-diagnostics-bus`"),
    SnippetCheck("M247-B015-DOC-EXP-15", "`compile:objc3c`"),
    SnippetCheck("M247-B015-DOC-EXP-16", "`test:objc3c:perf-budget`"),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B015-DOC-PKT-01",
        "# M247-B015 Semantic Hot-Path Analysis and Budgeting Advanced Core Workpack (Shard 1) Packet",
    ),
    SnippetCheck("M247-B015-DOC-PKT-02", "Packet: `M247-B015`"),
    SnippetCheck("M247-B015-DOC-PKT-03", "Issue: `#6738`"),
    SnippetCheck("M247-B015-DOC-PKT-04", "Dependencies: `M247-B014`"),
    SnippetCheck(
        "M247-B015-DOC-PKT-05",
        "`scripts/check_m247_b015_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_contract.py`",
    ),
    SnippetCheck(
        "M247-B015-DOC-PKT-06",
        "`tests/tooling/test_check_m247_b015_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_contract.py`",
    ),
    SnippetCheck("M247-B015-DOC-PKT-07", "`scripts/run_m247_b015_lane_b_readiness.py`"),
    SnippetCheck(
        "M247-B015-DOC-PKT-08",
        "`scripts/check_m247_b014_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M247-B015-DOC-PKT-09",
        "`check:objc3c:m247-b015-semantic-hot-path-analysis-and-budgeting-advanced-core-workpack-shard-1-contract`",
    ),
    SnippetCheck(
        "M247-B015-DOC-PKT-10",
        "`test:tooling:m247-b015-semantic-hot-path-analysis-and-budgeting-advanced-core-workpack-shard-1-contract`",
    ),
    SnippetCheck("M247-B015-DOC-PKT-11", "`check:objc3c:m247-b015-lane-b-readiness`"),
    SnippetCheck("M247-B015-DOC-PKT-12", "`check:objc3c:m247-b014-lane-b-readiness`"),
    SnippetCheck("M247-B015-DOC-PKT-13", "mandatory scope inputs."),
    SnippetCheck(
        "M247-B015-DOC-PKT-14",
        "`tmp/reports/m247/M247-B015/semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_contract_summary.json`",
    ),
)

B014_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B015-B014-DOC-01",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-release-candidate-and-replay-dry-run/m247-b014-v1`",
    ),
    SnippetCheck("M247-B015-B014-DOC-02", "Dependencies: `M247-B013`"),
    SnippetCheck(
        "M247-B015-B014-DOC-03",
        "Issue `#6737` defines canonical lane-B release-candidate and replay dry-run scope.",
    ),
    SnippetCheck(
        "M247-B015-B014-DOC-04",
        "scripts/check_m247_b014_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py",
    ),
)

B014_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-B015-B014-PKT-01", "Packet: `M247-B014`"),
    SnippetCheck("M247-B015-B014-PKT-02", "Issue: `#6737`"),
    SnippetCheck("M247-B015-B014-PKT-03", "Dependencies: `M247-B013`"),
    SnippetCheck(
        "M247-B015-B014-PKT-04",
        "scripts/check_m247_b014_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck("M247-B015-B014-PKT-05", "check:objc3c:m247-b014-lane-b-readiness"),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-B015-RUN-01", '"""Run M247-B015 lane-B readiness checks without deep npm nesting."""'),
    SnippetCheck("M247-B015-RUN-02", "scripts/run_m247_b014_lane_b_readiness.py"),
    SnippetCheck(
        "M247-B015-RUN-03",
        "scripts/check_m247_b015_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_contract.py",
    ),
    SnippetCheck(
        "M247-B015-RUN-04",
        "tests/tooling/test_check_m247_b015_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_contract.py",
    ),
    SnippetCheck("M247-B015-RUN-05", "BASELINE_DEPENDENCIES = (\"M247-B014\",)"),
    SnippetCheck("M247-B015-RUN-06", "[ok] M247-B015 lane-B readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B015-ARCH-01",
        "M247 lane-B B015 semantic hot-path analysis/budgeting advanced core workpack (shard 1) anchors",
    ),
    SnippetCheck(
        "M247-B015-ARCH-02",
        "`M247-B014` dependency continuity and advanced core",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B015-SPC-01",
        "semantic hot-path analysis/budgeting advanced core workpack (shard 1) wiring",
    ),
    SnippetCheck(
        "M247-B015-SPC-02",
        "advanced-core-workpack command sequencing continuity, advanced-core-workpack-shard-1-key continuity, or contract-gating evidence commands drift.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B015-META-01",
        "deterministic lane-B semantic hot-path analysis/budgeting advanced core workpack (shard 1) metadata anchors for `M247-B015`",
    ),
    SnippetCheck(
        "M247-B015-META-02",
        "with explicit `M247-B014` dependency continuity so lane-B advanced core workpack (shard 1) contract-gating evidence remains fail-closed.",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-B015-PKG-01",
        '"check:objc3c:m247-b015-semantic-hot-path-analysis-and-budgeting-advanced-core-workpack-shard-1-contract": '
        '"python scripts/check_m247_b015_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_contract.py"',
    ),
    SnippetCheck(
        "M247-B015-PKG-02",
        '"test:tooling:m247-b015-semantic-hot-path-analysis-and-budgeting-advanced-core-workpack-shard-1-contract": '
        '"python -m pytest tests/tooling/test_check_m247_b015_semantic_hot_path_analysis_and_budgeting_advanced_core_workpack_shard_1_contract.py -q"',
    ),
    SnippetCheck(
        "M247-B015-PKG-03",
        '"check:objc3c:m247-b015-lane-b-readiness": "python scripts/run_m247_b015_lane_b_readiness.py"',
    ),
    SnippetCheck("M247-B015-PKG-04", '"test:objc3c:sema-pass-manager-diagnostics-bus": '),
    SnippetCheck("M247-B015-PKG-05", '"compile:objc3c": '),
    SnippetCheck("M247-B015-PKG-06", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--b014-expectations-doc", type=Path, default=DEFAULT_B014_EXPECTATIONS_DOC)
    parser.add_argument("--b014-packet-doc", type=Path, default=DEFAULT_B014_PACKET_DOC)
    parser.add_argument("--b014-checker", type=Path, default=DEFAULT_B014_CHECKER)
    parser.add_argument("--b014-test", type=Path, default=DEFAULT_B014_TEST)
    parser.add_argument("--b014-readiness-runner", type=Path, default=DEFAULT_B014_READINESS_RUNNER)
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
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


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M247-B015-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-B015-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.b014_expectations_doc, "M247-B015-B014-DOC-EXISTS", B014_EXPECTATIONS_SNIPPETS),
        (args.b014_packet_doc, "M247-B015-B014-PKT-EXISTS", B014_PACKET_SNIPPETS),
        (args.readiness_runner, "M247-B015-RUN-EXISTS", READINESS_SNIPPETS),
        (args.architecture_doc, "M247-B015-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M247-B015-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M247-B015-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M247-B015-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.b014_checker, "M247-B015-DEP-B014-ARG-01"),
        (args.b014_test, "M247-B015-DEP-B014-ARG-02"),
        (args.b014_readiness_runner, "M247-B015-DEP-B014-ARG-03"),
    ):
        checks_total += 1
        if not path.exists():
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=check_id,
                    detail=f"required dependency path is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=check_id,
                    detail=f"required dependency path is not a file: {display_path(path)}",
                )
            )

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

    summary_path = args.summary_out
    if not summary_path.is_absolute():
        summary_path = ROOT / summary_path
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
