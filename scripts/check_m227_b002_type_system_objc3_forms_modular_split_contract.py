#!/usr/bin/env python3
"""Fail-closed validator for M227-B002 ObjC3 type-form modular split contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-type-system-objc3-forms-modular-split-b002-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.h",
    "scaffold_source": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.cpp",
    "semantic_passes": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp",
    "cmake_file": ROOT / "native" / "objc3c" / "CMakeLists.txt",
    "build_script": ROOT / "scripts" / "build_objc3c_native.ps1",
    "contract_doc": ROOT / "docs" / "contracts" / "m227_type_system_objc3_forms_modular_split_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M227-B002-HDR-01", "struct Objc3TypeFormScaffoldSummary {"),
        ("M227-B002-HDR-02", "Objc3TypeFormScaffoldSummary BuildObjc3TypeFormScaffoldSummary();"),
        ("M227-B002-HDR-03", "bool IsReadyObjc3TypeFormScaffoldSummary(const Objc3TypeFormScaffoldSummary &summary);"),
    ),
    "scaffold_source": (
        ("M227-B002-SRC-01", "HasUniqueValueTypes"),
        ("M227-B002-SRC-02", "IsSubsetOfCanonicalReferenceForms"),
        ("M227-B002-SRC-03", "summary.canonical_bridge_top_subset_of_reference"),
        ("M227-B002-SRC-04", "summary.canonical_bridge_top_forms_unique"),
        ("M227-B002-SRC-05", "IsReadyObjc3TypeFormScaffoldSummary"),
    ),
    "semantic_passes": (
        ("M227-B002-SEM-01", '#include "sema/objc3_type_form_scaffold.h"'),
        ("M227-B002-SEM-02", "IsCanonicalObjc3TypeFormScaffoldReady"),
        ("M227-B002-SEM-03", "BuildObjc3TypeFormScaffoldSummary()"),
        ("M227-B002-SEM-04", "if (!IsCanonicalObjc3TypeFormScaffoldReady()) {"),
    ),
    "cmake_file": (
        ("M227-B002-CFG-01", "src/sema/objc3_type_form_scaffold.cpp"),
    ),
    "build_script": (
        ("M227-B002-CFG-02", "native/objc3c/src/sema/objc3_type_form_scaffold.cpp"),
    ),
    "contract_doc": (
        ("M227-B002-DOC-01", "Contract ID: `objc3c-type-system-objc3-forms-modular-split/m227-b002-v1`"),
        ("M227-B002-DOC-02", "objc3_type_form_scaffold.h"),
        ("M227-B002-DOC-03", "objc3_type_form_scaffold.cpp"),
        ("M227-B002-DOC-04", "BuildObjc3TypeFormScaffoldSummary"),
        ("M227-B002-DOC-05", "tmp/reports/m227/M227-B002/type_system_objc3_forms_modular_split_contract_summary.json"),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "semantic_passes": (
        ("M227-B002-FORB-01", "target.type == ValueType::ObjCId || value.type == ValueType::ObjCId"),
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
        default=Path("tmp/reports/m227/M227-B002/type_system_objc3_forms_modular_split_contract_summary.json"),
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
