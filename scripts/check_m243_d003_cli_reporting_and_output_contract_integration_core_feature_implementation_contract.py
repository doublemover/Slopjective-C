#!/usr/bin/env python3
"""Fail-closed validator for M243-D003 CLI/reporting output-contract core feature implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-d003-cli-reporting-output-contract-integration-core-feature-implementation-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "io"
    / "objc3_cli_reporting_output_contract_core_feature_surface.h",
    "scaffold_header": ROOT / "native" / "objc3c" / "src" / "io" / "objc3_cli_reporting_output_contract_scaffold.h",
    "diagnostics_artifacts_source": ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.cpp",
    "c_api_runner_source": ROOT / "native" / "objc3c" / "src" / "tools" / "objc3c_frontend_c_api_runner.cpp",
    "cli_frontend_source": ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "objc3_cli_frontend.cpp",
    "pipeline_contract": ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "lowering_spec": ROOT / "spec" / "LOWERING_AND_RUNTIME_CONTRACTS.md",
    "metadata_spec": ROOT / "spec" / "MODULE_METADATA_AND_ABI_TABLES.md",
    "d002_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_modular_split_scaffolding_d002_expectations.md",
    "d002_packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_packet.md",
    "d002_checker": ROOT
    / "scripts"
    / "check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py",
    "d002_tooling_test": ROOT
    / "tests"
    / "tooling"
    / "test_check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py",
    "package_json": ROOT / "package.json",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_packet.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_cli_reporting_and_output_contract_integration_core_feature_implementation_d003_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M243-D003-SUR-01", "struct Objc3CliReportingOutputContractCoreFeatureSurface {"),
        ("M243-D003-SUR-02", "bool scaffold_ready = false;"),
        ("M243-D003-SUR-03", "bool summary_mode_pinned = false;"),
        ("M243-D003-SUR-04", "bool diagnostics_schema_version_pinned = false;"),
        ("M243-D003-SUR-05", "bool core_feature_impl_ready = false;"),
        ("M243-D003-SUR-06", "BuildObjc3CliReportingOutputContractCoreFeatureKey("),
        ("M243-D003-SUR-07", "BuildObjc3CliReportingOutputContractCoreFeatureSurface("),
        ("M243-D003-SUR-08", "surface.core_feature_impl_ready ="),
        (
            "M243-D003-SUR-09",
            'surface.failure_reason = "summary and diagnostics output paths must be distinct";',
        ),
        (
            "M243-D003-SUR-10",
            "IsObjc3CliReportingOutputContractCoreFeatureSurfaceReady(",
        ),
    ),
    "scaffold_header": (
        ("M243-D003-SCA-01", "struct Objc3CliReportingOutputContractScaffold {"),
        ("M243-D003-SCA-02", "bool modular_split_ready = false;"),
    ),
    "diagnostics_artifacts_source": (
        ("M243-D003-DIA-01", 'out << "  \\"schema_version\\": \\"1.0.0\\",\\n";'),
        ("M243-D003-DIA-02", 'WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());'),
    ),
    "c_api_runner_source": (
        ("M243-D003-RUN-01", '#include "io/objc3_cli_reporting_output_contract_core_feature_surface.h"'),
        ("M243-D003-RUN-02", "BuildObjc3CliReportingOutputContractScaffold("),
        (
            "M243-D003-RUN-03",
            "BuildObjc3CliReportingOutputContractCoreFeatureSurface(",
        ),
        (
            "M243-D003-RUN-04",
            "IsObjc3CliReportingOutputContractCoreFeatureSurfaceReady(",
        ),
        (
            "M243-D003-RUN-05",
            "cli/reporting output core feature fail-closed: ",
        ),
        (
            "M243-D003-RUN-06",
            'out << "  \\"output_contract\\": {\\n";',
        ),
        (
            "M243-D003-RUN-07",
            'out << "    \\"core_feature_impl_ready\\": "',
        ),
        (
            "M243-D003-RUN-08",
            'options.summary_out.empty() ? (options.out_dir / (options.emit_prefix + ".c_api_summary.json")) : options.summary_out;',
        ),
        (
            "M243-D003-RUN-09",
            'WriteStageSummaryJson(out, "emit", result.emit, false);',
        ),
    ),
    "cli_frontend_source": (
        ("M243-D003-CLI-01", "Objc3FrontendArtifactBundle CompileObjc3SourceForCli("),
        ("M243-D003-CLI-02", "return std::move(product.artifact_bundle);"),
    ),
    "pipeline_contract": (
        ("M243-D003-PIP-01", 'std::string output_dir = "tmp/artifacts/compilation/objc3c-native";'),
        ("M243-D003-PIP-02", "ErrorPropagationModel error_model = ErrorPropagationModel::NoThrowFailClosed;"),
    ),
    "architecture_doc": (
        (
            "M243-D003-ARCH-01",
            "M243 lane-D D003 core feature implementation anchors CLI/reporting output contract integration",
        ),
        (
            "M243-D003-ARCH-02",
            "`io/objc3_cli_reporting_output_contract_core_feature_surface.h`",
        ),
    ),
    "lowering_spec": (
        (
            "M243-D003-SPC-01",
            "CLI/reporting and output core feature implementation governance shall preserve",
        ),
        (
            "M243-D003-SPC-02",
            "explicit lane-D dependency anchors (`M243-D002`) and fail closed on",
        ),
    ),
    "metadata_spec": (
        (
            "M243-D003-META-01",
            "deterministic lane-D CLI/reporting output core feature metadata anchors for `M243-D003`",
        ),
        (
            "M243-D003-META-02",
            "explicit `M243-D002` dependency continuity so core feature implementation drift fails closed.",
        ),
    ),
    "d002_contract_doc": (
        (
            "M243-D003-D002-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-modular-split-scaffolding/m243-d002-v1`",
        ),
    ),
    "d002_packet_doc": (
        ("M243-D003-D002-02", "Packet: `M243-D002`"),
        ("M243-D003-D002-03", "Dependencies: `M243-D001`"),
    ),
    "d002_checker": (
        (
            "M243-D003-D002-04",
            'MODE = "m243-d002-cli-reporting-output-contract-integration-modular-split-scaffolding-contract-v1"',
        ),
    ),
    "d002_tooling_test": (
        (
            "M243-D003-D002-05",
            "check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract",
        ),
    ),
    "package_json": (
        (
            "M243-D003-CFG-01",
            '"check:objc3c:m243-d003-cli-reporting-output-contract-integration-core-feature-implementation-contract": '
            '"python scripts/check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py"',
        ),
        (
            "M243-D003-CFG-02",
            '"test:tooling:m243-d003-cli-reporting-output-contract-integration-core-feature-implementation-contract": '
            '"python -m pytest tests/tooling/test_check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py -q"',
        ),
        (
            "M243-D003-CFG-03",
            '"check:objc3c:m243-d003-lane-d-readiness": '
            '"npm run check:objc3c:m243-d002-lane-d-readiness '
            "&& npm run check:objc3c:m243-d003-cli-reporting-output-contract-integration-core-feature-implementation-contract "
            '&& npm run test:tooling:m243-d003-cli-reporting-output-contract-integration-core-feature-implementation-contract"',
        ),
        ("M243-D003-CFG-04", '"compile:objc3c": '),
        ("M243-D003-CFG-05", '"proof:objc3c": '),
        ("M243-D003-CFG-06", '"test:objc3c:execution-replay-proof": '),
        ("M243-D003-CFG-07", '"test:objc3c:perf-budget": '),
    ),
    "packet_doc": (
        (
            "M243-D003-PKT-01",
            "# M243-D003 CLI/Reporting and Output Contract Integration Core Feature Implementation Packet",
        ),
        ("M243-D003-PKT-02", "Packet: `M243-D003`"),
        ("M243-D003-PKT-03", "Dependencies: `M243-D002`"),
        (
            "M243-D003-PKT-04",
            "scripts/check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py",
        ),
        (
            "M243-D003-PKT-05",
            "tests/tooling/test_check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py",
        ),
        ("M243-D003-PKT-06", "`npm run check:objc3c:m243-d003-lane-d-readiness`"),
    ),
    "contract_doc": (
        (
            "M243-D003-DOC-01",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-core-feature-implementation/m243-d003-v1`",
        ),
        (
            "M243-D003-DOC-02",
            "Objc3CliReportingOutputContractCoreFeatureSurface",
        ),
        (
            "M243-D003-DOC-03",
            "BuildObjc3CliReportingOutputContractCoreFeatureSurface",
        ),
        (
            "M243-D003-DOC-04",
            "Dependencies: `M243-D002`",
        ),
        (
            "M243-D003-DOC-05",
            "spec/planning/compiler/m243/m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_packet.md",
        ),
        (
            "M243-D003-DOC-06",
            "python scripts/check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py",
        ),
        (
            "M243-D003-DOC-07",
            "python -m pytest tests/tooling/test_check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py -q",
        ),
        (
            "M243-D003-DOC-08",
            "npm run check:objc3c:m243-d003-lane-d-readiness",
        ),
        (
            "M243-D003-DOC-09",
            "tmp/reports/m243/M243-D003/cli_reporting_and_output_contract_integration_core_feature_implementation_contract_summary.json",
        ),
        (
            "M243-D003-DOC-10",
            "Code/spec anchors and milestone",
        ),
        (
            "M243-D003-DOC-11",
            "optimization improvements are mandatory scope inputs.",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M243-D003-FORB-01", "surface.core_feature_impl_ready = true;"),
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
            "tmp/reports/m243/M243-D003/cli_reporting_and_output_contract_integration_core_feature_implementation_contract_summary.json"
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
