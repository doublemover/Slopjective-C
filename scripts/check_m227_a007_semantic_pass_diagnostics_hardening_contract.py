#!/usr/bin/env python3
"""Fail-closed validator for M227-A007 semantic pass diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-sema-pass-diagnostics-hardening-a007-v1"

ARTIFACTS: dict[str, Path] = {
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h",
    "sema_manager": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp",
    "artifact_projection": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m227_semantic_pass_diagnostics_hardening_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_contract": (
        ("M227-A007-CNT-01", "bool diagnostics_accounting_consistent = false;"),
        ("M227-A007-CNT-02", "bool diagnostics_bus_publish_consistent = false;"),
        ("M227-A007-CNT-03", "bool diagnostics_canonicalized = false;"),
        ("M227-A007-CNT-04", "bool diagnostics_hardening_satisfied = false;"),
        ("M227-A007-CNT-05", "surface.diagnostics_accounting_consistent &&"),
        ("M227-A007-CNT-06", "surface.diagnostics_bus_publish_consistent &&"),
        ("M227-A007-CNT-07", "surface.diagnostics_canonicalized &&"),
        ("M227-A007-CNT-08", "surface.diagnostics_hardening_satisfied &&"),
    ),
    "sema_manager": (
        ("M227-A007-SRC-01", "bool diagnostics_canonicalized = true;"),
        ("M227-A007-SRC-02", "const bool pass_diagnostics_canonical = IsCanonicalPassDiagnostics(pass_diagnostics);"),
        ("M227-A007-SRC-03", "result.diagnostics_hardening_satisfied ="),
        ("M227-A007-SRC-04", "result.sema_pass_flow_summary.diagnostics_hardening_satisfied ="),
        ("M227-A007-SRC-05", "result.parity_surface.diagnostics_hardening_satisfied ="),
    ),
    "artifact_projection": (
        ("M227-A007-ART-01", "diagnostics_accounting_consistent"),
        ("M227-A007-ART-02", "diagnostics_bus_publish_consistent"),
        ("M227-A007-ART-03", "diagnostics_canonicalized"),
        ("M227-A007-ART-04", "diagnostics_hardening_satisfied"),
        ("M227-A007-ART-05", "pass_flow_diagnostics_hardening_satisfied"),
    ),
    "contract_doc": (
        ("M227-A007-DOC-01", "Contract ID: `objc3c-sema-pass-diagnostics-hardening/m227-a007-v1`"),
        ("M227-A007-DOC-02", "diagnostics_hardening_satisfied"),
        ("M227-A007-DOC-03", "Objc3SemaParityContractSurface"),
        ("M227-A007-DOC-04", "tmp/reports/m227/M227-A007/semantic_pass_diagnostics_hardening_contract_summary.json"),
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
        default=Path("tmp/reports/m227/M227-A007/semantic_pass_diagnostics_hardening_contract_summary.json"),
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
