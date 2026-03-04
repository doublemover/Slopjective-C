#!/usr/bin/env python3
"""Fail-closed checker for M247-E007 performance SLO diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-e007-performance-slo-gate-reporting-diagnostics-hardening-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_diagnostics_hardening_e007_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e007_performance_slo_gate_and_reporting_diagnostics_hardening_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_e007_lane_e_readiness.py"
DEFAULT_E006_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_e_performance_slo_gate_and_reporting_edge_case_expansion_and_robustness_e006_expectations.md"
)
DEFAULT_E006_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_e006_performance_slo_gate_and_reporting_edge_case_expansion_and_robustness_packet.md"
)
DEFAULT_E006_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_e006_performance_slo_gate_and_reporting_edge_case_expansion_and_robustness_contract.py"
)
DEFAULT_E006_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_e006_performance_slo_gate_and_reporting_edge_case_expansion_and_robustness_contract.py"
)
DEFAULT_E006_RUNNER = ROOT / "scripts" / "run_m247_e006_lane_e_readiness.py"
DEFAULT_D007_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_runtime_link_build_throughput_optimization_diagnostics_hardening_d007_expectations.md"
)
DEFAULT_D007_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_d007_runtime_link_build_throughput_optimization_diagnostics_hardening_packet.md"
)
DEFAULT_D007_CHECKER = (
    ROOT
    / "scripts"
    / "check_m247_d007_runtime_link_build_throughput_optimization_diagnostics_hardening_contract.py"
)
DEFAULT_D007_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m247_d007_runtime_link_build_throughput_optimization_diagnostics_hardening_contract.py"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-E007/performance_slo_gate_and_reporting_diagnostics_hardening_contract_summary.json"
)


@dataclass(frozen=True)
class AssetCheck:
    check_id: str
    lane_task: str
    relative_path: Path


@dataclass(frozen=True)
class SnippetCheck:
    check_id: str
    snippet: str


@dataclass(frozen=True)
class PackageScriptKeyCheck:
    check_id: str
    script_key: str


@dataclass(frozen=True)
class PackageScriptCheck:
    check_id: str
    script_key: str
    expected_value: str


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


PREREQUISITE_ASSETS: tuple[AssetCheck, ...] = (
    AssetCheck(
        "M247-E007-DEP-E006-01",
        "M247-E006",
        Path("docs/contracts/m247_lane_e_performance_slo_gate_and_reporting_edge_case_expansion_and_robustness_e006_expectations.md"),
    ),
    AssetCheck(
        "M247-E007-DEP-E006-02",
        "M247-E006",
        Path("spec/planning/compiler/m247/m247_e006_performance_slo_gate_and_reporting_edge_case_expansion_and_robustness_packet.md"),
    ),
    AssetCheck(
        "M247-E007-DEP-E006-03",
        "M247-E006",
        Path("scripts/check_m247_e006_performance_slo_gate_and_reporting_edge_case_expansion_and_robustness_contract.py"),
    ),
    AssetCheck(
        "M247-E007-DEP-E006-04",
        "M247-E006",
        Path(
            "tests/tooling/test_check_m247_e006_performance_slo_gate_and_reporting_edge_case_expansion_and_robustness_contract.py"
        ),
    ),
    AssetCheck(
        "M247-E007-DEP-E006-05",
        "M247-E006",
        Path("scripts/run_m247_e006_lane_e_readiness.py"),
    ),
    AssetCheck(
        "M247-E007-DEP-D007-01",
        "M247-D007",
        Path("docs/contracts/m247_runtime_link_build_throughput_optimization_diagnostics_hardening_d007_expectations.md"),
    ),
    AssetCheck(
        "M247-E007-DEP-D007-02",
        "M247-D007",
        Path("spec/planning/compiler/m247/m247_d007_runtime_link_build_throughput_optimization_diagnostics_hardening_packet.md"),
    ),
    AssetCheck(
        "M247-E007-DEP-D007-03",
        "M247-D007",
        Path("scripts/check_m247_d007_runtime_link_build_throughput_optimization_diagnostics_hardening_contract.py"),
    ),
    AssetCheck(
        "M247-E007-DEP-D007-04",
        "M247-D007",
        Path("tests/tooling/test_check_m247_d007_runtime_link_build_throughput_optimization_diagnostics_hardening_contract.py"),
    ),
)

EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E007-DOC-EXP-01",
        "# M247 Lane E Performance SLO Gate and Reporting Diagnostics Hardening Expectations (E007)",
    ),
    SnippetCheck(
        "M247-E007-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-diagnostics-hardening-contract/m247-e007-v1`",
    ),
    SnippetCheck(
        "M247-E007-DOC-EXP-03",
        "Dependencies: `M247-E006`, `M247-A007`, `M247-B007`, `M247-C007`, `M247-D007`",
    ),
    SnippetCheck(
        "M247-E007-DOC-EXP-04",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-E007-DOC-EXP-05",
        "`scripts/run_m247_e007_lane_e_readiness.py`",
    ),
    SnippetCheck(
        "M247-E007-DOC-EXP-06",
        "`check:objc3c:m247-e007-lane-e-readiness`",
    ),
    SnippetCheck(
        "M247-E007-DOC-EXP-07",
        "`tmp/reports/m247/M247-E007/performance_slo_gate_and_reporting_diagnostics_hardening_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E007-DOC-PKT-01",
        "# M247-E007 Performance SLO Gate and Reporting Diagnostics Hardening Packet",
    ),
    SnippetCheck("M247-E007-DOC-PKT-02", "Packet: `M247-E007`"),
    SnippetCheck("M247-E007-DOC-PKT-03", "Issue: `#6778`"),
    SnippetCheck(
        "M247-E007-DOC-PKT-04",
        "Dependencies: `M247-E006`, `M247-A007`, `M247-B007`, `M247-C007`, `M247-D007`",
    ),
    SnippetCheck("M247-E007-DOC-PKT-05", "Freeze date: `2026-03-04`"),
    SnippetCheck(
        "M247-E007-DOC-PKT-06",
        "`scripts/check_m247_e007_performance_slo_gate_and_reporting_diagnostics_hardening_contract.py`",
    ),
    SnippetCheck(
        "M247-E007-DOC-PKT-07",
        "`scripts/run_m247_e007_lane_e_readiness.py`",
    ),
    SnippetCheck(
        "M247-E007-DOC-PKT-08",
        "`tests/tooling/test_check_m247_e007_performance_slo_gate_and_reporting_diagnostics_hardening_contract.py`",
    ),
    SnippetCheck(
        "M247-E007-DOC-PKT-09",
        "`check:objc3c:m247-e007-lane-e-readiness`",
    ),
    SnippetCheck(
        "M247-E007-DOC-PKT-10",
        "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-E007-DOC-PKT-11",
        "`E006 readiness -> E007 checker -> E007 pytest`",
    ),
)

E006_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E007-E006-DOC-01",
        "# M247 Lane E Performance SLO Gate and Reporting Edge-Case Expansion and Robustness Expectations (E006)",
    ),
    SnippetCheck(
        "M247-E007-E006-DOC-02",
        "Contract ID: `objc3c-lane-e-performance-slo-gate-reporting-edge-case-expansion-and-robustness-contract/m247-e006-v1`",
    ),
)

D007_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E007-D007-DOC-01",
        "# M247 Runtime/Link/Build Throughput Optimization Diagnostics Hardening Expectations (D007)",
    ),
    SnippetCheck(
        "M247-E007-D007-DOC-02",
        "Contract ID: `objc3c-runtime-link-build-throughput-optimization-diagnostics-hardening/m247-d007-v1`",
    ),
    SnippetCheck(
        "M247-E007-D007-DOC-03",
        "Dependencies: `M247-D006`",
    ),
)

D007_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-E007-D007-PKT-01", "Packet: `M247-D007`"),
    SnippetCheck("M247-E007-D007-PKT-02", "Issue: `#6765`"),
    SnippetCheck("M247-E007-D007-PKT-03", "Dependencies: `M247-D006`"),
    SnippetCheck("M247-E007-D007-PKT-04", "Freeze date: `2026-03-04`"),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E007-RUN-01",
        '"""Run M247-E007 lane-E readiness checks without deep npm nesting."""',
    ),
    SnippetCheck(
        "M247-E007-RUN-02",
        'BASELINE_DEPENDENCIES = ("M247-E006",)',
    ),
    SnippetCheck(
        "M247-E007-RUN-03",
        'PENDING_SEEDED_DEPENDENCY_TOKENS = ("M247-A007", "M247-B007", "M247-C007", "M247-D007")',
    ),
    SnippetCheck("M247-E007-RUN-04", "check:objc3c:m247-e006-lane-e-readiness"),
    SnippetCheck("M247-E007-RUN-05", "check:objc3c:m247-a007-lane-a-readiness"),
    SnippetCheck("M247-E007-RUN-06", "check:objc3c:m247-b007-lane-b-readiness"),
    SnippetCheck("M247-E007-RUN-07", "check:objc3c:m247-c007-lane-c-readiness"),
    SnippetCheck("M247-E007-RUN-08", "check:objc3c:m247-d007-lane-d-readiness"),
    SnippetCheck(
        "M247-E007-RUN-09",
        "scripts/check_m247_e007_performance_slo_gate_and_reporting_diagnostics_hardening_contract.py",
    ),
    SnippetCheck(
        "M247-E007-RUN-10",
        "tests/tooling/test_check_m247_e007_performance_slo_gate_and_reporting_diagnostics_hardening_contract.py",
    ),
    SnippetCheck("M247-E007-RUN-11", "[ok] M247-E007 lane-E readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E007-ARCH-01",
        "M247 lane-E E007 performance SLO gate/reporting diagnostics hardening anchors",
    ),
    SnippetCheck(
        "M247-E007-ARCH-02",
        "dependency references (`M247-E006`, `M247-A007`, `M247-B007`, `M247-C007`,",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E007-SPC-01",
        "performance SLO diagnostics hardening wiring shall preserve explicit",
    ),
    SnippetCheck(
        "M247-E007-SPC-02",
        "lane-E dependency anchors (`M247-E006`, `M247-A007`, `M247-B007`, `M247-C007`,",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-E007-META-01",
        "deterministic lane-E performance SLO diagnostics-hardening metadata anchors for `M247-E007`",
    ),
    SnippetCheck(
        "M247-E007-META-02",
        "`M247-D007` dependency continuity so lane-E diagnostics-hardening contract-gating",
    ),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M247-E007-CFG-01",
        "check:objc3c:m247-e007-performance-slo-gate-reporting-diagnostics-hardening-contract",
    ),
    PackageScriptKeyCheck(
        "M247-E007-CFG-02",
        "test:tooling:m247-e007-performance-slo-gate-reporting-diagnostics-hardening-contract",
    ),
    PackageScriptKeyCheck("M247-E007-CFG-03", "check:objc3c:m247-e007-lane-e-readiness"),
    PackageScriptKeyCheck("M247-E007-CFG-04", "check:objc3c:m247-e006-lane-e-readiness"),
    PackageScriptKeyCheck("M247-E007-CFG-08", "compile:objc3c"),
    PackageScriptKeyCheck("M247-E007-CFG-09", "proof:objc3c"),
    PackageScriptKeyCheck("M247-E007-CFG-10", "test:objc3c:execution-replay-proof"),
    PackageScriptKeyCheck("M247-E007-CFG-11", "test:objc3c:perf-budget"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M247-E007-CFG-05",
        "check:objc3c:m247-e007-performance-slo-gate-reporting-diagnostics-hardening-contract",
        "python scripts/check_m247_e007_performance_slo_gate_and_reporting_diagnostics_hardening_contract.py",
    ),
    PackageScriptCheck(
        "M247-E007-CFG-06",
        "test:tooling:m247-e007-performance-slo-gate-reporting-diagnostics-hardening-contract",
        "python -m pytest tests/tooling/test_check_m247_e007_performance_slo_gate_and_reporting_diagnostics_hardening_contract.py -q",
    ),
    PackageScriptCheck(
        "M247-E007-CFG-07",
        "check:objc3c:m247-e007-lane-e-readiness",
        "python scripts/run_m247_e007_lane_e_readiness.py",
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
    parser.add_argument("--e006-runner", type=Path, default=DEFAULT_E006_RUNNER)
    parser.add_argument("--d007-expectations-doc", type=Path, default=DEFAULT_D007_EXPECTATIONS_DOC)
    parser.add_argument("--d007-packet-doc", type=Path, default=DEFAULT_D007_PACKET_DOC)
    parser.add_argument("--d007-checker", type=Path, default=DEFAULT_D007_CHECKER)
    parser.add_argument("--d007-test", type=Path, default=DEFAULT_D007_TEST)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
    return parser.parse_args(argv)


def check_prerequisite_assets() -> tuple[int, list[Finding]]:
    checks_total = 0
    findings: list[Finding] = []
    for asset in PREREQUISITE_ASSETS:
        checks_total += 1
        absolute = ROOT / asset.relative_path
        if not absolute.exists() or not absolute.is_file():
            findings.append(
                Finding(
                    artifact=asset.relative_path.as_posix(),
                    check_id=asset.check_id,
                    detail=f"{asset.lane_task} prerequisite missing: {asset.relative_path.as_posix()}",
                )
            )
    return checks_total, findings


def check_doc_contract(
    *,
    artifact_name: str,
    path: Path,
    exists_check_id: str,
    snippets: tuple[SnippetCheck, ...],
) -> tuple[int, list[Finding]]:
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

    for snippet_check in snippets:
        checks_total += 1
        if snippet_check.snippet not in text:
            findings.append(
                Finding(
                    artifact=artifact_name,
                    check_id=snippet_check.check_id,
                    detail=f"expected snippet missing: {snippet_check.snippet}",
                )
            )
    return checks_total, findings


def check_dependency_path(path: Path, check_id: str) -> tuple[int, list[Finding]]:
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=check_id,
                detail=f"required dependency path is missing: {display_path(path)}",
            )
        )
    elif not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=check_id,
                detail=f"required dependency path is not a file: {display_path(path)}",
            )
        )
    return 1, findings


def check_package_contract(path: Path) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists() or not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M247-E007-CFG-00",
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (json.JSONDecodeError, OSError) as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M247-E007-CFG-00",
                detail=f"unable to parse package.json: {exc}",
            )
        )
        return checks_total, findings

    scripts = payload.get("scripts")
    checks_total += 1
    if not isinstance(scripts, dict):
        findings.append(
            Finding(
                artifact="package_json",
                check_id="M247-E007-CFG-00",
                detail='expected top-level "scripts" object in package.json',
            )
        )
        return checks_total, findings

    for key_check in PACKAGE_SCRIPT_KEY_CHECKS:
        checks_total += 1
        if key_check.script_key not in scripts:
            findings.append(
                Finding(
                    artifact="package_json",
                    check_id=key_check.check_id,
                    detail=f"missing required script key: {key_check.script_key}",
                )
            )

    for script_check in PACKAGE_SCRIPT_CHECKS:
        checks_total += 1
        actual_value = scripts.get(script_check.script_key)
        if actual_value != script_check.expected_value:
            findings.append(
                Finding(
                    artifact="package_json",
                    check_id=script_check.check_id,
                    detail=(
                        f"script {script_check.script_key} mismatch; expected "
                        f"{script_check.expected_value!r}, got {actual_value!r}"
                    ),
                )
            )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    checks_total = 0
    failures: list[Finding] = []

    count, findings = check_prerequisite_assets()
    checks_total += count
    failures.extend(findings)

    for artifact_name, path, exists_check_id, snippets in (
        ("expectations_doc", args.expectations_doc, "M247-E007-DOC-EXP-00", EXPECTATIONS_SNIPPETS),
        ("packet_doc", args.packet_doc, "M247-E007-DOC-PKT-00", PACKET_SNIPPETS),
        ("e006_expectations_doc", args.e006_expectations_doc, "M247-E007-E006-DOC-00", E006_EXPECTATIONS_SNIPPETS),
        ("d007_expectations_doc", args.d007_expectations_doc, "M247-E007-D007-DOC-00", D007_EXPECTATIONS_SNIPPETS),
        ("d007_packet_doc", args.d007_packet_doc, "M247-E007-D007-PKT-00", D007_PACKET_SNIPPETS),
        ("readiness_script", args.readiness_script, "M247-E007-RUN-00", READINESS_SNIPPETS),
        ("architecture_doc", args.architecture_doc, "M247-E007-ARCH-00", ARCHITECTURE_SNIPPETS),
        ("lowering_spec", args.lowering_spec, "M247-E007-SPC-00", LOWERING_SPEC_SNIPPETS),
        ("metadata_spec", args.metadata_spec, "M247-E007-META-00", METADATA_SPEC_SNIPPETS),
    ):
        count, findings = check_doc_contract(
            artifact_name=artifact_name,
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.e006_packet_doc, "M247-E007-DEP-01"),
        (args.e006_checker, "M247-E007-DEP-02"),
        (args.e006_test, "M247-E007-DEP-03"),
        (args.e006_runner, "M247-E007-DEP-04"),
        (args.d007_packet_doc, "M247-E007-DEP-05"),
        (args.d007_checker, "M247-E007-DEP-06"),
        (args.d007_test, "M247-E007-DEP-07"),
    ):
        count, findings = check_dependency_path(path, check_id)
        checks_total += count
        failures.extend(findings)

    count, findings = check_package_contract(args.package_json)
    checks_total += count
    failures.extend(findings)

    failures = sorted(failures, key=lambda finding: (finding.artifact, finding.check_id, finding.detail))
    checks_passed = checks_total - len(failures)

    summary_payload = {
        "mode": MODE,
        "ok": len(failures) == 0,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in failures
        ],
    }

    summary_path = args.summary_out
    if not summary_path.is_absolute():
        summary_path = ROOT / summary_path
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary_payload), encoding="utf-8")

    if args.emit_json:
        sys.stdout.write(canonical_json(summary_payload))

    if failures:
        for finding in failures:
            sys.stderr.write(f"[{finding.check_id}] {finding.artifact}: {finding.detail}\n")
        return 1

    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
