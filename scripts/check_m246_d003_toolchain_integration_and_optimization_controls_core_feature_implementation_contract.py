#!/usr/bin/env python3
"""Fail-closed checker for M246-D003 toolchain integration/optimization controls core feature implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m246-d003-toolchain-integration-optimization-controls-core-feature-implementation-contract-v1"

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m246_toolchain_integration_and_optimization_controls_core_feature_implementation_d003_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m246"
    / "m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_packet.md"
)
DEFAULT_READINESS_SCRIPT = ROOT / "scripts" / "run_m246_d003_lane_d_readiness.py"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m246/M246-D003/toolchain_integration_optimization_controls_core_feature_implementation_contract_summary.json"
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
        "M246-D003-DOC-EXP-01",
        "# M246 Toolchain Integration and Optimization Controls Core Feature Implementation Expectations (D003)",
    ),
    SnippetCheck(
        "M246-D003-DOC-EXP-02",
        "Contract ID: `objc3c-toolchain-integration-optimization-controls-core-feature-implementation/m246-d003-v1`",
    ),
    SnippetCheck("M246-D003-DOC-EXP-03", "Issue `#5108` defines canonical lane-D core feature implementation scope."),
    SnippetCheck("M246-D003-DOC-EXP-04", "Dependencies: `M246-D002`"),
    SnippetCheck(
        "M246-D003-DOC-EXP-05",
        "`M246-D002` is a pending seeded lane-D modular split/scaffolding dependency token and must remain explicit until seed assets land.",
    ),
    SnippetCheck(
        "M246-D003-DOC-EXP-06",
        "scripts/check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py",
    ),
    SnippetCheck(
        "M246-D003-DOC-EXP-07",
        "tests/tooling/test_check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py",
    ),
    SnippetCheck(
        "M246-D003-DOC-EXP-08",
        "python scripts/run_m246_d003_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M246-D003-DOC-EXP-09",
        "python scripts/check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py --emit-json",
    ),
    SnippetCheck(
        "M246-D003-DOC-EXP-10",
        "code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
    ),
    SnippetCheck(
        "M246-D003-DOC-EXP-11",
        "tmp/reports/m246/M246-D003/toolchain_integration_optimization_controls_core_feature_implementation_contract_summary.json",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M246-D003-DOC-PKT-01",
        "# M246-D003 Toolchain Integration and Optimization Controls Core Feature Implementation Packet",
    ),
    SnippetCheck("M246-D003-DOC-PKT-02", "Packet: `M246-D003`"),
    SnippetCheck("M246-D003-DOC-PKT-03", "Issue: `#5108`"),
    SnippetCheck("M246-D003-DOC-PKT-04", "Dependencies: `M246-D002`"),
    SnippetCheck("M246-D003-DOC-PKT-05", "Freeze date: `2026-03-04`"),
    SnippetCheck("M246-D003-DOC-PKT-06", "Pending seeded dependency tokens:"),
    SnippetCheck(
        "M246-D003-DOC-PKT-07",
        "m246_d002_toolchain_integration_and_optimization_controls_modular_split_scaffolding_packet.md",
    ),
    SnippetCheck(
        "M246-D003-DOC-PKT-08",
        "scripts/run_m246_d003_lane_d_readiness.py",
    ),
    SnippetCheck(
        "M246-D003-DOC-PKT-09",
        "python -m pytest tests/tooling/test_check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py -q",
    ),
    SnippetCheck(
        "M246-D003-DOC-PKT-10",
        "tmp/reports/m246/M246-D003/toolchain_integration_optimization_controls_core_feature_implementation_contract_summary.json",
    ),
    SnippetCheck(
        "M246-D003-DOC-PKT-11",
        "python scripts/check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py --emit-json",
    ),
)

READINESS_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck("M246-D003-RDY-01", 'DEPENDENCY_TOKEN = "M246-D002"'),
    SnippetCheck(
        "M246-D003-RDY-02",
        "scripts/check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py",
    ),
    SnippetCheck(
        "M246-D003-RDY-03",
        "tests/tooling/test_check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py",
    ),
    SnippetCheck("M246-D003-RDY-04", "[ok] M246-D003 lane-D readiness chain completed"),
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
    parser.add_argument("--summary-out", type=Path, default=DEFAULT_SUMMARY_OUT)
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
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


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    checks_total = 0
    failures: list[Finding] = []

    for path, exists_check_id, snippets in (
        (args.expectations_doc, "M246-D003-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M246-D003-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.readiness_script, "M246-D003-RDY-EXISTS", READINESS_SNIPPETS),
    ):
        count, findings = check_text_artifact(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += count
        failures.extend(findings)

    failures = sorted(failures, key=lambda failure: (failure.check_id, failure.artifact, failure.detail))
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

    if args.emit_json:
        print(canonical_json(summary_payload), end="")

    if failures:
        if not args.emit_json:
            print(f"{MODE}: contract drift detected ({len(failures)} failed check(s)).", file=sys.stderr)
            for finding in failures:
                print(f"- [{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1

    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
