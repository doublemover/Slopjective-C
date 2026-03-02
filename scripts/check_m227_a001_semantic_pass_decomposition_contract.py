#!/usr/bin/env python3
"""Fail-closed validator for M227-A001 semantic pass decomposition freeze contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-sema-pass-decomposition-and-symbol-flow-freeze-contract-a001-v1"

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
    "pipeline_types": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_frontend_types.h",
    "pipeline_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_frontend_pipeline.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "contract_doc": ROOT / "docs" / "contracts" / "m227_semantic_pass_decomposition_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_pass_manager_contract": (
        ("M227-A001-CNT-01", "struct Objc3SemaPassFlowSummary {"),
        (
            "M227-A001-CNT-02",
            "std::array<Objc3SemaPassId, 3> configured_pass_order = kObjc3SemaPassOrder;",
        ),
        ("M227-A001-CNT-03", "inline bool IsReadyObjc3SemaPassFlowSummary("),
        ("M227-A001-CNT-04", "Objc3SemaPassFlowSummary sema_pass_flow_summary;"),
        (
            "M227-A001-CNT-05",
            "IsReadyObjc3SemaPassFlowSummary(surface.sema_pass_flow_summary)",
        ),
    ),
    "sema_pass_manager_source": (
        ("M227-A001-SRC-01", "result.sema_pass_flow_summary.pass_executed[pass_index] = true;"),
        (
            "M227-A001-SRC-02",
            "++result.sema_pass_flow_summary.executed_pass_count;",
        ),
        (
            "M227-A001-SRC-03",
            "result.sema_pass_flow_summary.pass_order_matches_contract =",
        ),
        (
            "M227-A001-SRC-04",
            "result.sema_pass_flow_summary.symbol_flow_counts_consistent =",
        ),
        (
            "M227-A001-SRC-05",
            "result.parity_surface.sema_pass_flow_summary = result.sema_pass_flow_summary;",
        ),
    ),
    "pipeline_types": (
        ("M227-A001-PIPE-01", "Objc3SemaPassFlowSummary sema_pass_flow_summary;"),
    ),
    "pipeline_source": (
        (
            "M227-A001-PIPE-02",
            "result.sema_pass_flow_summary = sema_result.sema_pass_flow_summary;",
        ),
    ),
    "architecture_doc": (
        ("M227-A001-ARCH-01", "M227 extends the sema boundary with pass-order and symbol-flow freeze rules"),
        ("M227-A001-ARCH-02", "`Objc3SemaPassFlowSummary`"),
    ),
    "contract_doc": (
        (
            "M227-A001-DOC-01",
            "Contract ID: `objc3c-sema-pass-decomposition-and-symbol-flow-freeze/m227-a001-v1`",
        ),
        ("M227-A001-DOC-02", "Objc3SemaPassFlowSummary"),
        ("M227-A001-DOC-03", "Objc3SemaParityContractSurface"),
        ("M227-A001-DOC-04", "tmp/reports/m227/M227-A001/semantic_pass_decomposition_contract_summary.json"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "pipeline_source": (
        ("M227-A001-PIPE-03", "BuildSemanticTypeMetadataHandoff("),
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
        default=Path("tmp/reports/m227/M227-A001/semantic_pass_decomposition_contract_summary.json"),
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
