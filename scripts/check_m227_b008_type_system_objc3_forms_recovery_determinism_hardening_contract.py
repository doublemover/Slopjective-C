#!/usr/bin/env python3
"""Fail-closed validator for M227-B008 ObjC3 type-form recovery/determinism hardening contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-type-system-objc3-forms-recovery-determinism-hardening-b008-v1"

ARTIFACTS: dict[str, Path] = {
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.h",
    "scaffold_source": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.cpp",
    "sema_contract": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h",
    "semantic_passes": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp",
    "b007_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_type_system_objc3_forms_diagnostics_hardening_b007_expectations.md",
    "b007_checker": ROOT / "scripts" / "check_m227_b007_type_system_objc3_forms_diagnostics_hardening_contract.py",
    "b007_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m227_b007_type_system_objc3_forms_diagnostics_hardening_contract.py",
    "b007_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_b007_type_system_objc3_forms_diagnostics_hardening_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_type_system_objc3_forms_recovery_determinism_hardening_b008_expectations.md",
    "planning_packet": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_b008_type_system_objc3_forms_recovery_determinism_hardening_packet.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_header": (
        ("M227-B008-HDR-01", "bool recovery_determinism_consistent = false;"),
        ("M227-B008-HDR-02", "bool recovery_determinism_ready = false;"),
        ("M227-B008-HDR-03", "std::string recovery_determinism_key;"),
    ),
    "scaffold_source": (
        ("M227-B008-SRC-01", "summary.recovery_determinism_consistent ="),
        ("M227-B008-SRC-02", "summary.recovery_determinism_key ="),
        ("M227-B008-SRC-03", "summary.recovery_determinism_ready ="),
        (
            "M227-B008-SRC-04",
            "type-form-recovery-determinism;diagnostics-consistent=",
        ),
        ("M227-B008-SRC-05", ";diagnostics-ready="),
        ("M227-B008-SRC-06", ";diagnostics-key-ready="),
        ("M227-B008-SRC-07", "summary.recovery_determinism_consistent &&"),
        ("M227-B008-SRC-08", "summary.recovery_determinism_ready &&"),
        ("M227-B008-SRC-09", "!summary.recovery_determinism_key.empty()"),
    ),
    "sema_contract": (
        (
            "M227-B008-SEM-01",
            "bool canonical_type_form_recovery_determinism_consistent = false;",
        ),
        (
            "M227-B008-SEM-02",
            "bool canonical_type_form_recovery_determinism_ready = false;",
        ),
        (
            "M227-B008-SEM-03",
            "std::string canonical_type_form_recovery_determinism_key;",
        ),
    ),
    "semantic_passes": (
        (
            "M227-B008-PSS-01",
            "summary.canonical_type_form_recovery_determinism_consistent = scaffold.recovery_determinism_consistent;",
        ),
        (
            "M227-B008-PSS-02",
            "summary.canonical_type_form_recovery_determinism_ready = scaffold.recovery_determinism_ready;",
        ),
        (
            "M227-B008-PSS-03",
            "summary.canonical_type_form_recovery_determinism_key = scaffold.recovery_determinism_key;",
        ),
        ("M227-B008-PSS-04", "summary.canonical_type_form_recovery_determinism_consistent &&"),
        ("M227-B008-PSS-05", "summary.canonical_type_form_recovery_determinism_ready &&"),
        ("M227-B008-PSS-06", "!summary.canonical_type_form_recovery_determinism_key.empty() &&"),
        (
            "M227-B008-PSS-07",
            ".canonical_type_form_recovery_determinism_consistent ==",
        ),
        (
            "M227-B008-PSS-08",
            ".canonical_type_form_recovery_determinism_ready ==",
        ),
        (
            "M227-B008-PSS-09",
            ".canonical_type_form_recovery_determinism_key ==",
        ),
        (
            "M227-B008-PSS-10",
            ".canonical_type_form_recovery_determinism_key.empty()",
        ),
    ),
    "b007_contract_doc": (
        (
            "M227-B008-DEP-01",
            "Contract ID: `objc3c-type-system-objc3-forms-diagnostics-hardening/m227-b007-v1`",
        ),
    ),
    "b007_checker": (
        (
            "M227-B008-DEP-02",
            'MODE = "m227-type-system-objc3-forms-diagnostics-hardening-b007-v1"',
        ),
    ),
    "b007_tooling_test": (
        ("M227-B008-DEP-03", "def test_contract_passes_on_repository_sources"),
    ),
    "b007_packet_doc": (
        ("M227-B008-DEP-04", "Packet: `M227-B007`"),
    ),
    "contract_doc": (
        (
            "M227-B008-DOC-01",
            "Contract ID: `objc3c-type-system-objc3-forms-recovery-determinism-hardening/m227-b008-v1`",
        ),
        ("M227-B008-DOC-02", "Execute issue `#4849`"),
        ("M227-B008-DOC-03", "Dependencies: `M227-B007`"),
        (
            "M227-B008-DOC-04",
            "tmp/reports/m227/M227-B008/type_system_objc3_forms_recovery_determinism_hardening_contract_summary.json",
        ),
        ("M227-B008-DOC-05", "check:objc3c:m227-b008-lane-b-readiness"),
    ),
    "planning_packet": (
        ("M227-B008-PKT-01", "Packet: `M227-B008`"),
        ("M227-B008-PKT-02", "Issue: `#4849`"),
        ("M227-B008-PKT-03", "Dependencies: `M227-B007`"),
        (
            "M227-B008-PKT-04",
            "python scripts/check_m227_b008_type_system_objc3_forms_recovery_determinism_hardening_contract.py",
        ),
    ),
    "package_json": (
        (
            "M227-B008-PKG-01",
            '"check:objc3c:m227-b008-type-system-objc3-forms-recovery-determinism-hardening-contract"',
        ),
        (
            "M227-B008-PKG-02",
            '"test:tooling:m227-b008-type-system-objc3-forms-recovery-determinism-hardening-contract"',
        ),
        ("M227-B008-PKG-03", '"check:objc3c:m227-b007-lane-b-readiness"'),
        ("M227-B008-PKG-04", '"check:objc3c:m227-b008-lane-b-readiness"'),
    ),
    "architecture_doc": (
        (
            "M227-B008-ARC-01",
            "M227 lane-B B008 type-system recovery and determinism hardening anchors",
        ),
    ),
    "lowering_spec": (
        (
            "M227-B008-SPC-01",
            "type-system recovery/determinism hardening governance shall preserve explicit lane-B dependency anchors (`M227-B008`, `M227-B007`)",
        ),
    ),
    "metadata_spec": (
        (
            "M227-B008-META-01",
            "deterministic lane-B type-system recovery/determinism hardening metadata anchors for `M227-B008`",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "scaffold_source": (
        ("M227-B008-FORB-01", "summary.recovery_determinism_ready = true;"),
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
        default=Path("tmp/reports/m227/M227-B008/type_system_objc3_forms_recovery_determinism_hardening_contract_summary.json"),
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
            findings.append(Finding(artifact, f"M227-B008-MISS-{artifact.upper()}", str(exc)))
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
