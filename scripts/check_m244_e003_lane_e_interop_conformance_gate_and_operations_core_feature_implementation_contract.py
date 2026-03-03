#!/usr/bin/env python3
"""Fail-closed contract checker for M244-E003 interop conformance core-feature implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m244-e003-lane-e-interop-conformance-gate-operations-core-feature-implementation-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m244_lane_e_interop_conformance_gate_and_operations_core_feature_implementation_e003_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m244"
    / "m244_e003_lane_e_interop_conformance_gate_and_operations_core_feature_implementation_packet.md",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

EXISTENCE_CHECK_IDS: dict[str, str] = {
    "expectations_doc": "M244-E003-DOC-EXP-EXISTS",
    "packet_doc": "M244-E003-DOC-PKT-EXISTS",
    "architecture_doc": "M244-E003-ARCH-EXISTS",
    "lowering_spec": "M244-E003-SPC-EXISTS",
    "metadata_spec": "M244-E003-META-EXISTS",
    "package_json": "M244-E003-PKG-EXISTS",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M244-E003-DOC-EXP-01",
            "# M244 Lane E Interop Conformance Gate and Operations Core Feature Implementation Expectations (E003)",
        ),
        (
            "M244-E003-DOC-EXP-02",
            "Contract ID: `objc3c-lane-e-interop-conformance-gate-operations-core-feature-implementation-contract/m244-e003-v1`",
        ),
        (
            "M244-E003-DOC-EXP-03",
            "Dependencies: `M244-E002`, `M244-A002`, `M244-B003`, `M244-C004`, `M244-D004`",
        ),
        (
            "M244-E003-DOC-EXP-04",
            "Issue `#6597` governs lane-E core-feature implementation scope and dependency-token/reference continuity.",
        ),
        (
            "M244-E003-DOC-EXP-05",
            "The E003 checker and readiness wiring fail close on dependency token/reference",
        ),
        (
            "M244-E003-DOC-EXP-06",
            "`npm run --if-present check:objc3c:m244-b003-lane-b-readiness`",
        ),
        (
            "M244-E003-DOC-EXP-07",
            "`npm run --if-present check:objc3c:m244-c004-lane-c-readiness`",
        ),
        (
            "M244-E003-DOC-EXP-08",
            "`npm run --if-present check:objc3c:m244-d004-lane-d-readiness`",
        ),
        (
            "M244-E003-DOC-EXP-09",
            "scripts/check_m244_e003_lane_e_interop_conformance_gate_and_operations_core_feature_implementation_contract.py",
        ),
        (
            "M244-E003-DOC-EXP-10",
            "tests/tooling/test_check_m244_e003_lane_e_interop_conformance_gate_and_operations_core_feature_implementation_contract.py",
        ),
        ("M244-E003-DOC-EXP-11", "`check:objc3c:m244-e003-lane-e-readiness`"),
        ("M244-E003-DOC-EXP-12", "`test:objc3c:diagnostics-replay-proof`"),
        ("M244-E003-DOC-EXP-13", "`test:objc3c:execution-replay-proof`"),
        (
            "M244-E003-DOC-EXP-14",
            "tmp/reports/m244/M244-E003/lane_e_interop_conformance_gate_operations_core_feature_implementation_summary.json",
        ),
    ),
    "packet_doc": (
        (
            "M244-E003-DOC-PKT-01",
            "# M244-E003 Lane-E Interop Conformance Gate and Operations Core Feature Implementation Packet",
        ),
        ("M244-E003-DOC-PKT-02", "Packet: `M244-E003`"),
        ("M244-E003-DOC-PKT-03", "Issue: `#6597`"),
        (
            "M244-E003-DOC-PKT-04",
            "Dependencies: `M244-E002`, `M244-A002`, `M244-B003`, `M244-C004`, `M244-D004`",
        ),
        (
            "M244-E003-DOC-PKT-05",
            "docs/contracts/m244_lane_e_interop_conformance_gate_and_operations_core_feature_implementation_e003_expectations.md",
        ),
        (
            "M244-E003-DOC-PKT-06",
            "scripts/check_m244_e003_lane_e_interop_conformance_gate_and_operations_core_feature_implementation_contract.py",
        ),
        (
            "M244-E003-DOC-PKT-07",
            "tests/tooling/test_check_m244_e003_lane_e_interop_conformance_gate_and_operations_core_feature_implementation_contract.py",
        ),
        (
            "M244-E003-DOC-PKT-08",
            "including code/spec anchors and milestone",
        ),
        (
            "M244-E003-DOC-PKT-09",
            "`npm run --if-present check:objc3c:m244-b003-lane-b-readiness`",
        ),
        (
            "M244-E003-DOC-PKT-10",
            "`npm run --if-present check:objc3c:m244-c004-lane-c-readiness`",
        ),
        (
            "M244-E003-DOC-PKT-11",
            "`npm run --if-present check:objc3c:m244-d004-lane-d-readiness`",
        ),
        ("M244-E003-DOC-PKT-12", "`test:objc3c:diagnostics-replay-proof`"),
        ("M244-E003-DOC-PKT-13", "`test:objc3c:execution-replay-proof`"),
        (
            "M244-E003-DOC-PKT-14",
            "tmp/reports/m244/M244-E003/lane_e_interop_conformance_gate_operations_core_feature_implementation_summary.json",
        ),
    ),
    "architecture_doc": (
        (
            "M244-E003-ARCH-01",
            "M244 lane-E E003 interop conformance gate and operations core-feature implementation",
        ),
        (
            "M244-E003-ARCH-02",
            "(`M244-E002`, `M244-A002`, `M244-B003`, `M244-C004`, and `M244-D004`)",
        ),
        (
            "M244-E003-ARCH-03",
            "docs/contracts/m244_lane_e_interop_conformance_gate_and_operations_core_feature_implementation_e003_expectations.md",
        ),
        (
            "M244-E003-ARCH-04",
            "spec/planning/compiler/m244/m244_e003_lane_e_interop_conformance_gate_and_operations_core_feature_implementation_packet.md",
        ),
        (
            "M244-E003-ARCH-05",
            "deterministic and fail-closed on dependency token/reference drift while",
        ),
        (
            "M244-E003-ARCH-06",
            "lane-B/C/D core-feature assets remain pending GH seed.",
        ),
    ),
    "lowering_spec": (
        (
            "M244-E003-SPC-01",
            "interop conformance gate and operations core-feature implementation wiring shall preserve explicit",
        ),
        (
            "M244-E003-SPC-02",
            "lane-E dependency anchors (`M244-E002`, `M244-A002`, `M244-B003`, `M244-C004`, and `M244-D004`)",
        ),
        (
            "M244-E003-SPC-03",
            "preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D core-feature readiness hooks",
        ),
        (
            "M244-E003-SPC-04",
            "fail closed when dependency token/reference continuity, interop evidence commands,",
        ),
        (
            "M244-E003-SPC-05",
            "or lane-E core-feature readiness hooks drift.",
        ),
    ),
    "metadata_spec": (
        (
            "M244-E003-META-01",
            "deterministic lane-E interop conformance gate and operations core-feature implementation dependency anchors for",
        ),
        (
            "M244-E003-META-02",
            "`M244-E002`, `M244-A002`, `M244-B003`, `M244-C004`, and `M244-D004`",
        ),
        (
            "M244-E003-META-03",
            "including dependency-reference tokens",
        ),
        (
            "M244-E003-META-04",
            "wired through `npm run --if-present` readiness hooks so governance evidence stays fail-closed",
        ),
        (
            "M244-E003-META-05",
            "on token/reference drift while staged lane-B/C/D core-feature assets remain pending GH seed.",
        ),
    ),
    "package_json": (
        (
            "M244-E003-PKG-01",
            '"check:objc3c:m244-e003-lane-e-interop-conformance-gate-operations-core-feature-implementation-contract": '
            '"python scripts/check_m244_e003_lane_e_interop_conformance_gate_and_operations_core_feature_implementation_contract.py"',
        ),
        (
            "M244-E003-PKG-02",
            '"test:tooling:m244-e003-lane-e-interop-conformance-gate-operations-core-feature-implementation-contract": '
            '"python -m pytest tests/tooling/test_check_m244_e003_lane_e_interop_conformance_gate_and_operations_core_feature_implementation_contract.py -q"',
        ),
        (
            "M244-E003-PKG-03",
            '"check:objc3c:m244-e003-lane-e-readiness": '
            '"npm run check:objc3c:m244-e002-lane-e-readiness '
            '&& npm run check:objc3c:m244-a002-lane-a-readiness '
            '&& npm run --if-present check:objc3c:m244-b003-lane-b-readiness '
            '&& npm run --if-present check:objc3c:m244-c004-lane-c-readiness '
            '&& npm run --if-present check:objc3c:m244-d004-lane-d-readiness '
            '&& npm run check:objc3c:m244-e003-lane-e-interop-conformance-gate-operations-core-feature-implementation-contract '
            '&& npm run test:tooling:m244-e003-lane-e-interop-conformance-gate-operations-core-feature-implementation-contract"',
        ),
        (
            "M244-E003-PKG-04",
            '"test:objc3c:diagnostics-replay-proof": ',
        ),
        (
            "M244-E003-PKG-05",
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
            "tmp/reports/m244/M244-E003/lane_e_interop_conformance_gate_operations_core_feature_implementation_summary.json"
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
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
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
