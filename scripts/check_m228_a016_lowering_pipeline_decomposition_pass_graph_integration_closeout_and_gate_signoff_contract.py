#!/usr/bin/env python3
"""Fail-closed validator for M228-A016 integration closeout and gate sign-off contract."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m228-lowering-pipeline-pass-graph-integration-closeout-gate-signoff-contract-a016-v1"

ARTIFACTS: dict[str, Path] = {
    "a015_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_advanced_core_workpack_shard1_a015_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m228_lowering_pipeline_decomposition_pass_graph_integration_closeout_and_gate_signoff_a016_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m228"
    / "m228_a016_lowering_pipeline_decomposition_pass_graph_integration_closeout_and_gate_signoff_packet.md",
    "frontend_types": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_frontend_types.h",
    "parse_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "artifacts_source": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_frontend_artifacts.cpp",
    "runbook": ROOT / "docs" / "runbooks" / "m228_wave_execution_runbook.md",
    "package_json": ROOT / "package.json",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "a015_contract_doc": (
        (
            "M228-A016-DEP-01",
            "Contract ID: `objc3c-lowering-pipeline-pass-graph-advanced-core-workpack-shard1/m228-a015-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M228-A016-DOC-01",
            "Contract ID: `objc3c-lowering-pipeline-pass-graph-integration-closeout-gate-signoff/m228-a016-v1`",
        ),
        ("M228-A016-DOC-02", "Dependencies: `M228-A015`"),
        ("M228-A016-DOC-03", "toolchain_runtime_ga_operations_integration_closeout_signoff_consistent"),
        ("M228-A016-DOC-04", "toolchain_runtime_ga_operations_integration_closeout_signoff_ready"),
        ("M228-A016-DOC-05", "toolchain_runtime_ga_operations_integration_closeout_signoff_key"),
        ("M228-A016-DOC-06", "npm run check:objc3c:m228-a016-lane-a-readiness"),
    ),
    "packet_doc": (
        ("M228-A016-PKT-01", "Packet: `M228-A016`"),
        ("M228-A016-PKT-02", "Dependencies: `M228-A015`"),
        (
            "M228-A016-PKT-03",
            "scripts/check_m228_a016_lowering_pipeline_decomposition_pass_graph_integration_closeout_and_gate_signoff_contract.py",
        ),
    ),
    "frontend_types": (
        (
            "M228-A016-TYP-01",
            "bool toolchain_runtime_ga_operations_integration_closeout_signoff_consistent = false;",
        ),
        (
            "M228-A016-TYP-02",
            "bool toolchain_runtime_ga_operations_integration_closeout_signoff_ready = false;",
        ),
        (
            "M228-A016-TYP-03",
            "std::string toolchain_runtime_ga_operations_integration_closeout_signoff_key;",
        ),
    ),
    "parse_surface": (
        (
            "M228-A016-SUR-01",
            "IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffConsistent(",
        ),
        (
            "M228-A016-SUR-02",
            "IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffReady(",
        ),
        (
            "M228-A016-SUR-03",
            "BuildObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffKey(",
        ),
        (
            "M228-A016-SUR-04",
            "surface.toolchain_runtime_ga_operations_integration_closeout_signoff_consistent =",
        ),
        (
            "M228-A016-SUR-05",
            "surface.toolchain_runtime_ga_operations_integration_closeout_signoff_ready =",
        ),
        (
            "M228-A016-SUR-06",
            "surface.toolchain_runtime_ga_operations_integration_closeout_signoff_key =",
        ),
        ("M228-A016-SUR-07", "surface.long_tail_grammar_gate_signoff_ready ="),
        (
            "M228-A016-SUR-08",
            "\"toolchain/runtime GA operations integration closeout and sign-off is not ready\"",
        ),
    ),
    "artifacts_source": (
        (
            "M228-A016-ART-01",
            ",\\\"toolchain_runtime_ga_operations_integration_closeout_signoff_consistent\\\": ",
        ),
        (
            "M228-A016-ART-02",
            ",\\\"toolchain_runtime_ga_operations_integration_closeout_signoff_ready\\\": ",
        ),
        (
            "M228-A016-ART-03",
            "\\\",\\\"toolchain_runtime_ga_operations_integration_closeout_signoff_key\\\":\\\"",
        ),
    ),
    "runbook": (
        (
            "M228-A016-RBK-01",
            "objc3c-lowering-pipeline-pass-graph-integration-closeout-gate-signoff/m228-a016-v1",
        ),
        (
            "M228-A016-RBK-02",
            "python scripts/check_m228_a016_lowering_pipeline_decomposition_pass_graph_integration_closeout_and_gate_signoff_contract.py",
        ),
        (
            "M228-A016-RBK-03",
            "python -m pytest tests/tooling/test_check_m228_a016_lowering_pipeline_decomposition_pass_graph_integration_closeout_and_gate_signoff_contract.py -q",
        ),
        ("M228-A016-RBK-04", "npm run check:objc3c:m228-a016-lane-a-readiness"),
    ),
    "package_json": (
        (
            "M228-A016-CFG-01",
            '"check:objc3c:m228-a016-lowering-pipeline-pass-graph-integration-closeout-gate-signoff-contract"',
        ),
        (
            "M228-A016-CFG-02",
            '"test:tooling:m228-a016-lowering-pipeline-pass-graph-integration-closeout-gate-signoff-contract"',
        ),
        ("M228-A016-CFG-03", '"check:objc3c:m228-a016-lane-a-readiness"'),
        ("M228-A016-CFG-04", "check:objc3c:m228-a015-lane-a-readiness"),
    ),
    "architecture_doc": (
        (
            "M228-A016-ARC-01",
            "M228 lane-A A016 integration closeout and gate sign-off anchors deterministic",
        ),
    ),
    "lowering_spec": (
        (
            "M228-A016-SPC-01",
            "integration closeout and gate sign-off wiring shall preserve deterministic",
        ),
    ),
    "metadata_spec": (
        (
            "M228-A016-META-01",
            "deterministic lane-A integration closeout and gate sign-off anchors for",
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
            "tmp/reports/m228/M228-A016/integration_closeout_and_gate_signoff_contract_summary.json"
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
