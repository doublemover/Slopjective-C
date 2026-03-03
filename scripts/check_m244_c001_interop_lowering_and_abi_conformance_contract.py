#!/usr/bin/env python3
"""Fail-closed contract checker for M244-C001 interop lowering and ABI conformance freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m244-c001-interop-lowering-and-abi-conformance-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m244_interop_lowering_and_abi_conformance_contract_and_architecture_freeze_c001_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m244"
    / "m244_c001_interop_lowering_and_abi_conformance_contract_and_architecture_freeze_packet.md",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

EXISTENCE_CHECK_IDS: dict[str, str] = {
    "expectations_doc": "M244-C001-DOC-EXISTS",
    "packet_doc": "M244-C001-PKT-EXISTS",
    "architecture_doc": "M244-C001-ARCH-EXISTS",
    "lowering_spec": "M244-C001-SPC-EXISTS",
    "metadata_spec": "M244-C001-META-EXISTS",
    "package_json": "M244-C001-PKG-EXISTS",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M244-C001-DOC-01",
            "# M244 Interop Lowering and ABI Conformance Contract and Architecture Freeze Expectations (C001)",
        ),
        (
            "M244-C001-DOC-02",
            "Contract ID: `objc3c-interop-lowering-and-abi-conformance-freeze/m244-c001-v1`",
        ),
        ("M244-C001-DOC-03", "Dependencies: none"),
        (
            "M244-C001-DOC-04",
            "Deterministic anchors, dependency tokens, and fail-closed behavior are mandatory scope inputs.",
        ),
        (
            "M244-C001-DOC-05",
            "scripts/check_m244_c001_interop_lowering_and_abi_conformance_contract.py",
        ),
        (
            "M244-C001-DOC-06",
            "tests/tooling/test_check_m244_c001_interop_lowering_and_abi_conformance_contract.py",
        ),
        ("M244-C001-DOC-07", "`check:objc3c:m244-c001-lane-c-readiness`"),
        (
            "M244-C001-DOC-08",
            "tmp/reports/m244/M244-C001/interop_lowering_and_abi_conformance_contract_summary.json",
        ),
    ),
    "packet_doc": (
        (
            "M244-C001-PKT-01",
            "# M244-C001 Interop Lowering and ABI Conformance Contract and Architecture Freeze Packet",
        ),
        ("M244-C001-PKT-02", "Packet: `M244-C001`"),
        ("M244-C001-PKT-03", "Freeze date: `2026-03-03`"),
        ("M244-C001-PKT-04", "Dependencies: none"),
        (
            "M244-C001-PKT-05",
            "docs/contracts/m244_interop_lowering_and_abi_conformance_contract_and_architecture_freeze_c001_expectations.md",
        ),
        (
            "M244-C001-PKT-06",
            "scripts/check_m244_c001_interop_lowering_and_abi_conformance_contract.py",
        ),
        (
            "M244-C001-PKT-07",
            "tests/tooling/test_check_m244_c001_interop_lowering_and_abi_conformance_contract.py",
        ),
        (
            "M244-C001-PKT-08",
            "Deterministic anchors, dependency tokens, and fail-closed behavior remain mandatory scope controls.",
        ),
        (
            "M244-C001-PKT-09",
            "`python scripts/check_m244_c001_interop_lowering_and_abi_conformance_contract.py --emit-json`",
        ),
    ),
    "architecture_doc": (
        (
            "M244-C001-ARCH-01",
            "M244 lane-C C001 interop lowering and ABI conformance anchors explicit",
        ),
        (
            "M244-C001-ARCH-02",
            "docs/contracts/m244_interop_lowering_and_abi_conformance_contract_and_architecture_freeze_c001_expectations.md",
        ),
        (
            "M244-C001-ARCH-03",
            "spec/planning/compiler/m244/m244_c001_interop_lowering_and_abi_conformance_contract_and_architecture_freeze_packet.md",
        ),
        (
            "M244-C001-ARCH-04",
            "deterministic lowering/ABI anchors, dependency tokens, and fail-closed behavior remain frozen",
        ),
    ),
    "lowering_spec": (
        (
            "M244-C001-SPC-01",
            "interop lowering and ABI conformance governance shall preserve",
        ),
        (
            "M244-C001-SPC-02",
            "deterministic lane-C anchors, explicit dependency tokens (`none` for `M244-C001`)",
        ),
        (
            "M244-C001-SPC-03",
            "fail closed on lowering and ABI conformance boundary drift",
        ),
    ),
    "metadata_spec": (
        (
            "M244-C001-META-01",
            "deterministic lane-C interop lowering/ABI conformance metadata anchors for `M244-C001`",
        ),
        (
            "M244-C001-META-02",
            "explicit dependency tokens (`none`) and fail-closed evidence continuity",
        ),
    ),
    "package_json": (
        (
            "M244-C001-PKG-01",
            '"check:objc3c:m244-c001-interop-lowering-abi-conformance-contract": '
            '"python scripts/check_m244_c001_interop_lowering_and_abi_conformance_contract.py"',
        ),
        (
            "M244-C001-PKG-02",
            '"test:tooling:m244-c001-interop-lowering-abi-conformance-contract": '
            '"python -m pytest tests/tooling/test_check_m244_c001_interop_lowering_and_abi_conformance_contract.py -q"',
        ),
        (
            "M244-C001-PKG-03",
            '"check:objc3c:m244-c001-lane-c-readiness": '
            '"npm run check:objc3c:m244-c001-interop-lowering-abi-conformance-contract '
            '&& npm run test:tooling:m244-c001-interop-lowering-abi-conformance-contract"',
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "package_json": (
        (
            "M244-C001-FORB-01",
            '"check:objc3c:m244-c001-lane-c-readiness": '
            '"npm run check:objc3c:m244-c001-interop-lowering-abi-conformance-contract"',
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
            "tmp/reports/m244/M244-C001/interop_lowering_and_abi_conformance_contract_summary.json"
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

        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                findings.append(
                    Finding(
                        display_path(path),
                        check_id,
                        f"forbidden snippet present: {snippet}",
                    )
                )
            else:
                checks_passed += 1

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
