#!/usr/bin/env python3
"""Fail-closed validator for M250-A008 long-tail grammar recovery/determinism hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-frontend-stability-long-tail-grammar-recovery-determinism-hardening-contract-a008-v1"

ARTIFACTS: dict[str, Path] = {
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "parse_readiness": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_parse_lowering_readiness_surface.h",
    "frontend_artifacts": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "a007_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_diagnostics_hardening_a007_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_recovery_determinism_hardening_a008_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_a008_frontend_stability_long_tail_grammar_recovery_determinism_hardening_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_types": (
        ("M250-A008-TYP-01", "bool long_tail_grammar_recovery_determinism_consistent = false;"),
        ("M250-A008-TYP-02", "bool long_tail_grammar_recovery_determinism_ready = false;"),
        ("M250-A008-TYP-03", "std::string long_tail_grammar_recovery_determinism_key;"),
    ),
    "parse_readiness": (
        ("M250-A008-REA-01", "inline std::string BuildObjc3LongTailGrammarRecoveryDeterminismKey("),
        ("M250-A008-REA-02", "surface.long_tail_grammar_recovery_determinism_consistent ="),
        ("M250-A008-REA-03", "surface.long_tail_grammar_recovery_determinism_ready ="),
        ("M250-A008-REA-04", "surface.long_tail_grammar_recovery_determinism_key ="),
        (
            "M250-A008-REA-05",
            "surface.failure_reason = \"long-tail grammar recovery/determinism hardening is inconsistent\";",
        ),
        (
            "M250-A008-REA-06",
            "surface.failure_reason = \"long-tail grammar recovery/determinism hardening is not ready\";",
        ),
        ("M250-A008-REA-07", "surface.long_tail_grammar_recovery_determinism_ready &&"),
    ),
    "frontend_artifacts": (
        ("M250-A008-ART-01", "long_tail_grammar_recovery_determinism_consistent"),
        ("M250-A008-ART-02", "long_tail_grammar_recovery_determinism_ready"),
        ("M250-A008-ART-03", "long_tail_grammar_recovery_determinism_key"),
    ),
    "architecture_doc": (
        ("M250-A008-ARC-01", "M250 lane-A A008 recovery/determinism hardening anchors explicit long-tail"),
        ("M250-A008-ARC-02", "long_tail_grammar_recovery_determinism_*"),
    ),
    "a007_contract_doc": (
        (
            "M250-A008-DEP-01",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-diagnostics-hardening/m250-a007-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M250-A008-DOC-01",
            "Contract ID: `objc3c-frontend-stability-long-tail-grammar-recovery-determinism-hardening/m250-a008-v1`",
        ),
        ("M250-A008-DOC-02", "long_tail_grammar_recovery_determinism_consistent"),
        ("M250-A008-DOC-03", "long_tail_grammar_recovery_determinism_ready"),
        ("M250-A008-DOC-04", "npm run check:objc3c:m250-a008-lane-a-readiness"),
    ),
    "packet_doc": (
        ("M250-A008-PKT-01", "Packet: `M250-A008`"),
        ("M250-A008-PKT-02", "Dependencies: `M250-A007`"),
        (
            "M250-A008-PKT-03",
            "scripts/check_m250_a008_frontend_stability_long_tail_grammar_recovery_determinism_hardening_contract.py",
        ),
    ),
    "package_json": (
        (
            "M250-A008-CFG-01",
            '"check:objc3c:m250-a008-frontend-stability-long-tail-grammar-recovery-determinism-hardening-contract"',
        ),
        (
            "M250-A008-CFG-02",
            '"test:tooling:m250-a008-frontend-stability-long-tail-grammar-recovery-determinism-hardening-contract"',
        ),
        ("M250-A008-CFG-03", '"check:objc3c:m250-a008-lane-a-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parse_readiness": (
        ("M250-A008-FORB-01", "surface.long_tail_grammar_recovery_determinism_ready = true;"),
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
            "tmp/reports/m250/M250-A008/frontend_stability_long_tail_grammar_recovery_determinism_hardening_contract_summary.json"
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
        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                passed_checks += 1

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
