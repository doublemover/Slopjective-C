#!/usr/bin/env python3
"""Fail-closed validator for M243-B001 semantic diagnostic taxonomy/fix-it freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-semantic-diagnostic-taxonomy-and-fixit-synthesis-freeze-contract-b001-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_b001_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_b001_semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract_and_architecture_freeze_packet.md",
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h",
    "sema_scaffold": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_flow_scaffold.h",
    "typed_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_typed_sema_to_lowering_contract_surface.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M243-B001-DOC-01",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-freeze/m243-b001-v1`",
        ),
        (
            "M243-B001-DOC-02",
            "scripts/check_m243_b001_semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract.py",
        ),
        (
            "M243-B001-DOC-03",
            "tests/tooling/test_check_m243_b001_semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract.py",
        ),
        (
            "M243-B001-DOC-04",
            "tmp/reports/m243/M243-B001/semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M243-B001-PKT-01", "Packet: `M243-B001`"),
        ("M243-B001-PKT-02", "Dependencies: none"),
        (
            "M243-B001-PKT-03",
            "scripts/check_m243_b001_semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract.py",
        ),
    ),
    "sema_contract": (
        ("M243-B001-SEM-01", "struct Objc3SemaPassFlowSummary {"),
        ("M243-B001-SEM-02", "bool diagnostics_after_pass_monotonic = false;"),
        ("M243-B001-SEM-03", "bool diagnostics_bus_publish_consistent = false;"),
        ("M243-B001-SEM-04", "bool diagnostics_hardening_satisfied = false;"),
        ("M243-B001-SEM-05", "struct Objc3SemaDiagnosticsBus {"),
        ("M243-B001-SEM-06", "void Publish(const std::string &diagnostic) const {"),
        ("M243-B001-SEM-07", "void PublishBatch(const std::vector<std::string> &batch) const {"),
        ("M243-B001-SEM-08", "bool deterministic_arc_diagnostics_fixit_handoff = false;"),
    ),
    "sema_scaffold": (
        ("M243-B001-SCF-01", "void FinalizeObjc3SemaPassFlowSummary("),
        ("M243-B001-SCF-02", "bool diagnostics_after_pass_monotonic,"),
        ("M243-B001-SCF-03", "bool deterministic_semantic_diagnostics,"),
        ("M243-B001-SCF-04", "bool deterministic_type_metadata_handoff);"),
    ),
    "typed_surface": (
        ("M243-B001-TYP-01", "pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics &&"),
        ("M243-B001-TYP-02", "pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff;"),
    ),
    "architecture_doc": (
        (
            "M243-B001-ARC-01",
            "M243 lane-B B001 semantic diagnostic taxonomy/fix-it synthesis anchors explicit",
        ),
        ("M243-B001-ARC-02", "sema/objc3_sema_pass_manager_contract.h"),
        ("M243-B001-ARC-03", "sema/objc3_sema_pass_flow_scaffold.h"),
    ),
    "package_json": (
        (
            "M243-B001-CFG-01",
            '"check:objc3c:m243-b001-semantic-diagnostic-taxonomy-and-fix-it-synthesis-contract"',
        ),
        (
            "M243-B001-CFG-02",
            '"test:tooling:m243-b001-semantic-diagnostic-taxonomy-and-fix-it-synthesis-contract"',
        ),
        ("M243-B001-CFG-03", '"check:objc3c:m243-b001-lane-b-readiness"'),
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
            "tmp/reports/m243/M243-B001/semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract_summary.json"
        ),
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

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
