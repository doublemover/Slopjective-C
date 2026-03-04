#!/usr/bin/env python3
"""Fail-closed checker for M247-E015 performance SLO advanced core workpack shard 1."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-e015-performance-slo-gate-reporting-advanced-core-workpack-shard-1-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_e015_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e015_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_e015_lane_e_readiness.py"

DEFAULT_E014_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_e014_expectations.md"
)
DEFAULT_E014_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_E014_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_E014_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_E014_RUNNER = ROOT / "scripts" / "run_m247_e014_lane_e_readiness.py"

DEFAULT_A015_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_a015_expectations.md"
)
DEFAULT_A015_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_a015_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_packet.md"
)
DEFAULT_A015_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_a015_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_contract.py"
)
DEFAULT_A015_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_a015_frontend_profiling_and_hot_path_decomposition_advanced_core_workpack_shard_1_contract.py"
)
DEFAULT_A015_RUNNER = ROOT / "scripts" / "run_m247_a015_lane_a_readiness.py"

DEFAULT_B017_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_b017_expectations.md"
)
DEFAULT_B017_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_B017_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_B017_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_b017_semantic_hot_path_analysis_and_budgeting_release_candidate_and_replay_dry_run_contract.py"
)
DEFAULT_B017_RUNNER = ROOT / "scripts" / "run_m247_b017_lane_b_readiness.py"

DEFAULT_C016_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_c_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_c016_expectations.md"
)
DEFAULT_C016_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_c016_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_packet.md"
)
DEFAULT_C016_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_c016_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_contract.py"
)
DEFAULT_C016_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_c016_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_contract.py"
)
DEFAULT_C016_RUNNER = ROOT / "scripts" / "run_m247_c016_lane_c_readiness.py"

DEFAULT_D012_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_runtime_link_build_throughput_optimization_cross_lane_integration_sync_d012_expectations.md"
)
DEFAULT_D012_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_d012_runtime_link_build_throughput_optimization_cross_lane_integration_sync_packet.md"
)
DEFAULT_D012_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_d012_runtime_link_build_throughput_optimization_cross_lane_integration_sync_contract.py"
)
DEFAULT_D012_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_d012_runtime_link_build_throughput_optimization_cross_lane_integration_sync_contract.py"
)
DEFAULT_D012_RUNNER = ROOT / "scripts" / "run_m247_d012_lane_d_readiness.py"

DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-E015/performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract_summary.json"
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
        "M247-E015-DOC-EXP-01",
        "# M247 Lane E Performance SLO Gate and Reporting Advanced Core Workpack (Shard 1) Expectations (E015)",
    ),
    SnippetCheck(
        "M247-E015-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-advanced-core-workpack-shard-1-contract/m247-e015-v1`",
    ),
    SnippetCheck(
        "M247-E015-DOC-EXP-03",
        "Dependencies: `M247-E014`, `M247-A015`, `M247-B017`, `M247-C016`, `M247-D012`",
    ),
    SnippetCheck(
        "M247-E015-DOC-EXP-04",
        "Issue `#6786` defines canonical lane-E advanced core workpack (shard 1) scope.",
    ),
    SnippetCheck("M247-E015-DOC-EXP-05", "`check:objc3c:m247-e014-lane-e-readiness` (`--if-present`)"),
    SnippetCheck("M247-E015-DOC-EXP-06", "`check:objc3c:m247-a015-lane-a-readiness` (`--if-present`)"),
    SnippetCheck("M247-E015-DOC-EXP-07", "`check:objc3c:m247-b017-lane-b-readiness` (`--if-present`)"),
    SnippetCheck("M247-E015-DOC-EXP-08", "`check:objc3c:m247-c016-lane-c-readiness` (`--if-present`)"),
    SnippetCheck("M247-E015-DOC-EXP-09", "`check:objc3c:m247-d012-lane-d-readiness` (`--if-present`)"),
    SnippetCheck(
        "M247-E015-DOC-EXP-10",
        "`docs/contracts/m247_lane_e_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_e014_expectations.md`",
    ),
    SnippetCheck(
        "M247-E015-DOC-EXP-11",
        "`scripts/check_m247_e015_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract.py`",
    ),
    SnippetCheck(
        "M247-E015-DOC-EXP-12",
        "`tests/tooling/test_check_m247_e015_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract.py`",
    ),
    SnippetCheck(
        "M247-E015-DOC-EXP-13",
        "`tmp/reports/m247/M247-E015/performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E015-DOC-PKT-01",
        "# M247-E015 Performance SLO Gate and Reporting Advanced Core Workpack (Shard 1) Packet",
    ),
    SnippetCheck("M247-E015-DOC-PKT-02", "Packet: `M247-E015`"),
    SnippetCheck("M247-E015-DOC-PKT-03", "Issue: `#6786`"),
    SnippetCheck(
        "M247-E015-DOC-PKT-04",
        "Dependencies: `M247-E014`, `M247-A015`, `M247-B017`, `M247-C016`, `M247-D012`",
    ),
    SnippetCheck(
        "M247-E015-DOC-PKT-05",
        "scripts/check_m247_e015_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract.py",
    ),
    SnippetCheck(
        "M247-E015-DOC-PKT-06",
        "scripts/run_m247_e015_lane_e_readiness.py",
    ),
    SnippetCheck(
        "M247-E015-DOC-PKT-07",
        "docs/contracts/m247_lane_e_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_e014_expectations.md",
    ),
    SnippetCheck(
        "M247-E015-DOC-PKT-08",
        "scripts/check_m247_e014_performance_slo_gate_and_reporting_release_candidate_and_replay_dry_run_contract.py",
    ),
)

E014_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E015-E014-DOC-01",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-release-candidate-and-replay-dry-run-contract/m247-e014-v1`",
    ),
    SnippetCheck(
        "M247-E015-E014-DOC-02",
        "Issue `#6785` defines canonical lane-E release-candidate and replay dry-run",
    ),
)

E014_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E015-E014-PKT-01", "Packet: `M247-E014`"),
    SnippetCheck("M247-E015-E014-PKT-02", "Issue: `#6785`"),
)

A015_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E015-A015-DOC-01",
        "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-advanced-core-workpack-shard-1/m247-a015-v1`",
    ),
    SnippetCheck(
        "M247-E015-A015-DOC-02",
        "Issue `#6722` defines canonical lane-A advanced core workpack (shard 1) scope.",
    ),
)

A015_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E015-A015-PKT-01", "Packet: `M247-A015`"),
    SnippetCheck("M247-E015-A015-PKT-02", "Issue: `#6722`"),
)

B017_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E015-B017-DOC-01",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-release-candidate-and-replay-dry-run/m247-b017-v1`",
    ),
    SnippetCheck(
        "M247-E015-B017-DOC-02",
        "Issue `#6740` defines canonical lane-B release-candidate and replay dry-run scope.",
    ),
)

B017_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E015-B017-PKT-01", "Packet: `M247-B017`"),
    SnippetCheck("M247-E015-B017-PKT-02", "Issue: `#6740`"),
)

C016_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E015-C016-DOC-01",
        "Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-controls-advanced-edge-compatibility-workpack-shard-1/m247-c016-v1`",
    ),
    SnippetCheck(
        "M247-E015-C016-DOC-02",
        "Issue `#6757` defines canonical lane-C advanced edge compatibility workpack (shard 1) scope.",
    ),
)

C016_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E015-C016-PKT-01", "Packet: `M247-C016`"),
    SnippetCheck("M247-E015-C016-PKT-02", "Issue: `#6757`"),
)

D012_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E015-D012-DOC-01",
        "Contract ID: `objc3c-runtime-link-build-throughput-optimization-cross-lane-integration-sync/m247-d012-v1`",
    ),
    SnippetCheck(
        "M247-E015-D012-DOC-02",
        "Issue `#6770` defines canonical lane-D cross-lane integration sync scope.",
    ),
)

D012_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E015-D012-PKT-01", "Packet: `M247-D012`"),
    SnippetCheck("M247-E015-D012-PKT-02", "Issue: `#6770`"),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E015-RUN-01",
        '"""Run M247-E015 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M247-E015-RUN-02", 'BASELINE_DEPENDENCIES = ("M247-E014",)'),
    SnippetCheck(
        "M247-E015-RUN-03",
        'PENDING_SEEDED_DEPENDENCY_TOKENS = ("M247-A015", "M247-B017", "M247-C016", "M247-D012")',
    ),
    SnippetCheck("M247-E015-RUN-04", "check:objc3c:m247-e014-lane-e-readiness"),
    SnippetCheck("M247-E015-RUN-05", "check:objc3c:m247-a015-lane-a-readiness"),
    SnippetCheck("M247-E015-RUN-06", "check:objc3c:m247-b017-lane-b-readiness"),
    SnippetCheck("M247-E015-RUN-07", "check:objc3c:m247-c016-lane-c-readiness"),
    SnippetCheck("M247-E015-RUN-08", "check:objc3c:m247-d012-lane-d-readiness"),
    SnippetCheck(
        "M247-E015-RUN-09",
        "scripts/check_m247_e015_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract.py",
    ),
    SnippetCheck(
        "M247-E015-RUN-10",
        "tests/tooling/test_check_m247_e015_performance_slo_gate_and_reporting_advanced_core_workpack_shard_1_contract.py",
    ),
    SnippetCheck("M247-E015-RUN-11", "[ok] M247-E015 lane-E readiness chain completed"),
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

    parser.add_argument("--e014-expectations-doc", type=Path, default=DEFAULT_E014_EXPECTATIONS_DOC)
    parser.add_argument("--e014-packet-doc", type=Path, default=DEFAULT_E014_PACKET_DOC)
    parser.add_argument("--e014-checker", type=Path, default=DEFAULT_E014_CHECKER)
    parser.add_argument("--e014-test", type=Path, default=DEFAULT_E014_TEST)
    parser.add_argument("--e014-runner", type=Path, default=DEFAULT_E014_RUNNER)

    parser.add_argument("--a015-expectations-doc", type=Path, default=DEFAULT_A015_EXPECTATIONS_DOC)
    parser.add_argument("--a015-packet-doc", type=Path, default=DEFAULT_A015_PACKET_DOC)
    parser.add_argument("--a015-checker", type=Path, default=DEFAULT_A015_CHECKER)
    parser.add_argument("--a015-test", type=Path, default=DEFAULT_A015_TEST)
    parser.add_argument("--a015-runner", type=Path, default=DEFAULT_A015_RUNNER)

    parser.add_argument("--b017-expectations-doc", type=Path, default=DEFAULT_B017_EXPECTATIONS_DOC)
    parser.add_argument("--b017-packet-doc", type=Path, default=DEFAULT_B017_PACKET_DOC)
    parser.add_argument("--b017-checker", type=Path, default=DEFAULT_B017_CHECKER)
    parser.add_argument("--b017-test", type=Path, default=DEFAULT_B017_TEST)
    parser.add_argument("--b017-runner", type=Path, default=DEFAULT_B017_RUNNER)

    parser.add_argument("--c016-expectations-doc", type=Path, default=DEFAULT_C016_EXPECTATIONS_DOC)
    parser.add_argument("--c016-packet-doc", type=Path, default=DEFAULT_C016_PACKET_DOC)
    parser.add_argument("--c016-checker", type=Path, default=DEFAULT_C016_CHECKER)
    parser.add_argument("--c016-test", type=Path, default=DEFAULT_C016_TEST)
    parser.add_argument("--c016-runner", type=Path, default=DEFAULT_C016_RUNNER)

    parser.add_argument("--d012-expectations-doc", type=Path, default=DEFAULT_D012_EXPECTATIONS_DOC)
    parser.add_argument("--d012-packet-doc", type=Path, default=DEFAULT_D012_PACKET_DOC)
    parser.add_argument("--d012-checker", type=Path, default=DEFAULT_D012_CHECKER)
    parser.add_argument("--d012-test", type=Path, default=DEFAULT_D012_TEST)
    parser.add_argument("--d012-runner", type=Path, default=DEFAULT_D012_RUNNER)

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
        (args.expectations_doc, "M247-E015-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-E015-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.e014_expectations_doc, "M247-E015-E014-DOC-EXISTS", E014_EXPECTATIONS_SNIPPETS),
        (args.e014_packet_doc, "M247-E015-E014-PKT-EXISTS", E014_PACKET_SNIPPETS),
        (args.a015_expectations_doc, "M247-E015-A015-DOC-EXISTS", A015_EXPECTATIONS_SNIPPETS),
        (args.a015_packet_doc, "M247-E015-A015-PKT-EXISTS", A015_PACKET_SNIPPETS),
        (args.b017_expectations_doc, "M247-E015-B017-DOC-EXISTS", B017_EXPECTATIONS_SNIPPETS),
        (args.b017_packet_doc, "M247-E015-B017-PKT-EXISTS", B017_PACKET_SNIPPETS),
        (args.c016_expectations_doc, "M247-E015-C016-DOC-EXISTS", C016_EXPECTATIONS_SNIPPETS),
        (args.c016_packet_doc, "M247-E015-C016-PKT-EXISTS", C016_PACKET_SNIPPETS),
        (args.d012_expectations_doc, "M247-E015-D012-DOC-EXISTS", D012_EXPECTATIONS_SNIPPETS),
        (args.d012_packet_doc, "M247-E015-D012-PKT-EXISTS", D012_PACKET_SNIPPETS),
        (args.readiness_script, "M247-E015-RUN-EXISTS", READINESS_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.e014_checker, "M247-E015-DEP-E014-ARG-01"),
        (args.e014_test, "M247-E015-DEP-E014-ARG-02"),
        (args.e014_runner, "M247-E015-DEP-E014-ARG-03"),
        (args.a015_checker, "M247-E015-DEP-A015-ARG-01"),
        (args.a015_test, "M247-E015-DEP-A015-ARG-02"),
        (args.a015_runner, "M247-E015-DEP-A015-ARG-03"),
        (args.b017_checker, "M247-E015-DEP-B017-ARG-01"),
        (args.b017_test, "M247-E015-DEP-B017-ARG-02"),
        (args.b017_runner, "M247-E015-DEP-B017-ARG-03"),
        (args.c016_checker, "M247-E015-DEP-C016-ARG-01"),
        (args.c016_test, "M247-E015-DEP-C016-ARG-02"),
        (args.c016_runner, "M247-E015-DEP-C016-ARG-03"),
        (args.d012_checker, "M247-E015-DEP-D012-ARG-01"),
        (args.d012_test, "M247-E015-DEP-D012-ARG-02"),
        (args.d012_runner, "M247-E015-DEP-D012-ARG-03"),
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
