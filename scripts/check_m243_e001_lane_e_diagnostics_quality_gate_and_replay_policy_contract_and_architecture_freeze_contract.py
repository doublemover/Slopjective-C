#!/usr/bin/env python3
"""Fail-closed contract checker for M243-E001 diagnostics quality gate/replay policy freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-e001-lane-e-diagnostics-quality-gate-replay-policy-"
    "contract-architecture-freeze-contract-v1"
)

DEFAULT_EXPECTATIONS_DOC = (
    ROOT
    / "docs"
    / "contracts"
    / "m243_lane_e_diagnostics_quality_gate_and_replay_policy_contract_and_architecture_freeze_e001_expectations.md"
)
DEFAULT_PACKET_DOC = (
    ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_e001_lane_e_diagnostics_quality_gate_and_replay_policy_contract_and_architecture_freeze_packet.md"
)
DEFAULT_ARCHITECTURE_DOC = ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md"
DEFAULT_LOWERING_SPEC = ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md"
DEFAULT_METADATA_SPEC = ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md"
DEFAULT_PACKAGE_JSON = ROOT / "package.json"
DEFAULT_SUMMARY_OUT = Path(
    "tmp/reports/m243/M243-E001/lane_e_diagnostics_quality_gate_replay_policy_contract_architecture_freeze_summary.json"
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
        "M243-E001-DOC-EXP-01",
        "# M243 Lane E Diagnostics Quality Gate and Replay Policy Contract and Architecture Freeze Expectations (E001)",
    ),
    SnippetCheck(
        "M243-E001-DOC-EXP-02",
        "Contract ID: `objc3c-lane-e-diagnostics-quality-gate-replay-policy-contract-architecture-freeze/m243-e001-v1`",
    ),
    SnippetCheck("M243-E001-DOC-EXP-03", "`M243-A001`"),
    SnippetCheck("M243-E001-DOC-EXP-04", "`M243-B001`"),
    SnippetCheck("M243-E001-DOC-EXP-05", "`M243-C001`"),
    SnippetCheck("M243-E001-DOC-EXP-06", "`M243-D001`"),
    SnippetCheck(
        "M243-E001-DOC-EXP-07",
        "diagnostics quality gate and replay policy",
    ),
    SnippetCheck(
        "M243-E001-DOC-EXP-08",
        "`check:objc3c:m243-e001-lane-e-diagnostics-quality-gate-replay-policy-contract-architecture-freeze-contract`",
    ),
    SnippetCheck(
        "M243-E001-DOC-EXP-09",
        "`check:objc3c:m243-e001-lane-e-readiness`",
    ),
    SnippetCheck(
        "M243-E001-DOC-EXP-10",
        "`tmp/reports/m243/M243-E001/lane_e_diagnostics_quality_gate_replay_policy_contract_architecture_freeze_summary.json`",
    ),
)

PACKET_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E001-DOC-PKT-01",
        "# M243-E001 Lane-E Diagnostics Quality Gate and Replay Policy Contract and Architecture Freeze Packet",
    ),
    SnippetCheck("M243-E001-DOC-PKT-02", "Packet: `M243-E001`"),
    SnippetCheck(
        "M243-E001-DOC-PKT-03",
        "Dependencies: `M243-A001`, `M243-B001`, `M243-C001`, `M243-D001`",
    ),
    SnippetCheck(
        "M243-E001-DOC-PKT-04",
        "`scripts/check_m243_e001_lane_e_diagnostics_quality_gate_and_replay_policy_contract_and_architecture_freeze_contract.py`",
    ),
    SnippetCheck(
        "M243-E001-DOC-PKT-05",
        "`tests/tooling/test_check_m243_e001_lane_e_diagnostics_quality_gate_and_replay_policy_contract_and_architecture_freeze_contract.py`",
    ),
    SnippetCheck(
        "M243-E001-DOC-PKT-06",
        "including code/spec anchors and milestone optimization",
    ),
    SnippetCheck("M243-E001-DOC-PKT-07", "`test:objc3c:diagnostics-replay-proof`"),
    SnippetCheck("M243-E001-DOC-PKT-08", "`test:objc3c:execution-replay-proof`"),
)

ARCHITECTURE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E001-ARCH-01",
        "M243 lane-E E001 diagnostics quality gate/replay policy contract and architecture freeze",
    ),
    SnippetCheck(
        "M243-E001-ARCH-02",
        "`M243-A001`, `M243-B001`, `M243-C001`, and",
    ),
)

LOWERING_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E001-SPC-01",
        "diagnostics quality gate and replay policy wiring shall preserve explicit",
    ),
    SnippetCheck(
        "M243-E001-SPC-02",
        "lane-E dependency anchors (`M243-A001`, `M243-B001`, `M243-C001`, and",
    ),
)

METADATA_SPEC_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E001-META-01",
        "deterministic lane-E diagnostics quality gate and replay policy dependency anchors for",
    ),
    SnippetCheck(
        "M243-E001-META-02",
        "`M243-A001`, `M243-B001`, `M243-C001`, and `M243-D001`",
    ),
)

PACKAGE_SNIPPETS: tuple[SnippetCheck, ...] = (
    SnippetCheck(
        "M243-E001-PKG-01",
        '"check:objc3c:m243-e001-lane-e-diagnostics-quality-gate-replay-policy-contract-architecture-freeze-contract": '
        '"python scripts/check_m243_e001_lane_e_diagnostics_quality_gate_and_replay_policy_contract_and_architecture_freeze_contract.py"',
    ),
    SnippetCheck(
        "M243-E001-PKG-02",
        '"test:tooling:m243-e001-lane-e-diagnostics-quality-gate-replay-policy-contract-architecture-freeze-contract": '
        '"python -m pytest tests/tooling/test_check_m243_e001_lane_e_diagnostics_quality_gate_and_replay_policy_contract_and_architecture_freeze_contract.py -q"',
    ),
    SnippetCheck(
        "M243-E001-PKG-03",
        '"check:objc3c:m243-e001-lane-e-readiness": '
        '"npm run check:objc3c:m243-e001-lane-e-diagnostics-quality-gate-replay-policy-contract-architecture-freeze-contract '
        "&& npm run test:tooling:m243-e001-lane-e-diagnostics-quality-gate-replay-policy-contract-architecture-freeze-contract\"",
    ),
    SnippetCheck("M243-E001-PKG-04", '"test:objc3c:diagnostics-replay-proof": '),
    SnippetCheck("M243-E001-PKG-05", '"test:objc3c:execution-replay-proof": '),
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
        (args.expectations_doc, "M243-E001-DOC-EXP-EXISTS", EXPECTATIONS_SNIPPETS),
        (args.packet_doc, "M243-E001-DOC-PKT-EXISTS", PACKET_SNIPPETS),
        (args.architecture_doc, "M243-E001-ARCH-EXISTS", ARCHITECTURE_SNIPPETS),
        (args.lowering_spec, "M243-E001-SPC-EXISTS", LOWERING_SPEC_SNIPPETS),
        (args.metadata_spec, "M243-E001-META-EXISTS", METADATA_SPEC_SNIPPETS),
        (args.package_json, "M243-E001-PKG-EXISTS", PACKAGE_SNIPPETS),
    ):
        check_count, findings = check_doc_contract(
            path=path,
            exists_check_id=exists_check_id,
            snippets=snippets,
        )
        checks_total += check_count
        failures.extend(findings)

    checks_passed = checks_total - len(failures)
    ok = not failures
    summary_payload = {
        "mode": MODE,
        "ok": ok,
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

    if failures:
        for finding in failures:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


def main() -> int:
    return run(sys.argv[1:])


if __name__ == "__main__":
    raise SystemExit(main())
