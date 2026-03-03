#!/usr/bin/env python3
"""Fail-closed checker for M243-C010 lowering/runtime diagnostics conformance corpus expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-c010-lowering-runtime-diagnostics-surfacing-"
    "conformance-corpus-expansion-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_c010_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_packet.md",
    "c009_expectations_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_c009_expectations.md",
    "c009_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_packet.md",
    "c009_checker": ROOT
    / "scripts"
    / "check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract.py",
    "c009_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract.py",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "expectations_doc": (
        (
            "M243-C010-DOC-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-conformance-corpus-expansion/m243-c010-v1`",
        ),
        ("M243-C010-DOC-02", "- Dependencies: `M243-C009`"),
        ("M243-C010-DOC-03", "conformance-corpus consistency"),
        ("M243-C010-DOC-04", "conformance-corpus-key continuity"),
        (
            "M243-C010-DOC-05",
            "scripts/check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py",
        ),
        (
            "M243-C010-DOC-06",
            "tests/tooling/test_check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py",
        ),
        ("M243-C010-DOC-07", "check:objc3c:m243-c010-lane-c-readiness"),
        (
            "M243-C010-DOC-08",
            "python scripts/check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py --emit-json",
        ),
        (
            "M243-C010-DOC-09",
            "tmp/reports/m243/M243-C010/lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M243-C010-PKT-01", "Packet: `M243-C010`"),
        ("M243-C010-PKT-02", "Dependencies: `M243-C009`"),
        (
            "M243-C010-PKT-03",
            "scripts/check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py",
        ),
        (
            "M243-C010-PKT-04",
            "tests/tooling/test_check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py",
        ),
        ("M243-C010-PKT-05", "check:objc3c:m243-c010-lane-c-readiness"),
        (
            "M243-C010-PKT-06",
            "python scripts/check_m243_c010_lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract.py --emit-json",
        ),
        (
            "M243-C010-PKT-07",
            "tmp/reports/m243/M243-C010/lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract_summary.json",
        ),
    ),
    "c009_expectations_doc": (
        (
            "M243-C010-DEP-01",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-conformance-matrix-implementation/m243-c009-v1`",
        ),
    ),
    "c009_packet_doc": (
        ("M243-C010-DEP-02", "Packet: `M243-C009`"),
        ("M243-C010-DEP-03", "Dependencies: `M243-C008`"),
    ),
    "c009_checker": (
        (
            "M243-C010-DEP-04",
            'MODE = (\n    "m243-c009-lowering-runtime-diagnostics-surfacing-"\n    "conformance-matrix-implementation-contract-v1"\n)',
        ),
    ),
    "c009_tooling_test": (
        (
            "M243-C010-DEP-05",
            "check_m243_c009_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_contract",
        ),
    ),
    "architecture_doc": (
        (
            "M243-C010-ARC-01",
            "M243 lane-C C010 conformance corpus expansion anchors lowering/runtime diagnostics surfacing",
        ),
    ),
    "lowering_spec": (
        (
            "M243-C010-SPC-01",
            "lowering/runtime diagnostics surfacing conformance corpus expansion shall preserve",
        ),
        ("M243-C010-SPC-02", "lane-C dependency anchors (`M243-C009`)"),
    ),
    "metadata_spec": (
        (
            "M243-C010-META-01",
            "deterministic lane-C lowering/runtime diagnostics surfacing conformance corpus expansion metadata anchors for `M243-C010` with explicit",
        ),
        ("M243-C010-META-02", "`M243-C009` dependency continuity"),
    ),
    "package_json": (
        (
            "M243-C010-CFG-01",
            '"check:objc3c:m243-c010-lowering-runtime-diagnostics-surfacing-conformance-corpus-expansion-contract"',
        ),
        (
            "M243-C010-CFG-02",
            '"test:tooling:m243-c010-lowering-runtime-diagnostics-surfacing-conformance-corpus-expansion-contract"',
        ),
        ("M243-C010-CFG-03", '"check:objc3c:m243-c010-lane-c-readiness"'),
        (
            "M243-C010-CFG-04",
            "npm run check:objc3c:m243-c009-lane-c-readiness && npm run check:objc3c:m243-c010-lowering-runtime-diagnostics-surfacing-conformance-corpus-expansion-contract && npm run test:tooling:m243-c010-lowering-runtime-diagnostics-surfacing-conformance-corpus-expansion-contract",
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


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path(
            "tmp/reports/m243/M243-C010/lowering_runtime_diagnostics_surfacing_conformance_corpus_expansion_contract_summary.json"
        ),
    )
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit canonical summary JSON to stdout.",
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary), end="")

    if ok:
        if not args.emit_json:
            print(f"[ok] {MODE}: {passed_checks}/{total_checks} checks passed")
        return 0

    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
