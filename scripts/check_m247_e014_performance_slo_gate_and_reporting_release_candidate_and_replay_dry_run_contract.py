#!/usr/bin/env python3
"""Fail-closed checker for M247-E014 performance SLO release-candidate/replay dry-run."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-e014-performance-slo-gate-reporting-release-candidate-and-replay-dry-run-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_e014_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_e014_lane_e_readiness.py"

DEFAULT_A014_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_a014_expectations.md"
)
DEFAULT_A014_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_a014_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_A014_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_a014_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_A014_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_a014_frontend_profiling_and_hot_path_decomposition_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_A014_RUNNER = ROOT / "scripts" / "run_m247_a014_lane_a_readiness.py"

DEFAULT_B016_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_semantic_hot_path_analysis_and_budgeting_advanced_edge_compatibility_workpack_shard_1_b016_expectations.md"
)
DEFAULT_B016_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_b016_semantic_hot_path_analysis_and_budgeting_advanced_edge_compatibility_workpack_shard_1_packet.md"
)
DEFAULT_B016_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_b016_semantic_hot_path_analysis_and_budgeting_advanced_edge_compatibility_workpack_shard_1_contract.py"
)
DEFAULT_B016_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_b016_semantic_hot_path_analysis_and_budgeting_advanced_edge_compatibility_workpack_shard_1_contract.py"
)
DEFAULT_B016_RUNNER = ROOT / "scripts" / "run_m247_b016_lane_b_readiness.py"

DEFAULT_C015_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_c_lowering_codegen_cost_profiling_and_controls_advanced_core_workpack_shard_1_c015_expectations.md"
)
DEFAULT_C015_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_c015_lowering_codegen_cost_profiling_and_controls_advanced_core_workpack_shard_1_packet.md"
)
DEFAULT_C015_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_c015_lowering_codegen_cost_profiling_and_controls_advanced_core_workpack_shard_1_contract.py"
)
DEFAULT_C015_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_c015_lowering_codegen_cost_profiling_and_controls_advanced_core_workpack_shard_1_contract.py"
)
DEFAULT_C015_RUNNER = ROOT / "scripts" / "run_m247_c015_lane_c_readiness.py"

DEFAULT_D011_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_d011_expectations.md"
)
DEFAULT_D011_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_packet.md"
)
DEFAULT_D011_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py"
)
DEFAULT_D011_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_d011_runtime_link_build_throughput_optimization_performance_and_quality_guardrails_contract.py"
)
DEFAULT_D011_RUNNER = ROOT / "scripts" / "run_m247_d011_lane_d_readiness.py"

DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-E014/performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract_summary.json"
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
        "M247-E014-DOC-EXP-01",
        "# M247 Lane E Performance SLO Gate and Reporting Release-Candidate and Replay Dry-Run Expectations (E014)",
    ),
    SnippetCheck(
        "M247-E014-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-release-candidate-and-replay-dry-run-contract/m247-e014-v1`",
    ),
    SnippetCheck(
        "M247-E014-DOC-EXP-03",
        "Dependencies: `M247-E013`, `M247-A014`, `M247-B016`, `M247-C015`, `M247-D011`",
    ),
    SnippetCheck(
        "M247-E014-DOC-EXP-04",
        "Issue `#6785` defines canonical lane-E release-candidate and replay dry-run",
    ),
    SnippetCheck(
        "M247-E014-DOC-EXP-05",
        "Execute release-candidate and replay dry-run for Performance profiling and",
    ),
    SnippetCheck(
        "M247-E014-DOC-EXP-06",
        "Code/spec anchors and milestone optimization improvements",
    ),
    SnippetCheck("M247-E014-DOC-EXP-07", "`check:objc3c:m247-e013-lane-e-readiness` (`--if-present`)"),
    SnippetCheck("M247-E014-DOC-EXP-08", "`check:objc3c:m247-a014-lane-a-readiness` (`--if-present`)"),
    SnippetCheck("M247-E014-DOC-EXP-09", "`check:objc3c:m247-b016-lane-b-readiness` (`--if-present`)"),
    SnippetCheck("M247-E014-DOC-EXP-10", "`check:objc3c:m247-c015-lane-c-readiness` (`--if-present`)"),
    SnippetCheck("M247-E014-DOC-EXP-11", "`check:objc3c:m247-d011-lane-d-readiness` (`--if-present`)"),
    SnippetCheck(
        "M247-E014-DOC-EXP-12",
        "`tmp/reports/m247/M247-E014/performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E014-DOC-PKT-01",
        "# M247-E014 Performance SLO Gate and Reporting Release-Candidate and Replay Dry-Run Packet",
    ),
    SnippetCheck("M247-E014-DOC-PKT-02", "Packet: `M247-E014`"),
    SnippetCheck("M247-E014-DOC-PKT-03", "Issue: `#6785`"),
    SnippetCheck(
        "M247-E014-DOC-PKT-04",
        "Dependencies: `M247-E013`, `M247-A014`, `M247-B016`, `M247-C015`, `M247-D011`",
    ),
    SnippetCheck("M247-E014-DOC-PKT-05", "Predecessor: `M247-E013`"),
    SnippetCheck("M247-E014-DOC-PKT-06", "Theme: release-candidate and replay dry-run"),
    SnippetCheck(
        "M247-E014-DOC-PKT-07",
        "scripts/check_m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck("M247-E014-DOC-PKT-08", "scripts/run_m247_e014_lane_e_readiness.py"),
    SnippetCheck("M247-E014-DOC-PKT-09", "check:objc3c:m247-e013-lane-e-readiness"),
    SnippetCheck(
        "M247-E014-DOC-PKT-10",
        "tmp/reports/m247/M247-E014/performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract_summary.json",
    ),
)

A014_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E014-A014-DOC-01",
        "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-release-candidate-and-replay-dry-run/m247-a014-v1`",
    ),
    SnippetCheck(
        "M247-E014-A014-DOC-02",
        "Issue `#6721` defines canonical lane-A release-candidate and replay dry-run scope.",
    ),
)

A014_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E014-A014-PKT-01", "Packet: `M247-A014`"),
    SnippetCheck("M247-E014-A014-PKT-02", "Issue: `#6721`"),
    SnippetCheck("M247-E014-A014-PKT-03", "Dependencies: `M247-A013`"),
)

B016_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E014-B016-DOC-01",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-advanced-edge-compatibility-workpack-shard-1/m247-b016-v1`",
    ),
    SnippetCheck(
        "M247-E014-B016-DOC-02",
        "Issue `#6739` defines canonical lane-B advanced edge compatibility workpack (shard 1) scope.",
    ),
)

B016_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E014-B016-PKT-01", "Packet: `M247-B016`"),
    SnippetCheck("M247-E014-B016-PKT-02", "Issue: `#6739`"),
)

C015_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E014-C015-DOC-01",
        "Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-controls-advanced-core-workpack-shard-1/m247-c015-v1`",
    ),
    SnippetCheck(
        "M247-E014-C015-DOC-02",
        "Issue `#6756` defines canonical lane-C advanced core workpack (shard 1) scope.",
    ),
)

C015_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E014-C015-PKT-01", "Packet: `M247-C015`"),
    SnippetCheck("M247-E014-C015-PKT-02", "Issue: `#6756`"),
)

D011_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E014-D011-DOC-01",
        "Contract ID: `objc3c-runtime-link-build-throughput-optimization-performance-and-quality-guardrails/m247-d011-v1`",
    ),
    SnippetCheck(
        "M247-E014-D011-DOC-02",
        "Issue `#6769` defines canonical lane-D performance and quality guardrails scope.",
    ),
)

D011_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E014-D011-PKT-01", "Packet: `M247-D011`"),
    SnippetCheck("M247-E014-D011-PKT-02", "Issue: `#6769`"),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E014-RUN-01",
        '"""Run M247-E014 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M247-E014-RUN-02", 'BASELINE_DEPENDENCIES = ("M247-E013",)'),
    SnippetCheck(
        "M247-E014-RUN-03",
        'PENDING_SEEDED_DEPENDENCY_TOKENS = ("M247-A014", "M247-B016", "M247-C015", "M247-D011")',
    ),
    SnippetCheck("M247-E014-RUN-04", "check:objc3c:m247-e013-lane-e-readiness"),
    SnippetCheck("M247-E014-RUN-05", "check:objc3c:m247-a014-lane-a-readiness"),
    SnippetCheck("M247-E014-RUN-06", "check:objc3c:m247-b016-lane-b-readiness"),
    SnippetCheck("M247-E014-RUN-07", "check:objc3c:m247-c015-lane-c-readiness"),
    SnippetCheck("M247-E014-RUN-08", "check:objc3c:m247-d011-lane-d-readiness"),
    SnippetCheck(
        "M247-E014-RUN-09",
        "scripts/check_m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M247-E014-RUN-10",
        "tests/tooling/test_check_m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck("M247-E014-RUN-11", "[ok] M247-E014 lane-E readiness chain completed"),
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
    parser.add_argument("--a014-expectations-doc", type=Path, default=DEFAULT_A014_EXPECTATIONS_DOC)
    parser.add_argument("--a014-packet-doc", type=Path, default=DEFAULT_A014_PACKET_DOC)
    parser.add_argument("--b016-expectations-doc", type=Path, default=DEFAULT_B016_EXPECTATIONS_DOC)
    parser.add_argument("--b016-packet-doc", type=Path, default=DEFAULT_B016_PACKET_DOC)
    parser.add_argument("--c015-expectations-doc", type=Path, default=DEFAULT_C015_EXPECTATIONS_DOC)
    parser.add_argument("--c015-packet-doc", type=Path, default=DEFAULT_C015_PACKET_DOC)
    parser.add_argument("--d011-expectations-doc", type=Path, default=DEFAULT_D011_EXPECTATIONS_DOC)
    parser.add_argument("--d011-packet-doc", type=Path, default=DEFAULT_D011_PACKET_DOC)
    parser.add_argument("--a014-checker", type=Path, default=DEFAULT_A014_CHECKER)
    parser.add_argument("--a014-test", type=Path, default=DEFAULT_A014_TEST)
    parser.add_argument("--a014-runner", type=Path, default=DEFAULT_A014_RUNNER)
    parser.add_argument("--b016-checker", type=Path, default=DEFAULT_B016_CHECKER)
    parser.add_argument("--b016-test", type=Path, default=DEFAULT_B016_TEST)
    parser.add_argument("--b016-runner", type=Path, default=DEFAULT_B016_RUNNER)
    parser.add_argument("--c015-checker", type=Path, default=DEFAULT_C015_CHECKER)
    parser.add_argument("--c015-test", type=Path, default=DEFAULT_C015_TEST)
    parser.add_argument("--c015-runner", type=Path, default=DEFAULT_C015_RUNNER)
    parser.add_argument("--d011-checker", type=Path, default=DEFAULT_D011_CHECKER)
    parser.add_argument("--d011-test", type=Path, default=DEFAULT_D011_TEST)
    parser.add_argument("--d011-runner", type=Path, default=DEFAULT_D011_RUNNER)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument("--emit-json", action="store_true")
    return parser.parse_args(argv)


def check_doc_contract(*, path: Path, exists_check_id: str, snippets: tuple[SnippetCheck, ...]) -> tuple[int, list[Finding]]:
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
        (args.expectations_doc, "M247-E014-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-E014-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.a014_expectations_doc, "M247-E014-A014-DOC-EXISTS", A014_EXPECTATIONS_SNIPPETS),
        (args.a014_packet_doc, "M247-E014-A014-PKT-EXISTS", A014_PACKET_SNIPPETS),
        (args.b016_expectations_doc, "M247-E014-B016-DOC-EXISTS", B016_EXPECTATIONS_SNIPPETS),
        (args.b016_packet_doc, "M247-E014-B016-PKT-EXISTS", B016_PACKET_SNIPPETS),
        (args.c015_expectations_doc, "M247-E014-C015-DOC-EXISTS", C015_EXPECTATIONS_SNIPPETS),
        (args.c015_packet_doc, "M247-E014-C015-PKT-EXISTS", C015_PACKET_SNIPPETS),
        (args.d011_expectations_doc, "M247-E014-D011-DOC-EXISTS", D011_EXPECTATIONS_SNIPPETS),
        (args.d011_packet_doc, "M247-E014-D011-PKT-EXISTS", D011_PACKET_SNIPPETS),
        (args.readiness_script, "M247-E014-RUN-EXISTS", READINESS_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.a014_checker, "M247-E014-DEP-A014-ARG-01"),
        (args.a014_test, "M247-E014-DEP-A014-ARG-02"),
        (args.a014_runner, "M247-E014-DEP-A014-ARG-03"),
        (args.b016_checker, "M247-E014-DEP-B016-ARG-01"),
        (args.b016_test, "M247-E014-DEP-B016-ARG-02"),
        (args.b016_runner, "M247-E014-DEP-B016-ARG-03"),
        (args.c015_checker, "M247-E014-DEP-C015-ARG-01"),
        (args.c015_test, "M247-E014-DEP-C015-ARG-02"),
        (args.c015_runner, "M247-E014-DEP-C015-ARG-03"),
        (args.d011_checker, "M247-E014-DEP-D011-ARG-01"),
        (args.d011_test, "M247-E014-DEP-D011-ARG-02"),
        (args.d011_runner, "M247-E014-DEP-D011-ARG-03"),
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

    if args.emit_json:
        print(canonical_json(summary_payload), end="")

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    if args.emit_json:
        return 0

    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

