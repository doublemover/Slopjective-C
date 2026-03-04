#!/usr/bin/env python3
"""Fail-closed checker for M246-E013 optimization gate/perf evidence docs and operator runbook synchronization."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-e013-optimization-gate-perf-evidence-docs-and-operator-runbook-synchronization-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_e013_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_e013_lane_e_readiness.py"

DEFAULT_E012_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_cross_lane_integration_sync_e012_expectations.md"
)
DEFAULT_E012_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_e012_optimization_gate_and_perf_evidence_cross_lane_integration_sync_packet.md"
)
DEFAULT_E012_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_e012_optimization_gate_and_perf_evidence_cross_lane_integration_sync_contract.py"
)
DEFAULT_E012_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_e012_optimization_gate_and_perf_evidence_cross_lane_integration_sync_contract.py"
)
DEFAULT_E012_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_e012_lane_e_readiness.py"

DEFAULT_A010_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_frontend_optimization_hint_capture_conformance_corpus_expansion_a010_expectations.md"
)
DEFAULT_A010_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_a010_frontend_optimization_hint_capture_conformance_corpus_expansion_packet.md"
)
DEFAULT_A010_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_a010_frontend_optimization_hint_capture_conformance_corpus_expansion_contract.py"
)
DEFAULT_A010_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_a010_frontend_optimization_hint_capture_conformance_corpus_expansion_contract.py"
)
DEFAULT_A010_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_a010_lane_a_readiness.py"

DEFAULT_D010_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_toolchain_integration_and_optimization_controls_conformance_corpus_expansion_d010_expectations.md"
)
DEFAULT_D010_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_d010_toolchain_integration_and_optimization_controls_conformance_corpus_expansion_packet.md"
)
DEFAULT_D010_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_d010_toolchain_integration_and_optimization_controls_conformance_corpus_expansion_contract.py"
)
DEFAULT_D010_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_d010_toolchain_integration_and_optimization_controls_conformance_corpus_expansion_contract.py"
)
DEFAULT_D010_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_d010_lane_d_readiness.py"

DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-E013/optimization_gate_perf_evidence_docs_and_operator_runbook_synchronization_summary.json"
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
        "M246-E013-DEP-E012-01",
        Path("docs/contracts/m246_optimization_gate_and_perf_evidence_cross_lane_integration_sync_e012_expectations.md"),
    ),
    AssetCheck(
        "M246-E013-DEP-E012-02",
        Path("spec/planning/compiler/m246/m246_e012_optimization_gate_and_perf_evidence_cross_lane_integration_sync_packet.md"),
    ),
    AssetCheck(
        "M246-E013-DEP-E012-03",
        Path("scripts/check_m246_e012_optimization_gate_and_perf_evidence_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M246-E013-DEP-E012-04",
        Path("tests/tooling/test_check_m246_e012_optimization_gate_and_perf_evidence_cross_lane_integration_sync_contract.py"),
    ),
    AssetCheck(
        "M246-E013-DEP-E012-05",
        Path("scripts/run_m246_e012_lane_e_readiness.py"),
    ),
    AssetCheck(
        "M246-E013-DEP-A010-01",
        Path("docs/contracts/m246_frontend_optimization_hint_capture_conformance_corpus_expansion_a010_expectations.md"),
    ),
    AssetCheck(
        "M246-E013-DEP-A010-02",
        Path("spec/planning/compiler/m246/m246_a010_frontend_optimization_hint_capture_conformance_corpus_expansion_packet.md"),
    ),
    AssetCheck(
        "M246-E013-DEP-A010-03",
        Path("scripts/check_m246_a010_frontend_optimization_hint_capture_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M246-E013-DEP-A010-04",
        Path("tests/tooling/test_check_m246_a010_frontend_optimization_hint_capture_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M246-E013-DEP-A010-05",
        Path("scripts/run_m246_a010_lane_a_readiness.py"),
    ),
    AssetCheck(
        "M246-E013-DEP-D010-01",
        Path("docs/contracts/m246_toolchain_integration_and_optimization_controls_conformance_corpus_expansion_d010_expectations.md"),
    ),
    AssetCheck(
        "M246-E013-DEP-D010-02",
        Path("spec/planning/compiler/m246/m246_d010_toolchain_integration_and_optimization_controls_conformance_corpus_expansion_packet.md"),
    ),
    AssetCheck(
        "M246-E013-DEP-D010-03",
        Path("scripts/check_m246_d010_toolchain_integration_and_optimization_controls_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M246-E013-DEP-D010-04",
        Path("tests/tooling/test_check_m246_d010_toolchain_integration_and_optimization_controls_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M246-E013-DEP-D010-05",
        Path("scripts/run_m246_d010_lane_d_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E013-DOC-EXP-01",
        "# M246 Optimization Gate and Perf Evidence Docs and Operator Runbook Synchronization Expectations (E013)",
    ),
    SnippetCheck(
        "M246-E013-DOC-EXP-02",
        "Contract ID: `objc3c-optimization-gate-perf-evidence-docs-and-operator-runbook-synchronization/m246-e013-v1`",
    ),
    SnippetCheck("M246-E013-DOC-EXP-03", "- Issue: `#6704`"),
    SnippetCheck(
        "M246-E013-DOC-EXP-04",
        "- Dependencies: `M246-E012`, `M246-A010`, `M246-B014`, `M246-C024`, `M246-D010`",
    ),
    SnippetCheck(
        "M246-E013-DOC-EXP-05",
        "| `M246-B014` | Dependency token `M246-B014` is mandatory as pending seeded lane-B docs and operator runbook synchronization assets. |",
    ),
    SnippetCheck(
        "M246-E013-DOC-EXP-06",
        "| `M246-C024` | Dependency token `M246-C024` is mandatory as pending seeded lane-C docs and operator runbook synchronization assets. |",
    ),
    SnippetCheck(
        "M246-E013-DOC-EXP-07",
        "| `M246-D010` | Real dependency anchor `M246-D010` is mandatory for lane-D readiness chaining and must remain explicit in packet/readiness command wiring. |",
    ),
    SnippetCheck(
        "M246-E013-DOC-EXP-08",
        "`scripts/check_m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck(
        "M246-E013-DOC-EXP-09",
        "`tests/tooling/test_check_m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_contract.py`",
    ),
    SnippetCheck(
        "M246-E013-DOC-EXP-10",
        "`scripts/run_m246_e013_lane_e_readiness.py`",
    ),
    SnippetCheck(
        "M246-E013-DOC-EXP-11",
        "`npm run --if-present check:objc3c:m246-b014-lane-b-readiness`",
    ),
    SnippetCheck(
        "M246-E013-DOC-EXP-12",
        "`npm run --if-present check:objc3c:m246-c024-lane-c-readiness`",
    ),
    SnippetCheck(
        "M246-E013-DOC-EXP-13",
        "`python scripts/run_m246_d010_lane_d_readiness.py`",
    ),
    SnippetCheck(
        "M246-E013-DOC-EXP-14",
        "`tmp/reports/m246/M246-E013/optimization_gate_perf_evidence_docs_and_operator_runbook_synchronization_summary.json`",
    ),
    SnippetCheck(
        "M246-E013-DOC-EXP-15",
        "- Predecessor anchor: `M246-E012` cross-lane integration sync continuity is the mandatory baseline for E013.",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E013-DOC-PKT-01",
        "# M246-E013 Optimization Gate and Perf Evidence Docs and Operator Runbook Synchronization Packet",
    ),
    SnippetCheck("M246-E013-DOC-PKT-02", "Packet: `M246-E013`"),
    SnippetCheck("M246-E013-DOC-PKT-03", "Issue: `#6704`"),
    SnippetCheck(
        "M246-E013-DOC-PKT-04",
        "Dependencies: `M246-E012`, `M246-A010`, `M246-B014`, `M246-C024`, `M246-D010`",
    ),
    SnippetCheck("M246-E013-DOC-PKT-05", "Theme: docs and operator runbook synchronization"),
    SnippetCheck("M246-E013-DOC-PKT-06", "Pending seeded dependency tokens:"),
    SnippetCheck("M246-E013-DOC-PKT-07", "- `M246-B014`"),
    SnippetCheck("M246-E013-DOC-PKT-08", "- `M246-C024`"),
    SnippetCheck("M246-E013-DOC-PKT-09", "Real readiness dependency anchor:"),
    SnippetCheck("M246-E013-DOC-PKT-10", "- `M246-D010`"),
    SnippetCheck(
        "M246-E013-DOC-PKT-11",
        "python scripts/run_m246_e012_lane_e_readiness.py",
    ),
    SnippetCheck(
        "M246-E013-DOC-PKT-12",
        "python scripts/run_m246_a010_lane_a_readiness.py",
    ),
    SnippetCheck(
        "M246-E013-DOC-PKT-13",
        "npm run --if-present check:objc3c:m246-b014-lane-b-readiness",
    ),
    SnippetCheck(
        "M246-E013-DOC-PKT-14",
        "npm run --if-present check:objc3c:m246-c024-lane-c-readiness",
    ),
    SnippetCheck(
        "M246-E013-DOC-PKT-15",
        "python scripts/run_m246_d010_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M246-E013-DOC-PKT-16",
        "python scripts/check_m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck(
        "M246-E013-DOC-PKT-17",
        "python -m pytest tests/tooling/test_check_m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_contract.py -q",
    ),
    SnippetCheck(
        "M246-E013-DOC-PKT-18",
        "tmp/reports/m246/M246-E013/optimization_gate_perf_evidence_docs_and_operator_runbook_synchronization_summary.json",
    ),
    SnippetCheck("M246-E013-DOC-PKT-19", "Predecessor: `M246-E012`"),
    SnippetCheck(
        "M246-E013-DOC-PKT-20",
        "`M246-E012` contract/packet/checker/test/readiness assets are mandatory inheritance anchors for E013 fail-closed gating.",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E013-RUN-01",
        "\"\"\"Run M246-E013 lane-E readiness checks without deep npm nesting.\"\"\"",
    ),
    SnippetCheck(
        "M246-E013-RUN-02",
        "BASELINE_DEPENDENCIES = (\"M246-E012\", \"M246-A010\", \"M246-D010\")",
    ),
    SnippetCheck(
        "M246-E013-RUN-03",
        "PENDING_SEEDED_DEPENDENCY_TOKENS = (\"M246-B014\", \"M246-C024\")",
    ),
    SnippetCheck(
        "M246-E013-RUN-04",
        "scripts/run_m246_e012_lane_e_readiness.py",
    ),
    SnippetCheck(
        "M246-E013-RUN-05",
        "scripts/run_m246_a010_lane_a_readiness.py",
    ),
    SnippetCheck("M246-E013-RUN-06", "check:objc3c:m246-b014-lane-b-readiness"),
    SnippetCheck("M246-E013-RUN-07", "check:objc3c:m246-c024-lane-c-readiness"),
    SnippetCheck("M246-E013-RUN-08", "scripts/run_m246_d010_lane_d_readiness.py"),
    SnippetCheck(
        "M246-E013-RUN-09",
        "scripts/check_m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck(
        "M246-E013-RUN-10",
        "tests/tooling/test_check_m246_e013_optimization_gate_and_perf_evidence_docs_and_operator_runbook_synchronization_contract.py",
    ),
    SnippetCheck("M246-E013-RUN-11", "[ok] M246-E013 lane-E readiness chain completed"),
)

E012_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E013-E012-DOC-01",
        "# M246 Optimization Gate and Perf Evidence Cross-Lane Integration Sync Expectations (E012)",
    ),
    SnippetCheck(
        "M246-E013-E012-DOC-02",
        "Contract ID: `objc3c-optimization-gate-perf-evidence-cross-lane-integration-sync/m246-e012-v1`",
    ),
)

E012_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E013-E012-PKT-01", "Packet: `M246-E012`"),
    SnippetCheck("M246-E013-E012-PKT-02", "Issue: `#6703`"),
    SnippetCheck(
        "M246-E013-E012-PKT-03",
        "Dependencies: `M246-E011`, `M246-A008`, `M246-B012`, `M246-C020`, `M246-D008`",
    ),
)

A010_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E013-A010-DOC-01",
        "# M246 Frontend Optimization Hint Capture Conformance Corpus Expansion Expectations (A010)",
    ),
    SnippetCheck(
        "M246-E013-A010-DOC-02",
        "Contract ID: `objc3c-frontend-optimization-hint-capture-conformance-corpus-expansion/m246-a010-v1`",
    ),
)

A010_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E013-A010-PKT-01", "Packet: `M246-A010`"),
    SnippetCheck("M246-E013-A010-PKT-02", "Issue: `#5057`"),
    SnippetCheck("M246-E013-A010-PKT-03", "Dependencies: `M246-A009`"),
)

D010_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E013-D010-DOC-01",
        "# M246 Toolchain Integration and Optimization Controls Conformance Corpus Expansion Expectations (D010)",
    ),
    SnippetCheck(
        "M246-E013-D010-DOC-02",
        "Contract ID: `objc3c-toolchain-integration-optimization-controls-conformance-corpus-expansion/m246-d010-v1`",
    ),
)

D010_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E013-D010-PKT-01", "Packet: `M246-D010`"),
    SnippetCheck("M246-E013-D010-PKT-02", "Issue: `#6689`"),
    SnippetCheck("M246-E013-D010-PKT-03", "Dependencies: `M246-D009`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E013-ARCH-01",
        "M246 lane-E E006 optimization gate and perf evidence edge-case expansion and robustness anchors",
    ),
    SnippetCheck(
        "M246-E013-ARCH-02",
        "`M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E013-SPC-01",
        "optimization gate and perf evidence edge-case expansion and robustness wiring shall preserve",
    ),
    SnippetCheck(
        "M246-E013-SPC-02",
        "`M246-E005`, `M246-A005`, `M246-B006`,",
    ),
    SnippetCheck(
        "M246-E013-SPC-03",
        "`M246-C011`, and `M246-D005`) and fail closed on robustness handoff drift.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E013-META-01",
        "deterministic lane-E optimization gate and perf evidence edge-case expansion and robustness dependency anchors for",
    ),
    SnippetCheck(
        "M246-E013-META-02",
        "`M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005` so gate",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E013-PKG-01",
        '"compile:objc3c": ',
    ),
    SnippetCheck(
        "M246-E013-PKG-02",
        '"proof:objc3c": ',
    ),
    SnippetCheck(
        "M246-E013-PKG-03",
        '"test:objc3c:execution-replay-proof": ',
    ),
    SnippetCheck(
        "M246-E013-PKG-04",
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
    parser.add_argument("--e012-expectations-doc", type=Path, default=DEFAULT_E012_EXPECTATIONS_DOC)
    parser.add_argument("--e012-packet-doc", type=Path, default=DEFAULT_E012_PACKET_DOC)
    parser.add_argument("--e012-checker", type=Path, default=DEFAULT_E012_CHECKER)
    parser.add_argument("--e012-test", type=Path, default=DEFAULT_E012_TEST)
    parser.add_argument("--e012-readiness-script", type=Path, default=DEFAULT_E012_READINESS_SCRIPT)
    parser.add_argument("--a010-expectations-doc", type=Path, default=DEFAULT_A010_EXPECTATIONS_DOC)
    parser.add_argument("--a010-packet-doc", type=Path, default=DEFAULT_A010_PACKET_DOC)
    parser.add_argument("--a010-checker", type=Path, default=DEFAULT_A010_CHECKER)
    parser.add_argument("--a010-test", type=Path, default=DEFAULT_A010_TEST)
    parser.add_argument("--a010-readiness-script", type=Path, default=DEFAULT_A010_READINESS_SCRIPT)
    parser.add_argument("--d010-expectations-doc", type=Path, default=DEFAULT_D010_EXPECTATIONS_DOC)
    parser.add_argument("--d010-packet-doc", type=Path, default=DEFAULT_D010_PACKET_DOC)
    parser.add_argument("--d010-checker", type=Path, default=DEFAULT_D010_CHECKER)
    parser.add_argument("--d010-test", type=Path, default=DEFAULT_D010_TEST)
    parser.add_argument("--d010-readiness-script", type=Path, default=DEFAULT_D010_READINESS_SCRIPT)
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
        (args.expectations_doc, "M246-E013-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-E013-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.readiness_script, "M246-E013-RUN-EXISTS", READINESS_SNIPPETS),
        (args.e012_expectations_doc, "M246-E013-E012-DOC-EXISTS", E012_EXPECTATIONS_SNIPPETS),
        (args.e012_packet_doc, "M246-E013-E012-PKT-EXISTS", E012_PACKET_SNIPPETS),
        (args.a010_expectations_doc, "M246-E013-A010-DOC-EXISTS", A010_EXPECTATIONS_SNIPPETS),
        (args.a010_packet_doc, "M246-E013-A010-PKT-EXISTS", A010_PACKET_SNIPPETS),
        (args.d010_expectations_doc, "M246-E013-D010-DOC-EXISTS", D010_EXPECTATIONS_SNIPPETS),
        (args.d010_packet_doc, "M246-E013-D010-PKT-EXISTS", D010_PACKET_SNIPPETS),
        (args.architecture_doc, "M246-E013-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M246-E013-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M246-E013-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M246-E013-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, finding_batch = check_text_artifact(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(finding_batch)

    for path, check_id in (
        (args.e012_checker, "M246-E013-DEP-E012-ARG-01"),
        (args.e012_test, "M246-E013-DEP-E012-ARG-02"),
        (args.e012_readiness_script, "M246-E013-DEP-E012-ARG-03"),
        (args.a010_checker, "M246-E013-DEP-A010-ARG-01"),
        (args.a010_test, "M246-E013-DEP-A010-ARG-02"),
        (args.a010_readiness_script, "M246-E013-DEP-A010-ARG-03"),
        (args.d010_checker, "M246-E013-DEP-D010-ARG-01"),
        (args.d010_test, "M246-E013-DEP-D010-ARG-02"),
        (args.d010_readiness_script, "M246-E013-DEP-D010-ARG-03"),
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






