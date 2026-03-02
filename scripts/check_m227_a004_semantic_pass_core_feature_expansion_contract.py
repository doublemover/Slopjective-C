#!/usr/bin/env python3
"""Fail-closed validator for M227-A004 semantic pass core feature expansion contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-sema-pass-core-feature-expansion-contract-a004-v1"

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
    "contract_doc": ROOT / "docs" / "contracts" / "m227_semantic_pass_core_feature_expansion_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_pass_manager_contract": (
        ("M227-A004-CNT-01", "std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};"),
        ("M227-A004-CNT-02", "std::size_t transition_edge_count = 0;"),
        ("M227-A004-CNT-03", "bool diagnostics_emission_totals_consistent = false;"),
        ("M227-A004-CNT-04", "bool replay_key_deterministic = false;"),
    ),
    "sema_pass_manager_source": (
        (
            "M227-A004-SRC-01",
            "result.sema_pass_flow_summary.diagnostics_emitted_by_pass = result.diagnostics_emitted_by_pass;",
        ),
    ),
    "sema_pass_flow_scaffold": (
        ("M227-A004-SCF-01", "const std::size_t diagnostics_emitted_total = std::accumulate("),
        ("M227-A004-SCF-02", "summary.diagnostics_emission_totals_consistent = diagnostics_emitted_total == summary.diagnostics_total;"),
        ("M227-A004-SCF-03", "summary.transition_edge_count = summary.executed_pass_count > 0u ? summary.executed_pass_count - 1u : 0u;"),
        ("M227-A004-SCF-04", "summary.replay_key_deterministic ="),
    ),
    "frontend_artifacts": (
        ("M227-A004-ART-01", "pass_flow_diagnostics_emitted_by_build"),
        ("M227-A004-ART-02", "pass_flow_diagnostics_emitted_by_validate_bodies"),
        ("M227-A004-ART-03", "pass_flow_diagnostics_emitted_by_validate_pure_contract"),
        ("M227-A004-ART-04", "pass_flow_transition_edge_count"),
        ("M227-A004-ART-05", "pass_flow_diagnostics_emission_totals_consistent"),
        ("M227-A004-ART-06", "pass_flow_replay_key_deterministic"),
    ),
    "contract_doc": (
        ("M227-A004-DOC-01", "Contract ID: `objc3c-sema-pass-core-feature-expansion/m227-a004-v1`"),
        ("M227-A004-DOC-02", "diagnostics_emitted_by_pass"),
        ("M227-A004-DOC-03", "transition_edge_count"),
        ("M227-A004-DOC-04", "pass_flow_replay_key_deterministic"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "frontend_artifacts": (
        (
            "M227-A004-ART-07",
            'pass_flow_diagnostics_emitted_by_build\\":\\"',
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
        default=Path("tmp/reports/m227/M227-A004/semantic_pass_core_feature_expansion_contract_summary.json"),
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
