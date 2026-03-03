#!/usr/bin/env python3
"""Fail-closed validator for M227-A017 semantic pass advanced diagnostics shard1 contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m227-semantic-pass-advanced-diagnostics-workpack-shard1-contract-a017-v1"

ARTIFACTS: dict[str, Path] = {
    "a015_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_semantic_pass_advanced_core_workpack_shard1_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m227_semantic_pass_advanced_diagnostics_workpack_shard1_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m227"
    / "m227_a017_semantic_pass_advanced_diagnostics_workpack_shard1_packet.md",
    "parse_surface": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_parse_lowering_readiness_surface.h",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "runbook": ROOT / "docs" / "runbooks" / "m227_wave_execution_runbook.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "a015_contract_doc": (
        (
            "M227-A017-DEP-01",
            "Contract ID: `objc3c-semantic-pass-advanced-core-workpack-shard1/m227-a015-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M227-A017-DOC-01",
            "Contract ID: `objc3c-semantic-pass-advanced-diagnostics-workpack-shard1/m227-a017-v1`",
        ),
        ("M227-A017-DOC-02", "Dependencies: `M227-A016`"),
        ("M227-A017-DOC-03", "toolchain_runtime_ga_operations_advanced_diagnostics_consistent"),
        ("M227-A017-DOC-04", "toolchain_runtime_ga_operations_advanced_diagnostics_ready"),
        ("M227-A017-DOC-05", "toolchain_runtime_ga_operations_advanced_diagnostics_key"),
        (
            "M227-A017-DOC-06",
            "scripts/check_m227_a017_semantic_pass_advanced_diagnostics_workpack_shard1_contract.py",
        ),
        (
            "M227-A017-DOC-07",
            "tests/tooling/test_check_m227_a017_semantic_pass_advanced_diagnostics_workpack_shard1_contract.py",
        ),
        ("M227-A017-DOC-08", "npm run check:objc3c:m227-a017-lane-a-readiness"),
    ),
    "packet_doc": (
        ("M227-A017-PKT-01", "Packet: `M227-A017`"),
        ("M227-A017-PKT-02", "Dependencies: `M227-A016`"),
        ("M227-A017-PKT-03", "objc3_parse_lowering_readiness_surface.h"),
        (
            "M227-A017-PKT-04",
            "scripts/check_m227_a017_semantic_pass_advanced_diagnostics_workpack_shard1_contract.py",
        ),
    ),
    "parse_surface": (
        ("M227-A017-SUR-01", "IsObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsConsistent("),
        ("M227-A017-SUR-02", "IsObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsReady("),
        ("M227-A017-SUR-03", "BuildObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsKey("),
        (
            "M227-A017-SUR-04",
            "surface.toolchain_runtime_ga_operations_advanced_diagnostics_consistent =",
        ),
        (
            "M227-A017-SUR-05",
            "surface.toolchain_runtime_ga_operations_advanced_diagnostics_ready =",
        ),
        (
            "M227-A017-SUR-06",
            "surface.toolchain_runtime_ga_operations_advanced_diagnostics_key =",
        ),
        (
            "M227-A017-SUR-07",
            "\"toolchain/runtime GA operations advanced diagnostics workpack is inconsistent\"",
        ),
        (
            "M227-A017-SUR-08",
            "\"toolchain/runtime GA operations advanced diagnostics workpack is not ready\"",
        ),
    ),
    "frontend_types": (
        (
            "M227-A017-TYP-01",
            "bool toolchain_runtime_ga_operations_advanced_diagnostics_consistent = false;",
        ),
        (
            "M227-A017-TYP-02",
            "bool toolchain_runtime_ga_operations_advanced_diagnostics_ready = false;",
        ),
        (
            "M227-A017-TYP-03",
            "std::string toolchain_runtime_ga_operations_advanced_diagnostics_key;",
        ),
    ),
    "artifacts_source": (
        (
            "M227-A017-ART-01",
            ",\\\"toolchain_runtime_ga_operations_advanced_diagnostics_consistent\\\": ",
        ),
        (
            "M227-A017-ART-02",
            ",\\\"toolchain_runtime_ga_operations_advanced_diagnostics_ready\\\": ",
        ),
        (
            "M227-A017-ART-03",
            "\\\",\\\"toolchain_runtime_ga_operations_advanced_diagnostics_key\\\":\\\"",
        ),
    ),
    "runbook": (
        (
            "M227-A017-RBK-01",
            "objc3c-semantic-pass-advanced-diagnostics-workpack-shard1/m227-a017-v1",
        ),
        (
            "M227-A017-RBK-02",
            "python scripts/check_m227_a017_semantic_pass_advanced_diagnostics_workpack_shard1_contract.py",
        ),
        (
            "M227-A017-RBK-03",
            "python -m pytest tests/tooling/test_check_m227_a017_semantic_pass_advanced_diagnostics_workpack_shard1_contract.py -q",
        ),
        ("M227-A017-RBK-04", "npm run check:objc3c:m227-a017-lane-a-readiness"),
    ),
    "package_json": (
        (
            "M227-A017-CFG-01",
            '"check:objc3c:m227-a017-semantic-pass-advanced-diagnostics-workpack-shard1-contract"',
        ),
        (
            "M227-A017-CFG-02",
            '"test:tooling:m227-a017-semantic-pass-advanced-diagnostics-workpack-shard1-contract"',
        ),
        ("M227-A017-CFG-03", '"check:objc3c:m227-a017-lane-a-readiness"'),
        ("M227-A017-CFG-04", "check:objc3c:m227-a016-lane-a-readiness"),
    ),
    "architecture_doc": (
        (
            "M227-A017-ARC-01",
            "M227 lane-A A017 advanced diagnostics workpack (shard 1) anchors deterministic",
        ),
    ),
    "lowering_spec": (
        (
            "M227-A017-SPC-01",
            "semantic-pass advanced diagnostics workpack (shard 1) wiring shall preserve deterministic",
        ),
    ),
    "metadata_spec": (
        (
            "M227-A017-META-01",
            "deterministic lane-A semantic-pass advanced diagnostics workpack (shard 1) metadata anchors for `M227-A017`",
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
        default=Path(
            "tmp/reports/m227/M227-A017/advanced_diagnostics_workpack_shard1_contract_summary.json"
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

