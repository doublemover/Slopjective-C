#!/usr/bin/env python3
"""Fail-closed checker for M246-E014 optimization gate/perf evidence release-candidate and replay dry-run."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-e014-optimization-gate-perf-evidence-release-candidate-and-replay-dry-run-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_release_candidate_and_replay_dry_run_e014_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_e014_optimization_gate_and_perf_evidence_release_candidate_and_replay_dry_run_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_e014_lane_e_readiness.py"

DEFAULT_E013_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_e013_expectations.md"
)
DEFAULT_E013_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_packet.md"
)
DEFAULT_E013_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_contract.py"
)
DEFAULT_E013_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_contract.py"
)
DEFAULT_E013_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_e013_lane_e_readiness.py"

DEFAULT_A011_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_frontend_optimization_hint_capture_performance_and_quality_guardrails_a011_expectations.md"
)
DEFAULT_A011_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_a011_frontend_optimization_hint_capture_performance_and_quality_guardrails_packet.md"
)
DEFAULT_A011_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_a011_frontend_optimization_hint_capture_performance_and_quality_guardrails_contract.py"
)
DEFAULT_A011_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_a011_frontend_optimization_hint_capture_performance_and_quality_guardrails_contract.py"
)
DEFAULT_A011_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_a011_lane_a_readiness.py"

DEFAULT_D011_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_toolchain_integration_and_optimization_controls_performance_and_quality_guardrails_d011_expectations.md"
)
DEFAULT_D011_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_d011_toolchain_integration_and_optimization_controls_performance_and_quality_guardrails_packet.md"
)
DEFAULT_D011_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_d011_toolchain_integration_and_optimization_controls_performance_and_quality_guardrails_contract.py"
)
DEFAULT_D011_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_d011_toolchain_integration_and_optimization_controls_performance_and_quality_guardrails_contract.py"
)
DEFAULT_D011_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_d011_lane_d_readiness.py"

DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-E014/optimization_gate_perf_evidence_release_candidate_and_replay_dry_run_summary.json"
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
        "M246-E014-DEP-E013-01",
        Path("docs/contracts/m246_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_e013_expectations.md"),
    ),
    AssetCheck(
        "M246-E014-DEP-E013-02",
        Path("spec/planning/compiler/m246/m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_packet.md"),
    ),
    AssetCheck(
        "M246-E014-DEP-E013-03",
        Path("scripts/check_m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M246-E014-DEP-E013-04",
        Path("tests/tooling/test_check_m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_contract.py"),
    ),
    AssetCheck(
        "M246-E014-DEP-E013-05",
        Path("scripts/run_m246_e013_lane_e_readiness.py"),
    ),
    AssetCheck(
        "M246-E014-DEP-A011-01",
        Path("docs/contracts/m246_frontend_optimization_hint_capture_performance_and_quality_guardrails_a011_expectations.md"),
    ),
    AssetCheck(
        "M246-E014-DEP-A011-02",
        Path("spec/planning/compiler/m246/m246_a011_frontend_optimization_hint_capture_performance_and_quality_guardrails_packet.md"),
    ),
    AssetCheck(
        "M246-E014-DEP-A011-03",
        Path("scripts/check_m246_a011_frontend_optimization_hint_capture_performance_and_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M246-E014-DEP-A011-04",
        Path("tests/tooling/test_check_m246_a011_frontend_optimization_hint_capture_performance_and_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M246-E014-DEP-A011-05",
        Path("scripts/run_m246_a011_lane_a_readiness.py"),
    ),
    AssetCheck(
        "M246-E014-DEP-D011-01",
        Path("docs/contracts/m246_toolchain_integration_and_optimization_controls_performance_and_quality_guardrails_d011_expectations.md"),
    ),
    AssetCheck(
        "M246-E014-DEP-D011-02",
        Path("spec/planning/compiler/m246/m246_d011_toolchain_integration_and_optimization_controls_performance_and_quality_guardrails_packet.md"),
    ),
    AssetCheck(
        "M246-E014-DEP-D011-03",
        Path("scripts/check_m246_d011_toolchain_integration_and_optimization_controls_performance_and_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M246-E014-DEP-D011-04",
        Path("tests/tooling/test_check_m246_d011_toolchain_integration_and_optimization_controls_performance_and_quality_guardrails_contract.py"),
    ),
    AssetCheck(
        "M246-E014-DEP-D011-05",
        Path("scripts/run_m246_d011_lane_d_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E014-DOC-EXP-01",
        "# M246 Optimization Gate and Perf Evidence Release-Candidate and Replay Dry-Run Expectations (E014)",
    ),
    SnippetCheck(
        "M246-E014-DOC-EXP-02",
        "Contract ID: `objc3c-optimization-gate-perf-evidence-release-candidate-and-replay-dry-run/m246-e014-v1`",
    ),
    SnippetCheck("M246-E014-DOC-EXP-03", "- Issue: `#6705`"),
    SnippetCheck(
        "M246-E014-DOC-EXP-04",
        "- Dependencies: `M246-E013`, `M246-A011`, `M246-B015`, `M246-C025`, `M246-D011`",
    ),
    SnippetCheck(
        "M246-E014-DOC-EXP-05",
        "| `M246-B015` | Dependency token `M246-B015` is mandatory as pending seeded lane-B release-candidate and replay dry-run assets. |",
    ),
    SnippetCheck(
        "M246-E014-DOC-EXP-06",
        "| `M246-C025` | Dependency token `M246-C025` is mandatory as pending seeded lane-C release-candidate and replay dry-run assets. |",
    ),
    SnippetCheck(
        "M246-E014-DOC-EXP-07",
        "| `M246-D011` | Real dependency anchor `M246-D011` is mandatory for lane-D readiness chaining and must remain explicit in packet/readiness command wiring. |",
    ),
    SnippetCheck(
        "M246-E014-DOC-EXP-08",
        "`scripts/check_m246_e014_optimization_gate_and_perf_evidence_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M246-E014-DOC-EXP-09",
        "`tests/tooling/test_check_m246_e014_optimization_gate_and_perf_evidence_release_candidate_and_replay_dry_run_contract.py`",
    ),
    SnippetCheck(
        "M246-E014-DOC-EXP-10",
        "`scripts/run_m246_e014_lane_e_readiness.py`",
    ),
    SnippetCheck(
        "M246-E014-DOC-EXP-11",
        "`npm run --if-present check:objc3c:m246-b015-lane-b-readiness`",
    ),
    SnippetCheck(
        "M246-E014-DOC-EXP-12",
        "`npm run --if-present check:objc3c:m246-c025-lane-c-readiness`",
    ),
    SnippetCheck(
        "M246-E014-DOC-EXP-13",
        "`python scripts/run_m246_d011_lane_d_readiness.py`",
    ),
    SnippetCheck(
        "M246-E014-DOC-EXP-14",
        "`tmp/reports/m246/M246-E014/optimization_gate_perf_evidence_release_candidate_and_replay_dry_run_summary.json`",
    ),
    SnippetCheck(
        "M246-E014-DOC-EXP-15",
        "- Predecessor anchor: `M246-E013` docs and operator runbook synchronization continuity is the mandatory baseline for E014.",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E014-DOC-PKT-01",
        "# M246-E014 Optimization Gate and Perf Evidence Release-Candidate and Replay Dry-Run Packet",
    ),
    SnippetCheck("M246-E014-DOC-PKT-02", "Packet: `M246-E014`"),
    SnippetCheck("M246-E014-DOC-PKT-03", "Issue: `#6705`"),
    SnippetCheck(
        "M246-E014-DOC-PKT-04",
        "Dependencies: `M246-E013`, `M246-A011`, `M246-B015`, `M246-C025`, `M246-D011`",
    ),
    SnippetCheck("M246-E014-DOC-PKT-05", "Theme: release-candidate and replay dry-run"),
    SnippetCheck("M246-E014-DOC-PKT-06", "Pending seeded dependency tokens:"),
    SnippetCheck("M246-E014-DOC-PKT-07", "- `M246-B015`"),
    SnippetCheck("M246-E014-DOC-PKT-08", "- `M246-C025`"),
    SnippetCheck("M246-E014-DOC-PKT-09", "Real readiness dependency anchor:"),
    SnippetCheck("M246-E014-DOC-PKT-10", "- `M246-D011`"),
    SnippetCheck(
        "M246-E014-DOC-PKT-11",
        "python scripts/run_m246_e013_lane_e_readiness.py",
    ),
    SnippetCheck(
        "M246-E014-DOC-PKT-12",
        "python scripts/run_m246_a011_lane_a_readiness.py",
    ),
    SnippetCheck(
        "M246-E014-DOC-PKT-13",
        "npm run --if-present check:objc3c:m246-b015-lane-b-readiness",
    ),
    SnippetCheck(
        "M246-E014-DOC-PKT-14",
        "npm run --if-present check:objc3c:m246-c025-lane-c-readiness",
    ),
    SnippetCheck(
        "M246-E014-DOC-PKT-15",
        "python scripts/run_m246_d011_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M246-E014-DOC-PKT-16",
        "python scripts/check_m246_e014_optimization_gate_and_perf_evidence_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M246-E014-DOC-PKT-17",
        "python -m pytest tests/tooling/test_check_m246_e014_optimization_gate_and_perf_evidence_release_candidate_and_replay_dry_run_contract.py -q",
    ),
    SnippetCheck(
        "M246-E014-DOC-PKT-18",
        "tmp/reports/m246/M246-E014/optimization_gate_perf_evidence_release_candidate_and_replay_dry_run_summary.json",
    ),
    SnippetCheck("M246-E014-DOC-PKT-19", "Predecessor: `M246-E013`"),
    SnippetCheck(
        "M246-E014-DOC-PKT-20",
        "`M246-E013` contract/packet/checker/test/readiness assets are mandatory inheritance anchors for E014 fail-closed gating.",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E014-RUN-01",
        "\"\"\"Run M246-E014 lane-E readiness checks without deep npm nesting.\"\"\"",
    ),
    SnippetCheck(
        "M246-E014-RUN-02",
        "BASELINE_DEPENDENCIES = (\"M246-E013\", \"M246-A011\", \"M246-D011\")",
    ),
    SnippetCheck(
        "M246-E014-RUN-03",
        "PENDING_SEEDED_DEPENDENCY_TOKENS = (\"M246-B015\", \"M246-C025\")",
    ),
    SnippetCheck(
        "M246-E014-RUN-04",
        "scripts/run_m246_e013_lane_e_readiness.py",
    ),
    SnippetCheck(
        "M246-E014-RUN-05",
        "scripts/run_m246_a011_lane_a_readiness.py",
    ),
    SnippetCheck("M246-E014-RUN-06", "check:objc3c:m246-b015-lane-b-readiness"),
    SnippetCheck("M246-E014-RUN-07", "check:objc3c:m246-c025-lane-c-readiness"),
    SnippetCheck("M246-E014-RUN-08", "scripts/run_m246_d011_lane_d_readiness.py"),
    SnippetCheck(
        "M246-E014-RUN-09",
        "scripts/check_m246_e014_optimization_gate_and_perf_evidence_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck(
        "M246-E014-RUN-10",
        "tests/tooling/test_check_m246_e014_optimization_gate_and_perf_evidence_release_candidate_and_replay_dry_run_contract.py",
    ),
    SnippetCheck("M246-E014-RUN-11", "[ok] M246-E014 lane-E readiness chain completed"),
)

E013_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E014-E013-DOC-01",
        "# M246 Optimization Gate and Perf Evidence Docs and Operator Runbook Synchronization Expectations (E013)",
    ),
    SnippetCheck(
        "M246-E014-E013-DOC-02",
        "Contract ID: `objc3c-optimization-gate-perf-evidence-docs-and-operator-runbook-synchronization/m246-e013-v1`",
    ),
)

E013_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E014-E013-PKT-01", "Packet: `M246-E013`"),
    SnippetCheck("M246-E014-E013-PKT-02", "Issue: `#6704`"),
    SnippetCheck(
        "M246-E014-E013-PKT-03",
        "Dependencies: `M246-E012`, `M246-A010`, `M246-B014`, `M246-C024`, `M246-D010`",
    ),
)

A011_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E014-A011-DOC-01",
        "# M246 Frontend Optimization Hint Capture Performance and Quality Guardrails Expectations (A011)",
    ),
    SnippetCheck(
        "M246-E014-A011-DOC-02",
        "Contract ID: `objc3c-frontend-optimization-hint-capture-performance-and-quality-guardrails/m246-a011-v1`",
    ),
)

A011_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E014-A011-PKT-01", "Packet: `M246-A011`"),
    SnippetCheck("M246-E014-A011-PKT-02", "Issue: `#5058`"),
    SnippetCheck("M246-E014-A011-PKT-03", "Dependencies: `M246-A010`"),
)

D011_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E014-D011-DOC-01",
        "# M246 Toolchain Integration and Optimization Controls Performance and Quality Guardrails Expectations (D011)",
    ),
    SnippetCheck(
        "M246-E014-D011-DOC-02",
        "Contract ID: `objc3c-toolchain-integration-optimization-controls-performance-and-quality-guardrails/m246-d011-v1`",
    ),
)

D011_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E014-D011-PKT-01", "Packet: `M246-D011`"),
    SnippetCheck("M246-E014-D011-PKT-02", "Issue: `#6690`"),
    SnippetCheck("M246-E014-D011-PKT-03", "Dependencies: `M246-D010`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E014-ARCH-01",
        "M246 lane-E E006 optimization gate and perf evidence edge-case expansion and robustness anchors",
    ),
    SnippetCheck(
        "M246-E014-ARCH-02",
        "`M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E014-SPC-01",
        "optimization gate and perf evidence edge-case expansion and robustness wiring shall preserve",
    ),
    SnippetCheck(
        "M246-E014-SPC-02",
        "`M246-E005`, `M246-A005`, `M246-B006`,",
    ),
    SnippetCheck(
        "M246-E014-SPC-03",
        "`M246-C011`, and `M246-D005`) and fail closed on robustness handoff drift.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E014-META-01",
        "deterministic lane-E optimization gate and perf evidence edge-case expansion and robustness dependency anchors for",
    ),
    SnippetCheck(
        "M246-E014-META-02",
        "`M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005` so gate",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E014-PKG-01",
        '"compile:objc3c": ',
    ),
    SnippetCheck(
        "M246-E014-PKG-02",
        '"proof:objc3c": ',
    ),
    SnippetCheck(
        "M246-E014-PKG-03",
        '"test:objc3c:execution-replay-proof": ',
    ),
    SnippetCheck(
        "M246-E014-PKG-04",
        '"test:objc3c:perf-budget": ',
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
    parser.add_argument("--readiness-script", type=Path, default=DEFAULT_READINESS_SCRIPT)
    parser.add_argument("--e013-expectations-doc", type=Path, default=DEFAULT_E013_EXPECTATIONS_DOC)
    parser.add_argument("--e013-packet-doc", type=Path, default=DEFAULT_E013_PACKET_DOC)
    parser.add_argument("--e013-checker", type=Path, default=DEFAULT_E013_CHECKER)
    parser.add_argument("--e013-test", type=Path, default=DEFAULT_E013_TEST)
    parser.add_argument("--e013-readiness-script", type=Path, default=DEFAULT_E013_READINESS_SCRIPT)
    parser.add_argument("--a011-expectations-doc", type=Path, default=DEFAULT_A011_EXPECTATIONS_DOC)
    parser.add_argument("--a011-packet-doc", type=Path, default=DEFAULT_A011_PACKET_DOC)
    parser.add_argument("--a011-checker", type=Path, default=DEFAULT_A011_CHECKER)
    parser.add_argument("--a011-test", type=Path, default=DEFAULT_A011_TEST)
    parser.add_argument("--a011-readiness-script", type=Path, default=DEFAULT_A011_READINESS_SCRIPT)
    parser.add_argument("--d011-expectations-doc", type=Path, default=DEFAULT_D011_EXPECTATIONS_DOC)
    parser.add_argument("--d011-packet-doc", type=Path, default=DEFAULT_D011_PACKET_DOC)
    parser.add_argument("--d011-checker", type=Path, default=DEFAULT_D011_CHECKER)
    parser.add_argument("--d011-test", type=Path, default=DEFAULT_D011_TEST)
    parser.add_argument("--d011-readiness-script", type=Path, default=DEFAULT_D011_READINESS_SCRIPT)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    failures: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists():
            failures.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"missing prerequisite asset: {asset.relative_path.as_posix()}",
                )
            )
            continue
        if not absolute.is_file():
            failures.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"prerequisite path is not a file: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, failures


def check_text_artifact(
    *,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
    checks_total = 1
    failures: list[Finding] = []
    if not path.exists():
        failures.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, failures
    if not path.is_file():
        failures.append(
            Finding(
                artifact=display_path(path),
                check_id=exists_check_id,
                detail=f"required path is not a file: {display_path(path)}",
            )
        )
        return checks_total, failures

    text = path.read_text(encoding="utf-8")
    for snippet in snippets:
        checks_total += 1
        if snippet.snippet not in text:
            failures.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
                )
            )
    return checks_total, failures


def check_dependency_path(path: Path, check_id: str) -> tuple[int, list[Finding]]:
    failures: list[Finding] = []
    if not path.exists():
        failures.append(
            Finding(
                artifact=display_path(path),
                check_id=check_id,
                detail=f"required dependency path is missing: {display_path(path)}",
            )
        )
    elif not path.is_file():
        failures.append(
            Finding(
                artifact=display_path(path),
                check_id=check_id,
                detail=f"required dependency path is not a file: {display_path(path)}",
            )
        )
    return 1, failures


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
        (args.expectations_doc, "M246-E014-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-E014-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.readiness_script, "M246-E014-RUN-EXISTS", READINESS_SNIPPETS),
        (args.e013_expectations_doc, "M246-E014-E013-DOC-EXISTS", E013_EXPECTATIONS_SNIPPETS),
        (args.e013_packet_doc, "M246-E014-E013-PKT-EXISTS", E013_PACKET_SNIPPETS),
        (args.a011_expectations_doc, "M246-E014-A011-DOC-EXISTS", A011_EXPECTATIONS_SNIPPETS),
        (args.a011_packet_doc, "M246-E014-A011-PKT-EXISTS", A011_PACKET_SNIPPETS),
        (args.d011_expectations_doc, "M246-E014-D011-DOC-EXISTS", D011_EXPECTATIONS_SNIPPETS),
        (args.d011_packet_doc, "M246-E014-D011-PKT-EXISTS", D011_PACKET_SNIPPETS),
        (args.architecture_doc, "M246-E014-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M246-E014-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M246-E014-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M246-E014-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, finding_batch = check_text_artifact(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(finding_batch)

    for path, check_id in (
        (args.e013_checker, "M246-E014-DEP-E013-ARG-01"),
        (args.e013_test, "M246-E014-DEP-E013-ARG-02"),
        (args.e013_readiness_script, "M246-E014-DEP-E013-ARG-03"),
        (args.a011_checker, "M246-E014-DEP-A011-ARG-01"),
        (args.a011_test, "M246-E014-DEP-A011-ARG-02"),
        (args.a011_readiness_script, "M246-E014-DEP-A011-ARG-03"),
        (args.d011_checker, "M246-E014-DEP-D011-ARG-01"),
        (args.d011_test, "M246-E014-DEP-D011-ARG-02"),
        (args.d011_readiness_script, "M246-E014-DEP-D011-ARG-03"),
    ):
        count, finding_batch = check_dependency_path(path, check_id)
        checks_total += count
        failures.extend(finding_batch)

    failures = sorted(failures, key=finding_sort_key)
    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": failure.artifact, "check_id": failure.check_id, "detail": failure.detail}
            for failure in failures
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







