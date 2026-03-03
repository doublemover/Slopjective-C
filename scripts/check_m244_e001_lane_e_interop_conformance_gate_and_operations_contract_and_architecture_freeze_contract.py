#!/usr/bin/env python3
"""Fail-closed contract checker for M244-E001 interop conformance gate/operations freeze.

This checker freezes dependency tokens and dependency references without requiring
in-flight lane-B/C/D artifact files to exist before those lanes land.
"""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m244-e001-lane-e-interop-conformance-gate-operations-contract-architecture-freeze-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m244_lane_e_interop_conformance_gate_and_operations_contract_and_architecture_freeze_e001_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m244"
    / "m244_e001_lane_e_interop_conformance_gate_and_operations_contract_and_architecture_freeze_packet.md",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

EXISTENCE_CHECK_IDS: dict[str, str] = {
    "expectations_doc": "M244-E001-DOC-EXP-EXISTS",
    "packet_doc": "M244-E001-DOC-PKT-EXISTS",
    "architecture_doc": "M244-E001-ARCH-EXISTS",
    "lowering_spec": "M244-E001-SPC-EXISTS",
    "metadata_spec": "M244-E001-META-EXISTS",
    "package_json": "M244-E001-PKG-EXISTS",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M244-E001-DOC-EXP-01",
            "# M244 Lane E Interop Conformance Gate and Operations Contract and Architecture Freeze Expectations (E001)",
        ),
        (
            "M244-E001-DOC-EXP-02",
            "Contract ID: `objc3c-lane-e-interop-conformance-gate-operations-contract-architecture-freeze/m244-e001-v1`",
        ),
        (
            "M244-E001-DOC-EXP-03",
            "Dependencies: `M244-A001`, `M244-B001`, `M244-C001`, `M244-D001`",
        ),
        (
            "M244-E001-DOC-EXP-04",
            "The E001 checker and readiness wiring fail close on dependency token/reference",
        ),
        (
            "M244-E001-DOC-EXP-05",
            "`npm run --if-present check:objc3c:m244-b001-lane-b-readiness`",
        ),
        (
            "M244-E001-DOC-EXP-06",
            "`npm run --if-present check:objc3c:m244-c001-lane-c-readiness`",
        ),
        (
            "M244-E001-DOC-EXP-07",
            "`npm run --if-present check:objc3c:m244-d001-lane-d-readiness`",
        ),
        (
            "M244-E001-DOC-EXP-08",
            "Issue `#6531`; readiness key `check:objc3c:m244-b001-lane-b-readiness`.",
        ),
        (
            "M244-E001-DOC-EXP-09",
            "scripts/check_m244_e001_lane_e_interop_conformance_gate_and_operations_contract_and_architecture_freeze_contract.py",
        ),
        (
            "M244-E001-DOC-EXP-10",
            "tests/tooling/test_check_m244_e001_lane_e_interop_conformance_gate_and_operations_contract_and_architecture_freeze_contract.py",
        ),
        ("M244-E001-DOC-EXP-11", "`check:objc3c:m244-e001-lane-e-readiness`"),
        ("M244-E001-DOC-EXP-12", "`test:objc3c:diagnostics-replay-proof`"),
        ("M244-E001-DOC-EXP-13", "`test:objc3c:execution-replay-proof`"),
        (
            "M244-E001-DOC-EXP-14",
            "tmp/reports/m244/M244-E001/lane_e_interop_conformance_gate_operations_contract_architecture_freeze_summary.json",
        ),
    ),
    "packet_doc": (
        (
            "M244-E001-DOC-PKT-01",
            "# M244-E001 Lane-E Interop Conformance Gate and Operations Contract and Architecture Freeze Packet",
        ),
        ("M244-E001-DOC-PKT-02", "Packet: `M244-E001`"),
        ("M244-E001-DOC-PKT-03", "Freeze date: `2026-03-03`"),
        (
            "M244-E001-DOC-PKT-04",
            "Dependencies: `M244-A001`, `M244-B001`, `M244-C001`, `M244-D001`",
        ),
        (
            "M244-E001-DOC-PKT-05",
            "docs/contracts/m244_lane_e_interop_conformance_gate_and_operations_contract_and_architecture_freeze_e001_expectations.md",
        ),
        (
            "M244-E001-DOC-PKT-06",
            "scripts/check_m244_e001_lane_e_interop_conformance_gate_and_operations_contract_and_architecture_freeze_contract.py",
        ),
        (
            "M244-E001-DOC-PKT-07",
            "tests/tooling/test_check_m244_e001_lane_e_interop_conformance_gate_and_operations_contract_and_architecture_freeze_contract.py",
        ),
        (
            "M244-E001-DOC-PKT-08",
            "including code/spec anchors and milestone",
        ),
        (
            "M244-E001-DOC-PKT-09",
            "`npm run --if-present check:objc3c:m244-b001-lane-b-readiness`",
        ),
        (
            "M244-E001-DOC-PKT-10",
            "`npm run --if-present check:objc3c:m244-c001-lane-c-readiness`",
        ),
        (
            "M244-E001-DOC-PKT-11",
            "`npm run --if-present check:objc3c:m244-d001-lane-d-readiness`",
        ),
        ("M244-E001-DOC-PKT-12", "`test:objc3c:diagnostics-replay-proof`"),
        ("M244-E001-DOC-PKT-13", "`test:objc3c:execution-replay-proof`"),
        (
            "M244-E001-DOC-PKT-14",
            "tmp/reports/m244/M244-E001/lane_e_interop_conformance_gate_operations_contract_architecture_freeze_summary.json",
        ),
    ),
    "architecture_doc": (
        (
            "M244-E001-ARCH-01",
            "M244 lane-E E001 interop conformance gate and operations contract and architecture freeze",
        ),
        (
            "M244-E001-ARCH-02",
            "(`M244-A001`, `M244-B001`, `M244-C001`, and",
        ),
        (
            "M244-E001-ARCH-03",
            "docs/contracts/m244_lane_e_interop_conformance_gate_and_operations_contract_and_architecture_freeze_e001_expectations.md",
        ),
        (
            "M244-E001-ARCH-04",
            "spec/planning/compiler/m244/m244_e001_lane_e_interop_conformance_gate_and_operations_contract_and_architecture_freeze_packet.md",
        ),
        (
            "M244-E001-ARCH-05",
            "deterministic and fail-closed on dependency token/reference drift while",
        ),
        (
            "M244-E001-ARCH-06",
            "lane-B/C/D contract-freeze assets remain pending GH seed.",
        ),
    ),
    "lowering_spec": (
        (
            "M244-E001-SPC-01",
            "interop conformance gate and operations contract and architecture freeze wiring shall preserve explicit",
        ),
        (
            "M244-E001-SPC-02",
            "lane-E dependency anchors (`M244-A001`, `M244-B001`, `M244-C001`, and `M244-D001`)",
        ),
        (
            "M244-E001-SPC-03",
            "preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D readiness hooks",
        ),
        (
            "M244-E001-SPC-04",
            "fail closed when dependency token/reference continuity, interop evidence commands,",
        ),
        (
            "M244-E001-SPC-05",
            "or lane-E readiness hooks drift.",
        ),
    ),
    "metadata_spec": (
        (
            "M244-E001-META-01",
            "deterministic lane-E interop conformance gate and operations dependency anchors for",
        ),
        (
            "M244-E001-META-02",
            "`M244-A001`, `M244-B001`, `M244-C001`, and `M244-D001`",
        ),
        (
            "M244-E001-META-03",
            "including dependency-reference tokens",
        ),
        (
            "M244-E001-META-04",
            "wired through `npm run --if-present` readiness hooks so governance evidence stays fail-closed",
        ),
        (
            "M244-E001-META-05",
            "on token/reference drift without requiring pending lane-B/C/D artifacts before they land.",
        ),
    ),
    "package_json": (
        (
            "M244-E001-PKG-01",
            '"check:objc3c:m244-e001-lane-e-interop-conformance-gate-operations-contract-architecture-freeze-contract": '
            '"python scripts/check_m244_e001_lane_e_interop_conformance_gate_and_operations_contract_and_architecture_freeze_contract.py"',
        ),
        (
            "M244-E001-PKG-02",
            '"test:tooling:m244-e001-lane-e-interop-conformance-gate-operations-contract-architecture-freeze-contract": '
            '"python -m pytest tests/tooling/test_check_m244_e001_lane_e_interop_conformance_gate_and_operations_contract_and_architecture_freeze_contract.py -q"',
        ),
        (
            "M244-E001-PKG-03",
            '"check:objc3c:m244-e001-lane-e-readiness": '
            '"npm run check:objc3c:m244-a001-lane-a-readiness '
            '&& npm run --if-present check:objc3c:m244-b001-lane-b-readiness '
            '&& npm run --if-present check:objc3c:m244-c001-lane-c-readiness '
            '&& npm run --if-present check:objc3c:m244-d001-lane-d-readiness '
            '&& npm run check:objc3c:m244-e001-lane-e-interop-conformance-gate-operations-contract-architecture-freeze-contract '
            '&& npm run test:tooling:m244-e001-lane-e-interop-conformance-gate-operations-contract-architecture-freeze-contract"',
        ),
        (
            "M244-E001-PKG-04",
            '"test:objc3c:diagnostics-replay-proof": ',
        ),
        (
            "M244-E001-PKG-05",
            '"test:objc3c:execution-replay-proof": ',
        ),
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


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
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m244/M244-E001/lane_e_interop_conformance_gate_operations_contract_architecture_freeze_summary.json"
        ),
    )
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit summary JSON to stdout in addition to writing --summary-out.",
    )
    return parser.parse_args(argv)


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    checks_total = 0
    checks_passed = 0

    for artifact, path in ARTIFACTS.items():
        checks_total += 1
        if not path.exists():
            findings.append(
                Finding(
                    display_path(path),
                    EXISTENCE_CHECK_IDS[artifact],
                    f"required file is missing: {display_path(path)}",
                )
            )
            continue
        if not path.is_file():
            findings.append(
                Finding(
                    display_path(path),
                    EXISTENCE_CHECK_IDS[artifact],
                    f"required path is not a file: {display_path(path)}",
                )
            )
            continue

        checks_passed += 1
        text = path.read_text(encoding="utf-8")

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                checks_passed += 1
            else:
                findings.append(
                    Finding(
                        display_path(path),
                        check_id,
                        f"missing required snippet: {snippet}",
                    )
                )

    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in findings
        ],
    }

    summary_path = args.summary_out if args.summary_out.is_absolute() else ROOT / args.summary_out
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary), end="")

    if findings:
        for finding in findings:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    if not args.emit_json:
        print(f"[ok] {MODE}: {checks_passed}/{checks_total} checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
