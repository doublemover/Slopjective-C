#!/usr/bin/env python3
"""Fail-closed checker for M247-C001 lowering/codegen cost profiling and controls freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m247-c001-lowering-codegen-cost-profiling-controls-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m247_lane_c_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_c001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m247"
    / "m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m247_c001_lane_c_readiness.py"
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m247/M247-C001/lowering_codegen_cost_profiling_and_controls_contract_summary.json"
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
        "M247-C001-DOC-EXP-01",
        "# M247 Lane C Lowering/Codegen Cost Profiling and Controls Contract and Architecture Freeze Expectations (C001)",
    ),
    SnippetCheck(
        "M247-C001-DOC-EXP-02",
        "Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-controls-contract/m247-c001-v1`",
    ),
    SnippetCheck("M247-C001-DOC-EXP-03", "Dependencies: none"),
    SnippetCheck(
        "M247-C001-DOC-EXP-04",
        "Issue `#6742` defines canonical lane-C contract and architecture freeze scope.",
    ),
    SnippetCheck(
        "M247-C001-DOC-EXP-05",
        "milestone optimization improvements as mandatory scope",
    ),
    SnippetCheck(
        "M247-C001-DOC-EXP-06",
        "`check:objc3c:m247-c001-lowering-codegen-cost-profiling-controls-contract`",
    ),
    SnippetCheck("M247-C001-DOC-EXP-07", "`check:objc3c:m247-c001-lane-c-readiness`"),
    SnippetCheck(
        "M247-C001-DOC-EXP-08",
        "`tmp/reports/m247/M247-C001/lowering_codegen_cost_profiling_and_controls_contract_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C001-DOC-PKT-01",
        "# M247-C001 Lowering/Codegen Cost Profiling and Controls Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M247-C001-DOC-PKT-02", "Packet: `M247-C001`"),
    SnippetCheck("M247-C001-DOC-PKT-03", "Issue: `#6742`"),
    SnippetCheck("M247-C001-DOC-PKT-04", "Dependencies: none"),
    SnippetCheck(
        "M247-C001-DOC-PKT-05",
        "`scripts/check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py`",
    ),
    SnippetCheck(
        "M247-C001-DOC-PKT-06",
        "`scripts/run_m247_c001_lane_c_readiness.py`",
    ),
    SnippetCheck("M247-C001-DOC-PKT-07", "`compile:objc3c`"),
    SnippetCheck("M247-C001-DOC-PKT-08", "`proof:objc3c`"),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M247-C001-RUN-01", "dependency continuity token: none"),
    SnippetCheck(
        "M247-C001-RUN-02",
        "scripts/check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck(
        "M247-C001-RUN-03",
        "tests/tooling/test_check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck("M247-C001-RUN-04", "[ok] M247-C001 lane-C readiness chain completed"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C001-ARCH-01",
        "M247 lane-C C001 lowering/codegen cost profiling and controls contract-freeze anchors",
    ),
    SnippetCheck(
        "M247-C001-ARCH-02",
        "docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_c001_expectations.md",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C001-SPC-01",
        "lowering/codegen cost profiling and controls contract-freeze wiring shall preserve explicit lane-C dependency anchors (`none`)",
    ),
    SnippetCheck(
        "M247-C001-SPC-02",
        "compile-route proof hooks, or perf-budget evidence commands drift",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C001-META-01",
        "deterministic lane-C lowering/codegen cost profiling and controls metadata anchors for `M247-C001`",
    ),
    SnippetCheck(
        "M247-C001-META-02",
        "explicit dependency token (`none`) and fail-closed cost-profile evidence continuity",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M247-C001-PKG-01",
        '"check:objc3c:m247-c001-lowering-codegen-cost-profiling-controls-contract": '
        '"python scripts/check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py"',
    ),
    SnippetCheck(
        "M247-C001-PKG-02",
        '"test:tooling:m247-c001-lowering-codegen-cost-profiling-controls-contract": '
        '"python -m pytest tests/tooling/test_check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py -q"',
    ),
    SnippetCheck(
        "M247-C001-PKG-03",
        '"check:objc3c:m247-c001-lane-c-readiness": '
        '"python scripts/run_m247_c001_lane_c_readiness.py"',
    ),
    SnippetCheck("M247-C001-PKG-04", '"compile:objc3c": '),
    SnippetCheck("M247-C001-PKG-05", '"proof:objc3c": '),
    SnippetCheck("M247-C001-PKG-06", '"test:objc3c:execution-replay-proof": '),
    SnippetCheck("M247-C001-PKG-07", '"test:objc3c:perf-budget": '),
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
    parser.add_argument("--architecture-doc", type=Path, default=DEFAULT_ARCHITECTURE_DOC)
    parser.add_argument("--lowering-spec", type=Path, default=DEFAULT_LOWERING_SPEC)
    parser.add_argument("--metadata-spec", type=Path, default=DEFAULT_METADATA_SPEC)
    parser.add_argument("--package-json", type=Path, default=DEFAULT_PACKAGE_JSON)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_doc_contract(
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
        (args.expectations_doc, "M247-C001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M247-C001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.readiness_script, "M247-C001-RUN-EXISTS", READINESS_SNIPPETS),
        (args.architecture_doc, "M247-C001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M247-C001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M247-C001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M247-C001-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        count, findings = check_doc_contract(path=path, exists_check_id=exists_check_id, snippets=snippets)
        checks_total += count
        failures.extend(findings)

    checks_passed = checks_total - len(failures)
    summary_payload = {
        "mode": MODE,
        "ok": not failures,
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

