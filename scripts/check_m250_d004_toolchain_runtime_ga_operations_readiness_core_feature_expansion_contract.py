#!/usr/bin/env python3
"""Fail-closed validator for M250-D004 toolchain/runtime GA operations core-feature expansion."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m250-toolchain-runtime-ga-operations-readiness-core-feature-expansion-contract-d004-v1"

ARTIFACTS: dict[str, Path] = {
    "core_surface_header": ROOT / "native" / "objc3c" / "src" / "io" / "objc3_toolchain_runtime_ga_operations_core_feature_surface.h",
    "driver_source": ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp",
    "frontend_types": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "d003_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_core_feature_implementation_d003_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m250_toolchain_runtime_ga_operations_readiness_core_feature_expansion_d004_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m250"
    / "m250_d004_toolchain_runtime_ga_operations_readiness_core_feature_expansion_packet.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M250-D004-SUR-01", "bool backend_output_path_deterministic = false;"),
        ("M250-D004-SUR-02", "bool backend_output_payload_consistent = false;"),
        ("M250-D004-SUR-03", "bool core_feature_expansion_ready = false;"),
        ("M250-D004-SUR-04", "std::string core_feature_expansion_key;"),
        (
            "M250-D004-SUR-05",
            "BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureExpansionKey(",
        ),
        ("M250-D004-SUR-06", "surface.backend_output_path_deterministic ="),
        ("M250-D004-SUR-07", "surface.backend_output_payload_consistent ="),
        ("M250-D004-SUR-08", "surface.core_feature_expansion_ready ="),
        (
            "M250-D004-SUR-09",
            "surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.core_feature_expansion_ready;",
        ),
        (
            "M250-D004-SUR-10",
            "surface.core_feature_expansion_key = BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureExpansionKey(surface);",
        ),
        (
            "M250-D004-SUR-11",
            'surface.failure_reason = "toolchain/runtime backend output path is not deterministic";',
        ),
        (
            "M250-D004-SUR-12",
            'surface.failure_reason = "toolchain/runtime backend output marker payload is inconsistent";',
        ),
        (
            "M250-D004-SUR-13",
            'surface.failure_reason = "toolchain/runtime core feature expansion is not ready";',
        ),
    ),
    "driver_source": (
        (
            "M250-D004-DRV-01",
            'const fs::path backend_out = cli_options.out_dir / (cli_options.emit_prefix + ".object-backend.txt");',
        ),
        ("M250-D004-DRV-02", "const std::string backend_text ="),
        ("M250-D004-DRV-03", "std::string backend_output_payload;"),
        ("M250-D004-DRV-04", "backend_output_payload = backend_text;"),
        (
            "M250-D004-DRV-05",
            "BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(",
        ),
        ("M250-D004-DRV-06", "backend_out,"),
        ("M250-D004-DRV-07", "backend_output_payload);"),
    ),
    "frontend_types": (
        (
            "M250-D004-TYP-01",
            "struct Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface {",
        ),
        ("M250-D004-TYP-02", "bool backend_output_path_deterministic = false;"),
        ("M250-D004-TYP-03", "bool backend_output_payload_consistent = false;"),
        ("M250-D004-TYP-04", "bool core_feature_expansion_ready = false;"),
        ("M250-D004-TYP-05", "std::string core_feature_expansion_key;"),
    ),
    "architecture_doc": (
        (
            "M250-D004-ARC-01",
            "M250 lane-D D004 core feature expansion anchors explicit backend marker-path",
        ),
        (
            "M250-D004-ARC-02",
            "`io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` plus",
        ),
        (
            "M250-D004-ARC-03",
            "`pipeline/objc3_frontend_types.h`",
        ),
    ),
    "d003_contract_doc": (
        (
            "M250-D004-DEP-01",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-core-feature-implementation/m250-d003-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M250-D004-DOC-01",
            "Contract ID: `objc3c-toolchain-runtime-ga-operations-readiness-core-feature-expansion/m250-d004-v1`",
        ),
        ("M250-D004-DOC-02", "Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface"),
        ("M250-D004-DOC-03", "backend_output_path_deterministic"),
        (
            "M250-D004-DOC-04",
            "python scripts/check_m250_d004_toolchain_runtime_ga_operations_readiness_core_feature_expansion_contract.py",
        ),
        (
            "M250-D004-DOC-05",
            "python -m pytest tests/tooling/test_check_m250_d004_toolchain_runtime_ga_operations_readiness_core_feature_expansion_contract.py -q",
        ),
        ("M250-D004-DOC-06", "npm run check:objc3c:m250-d004-lane-d-readiness"),
        (
            "M250-D004-DOC-07",
            "tmp/reports/m250/M250-D004/toolchain_runtime_ga_operations_readiness_core_feature_expansion_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M250-D004-PKT-01", "Packet: `M250-D004`"),
        ("M250-D004-PKT-02", "Lane: `D`"),
        ("M250-D004-PKT-03", "Dependencies: `M250-D003`"),
        (
            "M250-D004-PKT-04",
            "scripts/check_m250_d004_toolchain_runtime_ga_operations_readiness_core_feature_expansion_contract.py",
        ),
        (
            "M250-D004-PKT-05",
            "tests/tooling/test_check_m250_d004_toolchain_runtime_ga_operations_readiness_core_feature_expansion_contract.py",
        ),
    ),
    "package_json": (
        (
            "M250-D004-CFG-01",
            '"check:objc3c:m250-d004-toolchain-runtime-ga-operations-readiness-core-feature-expansion-contract"',
        ),
        (
            "M250-D004-CFG-02",
            '"test:tooling:m250-d004-toolchain-runtime-ga-operations-readiness-core-feature-expansion-contract"',
        ),
        (
            "M250-D004-CFG-03",
            '"check:objc3c:m250-d004-lane-d-readiness"',
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "core_surface_header": (
        ("M250-D004-FORB-01", "surface.core_feature_expansion_ready = true;"),
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
            "tmp/reports/m250/M250-D004/toolchain_runtime_ga_operations_readiness_core_feature_expansion_contract_summary.json"
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
