#!/usr/bin/env python3
"""Fail-closed checker for M247-E012 performance SLO cross-lane integration sync."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-e012-performance-slo-gate-reporting-cross-lane-integration-sync-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_cross_lane_integration_sync_e012_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e012_performance_slo_gate_and_reporting_cross_lane_integration_sync_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_e012_lane_e_readiness.py"

DEFAULT_E011_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_performance_and_quality_guardrails_e011_expectations.md"
)
DEFAULT_E011_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e011_performance_slo_gate_and_reporting_performance_and_quality_guardrails_packet.md"
)

DEFAULT_A012_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_frontend_profiling_and_hot_path_decomposition_cross_lane_integration_sync_a012_expectations.md"
)
DEFAULT_A012_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_a012_frontend_profiling_and_hot_path_decomposition_cross_lane_integration_sync_packet.md"
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

DEFAULT_C013_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_c_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_c013_expectations.md"
)
DEFAULT_C013_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_c013_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_packet.md"
)

DEFAULT_D010_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_runtime_link_build_throughput_optimization_conformance_corpus_expansion_d010_expectations.md"
)
DEFAULT_D010_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_d010_runtime_link_build_throughput_optimization_conformance_corpus_expansion_packet.md"
)

DEFAULT_E011_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_e011_performance_slo_gate_and_reporting_performance_and_quality_guardrails_contract.py"
)
DEFAULT_E011_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_e011_performance_slo_gate_and_reporting_performance_and_quality_guardrails_contract.py"
)
DEFAULT_E011_RUNNER = ROOT / "scripts" / "run_m247_e011_lane_e_readiness.py"

DEFAULT_A012_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_a012_frontend_profiling_and_hot_path_decomposition_cross_lane_integration_sync_contract.py"
)
DEFAULT_A012_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_a012_frontend_profiling_and_hot_path_decomposition_cross_lane_integration_sync_contract.py"
)
DEFAULT_A012_RUNNER = ROOT / "scripts" / "run_m247_a012_lane_a_readiness.py"

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
DEFAULT_B014_RUNNER = ROOT / "scripts" / "run_m247_b014_lane_b_readiness.py"

DEFAULT_C013_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_c013_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_contract.py"
)
DEFAULT_C013_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_c013_lowering_codegen_cost_profiling_and_controls_docs_and_operator_runbook_synchronization_contract.py"
)
DEFAULT_C013_RUNNER = ROOT / "scripts" / "run_m247_c013_lane_c_readiness.py"

DEFAULT_D010_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_d010_runtime_link_build_throughput_optimization_conformance_corpus_expansion_contract.py"
)
DEFAULT_D010_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_d010_runtime_link_build_throughput_optimization_conformance_corpus_expansion_contract.py"
)
DEFAULT_D010_RUNNER = ROOT / "scripts" / "run_m247_d010_lane_d_readiness.py"

DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-E012/performance_slo_gate_and_reporting_cross_lane_integration_sync_contract_summary.json"
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
        "M247-E012-DOC-EXP-01",
        "# M247 Lane E Performance SLO Gate and Reporting Cross-Lane Integration Sync Expectations (E012)",
    ),
    SnippetCheck(
        "M247-E012-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-cross-lane-integration-sync-contract/m247-e012-v1`",
    ),
    SnippetCheck(
        "M247-E012-DOC-EXP-03",
        "Dependencies: `M247-E011`, `M247-A012`, `M247-B014`, `M247-C013`, `M247-D010`",
    ),
    SnippetCheck(
        "M247-E012-DOC-EXP-04",
        "Performance SLO gate and reporting. Execute cross-lane integration sync for",
    ),
    SnippetCheck(
        "M247-E012-DOC-EXP-05",
        "Issue `#6783` defines canonical lane-E cross-lane integration sync scope.",
    ),
    SnippetCheck("M247-E012-DOC-EXP-06", "`python scripts/run_m247_e011_lane_e_readiness.py`"),
    SnippetCheck("M247-E012-DOC-EXP-07", "`check:objc3c:m247-d010-lane-d-readiness` (`--if-present`)"),
    SnippetCheck(
        "M247-E012-DOC-EXP-08",
        "`tmp/reports/m247/M247-E012/performance_slo_gate_and_reporting_cross_lane_integration_sync_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E012-DOC-PKT-01",
        "# M247-E012 Performance SLO Gate and Reporting Cross-Lane Integration Sync Packet",
    ),
    SnippetCheck("M247-E012-DOC-PKT-02", "Packet: `M247-E012`"),
    SnippetCheck("M247-E012-DOC-PKT-03", "Issue: `#6783`"),
    SnippetCheck(
        "M247-E012-DOC-PKT-04",
        "Dependencies: `M247-E011`, `M247-A012`, `M247-B014`, `M247-C013`, `M247-D010`",
    ),
    SnippetCheck(
        "M247-E012-DOC-PKT-05",
        "scripts/check_m247_e012_performance_slo_gate_and_reporting_cross_lane_integration_sync_contract.py",
    ),
    SnippetCheck(
        "M247-E012-DOC-PKT-06",
        "scripts/run_m247_e012_lane_e_readiness.py",
    ),
)

E011_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E012-E011-DOC-01",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-performance-and-quality-guardrails-contract/m247-e011-v1`",
    ),
)

E011_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E012-E011-PKT-01", "Packet: `M247-E011`"),
    SnippetCheck("M247-E012-E011-PKT-02", "Issue: `#6782`"),
)

A012_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E012-A012-DOC-01",
        "Contract ID: `objc3c-frontend-profiling-hot-path-decomposition-cross-lane-integration-sync/m247-a012-v1`",
    ),
)

A012_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E012-A012-PKT-01", "Packet: `M247-A012`"),
    SnippetCheck("M247-E012-A012-PKT-02", "Issue: `#6719`"),
)

B014_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E012-B014-DOC-01",
        "Contract ID: `objc3c-semantic-hot-path-analysis-and-budgeting-release-candidate-and-replay-dry-run/m247-b014-v1`",
    ),
)

B014_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E012-B014-PKT-01", "Packet: `M247-B014`"),
    SnippetCheck("M247-E012-B014-PKT-02", "Issue: `#6737`"),
)

C013_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E012-C013-DOC-01",
        "Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-controls-docs-and-operator-runbook-synchronization/m247-c013-v1`",
    ),
)

C013_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E012-C013-PKT-01", "Packet: `M247-C013`"),
    SnippetCheck("M247-E012-C013-PKT-02", "Issue: `#6754`"),
)

D010_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E012-D010-DOC-01",
        "Contract ID: `objc3c-runtime-link-build-throughput-optimization-conformance-corpus-expansion/m247-d010-v1`",
    ),
)

D010_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E012-D010-PKT-01", "Packet: `M247-D010`"),
    SnippetCheck("M247-E012-D010-PKT-02", "Issue: `#6768`"),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E012-RUN-01",
        '"""Run M247-E012 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck("M247-E012-RUN-02", 'BASELINE_DEPENDENCIES = ("M247-E011",)'),
    SnippetCheck("M247-E012-RUN-03", 'PENDING_SEEDED_DEPENDENCY_TOKENS = ("M247-A012", "M247-B014", "M247-C013", "M247-D010")'),
    SnippetCheck("M247-E012-RUN-04", "scripts/run_m247_e011_lane_e_readiness.py"),
    SnippetCheck("M247-E012-RUN-05", "check:objc3c:m247-a012-lane-a-readiness"),
    SnippetCheck("M247-E012-RUN-06", "check:objc3c:m247-b014-lane-b-readiness"),
    SnippetCheck("M247-E012-RUN-07", "check:objc3c:m247-c013-lane-c-readiness"),
    SnippetCheck("M247-E012-RUN-08", "check:objc3c:m247-d010-lane-d-readiness"),
    SnippetCheck("M247-E012-RUN-09", "[ok] M247-E012 lane-E readiness chain completed"),
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
    parser.add_argument("--e011-expectations-doc", type=Path, default=DEFAULT_E011_EXPECTATIONS_DOC)
    parser.add_argument("--e011-packet-doc", type=Path, default=DEFAULT_E011_PACKET_DOC)
    parser.add_argument("--a012-expectations-doc", type=Path, default=DEFAULT_A012_EXPECTATIONS_DOC)
    parser.add_argument("--a012-packet-doc", type=Path, default=DEFAULT_A012_PACKET_DOC)
    parser.add_argument("--b014-expectations-doc", type=Path, default=DEFAULT_B014_EXPECTATIONS_DOC)
    parser.add_argument("--b014-packet-doc", type=Path, default=DEFAULT_B014_PACKET_DOC)
    parser.add_argument("--c013-expectations-doc", type=Path, default=DEFAULT_C013_EXPECTATIONS_DOC)
    parser.add_argument("--c013-packet-doc", type=Path, default=DEFAULT_C013_PACKET_DOC)
    parser.add_argument("--d010-expectations-doc", type=Path, default=DEFAULT_D010_EXPECTATIONS_DOC)
    parser.add_argument("--d010-packet-doc", type=Path, default=DEFAULT_D010_PACKET_DOC)
    parser.add_argument("--e011-checker", type=Path, default=DEFAULT_E011_CHECKER)
    parser.add_argument("--e011-test", type=Path, default=DEFAULT_E011_TEST)
    parser.add_argument("--e011-runner", type=Path, default=DEFAULT_E011_RUNNER)
    parser.add_argument("--a012-checker", type=Path, default=DEFAULT_A012_CHECKER)
    parser.add_argument("--a012-test", type=Path, default=DEFAULT_A012_TEST)
    parser.add_argument("--a012-runner", type=Path, default=DEFAULT_A012_RUNNER)
    parser.add_argument("--b014-checker", type=Path, default=DEFAULT_B014_CHECKER)
    parser.add_argument("--b014-test", type=Path, default=DEFAULT_B014_TEST)
    parser.add_argument("--b014-runner", type=Path, default=DEFAULT_B014_RUNNER)
    parser.add_argument("--c013-checker", type=Path, default=DEFAULT_C013_CHECKER)
    parser.add_argument("--c013-test", type=Path, default=DEFAULT_C013_TEST)
    parser.add_argument("--c013-runner", type=Path, default=DEFAULT_C013_RUNNER)
    parser.add_argument("--d010-checker", type=Path, default=DEFAULT_D010_CHECKER)
    parser.add_argument("--d010-test", type=Path, default=DEFAULT_D010_TEST)
    parser.add_argument("--d010-runner", type=Path, default=DEFAULT_D010_RUNNER)
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
        (args.expectations_doc, "M247-E012-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-E012-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.e011_expectations_doc, "M247-E012-E011-DOC-EXISTS", E011_EXPECTATIONS_SNIPPETS),
        (args.e011_packet_doc, "M247-E012-E011-PKT-EXISTS", E011_PACKET_SNIPPETS),
        (args.a012_expectations_doc, "M247-E012-A012-DOC-EXISTS", A012_EXPECTATIONS_SNIPPETS),
        (args.a012_packet_doc, "M247-E012-A012-PKT-EXISTS", A012_PACKET_SNIPPETS),
        (args.b014_expectations_doc, "M247-E012-B014-DOC-EXISTS", B014_EXPECTATIONS_SNIPPETS),
        (args.b014_packet_doc, "M247-E012-B014-PKT-EXISTS", B014_PACKET_SNIPPETS),
        (args.c013_expectations_doc, "M247-E012-C013-DOC-EXISTS", C013_EXPECTATIONS_SNIPPETS),
        (args.c013_packet_doc, "M247-E012-C013-PKT-EXISTS", C013_PACKET_SNIPPETS),
        (args.d010_expectations_doc, "M247-E012-D010-DOC-EXISTS", D010_EXPECTATIONS_SNIPPETS),
        (args.d010_packet_doc, "M247-E012-D010-PKT-EXISTS", D010_PACKET_SNIPPETS),
        (args.readiness_script, "M247-E012-RUN-EXISTS", READINESS_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.e011_checker, "M247-E012-DEP-E011-ARG-01"),
        (args.e011_test, "M247-E012-DEP-E011-ARG-02"),
        (args.e011_runner, "M247-E012-DEP-E011-ARG-03"),
        (args.a012_checker, "M247-E012-DEP-A012-ARG-01"),
        (args.a012_test, "M247-E012-DEP-A012-ARG-02"),
        (args.a012_runner, "M247-E012-DEP-A012-ARG-03"),
        (args.b014_checker, "M247-E012-DEP-B014-ARG-01"),
        (args.b014_test, "M247-E012-DEP-B014-ARG-02"),
        (args.b014_runner, "M247-E012-DEP-B014-ARG-03"),
        (args.c013_checker, "M247-E012-DEP-C013-ARG-01"),
        (args.c013_test, "M247-E012-DEP-C013-ARG-02"),
        (args.c013_runner, "M247-E012-DEP-C013-ARG-03"),
        (args.d010_checker, "M247-E012-DEP-D010-ARG-01"),
        (args.d010_test, "M247-E012-DEP-D010-ARG-02"),
        (args.d010_runner, "M247-E012-DEP-D010-ARG-03"),
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
