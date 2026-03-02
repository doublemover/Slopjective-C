#!/usr/bin/env python3
"""Fail-closed validator for M227-B005 ObjC3 type-form edge compatibility contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-type-system-objc3-forms-edge-compat-b005-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.h",
    "scaffold_source": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.cpp",
    "semantic_passes": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m227_type_system_objc3_forms_edge_compat_b005_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M227-B005-HDR-01", "bool canonical_message_scalars_disjoint_from_reference = false;"),
        ("M227-B005-HDR-02", "bool canonical_bridge_top_excludes_sel = false;"),
    ),
    "scaffold_source": (
        ("M227-B005-SRC-01", "AreDisjointFromCanonicalReferenceForms"),
        ("M227-B005-SRC-02", "summary.canonical_message_scalars_disjoint_from_reference ="),
        ("M227-B005-SRC-03", "summary.canonical_bridge_top_excludes_sel ="),
        (
            "M227-B005-SRC-04",
            "!ContainsValueType(kObjc3CanonicalBridgeTopReferenceTypeForms, ValueType::ObjCSel)",
        ),
        ("M227-B005-SRC-05", "summary.canonical_message_scalars_disjoint_from_reference &&"),
        ("M227-B005-SRC-06", "summary.canonical_bridge_top_excludes_sel &&"),
    ),
    "semantic_passes": (
        ("M227-B005-SEM-01", "IsCanonicalObjc3TypeFormScaffoldReady"),
    ),
    "contract_doc": (
        ("M227-B005-DOC-01", "Contract ID: `objc3c-type-system-objc3-forms-edge-compat/m227-b005-v1`"),
        ("M227-B005-DOC-02", "canonical_message_scalars_disjoint_from_reference"),
        ("M227-B005-DOC-03", "canonical_bridge_top_excludes_sel"),
        ("M227-B005-DOC-04", "tmp/reports/m227/M227-B005/type_system_objc3_forms_edge_compat_contract_summary.json"),
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
        default=Path("tmp/reports/m227/M227-B005/type_system_objc3_forms_edge_compat_contract_summary.json"),
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
