#!/usr/bin/env python3
"""Fail-closed checker for M246-E011 optimization gate/perf evidence performance and quality guardrails."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-e011-optimization-gate-perf-evidence-performance-and-quality-guardrails-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_performance_and_quality_guardrails_e011_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_e011_optimization_gate_and_perf_evidence_performance_and_quality_guardrails_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_e011_lane_e_readiness.py"

DEFAULT_E010_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_conformance_corpus_expansion_e010_expectations.md"
)
DEFAULT_E010_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_e010_optimization_gate_and_perf_evidence_conformance_corpus_expansion_packet.md"
)
DEFAULT_E010_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_e010_optimization_gate_and_perf_evidence_conformance_corpus_expansion_contract.py"
)
DEFAULT_E010_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_e010_optimization_gate_and_perf_evidence_conformance_corpus_expansion_contract.py"
)
DEFAULT_E010_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_e010_lane_e_readiness.py"

DEFAULT_A008_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_frontend_optimization_hint_capture_recovery_and_determinism_hardening_a008_expectations.md"
)
DEFAULT_A008_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_a008_frontend_optimization_hint_capture_recovery_and_determinism_hardening_packet.md"
)
DEFAULT_A008_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_a008_frontend_optimization_hint_capture_recovery_and_determinism_hardening_contract.py"
)
DEFAULT_A008_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_a008_frontend_optimization_hint_capture_recovery_and_determinism_hardening_contract.py"
)
DEFAULT_A008_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_a008_lane_a_readiness.py"

DEFAULT_D008_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_d008_expectations.md"
)
DEFAULT_D008_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_d008_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_packet.md"
)
DEFAULT_D008_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_d008_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_contract.py"
)
DEFAULT_D008_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_d008_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_contract.py"
)
DEFAULT_D008_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_d008_lane_d_readiness.py"

DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-E011/optimization_gate_perf_evidence_performance_and_quality_guardrails_summary.json"
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
        "M246-E011-DEP-E010-01",
        Path("docs/contracts/m246_optimization_gate_and_perf_evidence_conformance_corpus_expansion_e010_expectations.md"),
    ),
    AssetCheck(
        "M246-E011-DEP-E010-02",
        Path("spec/planning/compiler/m246/m246_e010_optimization_gate_and_perf_evidence_conformance_corpus_expansion_packet.md"),
    ),
    AssetCheck(
        "M246-E011-DEP-E010-03",
        Path("scripts/check_m246_e010_optimization_gate_and_perf_evidence_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M246-E011-DEP-E010-04",
        Path("tests/tooling/test_check_m246_e010_optimization_gate_and_perf_evidence_conformance_corpus_expansion_contract.py"),
    ),
    AssetCheck(
        "M246-E011-DEP-E010-05",
        Path("scripts/run_m246_e010_lane_e_readiness.py"),
    ),
    AssetCheck(
        "M246-E011-DEP-A008-01",
        Path("docs/contracts/m246_frontend_optimization_hint_capture_recovery_and_determinism_hardening_a008_expectations.md"),
    ),
    AssetCheck(
        "M246-E011-DEP-A008-02",
        Path("spec/planning/compiler/m246/m246_a008_frontend_optimization_hint_capture_recovery_and_determinism_hardening_packet.md"),
    ),
    AssetCheck(
        "M246-E011-DEP-A008-03",
        Path("scripts/check_m246_a008_frontend_optimization_hint_capture_recovery_and_determinism_hardening_contract.py"),
    ),
    AssetCheck(
        "M246-E011-DEP-A008-04",
        Path("tests/tooling/test_check_m246_a008_frontend_optimization_hint_capture_recovery_and_determinism_hardening_contract.py"),
    ),
    AssetCheck(
        "M246-E011-DEP-A008-05",
        Path("scripts/run_m246_a008_lane_a_readiness.py"),
    ),
    AssetCheck(
        "M246-E011-DEP-D008-01",
        Path("docs/contracts/m246_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_d008_expectations.md"),
    ),
    AssetCheck(
        "M246-E011-DEP-D008-02",
        Path("spec/planning/compiler/m246/m246_d008_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_packet.md"),
    ),
    AssetCheck(
        "M246-E011-DEP-D008-03",
        Path("scripts/check_m246_d008_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_contract.py"),
    ),
    AssetCheck(
        "M246-E011-DEP-D008-04",
        Path("tests/tooling/test_check_m246_d008_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_contract.py"),
    ),
    AssetCheck(
        "M246-E011-DEP-D008-05",
        Path("scripts/run_m246_d008_lane_d_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E011-DOC-EXP-01",
        "# M246 Optimization Gate and Perf Evidence Performance and Quality Guardrails Expectations (E011)",
    ),
    SnippetCheck(
        "M246-E011-DOC-EXP-02",
        "Contract ID: `objc3c-optimization-gate-perf-evidence-performance-and-quality-guardrails/m246-e011-v1`",
    ),
    SnippetCheck("M246-E011-DOC-EXP-03", "- Issue: `#6702`"),
    SnippetCheck(
        "M246-E011-DOC-EXP-04",
        "- Dependencies: `M246-E010`, `M246-A008`, `M246-B012`, `M246-C020`, `M246-D008`",
    ),
    SnippetCheck(
        "M246-E011-DOC-EXP-05",
        "| `M246-B012` | Dependency token `M246-B012` is mandatory as pending seeded lane-B performance and quality guardrails assets. |",
    ),
    SnippetCheck(
        "M246-E011-DOC-EXP-06",
        "| `M246-C020` | Dependency token `M246-C020` is mandatory as pending seeded lane-C performance and quality guardrails assets. |",
    ),
    SnippetCheck(
        "M246-E011-DOC-EXP-07",
        "| `M246-D008` | Real dependency anchor `M246-D008` is mandatory for lane-D readiness chaining and must remain explicit in packet/readiness command wiring. |",
    ),
    SnippetCheck(
        "M246-E011-DOC-EXP-08",
        "`scripts/check_m246_e011_optimization_gate_and_perf_evidence_performance_and_quality_guardrails_contract.py`",
    ),
    SnippetCheck(
        "M246-E011-DOC-EXP-09",
        "`tests/tooling/test_check_m246_e011_optimization_gate_and_perf_evidence_performance_and_quality_guardrails_contract.py`",
    ),
    SnippetCheck(
        "M246-E011-DOC-EXP-10",
        "`scripts/run_m246_e011_lane_e_readiness.py`",
    ),
    SnippetCheck(
        "M246-E011-DOC-EXP-11",
        "`npm run --if-present check:objc3c:m246-b012-lane-b-readiness`",
    ),
    SnippetCheck(
        "M246-E011-DOC-EXP-12",
        "`npm run --if-present check:objc3c:m246-c020-lane-c-readiness`",
    ),
    SnippetCheck(
        "M246-E011-DOC-EXP-13",
        "`python scripts/run_m246_d008_lane_d_readiness.py`",
    ),
    SnippetCheck(
        "M246-E011-DOC-EXP-14",
        "`tmp/reports/m246/M246-E011/optimization_gate_perf_evidence_performance_and_quality_guardrails_summary.json`",
    ),
    SnippetCheck(
        "M246-E011-DOC-EXP-15",
        "- Predecessor anchor: `M246-E010` conformance corpus expansion continuity is the mandatory baseline for E011.",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E011-DOC-PKT-01",
        "# M246-E011 Optimization Gate and Perf Evidence Performance and Quality Guardrails Packet",
    ),
    SnippetCheck("M246-E011-DOC-PKT-02", "Packet: `M246-E011`"),
    SnippetCheck("M246-E011-DOC-PKT-03", "Issue: `#6702`"),
    SnippetCheck(
        "M246-E011-DOC-PKT-04",
        "Dependencies: `M246-E010`, `M246-A008`, `M246-B012`, `M246-C020`, `M246-D008`",
    ),
    SnippetCheck("M246-E011-DOC-PKT-05", "Theme: performance and quality guardrails"),
    SnippetCheck("M246-E011-DOC-PKT-06", "Pending seeded dependency tokens:"),
    SnippetCheck("M246-E011-DOC-PKT-07", "- `M246-B012`"),
    SnippetCheck("M246-E011-DOC-PKT-08", "- `M246-C020`"),
    SnippetCheck("M246-E011-DOC-PKT-09", "Real readiness dependency anchor:"),
    SnippetCheck("M246-E011-DOC-PKT-10", "- `M246-D008`"),
    SnippetCheck(
        "M246-E011-DOC-PKT-11",
        "python scripts/run_m246_e010_lane_e_readiness.py",
    ),
    SnippetCheck(
        "M246-E011-DOC-PKT-12",
        "python scripts/run_m246_a008_lane_a_readiness.py",
    ),
    SnippetCheck(
        "M246-E011-DOC-PKT-13",
        "npm run --if-present check:objc3c:m246-b012-lane-b-readiness",
    ),
    SnippetCheck(
        "M246-E011-DOC-PKT-14",
        "npm run --if-present check:objc3c:m246-c020-lane-c-readiness",
    ),
    SnippetCheck(
        "M246-E011-DOC-PKT-15",
        "python scripts/run_m246_d008_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M246-E011-DOC-PKT-16",
        "python scripts/check_m246_e011_optimization_gate_and_perf_evidence_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck(
        "M246-E011-DOC-PKT-17",
        "python -m pytest tests/tooling/test_check_m246_e011_optimization_gate_and_perf_evidence_performance_and_quality_guardrails_contract.py -q",
    ),
    SnippetCheck(
        "M246-E011-DOC-PKT-18",
        "tmp/reports/m246/M246-E011/optimization_gate_perf_evidence_performance_and_quality_guardrails_summary.json",
    ),
    SnippetCheck("M246-E011-DOC-PKT-19", "Predecessor: `M246-E010`"),
    SnippetCheck(
        "M246-E011-DOC-PKT-20",
        "`M246-E010` contract/packet/checker/test/readiness assets are mandatory inheritance anchors for E011 fail-closed gating.",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E011-RUN-01",
        "\"\"\"Run M246-E011 lane-E readiness checks without deep npm nesting.\"\"\"",
    ),
    SnippetCheck(
        "M246-E011-RUN-02",
        "BASELINE_DEPENDENCIES = (\"M246-E010\", \"M246-A008\", \"M246-D008\")",
    ),
    SnippetCheck(
        "M246-E011-RUN-03",
        "PENDING_SEEDED_DEPENDENCY_TOKENS = (\"M246-B012\", \"M246-C020\")",
    ),
    SnippetCheck(
        "M246-E011-RUN-04",
        "scripts/run_m246_e010_lane_e_readiness.py",
    ),
    SnippetCheck(
        "M246-E011-RUN-05",
        "scripts/run_m246_a008_lane_a_readiness.py",
    ),
    SnippetCheck("M246-E011-RUN-06", "check:objc3c:m246-b012-lane-b-readiness"),
    SnippetCheck("M246-E011-RUN-07", "check:objc3c:m246-c020-lane-c-readiness"),
    SnippetCheck("M246-E011-RUN-08", "scripts/run_m246_d008_lane_d_readiness.py"),
    SnippetCheck(
        "M246-E011-RUN-09",
        "scripts/check_m246_e011_optimization_gate_and_perf_evidence_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck(
        "M246-E011-RUN-10",
        "tests/tooling/test_check_m246_e011_optimization_gate_and_perf_evidence_performance_and_quality_guardrails_contract.py",
    ),
    SnippetCheck("M246-E011-RUN-11", "[ok] M246-E011 lane-E readiness chain completed"),
)

E010_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E011-E010-DOC-01",
        "# M246 Optimization Gate and Perf Evidence Conformance Corpus Expansion Expectations (E010)",
    ),
    SnippetCheck(
        "M246-E011-E010-DOC-02",
        "Contract ID: `objc3c-optimization-gate-perf-evidence-conformance-corpus-expansion/m246-e010-v1`",
    ),
)

E010_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E011-E010-PKT-01", "Packet: `M246-E010`"),
    SnippetCheck("M246-E011-E010-PKT-02", "Issue: `#6701`"),
    SnippetCheck(
        "M246-E011-E010-PKT-03",
        "Dependencies: `M246-E009`, `M246-A008`, `M246-B011`, `M246-C018`, `M246-D008`",
    ),
)

A008_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E011-A008-DOC-01",
        "# M246 Frontend Optimization Hint Capture Recovery and Determinism Hardening Expectations (A008)",
    ),
    SnippetCheck(
        "M246-E011-A008-DOC-02",
        "Contract ID: `objc3c-frontend-optimization-hint-capture-recovery-and-determinism-hardening/m246-a008-v1`",
    ),
)

A008_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E011-A008-PKT-01", "Packet: `M246-A008`"),
    SnippetCheck("M246-E011-A008-PKT-02", "Issue: `#5055`"),
    SnippetCheck("M246-E011-A008-PKT-03", "Dependencies: `M246-A007`"),
)

D008_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E011-D008-DOC-01",
        "# M246 Toolchain Integration and Optimization Controls Recovery and Determinism Hardening Expectations (D008)",
    ),
    SnippetCheck(
        "M246-E011-D008-DOC-02",
        "Contract ID: `objc3c-toolchain-integration-optimization-controls-recovery-and-determinism-hardening/m246-d008-v1`",
    ),
)

D008_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E011-D008-PKT-01", "Packet: `M246-D008`"),
    SnippetCheck("M246-E011-D008-PKT-02", "Issue: `#6687`"),
    SnippetCheck("M246-E011-D008-PKT-03", "Dependencies: `M246-D007`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E011-ARCH-01",
        "M246 lane-E E006 optimization gate and perf evidence edge-case expansion and robustness anchors",
    ),
    SnippetCheck(
        "M246-E011-ARCH-02",
        "`M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E011-SPC-01",
        "optimization gate and perf evidence edge-case expansion and robustness wiring shall preserve",
    ),
    SnippetCheck(
        "M246-E011-SPC-02",
        "`M246-E005`, `M246-A005`, `M246-B006`,",
    ),
    SnippetCheck(
        "M246-E011-SPC-03",
        "`M246-C011`, and `M246-D005`) and fail closed on robustness handoff drift.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E011-META-01",
        "deterministic lane-E optimization gate and perf evidence edge-case expansion and robustness dependency anchors for",
    ),
    SnippetCheck(
        "M246-E011-META-02",
        "`M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005` so gate",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E011-PKG-01",
        '"compile:objc3c": ',
    ),
    SnippetCheck(
        "M246-E011-PKG-02",
        '"proof:objc3c": ',
    ),
    SnippetCheck(
        "M246-E011-PKG-03",
        '"test:objc3c:execution-replay-proof": ',
    ),
    SnippetCheck(
        "M246-E011-PKG-04",
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
    parser.add_argument("--e010-expectations-doc", type=Path, default=DEFAULT_E010_EXPECTATIONS_DOC)
    parser.add_argument("--e010-packet-doc", type=Path, default=DEFAULT_E010_PACKET_DOC)
    parser.add_argument("--e010-checker", type=Path, default=DEFAULT_E010_CHECKER)
    parser.add_argument("--e010-test", type=Path, default=DEFAULT_E010_TEST)
    parser.add_argument("--e010-readiness-script", type=Path, default=DEFAULT_E010_READINESS_SCRIPT)
    parser.add_argument("--a008-expectations-doc", type=Path, default=DEFAULT_A008_EXPECTATIONS_DOC)
    parser.add_argument("--a008-packet-doc", type=Path, default=DEFAULT_A008_PACKET_DOC)
    parser.add_argument("--a008-checker", type=Path, default=DEFAULT_A008_CHECKER)
    parser.add_argument("--a008-test", type=Path, default=DEFAULT_A008_TEST)
    parser.add_argument("--a008-readiness-script", type=Path, default=DEFAULT_A008_READINESS_SCRIPT)
    parser.add_argument("--d008-expectations-doc", type=Path, default=DEFAULT_D008_EXPECTATIONS_DOC)
    parser.add_argument("--d008-packet-doc", type=Path, default=DEFAULT_D008_PACKET_DOC)
    parser.add_argument("--d008-checker", type=Path, default=DEFAULT_D008_CHECKER)
    parser.add_argument("--d008-test", type=Path, default=DEFAULT_D008_TEST)
    parser.add_argument("--d008-readiness-script", type=Path, default=DEFAULT_D008_READINESS_SCRIPT)
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
        (args.expectations_doc, "M246-E011-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-E011-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.readiness_script, "M246-E011-RUN-EXISTS", READINESS_SNIPPETS),
        (args.e010_expectations_doc, "M246-E011-E010-DOC-EXISTS", E010_EXPECTATIONS_SNIPPETS),
        (args.e010_packet_doc, "M246-E011-E010-PKT-EXISTS", E010_PACKET_SNIPPETS),
        (args.a008_expectations_doc, "M246-E011-A008-DOC-EXISTS", A008_EXPECTATIONS_SNIPPETS),
        (args.a008_packet_doc, "M246-E011-A008-PKT-EXISTS", A008_PACKET_SNIPPETS),
        (args.d008_expectations_doc, "M246-E011-D008-DOC-EXISTS", D008_EXPECTATIONS_SNIPPETS),
        (args.d008_packet_doc, "M246-E011-D008-PKT-EXISTS", D008_PACKET_SNIPPETS),
        (args.architecture_doc, "M246-E011-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M246-E011-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M246-E011-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M246-E011-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, finding_batch = check_text_artifact(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(finding_batch)

    for path, check_id in (
        (args.e010_checker, "M246-E011-DEP-E010-ARG-01"),
        (args.e010_test, "M246-E011-DEP-E010-ARG-02"),
        (args.e010_readiness_script, "M246-E011-DEP-E010-ARG-03"),
        (args.a008_checker, "M246-E011-DEP-A008-ARG-01"),
        (args.a008_test, "M246-E011-DEP-A008-ARG-02"),
        (args.a008_readiness_script, "M246-E011-DEP-A008-ARG-03"),
        (args.d008_checker, "M246-E011-DEP-D008-ARG-01"),
        (args.d008_test, "M246-E011-DEP-D008-ARG-02"),
        (args.d008_readiness_script, "M246-E011-DEP-D008-ARG-03"),
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




