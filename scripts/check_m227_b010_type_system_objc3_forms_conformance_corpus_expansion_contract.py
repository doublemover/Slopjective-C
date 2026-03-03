#!/usr/bin/env python3
"""Fail-closed validator for M227-B010 ObjC3 type-form conformance corpus expansion contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-type-system-objc3-forms-conformance-corpus-expansion-b010-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.h",
    "scaffold_source": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.cpp",
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h",
    "semantic_passes": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp",
    "b009_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_type_system_objc3_forms_conformance_matrix_implementation_b009_expectations.md",
    "b009_checker": ROOT / "scripts" / "check_m227_b009_type_system_objc3_forms_conformance_matrix_implementation_contract.py",
    "b009_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m227_b009_type_system_objc3_forms_conformance_matrix_implementation_contract.py",
    "b009_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_b009_type_system_objc3_forms_conformance_matrix_implementation_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_type_system_objc3_forms_conformance_corpus_expansion_b010_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_b010_type_system_objc3_forms_conformance_corpus_expansion_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M227-B010-HDR-01", "std::size_t conformance_corpus_case_count = 0;"),
        ("M227-B010-HDR-02", "std::size_t conformance_corpus_passed_case_count = 0;"),
        ("M227-B010-HDR-03", "std::size_t conformance_corpus_failed_case_count = 0;"),
        ("M227-B010-HDR-04", "bool conformance_corpus_consistent = false;"),
        ("M227-B010-HDR-05", "bool conformance_corpus_ready = false;"),
        ("M227-B010-HDR-06", "std::string conformance_corpus_key;"),
    ),
    "scaffold_source": (
        ("M227-B010-SRC-01", "summary.conformance_corpus_case_count ="),
        ("M227-B010-SRC-02", "summary.conformance_corpus_passed_case_count ="),
        ("M227-B010-SRC-03", "summary.conformance_corpus_failed_case_count ="),
        ("M227-B010-SRC-04", "summary.conformance_corpus_consistent ="),
        ("M227-B010-SRC-05", "summary.conformance_corpus_key ="),
        ("M227-B010-SRC-06", "summary.conformance_corpus_ready ="),
        ("M227-B010-SRC-07", "type-form-conformance-corpus;matrix-consistent="),
        ("M227-B010-SRC-08", ";matrix-ready="),
        ("M227-B010-SRC-09", ";matrix-key-ready="),
        ("M227-B010-SRC-10", ";case-count="),
        ("M227-B010-SRC-11", ";passed="),
        ("M227-B010-SRC-12", ";failed="),
        ("M227-B010-SRC-13", "summary.conformance_corpus_case_count > 0u &&"),
        (
            "M227-B010-SRC-14",
            "summary.conformance_corpus_passed_case_count == summary.conformance_corpus_case_count",
        ),
        ("M227-B010-SRC-15", "summary.conformance_corpus_failed_case_count == 0u"),
    ),
    "sema_contract": (
        (
            "M227-B010-SEM-01",
            "std::size_t canonical_type_form_conformance_corpus_case_count = 0;",
        ),
        (
            "M227-B010-SEM-02",
            "std::size_t canonical_type_form_conformance_corpus_passed_case_count = 0;",
        ),
        (
            "M227-B010-SEM-03",
            "std::size_t canonical_type_form_conformance_corpus_failed_case_count = 0;",
        ),
        (
            "M227-B010-SEM-04",
            "bool canonical_type_form_conformance_corpus_consistent = false;",
        ),
        (
            "M227-B010-SEM-05",
            "bool canonical_type_form_conformance_corpus_ready = false;",
        ),
        (
            "M227-B010-SEM-06",
            "std::string canonical_type_form_conformance_corpus_key;",
        ),
    ),
    "semantic_passes": (
        (
            "M227-B010-PSS-01",
            "summary.canonical_type_form_conformance_corpus_case_count = scaffold.conformance_corpus_case_count;",
        ),
        (
            "M227-B010-PSS-02",
            "summary.canonical_type_form_conformance_corpus_passed_case_count = scaffold.conformance_corpus_passed_case_count;",
        ),
        (
            "M227-B010-PSS-03",
            "summary.canonical_type_form_conformance_corpus_failed_case_count = scaffold.conformance_corpus_failed_case_count;",
        ),
        (
            "M227-B010-PSS-04",
            "summary.canonical_type_form_conformance_corpus_consistent = scaffold.conformance_corpus_consistent;",
        ),
        (
            "M227-B010-PSS-05",
            "summary.canonical_type_form_conformance_corpus_ready = scaffold.conformance_corpus_ready;",
        ),
        (
            "M227-B010-PSS-06",
            "summary.canonical_type_form_conformance_corpus_key = scaffold.conformance_corpus_key;",
        ),
        (
            "M227-B010-PSS-07",
            "summary.canonical_type_form_conformance_corpus_case_count > 0u &&",
        ),
        (
            "M227-B010-PSS-08",
            "summary.canonical_type_form_conformance_corpus_failed_case_count == 0u &&",
        ),
        (
            "M227-B010-PSS-09",
            "summary.canonical_type_form_conformance_corpus_consistent &&",
        ),
        (
            "M227-B010-PSS-10",
            "summary.canonical_type_form_conformance_corpus_ready &&",
        ),
        (
            "M227-B010-PSS-11",
            "!summary.canonical_type_form_conformance_corpus_key.empty() &&",
        ),
        (
            "M227-B010-PSS-12",
            ".canonical_type_form_conformance_corpus_case_count ==",
        ),
        (
            "M227-B010-PSS-13",
            ".canonical_type_form_conformance_corpus_key ==",
        ),
    ),
    "b009_contract_doc": (
        (
            "M227-B010-DEP-01",
            "Contract ID: `objc3c-type-system-objc3-forms-conformance-matrix-implementation/m227-b009-v1`",
        ),
    ),
    "b009_checker": (
        (
            "M227-B010-DEP-02",
            'MODE = "m227-type-system-objc3-forms-conformance-matrix-implementation-b009-v1"',
        ),
    ),
    "b009_tooling_test": (
        ("M227-B010-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "b009_packet_doc": (
        ("M227-B010-DEP-04", "Packet: `M227-B009`"),
    ),
    "contract_doc": (
        (
            "M227-B010-DOC-01",
            "Contract ID: `objc3c-type-system-objc3-forms-conformance-corpus-expansion/m227-b010-v1`",
        ),
        ("M227-B010-DOC-02", "Execute issue `#4851`"),
        ("M227-B010-DOC-03", "Dependencies: `M227-B009`"),
        (
            "M227-B010-DOC-04",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        (
            "M227-B010-DOC-05",
            "tmp/reports/m227/M227-B010/type_system_objc3_forms_conformance_corpus_expansion_contract_summary.json",
        ),
        ("M227-B010-DOC-06", "check:objc3c:m227-b010-lane-b-readiness"),
    ),
    "planning_packet": (
        ("M227-B010-PKT-01", "Packet: `M227-B010`"),
        ("M227-B010-PKT-02", "Issue: `#4851`"),
        ("M227-B010-PKT-03", "Dependencies: `M227-B009`"),
        (
            "M227-B010-PKT-04",
            "python scripts/check_m227_b010_type_system_objc3_forms_conformance_corpus_expansion_contract.py",
        ),
    ),
    "package_json": (
        (
            "M227-B010-PKG-01",
            '"check:objc3c:m227-b010-type-system-objc3-forms-conformance-corpus-expansion-contract"',
        ),
        (
            "M227-B010-PKG-02",
            '"test:tooling:m227-b010-type-system-objc3-forms-conformance-corpus-expansion-contract"',
        ),
        ("M227-B010-PKG-03", '"check:objc3c:m227-b010-lane-b-readiness"'),
        ("M227-B010-PKG-04", '"check:objc3c:m227-b009-lane-b-readiness"'),
        ("M227-B010-PKG-05", '"test:objc3c:sema-pass-manager-diagnostics-bus"'),
        ("M227-B010-PKG-06", '"test:objc3c:lowering-regression"'),
    ),
    "architecture_doc": (
        (
            "M227-B010-ARC-01",
            "M227 lane-B B010 type-system conformance corpus expansion anchors",
        ),
    ),
    "lowering_spec": (
        (
            "M227-B010-SPC-01",
            "type-system conformance corpus expansion governance shall preserve explicit lane-B dependency anchors (`M227-B010`, `M227-B009`)",
        ),
    ),
    "metadata_spec": (
        (
            "M227-B010-META-01",
            "deterministic lane-B type-system conformance corpus metadata anchors for `M227-B010`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_source": (
        ("M227-B010-FORB-01", "summary.conformance_corpus_ready = true;"),
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
            "tmp/reports/m227/M227-B010/type_system_objc3_forms_conformance_corpus_expansion_contract_summary.json"
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
    checks_total = 0
    checks_passed = 0

    for artifact, path in ARTIFACTS.items():
        try:
            text = load_text(path, artifact=artifact)
        except ValueError as exc:
            checks_total += 1
            findings.append(Finding(artifact, f"M227-B010-MISS-{artifact.upper()}", str(exc)))
            continue

        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                checks_passed += 1
            else:
                findings.append(Finding(artifact, check_id, f"missing snippet: {snippet}"))

        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            checks_total += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                checks_passed += 1

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": checks_total,
        "checks_passed": checks_passed,
        "failures": [{"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail} for f in findings],
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
