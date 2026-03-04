#!/usr/bin/env python3
"""Fail-closed checker for M246-D002 toolchain integration/optimization controls modular split and scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-d002-toolchain-integration-optimization-controls-modular-split-and-scaffolding-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_d002_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_d002_lane_d_readiness.py"
DEFAULT_D001_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_toolchain_integration_and_optimization_controls_contract_and_architecture_freeze_d001_expectations.md"
)
DEFAULT_D001_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_d001_toolchain_integration_and_optimization_controls_contract_and_architecture_freeze_packet.md"
)
DEFAULT_D001_CHECKER = (
    ROOT / "scripts" / "check_m246_d001_toolchain_integration_and_optimization_controls_contract_and_architecture_freeze_contract.py"
)
DEFAULT_D001_TEST = (
    ROOT
    / "tests"
    / "tooling"
    / "test_check_m246_d001_toolchain_integration_and_optimization_controls_contract_and_architecture_freeze_contract.py"
)
DEFAULT_D003_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_d003_lane_d_readiness.py"
DEFAULT_D004_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_d004_lane_d_readiness.py"
DEFAULT_D005_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_d005_lane_d_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-D002/toolchain_integration_optimization_controls_modular_split_and_scaffolding_contract_summary.json"
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
        "M246-D002-DOC-EXP-01",
        "# M246 Toolchain Integration and Optimization Controls Modular Split and Scaffolding Expectations (D002)",
    ),
    SnippetCheck(
        "M246-D002-DOC-EXP-02",
        "Contract ID: `objc3c-toolchain-integration-optimization-controls-modular-split-and-scaffolding/m246-d002-v1`",
    ),
    SnippetCheck(
        "M246-D002-DOC-EXP-03",
        "Issue `#5107` defines canonical lane-D modular split and scaffolding scope.",
    ),
    SnippetCheck("M246-D002-DOC-EXP-04", "Dependencies: `M246-D001`"),
    SnippetCheck(
        "M246-D002-DOC-EXP-05",
        "scripts/check_m246_d001_toolchain_integration_and_optimization_controls_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck(
        "M246-D002-DOC-EXP-06",
        "scripts/run_m246_d002_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M246-D002-DOC-EXP-07",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M246-D002-DOC-EXP-08",
        "scripts/run_m246_d003_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M246-D002-DOC-EXP-09",
        "tests/tooling/test_check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_contract.py",
    ),
    SnippetCheck(
        "M246-D002-DOC-EXP-10",
        "tmp/reports/m246/M246-D002/toolchain_integration_optimization_controls_modular_split_and_scaffolding_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-D002-DOC-PKT-01",
        "# M246-D002 Toolchain Integration and Optimization Controls Modular Split and Scaffolding Packet",
    ),
    SnippetCheck("M246-D002-DOC-PKT-02", "Packet: `M246-D002`"),
    SnippetCheck("M246-D002-DOC-PKT-03", "Issue: `#5107`"),
    SnippetCheck("M246-D002-DOC-PKT-04", "Dependencies: `M246-D001`"),
    SnippetCheck("M246-D002-DOC-PKT-05", "Freeze date: `2026-03-04`"),
    SnippetCheck("M246-D002-DOC-PKT-06", "Dependency anchors from `M246-D001`:"),
    SnippetCheck(
        "M246-D002-DOC-PKT-07",
        "scripts/run_m246_d002_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M246-D002-DOC-PKT-08",
        "python -m pytest tests/tooling/test_check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_contract.py -q",
    ),
    SnippetCheck(
        "M246-D002-DOC-PKT-09",
        "Forward compatibility anchors for existing chain:",
    ),
    SnippetCheck(
        "M246-D002-DOC-PKT-10",
        "tmp/reports/m246/M246-D002/toolchain_integration_optimization_controls_modular_split_and_scaffolding_contract_summary.json",
    ),
)

D001_EXPECTATIONS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-D002-D001-DOC-01",
        "Contract ID: `objc3c-toolchain-integration-optimization-controls/m246-d001-v1`",
    ),
    SnippetCheck(
        "M246-D002-D001-DOC-02",
        "Issue `#5106` defines canonical lane-D contract freeze scope.",
    ),
    SnippetCheck("M246-D002-D001-DOC-03", "Dependencies: none"),
)

D001_PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-D002-D001-PKT-01", "Packet: `M246-D001`"),
    SnippetCheck("M246-D002-D001-PKT-02", "Issue: `#5106`"),
    SnippetCheck("M246-D002-D001-PKT-03", "Dependencies: none"),
)

READINESS_REQUIRED_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-D002-RDY-01", 'DEPENDENCY_TOKEN = "M246-D001"'),
    SnippetCheck(
        "M246-D002-RDY-02",
        "scripts/check_m246_d001_toolchain_integration_and_optimization_controls_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck(
        "M246-D002-RDY-03",
        "tests/tooling/test_check_m246_d001_toolchain_integration_and_optimization_controls_contract_and_architecture_freeze_contract.py",
    ),
    SnippetCheck(
        "M246-D002-RDY-04",
        "scripts/check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_contract.py",
    ),
    SnippetCheck(
        "M246-D002-RDY-05",
        "tests/tooling/test_check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_contract.py",
    ),
    SnippetCheck("M246-D002-RDY-06", "[ok] M246-D002 lane-D readiness chain completed"),
)

READINESS_FORBIDDEN_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-D002-RDY-07",
        "scripts/run_m246_d003_lane_d_readiness.py",
    ),
)

D003_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-D002-D003-RDY-01", 'DEPENDENCY_TOKEN = "M246-D002"'),
    SnippetCheck("M246-D002-D003-RDY-02", "[ok] M246-D003 lane-D readiness chain completed"),
)

D004_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-D002-D004-RDY-01", 'DEPENDENCY_TOKEN = "M246-D003"'),
    SnippetCheck("M246-D002-D004-RDY-02", "scripts/run_m246_d003_lane_d_readiness.py"),
    SnippetCheck("M246-D002-D004-RDY-03", "[ok] M246-D004 lane-D readiness chain completed"),
)

D005_READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-D002-D005-RDY-01", 'DEPENDENCY_TOKEN = "M246-D004"'),
    SnippetCheck("M246-D002-D005-RDY-02", "scripts/run_m246_d004_lane_d_readiness.py"),
    SnippetCheck("M246-D002-D005-RDY-03", "[ok] M246-D005 lane-D readiness chain completed"),
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
    parser.add_argument("--d001-expectations-doc", type=Path, default=DEFAULT_D001_EXPECTATIONS_DOC)
    parser.add_argument("--d001-packet-doc", type=Path, default=DEFAULT_D001_PACKET_DOC)
    parser.add_argument("--d001-checker", type=Path, default=DEFAULT_D001_CHECKER)
    parser.add_argument("--d001-test", type=Path, default=DEFAULT_D001_TEST)
    parser.add_argument("--d003-readiness-script", type=Path, default=DEFAULT_D003_READINESS_SCRIPT)
    parser.add_argument("--d004-readiness-script", type=Path, default=DEFAULT_D004_READINESS_SCRIPT)
    parser.add_argument("--d005-readiness-script", type=Path, default=DEFAULT_D005_READINESS_SCRIPT)
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    return parser.parse_args(argv)


def check_text_artifact(
    *,
    path: Path,
    exists_check_id: str,
    required_snippets: tuple[SnippetCheck, ...],
    forbidden_snippets: tuple[SnippetCheck, ...] = (),
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

    for snippet in required_snippets:
        checks_total += 1
        if snippet.snippet not in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"missing required snippet: {snippet.snippet}",
                )
            )

    for snippet in forbidden_snippets:
        checks_total += 1
        if snippet.snippet in text:
            findings.append(
                Finding(
                    artifact=display_path(path),
                    check_id=snippet.check_id,
                    detail=f"forbidden snippet present: {snippet.snippet}",
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


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, required_snippets, forbidden_snippets in (
        (args.expectations_doc, "M246-D002-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS, ()),
        (args.packet_doc, "M246-D002-DOC-PKT-EXISTS", PACKET_SNIPPETS, ()),
        (args.d001_expectations_doc, "M246-D002-D001-DOC-EXISTS", D001_EXPECTATIONS_SNIPPETS, ()),
        (args.d001_packet_doc, "M246-D002-D001-PKT-EXISTS", D001_PACKET_SNIPPETS, ()),
        (
            args.readiness_script,
            "M246-D002-RDY-EXISTS",
            READINESS_REQUIRED_SNIPPETS,
            READINESS_FORBIDDEN_SNIPPETS,
        ),
        (args.d003_readiness_script, "M246-D002-D003-RDY-EXISTS", D003_READINESS_SNIPPETS, ()),
        (args.d004_readiness_script, "M246-D002-D004-RDY-EXISTS", D004_READINESS_SNIPPETS, ()),
        (args.d005_readiness_script, "M246-D002-D005-RDY-EXISTS", D005_READINESS_SNIPPETS, ()),
    ):
        count, findings = check_text_artifact(
            path=path,
            exists_check_id=exists_check_id,
            required_snippets=required_snippets,
            forbidden_snippets=forbidden_snippets,
        )
        checks_total += count
        failures.extend(findings)

    for path, check_id in (
        (args.d001_checker, "M246-D002-DEP-D001-ARG-01"),
        (args.d001_test, "M246-D002-DEP-D001-ARG-02"),
    ):
        count, findings = check_dependency_path(path, check_id)
        checks_total += count
        failures.extend(findings)

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
