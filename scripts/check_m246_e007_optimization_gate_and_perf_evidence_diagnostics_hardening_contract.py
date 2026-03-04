#!/usr/bin/env python3
"""Fail-closed checker for M246-E007 optimization gate/perf evidence diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-e007-optimization-gate-perf-evidence-diagnostics-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_diagnostics_hardening_e007_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_e007_lane_e_readiness.py"

DEFAULT_E006_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_e006_expectations.md"
)
DEFAULT_E006_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_packet.md"
)
DEFAULT_E006_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_contract.py"
)
DEFAULT_E006_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_contract.py"
)
DEFAULT_A005_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_a005_expectations.md"
)
DEFAULT_A005_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_a005_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_A005_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_a005_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_contract.py"
)
DEFAULT_A005_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_a005_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_contract.py"
)
DEFAULT_A005_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_a005_lane_a_readiness.py"
DEFAULT_D005_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_d005_expectations.md"
)
DEFAULT_D005_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_packet.md"
)
DEFAULT_D005_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_contract.py"
)
DEFAULT_D005_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_contract.py"
)
DEFAULT_D005_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_d005_lane_d_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-E007/optimization_gate_perf_evidence_diagnostics_hardening_summary.json"
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
        "M246-E007-DEP-E006-01",
        Path("docs/contracts/m246_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_e006_expectations.md"),
    ),
    AssetCheck(
        "M246-E007-DEP-E006-02",
        Path("spec/planning/compiler/m246/m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_packet.md"),
    ),
    AssetCheck(
        "M246-E007-DEP-E006-03",
        Path("scripts/check_m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M246-E007-DEP-E006-04",
        Path("tests/tooling/test_check_m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M246-E007-DEP-A005-01",
        Path("docs/contracts/m246_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_a005_expectations.md"),
    ),
    AssetCheck(
        "M246-E007-DEP-A005-02",
        Path("spec/planning/compiler/m246/m246_a005_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_packet.md"),
    ),
    AssetCheck(
        "M246-E007-DEP-A005-03",
        Path("scripts/check_m246_a005_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M246-E007-DEP-A005-04",
        Path("tests/tooling/test_check_m246_a005_frontend_optimization_hint_capture_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M246-E007-DEP-A005-05",
        Path("scripts/run_m246_a005_lane_a_readiness.py"),
    ),
    AssetCheck(
        "M246-E007-DEP-D005-01",
        Path("docs/contracts/m246_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_d005_expectations.md"),
    ),
    AssetCheck(
        "M246-E007-DEP-D005-02",
        Path("spec/planning/compiler/m246/m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_packet.md"),
    ),
    AssetCheck(
        "M246-E007-DEP-D005-03",
        Path("scripts/check_m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M246-E007-DEP-D005-04",
        Path("tests/tooling/test_check_m246_d005_toolchain_integration_and_optimization_controls_edge_case_and_compatibility_completion_contract.py"),
    ),
    AssetCheck(
        "M246-E007-DEP-D005-05",
        Path("scripts/run_m246_d005_lane_d_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E007-DOC-EXP-01",
        "# M246 Optimization Gate and Perf Evidence Diagnostics Hardening Expectations (E007)",
    ),
    SnippetCheck(
        "M246-E007-DOC-EXP-02",
        "Contract ID: `objc3c-optimization-gate-perf-evidence-diagnostics-hardening/m246-e007-v1`",
    ),
    SnippetCheck("M246-E007-DOC-EXP-03", "- Issue: `#6698`"),
    SnippetCheck(
        "M246-E007-DOC-EXP-04",
        "- Dependencies: `M246-E006`, `M246-A005`, `M246-B007`, `M246-C013`, `M246-D005`",
    ),
    SnippetCheck(
        "M246-E007-DOC-EXP-05",
        "| `M246-B007` | Dependency token `M246-B007` is mandatory as pending seeded lane-B diagnostics hardening assets. |",
    ),
    SnippetCheck(
        "M246-E007-DOC-EXP-06",
        "| `M246-C013` | Dependency token `M246-C013` is mandatory as pending seeded lane-C diagnostics hardening assets. |",
    ),
    SnippetCheck(
        "M246-E007-DOC-EXP-07",
        "`scripts/check_m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_contract.py`",
    ),
    SnippetCheck(
        "M246-E007-DOC-EXP-08",
        "`tests/tooling/test_check_m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_contract.py`",
    ),
    SnippetCheck(
        "M246-E007-DOC-EXP-09",
        "`scripts/run_m246_e007_lane_e_readiness.py`",
    ),
    SnippetCheck(
        "M246-E007-DOC-EXP-10",
        "`npm run --if-present check:objc3c:m246-b007-lane-b-readiness`",
    ),
    SnippetCheck(
        "M246-E007-DOC-EXP-11",
        "`npm run --if-present check:objc3c:m246-c013-lane-c-readiness`",
    ),
    SnippetCheck(
        "M246-E007-DOC-EXP-12",
        "`tmp/reports/m246/M246-E007/optimization_gate_perf_evidence_diagnostics_hardening_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E007-DOC-PKT-01",
        "# M246-E007 Optimization Gate and Perf Evidence Diagnostics Hardening Packet",
    ),
    SnippetCheck("M246-E007-DOC-PKT-02", "Packet: `M246-E007`"),
    SnippetCheck("M246-E007-DOC-PKT-03", "Issue: `#6698`"),
    SnippetCheck(
        "M246-E007-DOC-PKT-04",
        "Dependencies: `M246-E006`, `M246-A005`, `M246-B007`, `M246-C013`, `M246-D005`",
    ),
    SnippetCheck("M246-E007-DOC-PKT-05", "Theme: diagnostics hardening"),
    SnippetCheck("M246-E007-DOC-PKT-06", "Pending seeded dependency tokens:"),
    SnippetCheck("M246-E007-DOC-PKT-07", "- `M246-B007`"),
    SnippetCheck("M246-E007-DOC-PKT-08", "- `M246-C013`"),
    SnippetCheck(
        "M246-E007-DOC-PKT-09",
        "python scripts/check_m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_contract.py",
    ),
    SnippetCheck(
        "M246-E007-DOC-PKT-10",
        "python -m pytest tests/tooling/test_check_m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_contract.py -q",
    ),
    SnippetCheck(
        "M246-E007-DOC-PKT-11",
        "python scripts/check_m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_contract.py",
    ),
    SnippetCheck(
        "M246-E007-DOC-PKT-12",
        "python -m pytest tests/tooling/test_check_m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_contract.py -q",
    ),
    SnippetCheck(
        "M246-E007-DOC-PKT-13",
        "tmp/reports/m246/M246-E007/optimization_gate_perf_evidence_diagnostics_hardening_summary.json",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E007-RUN-01",
        "\"\"\"Run M246-E007 lane-E readiness checks without deep npm nesting.\"\"\"",
    ),
    SnippetCheck("M246-E007-RUN-02", "BASELINE_DEPENDENCY = \"M246-E006\""),
    SnippetCheck("M246-E007-RUN-03", "PENDING_SEEDED_DEPENDENCY_TOKENS = (\"M246-B007\", \"M246-C013\")"),
    SnippetCheck(
        "M246-E007-RUN-04",
        "scripts/check_m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_contract.py",
    ),
    SnippetCheck(
        "M246-E007-RUN-05",
        "tests/tooling/test_check_m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_contract.py",
    ),
    SnippetCheck("M246-E007-RUN-06", "check:objc3c:m246-b007-lane-b-readiness"),
    SnippetCheck("M246-E007-RUN-07", "check:objc3c:m246-c013-lane-c-readiness"),
    SnippetCheck(
        "M246-E007-RUN-08",
        "scripts/check_m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_contract.py",
    ),
    SnippetCheck(
        "M246-E007-RUN-09",
        "tests/tooling/test_check_m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_contract.py",
    ),
    SnippetCheck("M246-E007-RUN-10", "[ok] M246-E007 lane-E readiness chain completed"),
)

E006_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E007-E006-DOC-01",
        "# M246 Optimization Gate and Perf Evidence Edge-Case Expansion and Robustness Expectations (E006)",
    ),
    SnippetCheck(
        "M246-E007-E006-DOC-02",
        "Contract ID: `objc3c-optimization-gate-perf-evidence-edge-case-expansion-and-robustness/m246-e006-v1`",
    ),
)

E006_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E007-E006-PKT-01", "Packet: `M246-E006`"),
    SnippetCheck("M246-E007-E006-PKT-02", "Issue: `#6697`"),
    SnippetCheck(
        "M246-E007-E006-PKT-03",
        "Dependencies: `M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, `M246-D005`",
    ),
)

A005_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E007-A005-DOC-01",
        "# M246 Frontend Optimization Hint Capture Edge-Case and Compatibility Completion Expectations (A005)",
    ),
    SnippetCheck(
        "M246-E007-A005-DOC-02",
        "Contract ID: `objc3c-frontend-optimization-hint-capture-edge-case-and-compatibility-completion/m246-a005-v1`",
    ),
)

A005_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E007-A005-PKT-01", "Packet: `M246-A005`"),
    SnippetCheck("M246-E007-A005-PKT-02", "Issue: `#5052`"),
    SnippetCheck("M246-E007-A005-PKT-03", "Dependencies: `M246-A004`"),
)

D005_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E007-D005-DOC-01",
        "# M246 Toolchain Integration and Optimization Controls Edge-Case and Compatibility Completion Expectations (D005)",
    ),
    SnippetCheck(
        "M246-E007-D005-DOC-02",
        "Contract ID: `objc3c-toolchain-integration-optimization-controls-edge-case-and-compatibility-completion/m246-d005-v1`",
    ),
    SnippetCheck(
        "M246-E007-D005-DOC-03",
        "- Issue `#5110` defines canonical lane-D edge-case and compatibility completion scope.",
    ),
)

D005_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E007-D005-PKT-01", "Packet: `M246-D005`"),
    SnippetCheck("M246-E007-D005-PKT-02", "Issue: `#5110`"),
    SnippetCheck("M246-E007-D005-PKT-03", "Dependencies: `M246-D004`"),
)

D005_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E007-D005-RUN-01", 'DEPENDENCY_TOKEN = "M246-D004"'),
    SnippetCheck("M246-E007-D005-RUN-02", "scripts/run_m246_d004_lane_d_readiness.py"),
    SnippetCheck("M246-E007-D005-RUN-03", "[ok] M246-D005 lane-D readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E007-ARCH-01",
        "M246 lane-E E006 optimization gate and perf evidence edge-case expansion and robustness anchors",
    ),
    SnippetCheck(
        "M246-E007-ARCH-02",
        "`M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E007-SPC-01",
        "optimization gate and perf evidence edge-case expansion and robustness wiring shall preserve",
    ),
    SnippetCheck(
        "M246-E007-SPC-02",
        "`M246-E005`, `M246-A005`, `M246-B006`,",
    ),
    SnippetCheck(
        "M246-E007-SPC-03",
        "`M246-C011`, and `M246-D005`) and fail closed on robustness handoff drift.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E007-META-01",
        "deterministic lane-E optimization gate and perf evidence edge-case expansion and robustness dependency anchors for",
    ),
    SnippetCheck(
        "M246-E007-META-02",
        "`M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005` so gate",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E007-PKG-01",
        '"check:objc3c:m246-e006-lane-e-readiness": ',
    ),
    SnippetCheck(
        "M246-E007-PKG-02",
        '"compile:objc3c": ',
    ),
    SnippetCheck(
        "M246-E007-PKG-03",
        '"proof:objc3c": ',
    ),
    SnippetCheck(
        "M246-E007-PKG-04",
        '"test:objc3c:execution-replay-proof": ',
    ),
    SnippetCheck(
        "M246-E007-PKG-05",
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
    parser.add_argument("--e006-expectations-doc", type=Path, default=DEFAULT_E006_EXPECTATIONS_DOC)
    parser.add_argument("--e006-packet-doc", type=Path, default=DEFAULT_E006_PACKET_DOC)
    parser.add_argument("--e006-checker", type=Path, default=DEFAULT_E006_CHECKER)
    parser.add_argument("--e006-test", type=Path, default=DEFAULT_E006_TEST)
    parser.add_argument("--a005-expectations-doc", type=Path, default=DEFAULT_A005_EXPECTATIONS_DOC)
    parser.add_argument("--a005-packet-doc", type=Path, default=DEFAULT_A005_PACKET_DOC)
    parser.add_argument("--a005-checker", type=Path, default=DEFAULT_A005_CHECKER)
    parser.add_argument("--a005-test", type=Path, default=DEFAULT_A005_TEST)
    parser.add_argument("--a005-readiness-script", type=Path, default=DEFAULT_A005_READINESS_SCRIPT)
    parser.add_argument("--d005-expectations-doc", type=Path, default=DEFAULT_D005_EXPECTATIONS_DOC)
    parser.add_argument("--d005-packet-doc", type=Path, default=DEFAULT_D005_PACKET_DOC)
    parser.add_argument("--d005-checker", type=Path, default=DEFAULT_D005_CHECKER)
    parser.add_argument("--d005-test", type=Path, default=DEFAULT_D005_TEST)
    parser.add_argument("--d005-readiness-script", type=Path, default=DEFAULT_D005_READINESS_SCRIPT)
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


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    dep_checks, dep_failures = check_prerequisite_assets()
    checks_total += dep_checks
    failures.extend(dep_failures)

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M246-E007-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-E007-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.readiness_script, "M246-E007-RUN-EXISTS", READINESS_SNIPPETS),
        (args.e006_expectations_doc, "M246-E007-E006-DOC-EXISTS", E006_EXPECTATIONS_SNIPPETS),
        (args.e006_packet_doc, "M246-E007-E006-PKT-EXISTS", E006_PACKET_SNIPPETS),
        (args.a005_expectations_doc, "M246-E007-A005-DOC-EXISTS", A005_EXPECTATIONS_SNIPPETS),
        (args.a005_packet_doc, "M246-E007-A005-PKT-EXISTS", A005_PACKET_SNIPPETS),
        (args.d005_expectations_doc, "M246-E007-D005-DOC-EXISTS", D005_EXPECTATIONS_SNIPPETS),
        (args.d005_packet_doc, "M246-E007-D005-PKT-EXISTS", D005_PACKET_SNIPPETS),
        (args.d005_readiness_script, "M246-E007-D005-RUN-EXISTS", D005_READINESS_SNIPPETS),
        (args.architecture_doc, "M246-E007-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M246-E007-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M246-E007-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M246-E007-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, finding_batch = check_text_artifact(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(finding_batch)

    for path, check_id in (
        (args.e006_checker, "M246-E007-DEP-E006-ARG-01"),
        (args.e006_test, "M246-E007-DEP-E006-ARG-02"),
        (args.a005_checker, "M246-E007-DEP-A005-ARG-01"),
        (args.a005_test, "M246-E007-DEP-A005-ARG-02"),
        (args.a005_readiness_script, "M246-E007-DEP-A005-ARG-03"),
        (args.d005_checker, "M246-E007-DEP-D005-ARG-01"),
        (args.d005_test, "M246-E007-DEP-D005-ARG-02"),
    ):
        count, finding_batch = check_dependency_path(path, check_id)
        checks_total += count
        failures.extend(finding_batch)

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
