#!/usr/bin/env python3
"""Fail-closed validator for M227-A002 semantic pass modular split scaffolding contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-sema-pass-modular-split-scaffold-contract-a002-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_flow_scaffold.h",
    "scaffold_source": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_flow_scaffold.cpp",
    "sema_pass_manager_source": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp",
    "cmake": ROOT / "native" / "objc3c" / "CMakeLists.txt",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "a001_contract_doc": ROOT / "docs" / "contracts" / "m227_semantic_pass_decomposition_expectations.md",
    "contract_doc": ROOT / "docs" / "contracts" / "m227_semantic_pass_modular_split_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M227-A002-H-01", "void MarkObjc3SemaPassExecuted(Objc3SemaPassFlowSummary &summary, Objc3SemaPassId pass);"),
        ("M227-A002-H-02", "void FinalizeObjc3SemaPassFlowSummary("),
    ),
    "scaffold_source": (
        ("M227-A002-CPP-01", "#include \"sema/objc3_sema_pass_flow_scaffold.h\""),
        ("M227-A002-CPP-02", "void MarkObjc3SemaPassExecuted(Objc3SemaPassFlowSummary &summary, Objc3SemaPassId pass) {"),
        ("M227-A002-CPP-03", "void FinalizeObjc3SemaPassFlowSummary("),
        ("M227-A002-CPP-04", "summary.symbol_flow_counts_consistent ="),
    ),
    "sema_pass_manager_source": (
        ("M227-A002-SRC-01", "#include \"sema/objc3_sema_pass_flow_scaffold.h\""),
        ("M227-A002-SRC-02", "MarkObjc3SemaPassExecuted(result.sema_pass_flow_summary, pass);"),
        ("M227-A002-SRC-03", "FinalizeObjc3SemaPassFlowSummary(result.sema_pass_flow_summary,"),
    ),
    "cmake": (
        ("M227-A002-CMAKE-01", "src/sema/objc3_sema_pass_flow_scaffold.cpp"),
    ),
    "architecture_doc": (
        ("M227-A002-ARCH-01", "Objc3SemaPassFlowSummary"),
    ),
    "a001_contract_doc": (
        ("M227-A002-A001-01", "Objc3SemaPassFlowSummary"),
    ),
    "contract_doc": (
        ("M227-A002-DOC-01", "Contract ID: `objc3c-sema-pass-modular-split-scaffold/m227-a002-v1`"),
        ("M227-A002-DOC-02", "objc3_sema_pass_flow_scaffold.cpp"),
        ("M227-A002-DOC-03", "MarkObjc3SemaPassExecuted"),
        ("M227-A002-DOC-04", "FinalizeObjc3SemaPassFlowSummary"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_pass_manager_source": (
        ("M227-A002-SRC-04", "++result.sema_pass_flow_summary.executed_pass_count;"),
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
        default=Path("tmp/reports/m227/M227-A002/semantic_pass_modular_split_contract_summary.json"),
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
