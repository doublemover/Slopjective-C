#!/usr/bin/env python3
"""Fail-closed checker for M246-E008 optimization gate/perf evidence recovery and determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-e008-optimization-gate-perf-evidence-recovery-and-determinism-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_recovery_and_determinism_hardening_e008_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_e008_optimization_gate_and_perf_evidence_recovery_and_determinism_hardening_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_e008_lane_e_readiness.py"

DEFAULT_E007_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_optimization_gate_and_perf_evidence_diagnostics_hardening_e007_expectations.md"
)
DEFAULT_E007_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_packet.md"
)
DEFAULT_E007_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_contract.py"
)
DEFAULT_E007_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_contract.py"
)
DEFAULT_E007_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_e007_lane_e_readiness.py"

DEFAULT_A006_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_a006_expectations.md"
)
DEFAULT_A006_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_a006_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_packet.md"
)
DEFAULT_A006_CHECKER = (
    ROOT
    / "scripts"
    / "check_m246_a006_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_contract.py"
)
DEFAULT_A006_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_a006_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_contract.py"
)
DEFAULT_A006_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_a006_lane_a_readiness.py"

DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-E008/optimization_gate_perf_evidence_recovery_and_determinism_hardening_summary.json"
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
        "M246-E008-DEP-E007-01",
        Path("docs/contracts/m246_optimization_gate_and_perf_evidence_diagnostics_hardening_e007_expectations.md"),
    ),
    AssetCheck(
        "M246-E008-DEP-E007-02",
        Path("spec/planning/compiler/m246/m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_packet.md"),
    ),
    AssetCheck(
        "M246-E008-DEP-E007-03",
        Path("scripts/check_m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_contract.py"),
    ),
    AssetCheck(
        "M246-E008-DEP-E007-04",
        Path("tests/tooling/test_check_m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_contract.py"),
    ),
    AssetCheck(
        "M246-E008-DEP-E007-05",
        Path("scripts/run_m246_e007_lane_e_readiness.py"),
    ),
    AssetCheck(
        "M246-E008-DEP-A006-01",
        Path("docs/contracts/m246_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_a006_expectations.md"),
    ),
    AssetCheck(
        "M246-E008-DEP-A006-02",
        Path("spec/planning/compiler/m246/m246_a006_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_packet.md"),
    ),
    AssetCheck(
        "M246-E008-DEP-A006-03",
        Path("scripts/check_m246_a006_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M246-E008-DEP-A006-04",
        Path("tests/tooling/test_check_m246_a006_frontend_optimization_hint_capture_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M246-E008-DEP-A006-05",
        Path("scripts/run_m246_a006_lane_a_readiness.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E008-DOC-EXP-01",
        "# M246 Optimization Gate and Perf Evidence Recovery and Determinism Hardening Expectations (E008)",
    ),
    SnippetCheck(
        "M246-E008-DOC-EXP-02",
        "Contract ID: `objc3c-optimization-gate-perf-evidence-recovery-and-determinism-hardening/m246-e008-v1`",
    ),
    SnippetCheck("M246-E008-DOC-EXP-03", "- Issue: `#6699`"),
    SnippetCheck(
        "M246-E008-DOC-EXP-04",
        "- Dependencies: `M246-E007`, `M246-A006`, `M246-B009`, `M246-C015`, `M246-D006`",
    ),
    SnippetCheck(
        "M246-E008-DOC-EXP-05",
        "| `M246-B009` | Dependency token `M246-B009` is mandatory as pending seeded lane-B recovery and determinism hardening assets. |",
    ),
    SnippetCheck(
        "M246-E008-DOC-EXP-06",
        "| `M246-C015` | Dependency token `M246-C015` is mandatory as pending seeded lane-C recovery and determinism hardening assets. |",
    ),
    SnippetCheck(
        "M246-E008-DOC-EXP-07",
        "| `M246-D006` | Dependency token `M246-D006` is mandatory as pending seeded lane-D recovery and determinism hardening assets. |",
    ),
    SnippetCheck(
        "M246-E008-DOC-EXP-08",
        "`scripts/check_m246_e008_optimization_gate_and_perf_evidence_recovery_and_determinism_hardening_contract.py`",
    ),
    SnippetCheck(
        "M246-E008-DOC-EXP-09",
        "`tests/tooling/test_check_m246_e008_optimization_gate_and_perf_evidence_recovery_and_determinism_hardening_contract.py`",
    ),
    SnippetCheck(
        "M246-E008-DOC-EXP-10",
        "`scripts/run_m246_e008_lane_e_readiness.py`",
    ),
    SnippetCheck(
        "M246-E008-DOC-EXP-11",
        "`npm run --if-present check:objc3c:m246-b009-lane-b-readiness`",
    ),
    SnippetCheck(
        "M246-E008-DOC-EXP-12",
        "`npm run --if-present check:objc3c:m246-c015-lane-c-readiness`",
    ),
    SnippetCheck(
        "M246-E008-DOC-EXP-13",
        "`npm run --if-present check:objc3c:m246-d006-lane-d-readiness`",
    ),
    SnippetCheck(
        "M246-E008-DOC-EXP-14",
        "`tmp/reports/m246/M246-E008/optimization_gate_perf_evidence_recovery_and_determinism_hardening_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E008-DOC-PKT-01",
        "# M246-E008 Optimization Gate and Perf Evidence Recovery and Determinism Hardening Packet",
    ),
    SnippetCheck("M246-E008-DOC-PKT-02", "Packet: `M246-E008`"),
    SnippetCheck("M246-E008-DOC-PKT-03", "Issue: `#6699`"),
    SnippetCheck(
        "M246-E008-DOC-PKT-04",
        "Dependencies: `M246-E007`, `M246-A006`, `M246-B009`, `M246-C015`, `M246-D006`",
    ),
    SnippetCheck("M246-E008-DOC-PKT-05", "Theme: recovery and determinism hardening"),
    SnippetCheck("M246-E008-DOC-PKT-06", "Pending seeded dependency tokens:"),
    SnippetCheck("M246-E008-DOC-PKT-07", "- `M246-B009`"),
    SnippetCheck("M246-E008-DOC-PKT-08", "- `M246-C015`"),
    SnippetCheck("M246-E008-DOC-PKT-09", "- `M246-D006`"),
    SnippetCheck(
        "M246-E008-DOC-PKT-10",
        "python scripts/run_m246_e007_lane_e_readiness.py",
    ),
    SnippetCheck(
        "M246-E008-DOC-PKT-11",
        "python scripts/run_m246_a006_lane_a_readiness.py",
    ),
    SnippetCheck(
        "M246-E008-DOC-PKT-12",
        "npm run --if-present check:objc3c:m246-b009-lane-b-readiness",
    ),
    SnippetCheck(
        "M246-E008-DOC-PKT-13",
        "npm run --if-present check:objc3c:m246-c015-lane-c-readiness",
    ),
    SnippetCheck(
        "M246-E008-DOC-PKT-14",
        "npm run --if-present check:objc3c:m246-d006-lane-d-readiness",
    ),
    SnippetCheck(
        "M246-E008-DOC-PKT-15",
        "python scripts/check_m246_e008_optimization_gate_and_perf_evidence_recovery_and_determinism_hardening_contract.py",
    ),
    SnippetCheck(
        "M246-E008-DOC-PKT-16",
        "python -m pytest tests/tooling/test_check_m246_e008_optimization_gate_and_perf_evidence_recovery_and_determinism_hardening_contract.py -q",
    ),
    SnippetCheck(
        "M246-E008-DOC-PKT-17",
        "tmp/reports/m246/M246-E008/optimization_gate_perf_evidence_recovery_and_determinism_hardening_summary.json",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E008-RUN-01",
        "\"\"\"Run M246-E008 lane-E readiness checks without deep npm nesting.\"\"\"",
    ),
    SnippetCheck("M246-E008-RUN-02", "BASELINE_DEPENDENCIES = (\"M246-E007\", \"M246-A006\")"),
    SnippetCheck(
        "M246-E008-RUN-03",
        "PENDING_SEEDED_DEPENDENCY_TOKENS = (\"M246-B009\", \"M246-C015\", \"M246-D006\")",
    ),
    SnippetCheck(
        "M246-E008-RUN-04",
        "scripts/run_m246_e007_lane_e_readiness.py",
    ),
    SnippetCheck(
        "M246-E008-RUN-05",
        "scripts/run_m246_a006_lane_a_readiness.py",
    ),
    SnippetCheck("M246-E008-RUN-06", "check:objc3c:m246-b009-lane-b-readiness"),
    SnippetCheck("M246-E008-RUN-07", "check:objc3c:m246-c015-lane-c-readiness"),
    SnippetCheck("M246-E008-RUN-08", "check:objc3c:m246-d006-lane-d-readiness"),
    SnippetCheck(
        "M246-E008-RUN-09",
        "scripts/check_m246_e008_optimization_gate_and_perf_evidence_recovery_and_determinism_hardening_contract.py",
    ),
    SnippetCheck(
        "M246-E008-RUN-10",
        "tests/tooling/test_check_m246_e008_optimization_gate_and_perf_evidence_recovery_and_determinism_hardening_contract.py",
    ),
    SnippetCheck("M246-E008-RUN-11", "[ok] M246-E008 lane-E readiness chain completed"),
)

E007_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E008-E007-DOC-01",
        "# M246 Optimization Gate and Perf Evidence Diagnostics Hardening Expectations (E007)",
    ),
    SnippetCheck(
        "M246-E008-E007-DOC-02",
        "Contract ID: `objc3c-optimization-gate-perf-evidence-diagnostics-hardening/m246-e007-v1`",
    ),
)

E007_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E008-E007-PKT-01", "Packet: `M246-E007`"),
    SnippetCheck("M246-E008-E007-PKT-02", "Issue: `#6698`"),
    SnippetCheck(
        "M246-E008-E007-PKT-03",
        "Dependencies: `M246-E006`, `M246-A005`, `M246-B007`, `M246-C013`, `M246-D005`",
    ),
)

A006_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E008-A006-DOC-01",
        "# M246 Frontend Optimization Hint Capture Edge-Case Expansion and Robustness Expectations (A006)",
    ),
    SnippetCheck(
        "M246-E008-A006-DOC-02",
        "Contract ID: `objc3c-frontend-optimization-hint-capture-edge-case-expansion-and-robustness/m246-a006-v1`",
    ),
)

A006_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-E008-A006-PKT-01", "Packet: `M246-A006`"),
    SnippetCheck("M246-E008-A006-PKT-02", "Issue: `#5053`"),
    SnippetCheck("M246-E008-A006-PKT-03", "Dependencies: `M246-A005`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E008-ARCH-01",
        "M246 lane-E E006 optimization gate and perf evidence edge-case expansion and robustness anchors",
    ),
    SnippetCheck(
        "M246-E008-ARCH-02",
        "`M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E008-SPC-01",
        "optimization gate and perf evidence edge-case expansion and robustness wiring shall preserve",
    ),
    SnippetCheck(
        "M246-E008-SPC-02",
        "`M246-E005`, `M246-A005`, `M246-B006`,",
    ),
    SnippetCheck(
        "M246-E008-SPC-03",
        "`M246-C011`, and `M246-D005`) and fail closed on robustness handoff drift.",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E008-META-01",
        "deterministic lane-E optimization gate and perf evidence edge-case expansion and robustness dependency anchors for",
    ),
    SnippetCheck(
        "M246-E008-META-02",
        "`M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005` so gate",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-E008-PKG-01",
        '"compile:objc3c": ',
    ),
    SnippetCheck(
        "M246-E008-PKG-02",
        '"proof:objc3c": ',
    ),
    SnippetCheck(
        "M246-E008-PKG-03",
        '"test:objc3c:execution-replay-proof": ',
    ),
    SnippetCheck(
        "M246-E008-PKG-04",
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
    parser.add_argument("--e007-expectations-doc", type=Path, default=DEFAULT_E007_EXPECTATIONS_DOC)
    parser.add_argument("--e007-packet-doc", type=Path, default=DEFAULT_E007_PACKET_DOC)
    parser.add_argument("--e007-checker", type=Path, default=DEFAULT_E007_CHECKER)
    parser.add_argument("--e007-test", type=Path, default=DEFAULT_E007_TEST)
    parser.add_argument("--e007-readiness-script", type=Path, default=DEFAULT_E007_READINESS_SCRIPT)
    parser.add_argument("--a006-expectations-doc", type=Path, default=DEFAULT_A006_EXPECTATIONS_DOC)
    parser.add_argument("--a006-packet-doc", type=Path, default=DEFAULT_A006_PACKET_DOC)
    parser.add_argument("--a006-checker", type=Path, default=DEFAULT_A006_CHECKER)
    parser.add_argument("--a006-test", type=Path, default=DEFAULT_A006_TEST)
    parser.add_argument("--a006-readiness-script", type=Path, default=DEFAULT_A006_READINESS_SCRIPT)
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
        (args.expectations_doc, "M246-E008-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-E008-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.readiness_script, "M246-E008-RUN-EXISTS", READINESS_SNIPPETS),
        (args.e007_expectations_doc, "M246-E008-E007-DOC-EXISTS", E007_EXPECTATIONS_SNIPPETS),
        (args.e007_packet_doc, "M246-E008-E007-PKT-EXISTS", E007_PACKET_SNIPPETS),
        (args.a006_expectations_doc, "M246-E008-A006-DOC-EXISTS", A006_EXPECTATIONS_SNIPPETS),
        (args.a006_packet_doc, "M246-E008-A006-PKT-EXISTS", A006_PACKET_SNIPPETS),
        (args.architecture_doc, "M246-E008-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M246-E008-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M246-E008-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M246-E008-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, finding_batch = check_text_artifact(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(finding_batch)

    for path, check_id in (
        (args.e007_checker, "M246-E008-DEP-E007-ARG-01"),
        (args.e007_test, "M246-E008-DEP-E007-ARG-02"),
        (args.e007_readiness_script, "M246-E008-DEP-E007-ARG-03"),
        (args.a006_checker, "M246-E008-DEP-A006-ARG-01"),
        (args.a006_test, "M246-E008-DEP-A006-ARG-02"),
        (args.a006_readiness_script, "M246-E008-DEP-A006-ARG-03"),
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
