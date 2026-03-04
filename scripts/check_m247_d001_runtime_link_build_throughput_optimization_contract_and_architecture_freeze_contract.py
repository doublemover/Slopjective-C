#!/usr/bin/env python3
"""Fail-closed checker for M247-D001 runtime/link/build throughput optimization contract and architecture freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-d001-runtime-link-build-throughput-optimization-contract-and-architecture-freeze-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_runtime_link_build_throughput_optimization_contract_and_architecture_freeze_d001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_d001_runtime_link_build_throughput_optimization_contract_and_architecture_freeze_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_d001_lane_d_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-D001/runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract_summary.json"
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
        "M247-D001-DOC-EXP-01",
        "# M247 Runtime/Link/Build Throughput Optimization Contract and Architecture Freeze Expectations (D001)",
    ),
    SnippetCheck(
        "M247-D001-DOC-EXP-02",
        "Contract ID: `objc3c-runtime-link-build-throughput-optimization-contract-and-architecture-freeze/m247-d001-v1`",
    ),
    SnippetCheck(
        "M247-D001-DOC-EXP-03",
        "Issue `#6759` defines canonical lane-D contract and architecture freeze scope.",
    ),
    SnippetCheck("M247-D001-DOC-EXP-04", "Dependencies: none"),
    SnippetCheck(
        "M247-D001-DOC-EXP-05",
        "scripts/check_m247_d001_runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck(
        "M247-D001-DOC-EXP-06",
        "scripts/run_m247_d001_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M247-D001-DOC-EXP-07",
        "Code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M247-D001-DOC-EXP-08",
        "Readiness chain order: `D001 checker -> D001 pytest`.",
    ),
    SnippetCheck(
        "M247-D001-DOC-EXP-09",
        "tests/tooling/test_check_m247_d001_runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck(
        "M247-D001-DOC-EXP-10",
        "tmp/reports/m247/M247-D001/runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-D001-DOC-PKT-01",
        "# M247-D001 Runtime/Link/Build Throughput Optimization Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M247-D001-DOC-PKT-02", "Packet: `M247-D001`"),
    SnippetCheck("M247-D001-DOC-PKT-03", "Issue: `#6759`"),
    SnippetCheck("M247-D001-DOC-PKT-04", "Dependencies: none"),
    SnippetCheck("M247-D001-DOC-PKT-05", "Freeze date: `2026-03-04`"),
    SnippetCheck(
        "M247-D001-DOC-PKT-06",
        "scripts/check_m247_d001_runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck(
        "M247-D001-DOC-PKT-07",
        "scripts/run_m247_d001_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M247-D001-DOC-PKT-08",
        "python -m pytest tests/tooling/test_check_m247_d001_runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract.py -q",
    ),
    SnippetCheck("M247-D001-DOC-PKT-09", "D001 checker -> D001 pytest"),
    SnippetCheck(
        "M247-D001-DOC-PKT-10",
        "tmp/reports/m247/M247-D001/runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract_summary.json",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-D001-RDY-01", 'DEPENDENCY_TOKEN = "none"'),
    SnippetCheck(
        "M247-D001-RDY-02",
        "scripts/check_m247_d001_runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck(
        "M247-D001-RDY-03",
        "tests/tooling/test_check_m247_d001_runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck(
        "M247-D001-RDY-04",
        "dependency continuity token: none (M247-D001 contract-freeze baseline)",
    ),
    SnippetCheck("M247-D001-RDY-05", "[ok] M247-D001 lane-D readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-D001-ARCH-01",
        "M247 lane-E E001 performance SLO gate/reporting anchors dependency references",
    ),
    SnippetCheck(
        "M247-D001-ARCH-02",
        "`M247-A001`, `M247-B001`, `M247-C001`, `M247-D001`",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-D001-SPC-01",
        "performance SLO gate/reporting wiring shall preserve explicit lane-E",
    ),
    SnippetCheck(
        "M247-D001-SPC-02",
        "dependency anchors (`M247-A001`, `M247-B001`, `M247-C001`, `M247-D001`)",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-D001-META-01",
        "deterministic lane-E performance SLO dependency anchors for `M247-A001`, `M247-B001`,",
    ),
    SnippetCheck(
        "M247-D001-META-02",
        "`M247-C001`, and `M247-D001`, including pending-lane tokens needed",
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
    parser.add_argument("--readiness-script", type=Path, default=DEFAULT_READINESS_SCRIPT)
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
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

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M247-D001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-D001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.readiness_script, "M247-D001-RDY-EXISTS", READINESS_SNIPPETS),
        (args.architecture_doc, "M247-D001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M247-D001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M247-D001-META-EXISTS", METADATA_SPEC_SNIPPETS),
    ):
        count, findings = check_text_artifact(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    failures = sorted(
        failures,
        key=lambda finding: (finding.artifact, finding.check_id, finding.detail),
    )
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
    summary_ok, summary_error = write_summary(summary_path, summary_payload)
    if not summary_ok:
        print(
            f"[M247-D001-SUMMARY-WRITE-01] {display_path(summary_path)}: {summary_error}",
            file=sys.stderr,
        )
        return 1

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
