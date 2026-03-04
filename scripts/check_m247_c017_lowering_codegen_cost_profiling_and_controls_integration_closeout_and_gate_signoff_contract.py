#!/usr/bin/env python3
"""Fail-closed checker for M247-C017 lane-C integration closeout and gate sign-off."""

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
DEFAULT_C016_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_c016_lane_c_readiness.py"
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_c017_lane_c_readiness.py"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-C017/lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_summary.json"
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
        "M247-C017-DOC-EXP-01",
        "# M247 Lane C Lowering/Codegen Cost Profiling and Controls Integration Closeout and Gate Sign-Off Expectations (C017)",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-02",
        "Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-controls-integration-closeout-and-gate-signoff/m247-c017-v1`",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-03",
        "Issue `#6758` defines canonical lane-C integration closeout and gate sign-off scope.",
    ),
    SnippetCheck("M247-C017-DOC-EXP-04", "Dependencies: `M247-C016`"),
    SnippetCheck(
        "M247-C017-DOC-EXP-05",
        "Predecessor anchors inherited via `M247-C016`: `M247-C001`, `M247-C002`, `M247-C003`, `M247-C004`, `M247-C005`, `M247-C006`, `M247-C007`, `M247-C008`, `M247-C009`, `M247-C010`, `M247-C011`, `M247-C012`, `M247-C013`, `M247-C014`, `M247-C015`.",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-06",
        "docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_c016_expectations.md",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-07",
        "scripts/run_m247_c016_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-08",
        "scripts/check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-09",
        "tests/tooling/test_check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-10",
        "Integration-closeout-and-gate-signoff command",
    ),
    SnippetCheck(
        "M247-C017-DOC-EXP-11",
        "tmp/reports/m247/M247-C017/lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_summary.json",
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
    SnippetCheck(
        "M247-C017-DOC-PKT-06",
        "Predecessor anchors inherited via `M247-C016`: `M247-C001`, `M247-C002`, `M247-C003`, `M247-C004`, `M247-C005`, `M247-C006`, `M247-C007`, `M247-C008`, `M247-C009`, `M247-C010`, `M247-C011`, `M247-C012`, `M247-C013`, `M247-C014`, `M247-C015`.",
    ),
    SnippetCheck(
        "M247-C017-DOC-PKT-07",
        "scripts/check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M247-C017-DOC-PKT-08",
        "tests/tooling/test_check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M247-C017-DOC-PKT-09",
        "scripts/run_m247_c017_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C017-DOC-PKT-10",
        "scripts/run_m247_c016_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C017-DOC-PKT-11",
        "tmp/reports/m247/M247-C017/lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_summary.json",
    ),
)

C016_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C017-C016-EXP-01",
        "Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-controls-advanced-edge-compatibility-workpack-shard-1/m247-c016-v1`",
    ),
    SnippetCheck(
        "M247-C017-C016-EXP-02",
        "Dependencies: `M247-C015`",
    ),
    SnippetCheck(
        "M247-C017-C016-EXP-03",
        "Issue `#6757` defines canonical lane-C advanced edge compatibility workpack (shard 1) scope.",
    ),
)

C016_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-C017-C016-PKT-01", "Packet: `M247-C016`"),
    SnippetCheck("M247-C017-C016-PKT-02", "Issue: `#6757`"),
    SnippetCheck("M247-C017-C016-PKT-03", "Dependencies: `M247-C015`"),
)

C016_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C017-C016-RUN-01",
        "scripts/run_m247_c015_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C017-C016-RUN-02",
        "scripts/check_m247_c016_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_contract.py",
    ),
    SnippetCheck(
        "M247-C017-C016-RUN-03",
        "tests/tooling/test_check_m247_c016_lowering_codegen_cost_profiling_and_controls_advanced_edge_compatibility_workpack_shard_1_contract.py",
    ),
    SnippetCheck(
        "M247-C017-C016-RUN-04",
        "[ok] M247-C016 lane-C readiness chain completed",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C017-RUN-01",
        "scripts/run_m247_c016_lane_c_readiness.py",
    ),
    SnippetCheck(
        "M247-C017-RUN-02",
        "scripts/check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M247-C017-RUN-03",
        "tests/tooling/test_check_m247_c017_lowering_codegen_cost_profiling_and_controls_integration_closeout_and_gate_signoff_contract.py",
    ),
    SnippetCheck(
        "M247-C017-RUN-04",
        "[ok] M247-C017 lane-C readiness chain completed",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-C017-PKG-01", '"compile:objc3c": '),
    SnippetCheck("M247-C017-PKG-02", '"proof:objc3c": '),
    SnippetCheck("M247-C017-PKG-03", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M247-C017-PKG-04", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--c016-expectations-doc", type=Path, default=DEFAULT_C016_EXPECTATIONS_DOC)
    parser.add_argument("--c016-packet-doc", type=Path, default=DEFAULT_C016_PACKET_DOC)
    parser.add_argument("--c016-checker", type=Path, default=DEFAULT_C016_CHECKER)
    parser.add_argument("--c016-test", type=Path, default=DEFAULT_C016_TEST)
    parser.add_argument("--c016-readiness-script", type=Path, default=DEFAULT_C016_READINESS_SCRIPT)
    parser.add_argument("--readiness-script", type=Path, default=DEFAULT_READINESS_SCRIPT)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_text_artifact(
    *,
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

    for snippet_check in snippets:
        checks_total += 1
        if snippet_check.snippet not in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet_check.check_id,
                    detail=(
                        f"missing required snippet: {snippet_check.snippet}"
                    ),
                )
            )
    return checks_total, findings


def check_file_presence(path: Path, check_id: str) -> tuple[int, list[Finding]]:
    checks_total = 1
    findings: list[Finding] = []
    if not path.exists():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=check_id,
                detail=f"required path is missing: {display_path(path)}",
            )
        )
    elif not path.is_file():
        findings.append(
            Finding(
                artifact=display_path(path),
                check_id=check_id,
                detail=f"required path is not a file: {display_path(path)}",
            )
        )
    return checks_total, findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    checks_total = 0
    findings: list[Finding] = []

    text_checks = (
        (args.expectations_doc, "M247-C017-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-C017-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.c016_expectations_doc, "M247-C017-C016-EXP-EXISTS", C016_EXPECTATIONS_SNIPPETS),
        (args.c016_packet_doc, "M247-C017-C016-PKT-EXISTS", C016_PACKET_SNIPPETS),
        (args.c016_readiness_script, "M247-C017-C016-RUN-EXISTS", C016_READINESS_SNIPPETS),
        (args.readiness_script, "M247-C017-RUN-EXISTS", READINESS_SNIPPETS),
        (args.package_json, "M247-C017-PKG-EXISTS", PACKAGE_SNIPPETS),
    )

    for path, exists_check_id, snippets in text_checks:
        artifact_checks, artifact_findings = check_text_artifact(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += artifact_checks
        findings.extend(artifact_findings)

    file_checks = (
        (args.c016_checker, "M247-C017-DEP-C016-ARG-01"),
        (args.c016_test, "M247-C017-DEP-C016-ARG-02"),
    )
    for path, check_id in file_checks:
        artifact_checks, artifact_findings = check_file_presence(path, check_id)
        checks_total += artifact_checks
        findings.extend(artifact_findings)

    checks_passed = checks_total - len(findings)
    ok = not findings

    summary_payload = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [finding.__dict__ for finding in findings],
    }

    summary_out = args.summary_out
    if not summary_out.is_absolute():
        summary_out = ROOT / summary_out
    summary_out.parent.mkdir(parents=True, exist_ok=True)
    summary_out.write_text(canonical_json(summary_payload), encoding="utf-8")

    stream = sys.stdout if ok else sys.stderr
    status = "ok" if ok else "error"
    print(f"[{status}] {MODE}: {checks_passed}/{checks_total} checks passed", file=stream)
    for finding in findings:
        print(
            f"[{finding.check_id}] {finding.artifact}: {finding.detail}",
            file=stream,
        )

    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
