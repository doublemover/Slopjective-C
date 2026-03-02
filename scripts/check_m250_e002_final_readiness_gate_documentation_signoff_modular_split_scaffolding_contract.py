#!/usr/bin/env python3
"""Fail-closed validator for M250-E002 final readiness gate/doc/sign-off modular split scaffolding."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-final-readiness-gate-documentation-signoff-modular-split-scaffolding-contract-e002-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_modular_split_scaffolding_e002_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_e002_final_readiness_gate_documentation_signoff_modular_split_scaffolding_packet.md",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
    "e001_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_final_readiness_gate_documentation_signoff_contract_freeze_e001_expectations.md",
    "a002_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_frontend_stability_long_tail_grammar_modular_split_a002_expectations.md",
    "b002_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_semantic_stability_spec_delta_closure_modular_split_scaffolding_b002_expectations.md",
    "c002_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_c002_expectations.md",
    "d002_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_d002_expectations.md",
    "e001_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_e001_final_readiness_gate_documentation_signoff_contract_freeze_packet.md",
    "a002_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_a002_frontend_stability_long_tail_grammar_modular_split_packet.md",
    "b002_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_b002_semantic_stability_spec_delta_closure_modular_split_scaffolding_packet.md",
    "c002_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_c002_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_packet.md",
    "d002_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_d002_toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_packet.md",
    "e001_checker": ROOT / "scripts" / "check_m250_e001_final_readiness_gate_documentation_signoff_contract.py",
    "a002_checker": ROOT
    / "scripts"
    / "check_m250_a002_frontend_stability_long_tail_grammar_modular_split_contract.py",
    "b002_checker": ROOT
    / "scripts"
    / "check_m250_b002_semantic_stability_spec_delta_closure_modular_split_scaffolding_contract.py",
    "c002_checker": ROOT
    / "scripts"
    / "check_m250_c002_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract.py",
    "d002_checker": ROOT
    / "scripts"
    / "check_m250_d002_toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_contract.py",
    "e001_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m250_e001_final_readiness_gate_documentation_signoff_contract.py",
    "a002_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m250_a002_frontend_stability_long_tail_grammar_modular_split_contract.py",
    "b002_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m250_b002_semantic_stability_spec_delta_closure_modular_split_scaffolding_contract.py",
    "c002_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m250_c002_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract.py",
    "d002_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m250_d002_toolchain_runtime_ga_operations_readiness_modular_split_scaffolding_contract.py",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M250-E002-DOC-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-modular-split-scaffolding/m250-e002-v1`",
        ),
        ("M250-E002-DOC-02", "`M250-E001`"),
        ("M250-E002-DOC-03", "`M250-A002`"),
        ("M250-E002-DOC-04", "`M250-B002`"),
        ("M250-E002-DOC-05", "`M250-C002`"),
        ("M250-E002-DOC-06", "`M250-D002`"),
        ("M250-E002-DOC-07", "check:objc3c:m250-e002-lane-e-readiness"),
        (
            "M250-E002-DOC-08",
            "tmp/reports/m250/M250-E002/final_readiness_gate_documentation_signoff_modular_split_scaffolding_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M250-E002-PKT-01", "Packet: `M250-E002`"),
        (
            "M250-E002-PKT-02",
            "Dependencies: `M250-E001`, `M250-A002`, `M250-B002`, `M250-C002`, `M250-D002`",
        ),
        (
            "M250-E002-PKT-03",
            "scripts/check_m250_e002_final_readiness_gate_documentation_signoff_modular_split_scaffolding_contract.py",
        ),
        (
            "M250-E002-PKT-04",
            "tests/tooling/test_check_m250_e002_final_readiness_gate_documentation_signoff_modular_split_scaffolding_contract.py",
        ),
    ),
    "architecture_doc": (
        (
            "M250-E002-ARC-01",
            "Lane E: workflow/governance docs and CI policy wiring",
        ),
    ),
    "package_json": (
        (
            "M250-E002-CFG-01",
            '"check:objc3c:m250-e002-final-readiness-gate-documentation-signoff-modular-split-scaffolding-contract"',
        ),
        (
            "M250-E002-CFG-02",
            '"test:tooling:m250-e002-final-readiness-gate-documentation-signoff-modular-split-scaffolding-contract"',
        ),
        (
            "M250-E002-CFG-03",
            '"check:objc3c:m250-e002-lane-e-readiness"',
        ),
        ("M250-E002-CFG-04", "check:objc3c:m250-e001-lane-e-readiness"),
        ("M250-E002-CFG-05", "check:objc3c:m250-a002-lane-a-readiness"),
        ("M250-E002-CFG-06", "check:objc3c:m250-b002-lane-b-readiness"),
        ("M250-E002-CFG-07", "check:objc3c:m250-c002-lane-c-readiness"),
        ("M250-E002-CFG-08", "check:objc3c:m250-d002-lane-d-readiness"),
    ),
    "e001_contract_doc": (
        (
            "M250-E002-DEP-01",
            "Contract ID: `objc3c-final-readiness-gate-documentation-signoff-freeze/m250-e001-v1`",
        ),
    ),
    "a002_contract_doc": (
        ("M250-E002-DEP-02", "Contract ID: `objc3c-frontend-stability-long-tail-grammar-modular-split/m250-a002-v1`"),
    ),
    "b002_contract_doc": (
        (
            "M250-E002-DEP-03",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-modular-split-scaffolding/m250-b002-v1`",
        ),
    ),
    "c002_contract_doc": (
        (
            "M250-E002-DEP-04",
            "Contract ID: `objc3c-lowering-runtime-stability-invariant-proofs-modular-split-scaffolding/m250-c002-v1`",
        ),
    ),
    "d002_contract_doc": (
        (
            "M250-E002-DEP-05",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-modular-split-scaffolding/m250-d002-v1`",
        ),
    ),
    "e001_packet_doc": (
        ("M250-E002-UP-01", "Packet: `M250-E001`"),
    ),
    "a002_packet_doc": (
        ("M250-E002-UP-02", "Packet: `M250-A002`"),
    ),
    "b002_packet_doc": (
        ("M250-E002-UP-03", "Packet: `M250-B002`"),
    ),
    "c002_packet_doc": (
        ("M250-E002-UP-04", "Packet: `M250-C002`"),
    ),
    "d002_packet_doc": (
        ("M250-E002-UP-05", "Packet: `M250-D002`"),
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
            "tmp/reports/m250/M250-E002/"
            "final_readiness_gate_documentation_signoff_modular_split_scaffolding_contract_summary.json"
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
