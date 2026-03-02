#!/usr/bin/env python3
"""Fail-closed validator for M227-A003 semantic pass core feature contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-sema-pass-core-feature-implementation-contract-a003-v1"

ARTIFACTS: dict[str, Path] = {
    "sema_pass_manager_contract": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_manager_contract.h",
    "sema_pass_flow_scaffold": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_flow_scaffold.cpp",
    "frontend_artifacts": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_frontend_artifacts.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m227_semantic_pass_core_feature_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_pass_manager_contract": (
        ("M227-A003-CNT-01", "std::size_t diagnostics_total = 0;"),
        ("M227-A003-CNT-02", "std::uint64_t pass_execution_fingerprint = 1469598103934665603ull;"),
        ("M227-A003-CNT-03", "std::string deterministic_handoff_key;"),
        ("M227-A003-CNT-04", "summary.diagnostics_total == summary.diagnostics_after_pass.back()"),
        ("M227-A003-CNT-05", "!summary.deterministic_handoff_key.empty()"),
    ),
    "sema_pass_flow_scaffold": (
        ("M227-A003-SCF-01", "summary.diagnostics_total = summary.diagnostics_after_pass.back();"),
        ("M227-A003-SCF-02", "summary.pass_execution_fingerprint = fingerprint;"),
        ("M227-A003-SCF-03", "summary.deterministic_handoff_key = handoff_key.str();"),
        ("M227-A003-SCF-04", "sema-pass-flow:v1:"),
    ),
    "frontend_artifacts": (
        ("M227-A003-ART-01", '\\"pass_flow_configured_count\\":'),
        ("M227-A003-ART-02", '\\"pass_flow_executed_count\\":'),
        ("M227-A003-ART-03", '\\"pass_flow_fingerprint\\":'),
        ("M227-A003-ART-04", '\\"pass_flow_deterministic_handoff_key\\":\\"'),
        ("M227-A003-ART-05", '\\"pass_flow_deterministic\\":'),
    ),
    "contract_doc": (
        ("M227-A003-DOC-01", "Contract ID: `objc3c-sema-pass-core-feature-implementation/m227-a003-v1`"),
        ("M227-A003-DOC-02", "pass_execution_fingerprint"),
        ("M227-A003-DOC-03", "deterministic_handoff_key"),
        ("M227-A003-DOC-04", "objc3_frontend_artifacts.cpp"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_pass_flow_scaffold": (
        (
            "M227-A003-SCF-05",
            "summary.pass_execution_fingerprint = 1469598103934665603ull;",
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
        default=Path("tmp/reports/m227/M227-A003/semantic_pass_core_feature_contract_summary.json"),
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

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):  # required checks
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):  # forbidden checks
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
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
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
