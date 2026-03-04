#!/usr/bin/env python3
"""Fail-closed checker for M247-C017 lowering/codegen cost profiling and controls integration closeout and gate sign-off."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-c017-lowering-codegen-cost-profiling-controls-integration-closeout-and-gate-signoff-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_c_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_c017_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_packet.md"
)
DEFAULT_READINESS_RUNNER = ROOT / "scripts" / "run_m247_c017_lane_c_readiness.py"
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
DEFAULT_C016_READINESS_RUNNER = ROOT / "scripts" / "run_m247_c016_lane_c_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-C017/lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract_summary.json"
)


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


EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C017-DOC-EXP-01",
        "# M247 Lane C Lowering/Codegen Cost Profiling and Controls Integration Closeout and Gate Sign-Off Expectations (C017)",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-02",
        "Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-and-controls-integration-closeout-and-gate-signoff/m247-c017-v1`",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-03",
        "Issue `#6758` defines canonical lane-C integration closeout and gate sign-off scope.",
    ),
    SnippetCheck("M247-C017-DOC-EXP-04", "Dependencies: `M247-C016`"),
    SnippetCheck(
        "M247-C017-DOC-EXP-05",
        "scripts/check_m247_c016_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_contract.py",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-06",
        "scripts/run_m247_c017_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-07",
        "Readiness chain order: `C016 readiness -> C017 checker -> C017 pytest`.",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-08",
        "tests/tooling/test_check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck("M247-C017-DOC-EXP-09", "`check:objc3c:m247-c017-lane-c-readiness`"),
    SnippetCheck(
        "M247-C017-DOC-EXP-10",
        "tmp/reports/m247/M247-C017/lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C017-DOC-PKT-01",
        "# M247-C017 Lowering/Codegen Cost Profiling and Controls Integration Closeout and Gate Sign-Off Packet",
    ),
    SnippetCheck("M247-C017-DOC-PKT-02", "Packet: `M247-C017`"),
    SnippetCheck("M247-C017-DOC-PKT-03", "Issue: `#6758`"),
    SnippetCheck("M247-C017-DOC-PKT-04", "Dependencies: `M247-C016`"),
    SnippetCheck("M247-C017-DOC-PKT-05", "Freeze date: `2026-03-04`"),
    SnippetCheck("M247-C017-DOC-PKT-06", "Dependency anchors from `M247-C016`:"),
    SnippetCheck("M247-C017-DOC-PKT-07", "scripts/run_m247_c017_lane_c_readiness.py"),
    SnippetCheck(
        "M247-C017-DOC-PKT-08",
        "python -m pytest tests/tooling/test_check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py -q",
    ),
    SnippetCheck(
        "M247-C017-DOC-PKT-09",
        "`C016 readiness -> C017 checker -> C017 pytest`",
    ),
    SnippetCheck(
        "M247-C017-DOC-PKT-10",
        "tmp/reports/m247/M247-C017/lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract_summary.json",
    ),
)

C016_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C017-C016-DOC-01",
        "Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-controls-advanced-edge-compatibility-workpack-shard-1/m247-c016-v1`",
    ),
    SnippetCheck("M247-C017-C016-DOC-02", "Dependencies: `M247-C015`"),
    SnippetCheck(
        "M247-C017-C016-DOC-03",
        "Issue `#6757` defines canonical lane-C advanced edge compatibility workpack (shard 1) scope.",
    ),
)

C016_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-C017-C016-PKT-01", "Packet: `M247-C016`"),
    SnippetCheck("M247-C017-C016-PKT-02", "Issue: `#6757`"),
    SnippetCheck("M247-C017-C016-PKT-03", "Dependencies: `M247-C015`"),
    SnippetCheck("M247-C017-C016-PKT-04", "Freeze date: `2026-03-04`"),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-C017-RUN-01", 'DEPENDENCY_TOKEN = "M247-C016"'),
    SnippetCheck("M247-C017-RUN-02", "scripts/run_m247_c016_lane_c_readiness.py"),
    SnippetCheck(
        "M247-C017-RUN-03",
        "scripts/check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M247-C017-RUN-04",
        "tests/tooling/test_check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck("M247-C017-RUN-05", "[ok] M247-C017 lane-C readiness chain completed"),
)

C016_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-C017-C016-RUN-01", "scripts/run_m247_c015_lane_c_readiness.py"),
    SnippetCheck("M247-C017-C016-RUN-02", "[ok] M247-C016 lane-C readiness chain completed"),
)

PACKAGE_SCRIPT_KEY_CHECKS: tuple[PackageScriptKeyCheck, ...] = (
    PackageScriptKeyCheck(
        "M247-C017-PKG-KEY-01",
        "check:objc3c:m247-c017-lowering-codegen-cost-profiling-controls-integration-closeout-and-gate-signoff-contract",
    ),
    PackageScriptKeyCheck(
        "M247-C017-PKG-KEY-02",
        "test:tooling:m247-c017-lowering-codegen-cost-profiling-controls-integration-closeout-and-gate-signoff-contract",
    ),
    PackageScriptKeyCheck("M247-C017-PKG-KEY-03", "check:objc3c:m247-c017-lane-c-readiness"),
    PackageScriptKeyCheck("M247-C017-PKG-KEY-04", "compile:objc3c"),
    PackageScriptKeyCheck("M247-C017-PKG-KEY-05", "proof:objc3c"),
    PackageScriptKeyCheck("M247-C017-PKG-KEY-06", "test:objc3c:execution-replay-proof"),
    PackageScriptKeyCheck("M247-C017-PKG-KEY-07", "test:objc3c:perf-budget"),
)

PACKAGE_SCRIPT_CHECKS: tuple[PackageScriptCheck, ...] = (
    PackageScriptCheck(
        "M247-C017-PKG-VAL-01",
        "check:objc3c:m247-c017-lowering-codegen-cost-profiling-controls-integration-closeout-and-gate-signoff-contract",
        "python scripts/check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py",
    ),
    PackageScriptCheck(
        "M247-C017-PKG-VAL-02",
        "test:tooling:m247-c017-lowering-codegen-cost-profiling-controls-integration-closeout-and-gate-signoff-contract",
        "python -m pytest tests/tooling/test_check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py -q",
    ),
    PackageScriptCheck(
        "M247-C017-PKG-VAL-03",
        "check:objc3c:m247-c017-lane-c-readiness",
        "python scripts/run_m247_c017_lane_c_readiness.py",
    ),
)

PACKAGE_FORBIDDEN_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C017-PKG-FORB-01",
        '"check:objc3c:m247-c017-lane-c-readiness": "npm run check:objc3c:m247-c016-lane-c-readiness',
    ),
)


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2, sort_keys=True) + "\n"


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
    parser.add_argument("--readiness-runner", type=Path, default=DEFAULT_READINESS_RUNNER)
    parser.add_argument("--c016-expectations-doc", type=Path, default=DEFAULT_C016_EXPECTATIONS_DOC)
    parser.add_argument("--c016-packet-doc", type=Path, default=DEFAULT_C016_PACKET_DOC)
    parser.add_argument("--c016-checker", type=Path, default=DEFAULT_C016_CHECKER)
    parser.add_argument("--c016-test", type=Path, default=DEFAULT_C016_TEST)
    parser.add_argument("--c016-readiness-runner", type=Path, default=DEFAULT_C016_READINESS_RUNNER)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
    return parser.parse_args(argv)


def check_text_artifact(
    *,
    artifact_name: str,
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
                    artifact=artifact_name,
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
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
                check_id="M247-C017-PKG-00",
                detail=f"required document is missing: {display_path(path)}",
            )
        )
        return checks_total, findings

    try:
        package_text = path.read_text(encoding="utf-8")
        payload = json.loads(package_text)
    except (OSError, json.JSONDecodeError) as exc:
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id="M247-C017-PKG-00",
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
                check_id="M247-C017-PKG-00",
                detail='expected top-level "scripts" object in package.json',
            )
        )
        return checks_total, findings

    for check in PACKAGE_SCRIPT_KEY_CHECKS:
        checks_total += 1
        if check.script_key not in scripts:
            findings.append(
                Finding(
                    artifact="package_json",
                    check_id=check.check_id,
                    detail=f'expected scripts["{check.script_key}"] to exist',
                )
            )

    for check in PACKAGE_SCRIPT_CHECKS:
        checks_total += 1
        actual = scripts.get(check.script_key)
        if actual != check.expected_value:
            findings.append(
                Finding(
                    artifact="package_json",
                    check_id=check.check_id,
                    detail=f'expected scripts["{check.script_key}"] to equal "{check.expected_value}"',
                )
            )

    for forbidden in PACKAGE_FORBIDDEN_SNIPPETS:
        checks_total += 1
        if forbidden.snippet in package_text:
            findings.append(
                Finding(
                    artifact="package_json",
                    check_id=forbidden.check_id,
                    detail=f"forbidden snippet present: {forbidden.snippet}",
                )
            )
    return checks_total, findings


def write_summary(summary_path: Path, payload: object) -> tuple[bool, str]:
    try:
        summary_path.parent.mkdir(parents=True, exist_ok=True)
        summary_path.write_text(canonical_json(payload), encoding="utf-8")
    except OSError as exc:
        return False, f"unable to write summary file: {exc}"
    return True, ""


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for artifact_name, path, exists_check_id, snippets in (
        ("expectations_doc", args.expectations_doc, "M247-C017-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        ("packet_doc", args.packet_doc, "M247-C017-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        ("c016_expectations_doc", args.c016_expectations_doc, "M247-C017-C016-DOC-EXISTS", C016_EXPECTATIONS_SNIPPETS),
        ("c016_packet_doc", args.c016_packet_doc, "M247-C017-C016-PKT-EXISTS", C016_PACKET_SNIPPETS),
        ("readiness_runner", args.readiness_runner, "M247-C017-RUN-EXISTS", READINESS_SNIPPETS),
        ("c016_readiness_runner", args.c016_readiness_runner, "M247-C017-C016-RUN-EXISTS", C016_READINESS_SNIPPETS),
    ):
        count, findings = check_text_artifact(
            artifact_name=artifact_name,
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.c016_checker, "M247-C017-DEP-C016-ARG-01"),
        (args.c016_test, "M247-C017-DEP-C016-ARG-02"),
        (args.c016_readiness_runner, "M247-C017-DEP-C016-ARG-03"),
    ):
        count, findings = check_dependency_path(path, check_id)
        checks_total += count
        failures.extend(findings)

    package_checks, package_findings = check_package_contract(args.package_json)
    checks_total += package_checks
    failures.extend(package_findings)

    failures = sorted(failures, key=lambda finding: (finding.artifact, finding.check_id, finding.detail))
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(failures),
        "failures": [
            {"artifact": failure.artifact, "check_id": failure.check_id, "detail": failure.detail}
            for failure in failures
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_ok, summary_error = write_summary(summary_path, summary_payload)
    if not summary_ok:
        print(
            f"[M247-C017-SUMMARY-WRITE-01] {display_path(summary_path)}: {summary_error}",
            file=sys.stderr,
        )
        return 1

    if args.emit_json:
        print(canonical_json(summary_payload), end="")

    if failures:
        if not args.emit_json:
            for finding in failures:
                print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    if not args.emit_json:
        print(
            f"[ok] {MODE}: {summary_payload['checks_passed']}/{summary_payload['checks_total']} checks passed"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))



