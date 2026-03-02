#!/usr/bin/env python3
"""Fail-closed validator for M227-A005 semantic pass edge compatibility completion contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-sema-pass-edge-compat-completion-contract-a005-v1"

ARTIFACTS: dict[str, Path] = {
    "sema_pass_manager_contract": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_manager_contract.h",
    "sema_pass_manager_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_manager.cpp",
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
    "contract_doc": ROOT / "docs" / "contracts" / "m227_semantic_pass_edge_compat_completion_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_pass_manager_contract": (
        ("M227-A005-CNT-01", "Objc3SemaCompatibilityMode compatibility_mode = Objc3SemaCompatibilityMode::Canonical;"),
        ("M227-A005-CNT-02", "bool migration_assist_enabled = false;"),
        ("M227-A005-CNT-03", "std::size_t migration_legacy_literal_total = 0;"),
        ("M227-A005-CNT-04", "bool compatibility_handoff_consistent = false;"),
    ),
    "sema_pass_manager_source": (
        ("M227-A005-SRC-01", "result.sema_pass_flow_summary.compatibility_mode = input.compatibility_mode;"),
        ("M227-A005-SRC-02", "result.sema_pass_flow_summary.migration_assist_enabled = input.migration_assist;"),
        ("M227-A005-SRC-03", "result.sema_pass_flow_summary.migration_legacy_literal_total = input.migration_hints.legacy_total();"),
    ),
    "sema_pass_flow_scaffold": (
        ("M227-A005-SCF-01", "summary.compatibility_handoff_consistent ="),
        ("M227-A005-SCF-02", "summary.compatibility_mode == Objc3SemaCompatibilityMode::Canonical"),
        ("M227-A005-SCF-03", '<< ":compat="'),
        ("M227-A005-SCF-04", "summary.migration_legacy_literal_total"),
    ),
    "frontend_artifacts": (
        ("M227-A005-ART-01", "pass_flow_compatibility_mode"),
        ("M227-A005-ART-02", "pass_flow_migration_assist_enabled"),
        ("M227-A005-ART-03", "pass_flow_migration_legacy_literal_total"),
        ("M227-A005-ART-04", "pass_flow_compatibility_handoff_consistent"),
    ),
    "contract_doc": (
        ("M227-A005-DOC-01", "Contract ID: `objc3c-sema-pass-edge-compat-completion/m227-a005-v1`"),
        ("M227-A005-DOC-02", "compatibility_mode"),
        ("M227-A005-DOC-03", "migration_assist_enabled"),
        ("M227-A005-DOC-04", "pass_flow_compatibility_handoff_consistent"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_pass_flow_scaffold": (
        (
            "M227-A005-SCF-05",
            "summary.compatibility_handoff_consistent = true;",
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
        default=Path("tmp/reports/m227/M227-A005/semantic_pass_edge_compat_completion_contract_summary.json"),
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
