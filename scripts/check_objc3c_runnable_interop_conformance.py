#!/usr/bin/env python3
"""Validate runnable interop conformance against the integrated live workflow."""

from __future__ import annotations

import json
import tempfile
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

import check_objc3c_runtime_acceptance as runtime_acceptance


ROOT = Path(__file__).resolve().parents[1]
INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "runtime" / "architecture-integration" / "summary.json"
ACCEPTANCE_REPORT = ROOT / "tmp" / "reports" / "runtime" / "acceptance" / "summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-interop-conformance" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.interop.conformance.summary.v1"

REQUIRED_CASES = {
    "cross-module-runtime-package-interop-source-surface",
    "textual-binary-interface-parity-source-surface",
    "mixed-image-compatibility-interop-semantics",
    "imported-runtime-packaging-replay",
    "c-cpp-swift-bridge-compatibility-semantics",
    "import-version-feature-claim-diagnostics",
    "runtime-packaging-bridge-loader-artifact-surface",
    "mixed-image-package-lowering-bridge-emission",
    "cross-language-replay-import-surface-preservation",
    "runtime-package-loader-bridge-abi",
    "live-package-loading-interop-runtime-implementation",
}

REQUIRED_SURFACE_CONTRACTS = {
    "runtime_cross_module_package_interop_source_surface": (
        "objc3c.runtime.cross.module.package.interop.source.surface.v1"
    ),
    "runtime_textual_binary_interface_parity_source_surface": (
        "objc3c.runtime.textual.binary.interface.parity.source.surface.v1"
    ),
    "runtime_mixed_image_compatibility_interop_semantics_surface": (
        "objc3c.runtime.mixed.image.compatibility.interop.semantics.surface.v1"
    ),
    "runtime_package_loading_module_identity_semantics_surface": (
        "objc3c.runtime.package.loading.module.identity.semantics.surface.v1"
    ),
    "runtime_c_cpp_swift_bridge_compatibility_semantics_surface": (
        "objc3c.runtime.c.cpp.swift.bridge.compatibility.semantics.surface.v1"
    ),
    "runtime_import_version_feature_claim_diagnostics_surface": (
        "objc3c.runtime.import.version.feature.claim.diagnostics.surface.v1"
    ),
    "runtime_packaging_bridge_loader_artifact_surface": (
        "objc3c.runtime.packaging.bridge.loader.artifact.surface.v1"
    ),
    "runtime_mixed_image_package_lowering_bridge_emission_surface": (
        "objc3c.runtime.mixed.image.package.lowering.bridge.emission.surface.v1"
    ),
    "runtime_cross_language_replay_import_surface_preservation_surface": (
        "objc3c.runtime.cross.language.replay.import.surface.preservation.surface.v1"
    ),
    "runtime_package_loader_bridge_abi_surface": (
        "objc3c.runtime.package.loader.bridge.abi.surface.v1"
    ),
    "runtime_package_loading_interop_implementation_surface": (
        "objc3c.runtime.package.loading.interop.implementation.surface.v1"
    ),
}


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_json(path: Path) -> dict[str, Any] | None:
    if not path.is_file():
        return None
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def collect_live_results() -> tuple[list[runtime_acceptance.CaseResult], str]:
    fallback_root = ROOT / "tmp" / "reports" / "runtime" / "runnable-interop-conformance" / "live-case"
    fallback_root.mkdir(parents=True, exist_ok=True)
    clangxx = runtime_acceptance.find_clangxx()
    with tempfile.TemporaryDirectory(dir=fallback_root) as tmp_dir:
        run_dir = Path(tmp_dir)
        results = [
            runtime_acceptance.check_cross_module_runtime_package_interop_source_surface_case(run_dir),
            runtime_acceptance.check_textual_binary_interface_parity_source_surface_case(run_dir),
            runtime_acceptance.check_mixed_image_compatibility_interop_semantics_case(run_dir),
            runtime_acceptance.check_imported_runtime_packaging_replay_case(clangxx, run_dir),
            runtime_acceptance.check_c_cpp_swift_bridge_compatibility_semantics_case(run_dir),
            runtime_acceptance.check_import_version_feature_claim_diagnostics_case(run_dir),
            runtime_acceptance.check_runtime_packaging_bridge_loader_artifact_surface_case(run_dir),
            runtime_acceptance.check_mixed_image_package_lowering_bridge_emission_case(run_dir),
            runtime_acceptance.check_cross_language_replay_import_surface_preservation_case(run_dir),
            runtime_acceptance.check_runtime_package_loader_bridge_abi_case(clangxx, run_dir),
            runtime_acceptance.check_live_package_loading_interop_runtime_implementation_case(clangxx, run_dir),
        ]
        run_dir_rel = repo_rel(run_dir)
    return results, run_dir_rel


def build_live_surfaces(results: list[runtime_acceptance.CaseResult]) -> dict[str, dict[str, Any]]:
    return {
        "runtime_cross_module_package_interop_source_surface": runtime_acceptance.build_runtime_cross_module_package_interop_source_surface(
            results
        ),
        "runtime_textual_binary_interface_parity_source_surface": runtime_acceptance.build_runtime_textual_binary_interface_parity_source_surface(
            results
        ),
        "runtime_mixed_image_compatibility_interop_semantics_surface": runtime_acceptance.build_runtime_mixed_image_compatibility_interop_semantics_surface(
            results
        ),
        "runtime_package_loading_module_identity_semantics_surface": runtime_acceptance.build_runtime_package_loading_module_identity_semantics_surface(
            results
        ),
        "runtime_c_cpp_swift_bridge_compatibility_semantics_surface": runtime_acceptance.build_runtime_c_cpp_swift_bridge_compatibility_semantics_surface(
            results
        ),
        "runtime_import_version_feature_claim_diagnostics_surface": runtime_acceptance.build_runtime_import_version_feature_claim_diagnostics_surface(
            results
        ),
        "runtime_packaging_bridge_loader_artifact_surface": runtime_acceptance.build_runtime_packaging_bridge_loader_artifact_surface(
            results
        ),
        "runtime_mixed_image_package_lowering_bridge_emission_surface": runtime_acceptance.build_runtime_mixed_image_package_lowering_bridge_emission_surface(
            results
        ),
        "runtime_cross_language_replay_import_surface_preservation_surface": runtime_acceptance.build_runtime_cross_language_replay_import_surface_preservation_surface(
            results
        ),
        "runtime_package_loader_bridge_abi_surface": runtime_acceptance.build_runtime_package_loader_bridge_abi_surface(
            results
        ),
        "runtime_package_loading_interop_implementation_surface": runtime_acceptance.build_runtime_package_loading_interop_implementation_surface(
            results
        ),
    }


def main() -> int:
    acceptance_report = load_json(ACCEPTANCE_REPORT)
    integration_report = load_json(INTEGRATION_REPORT)
    case_map = {}
    if isinstance(acceptance_report, dict):
        cases = acceptance_report.get("cases", [])
        if isinstance(cases, list):
            case_map = {
                str(case.get("case_id")): case
                for case in cases
                if isinstance(case, dict) and case.get("case_id") is not None
            }

    report_has_required_cases = bool(case_map) and all(
        case_map.get(case_id, {}).get("passed") is True for case_id in REQUIRED_CASES
    )
    report_has_required_surfaces = isinstance(acceptance_report, dict) and all(
        isinstance(acceptance_report.get(surface_key), dict)
        for surface_key in REQUIRED_SURFACE_CONTRACTS
    )

    live_run_dir: str | None = None
    if report_has_required_cases and report_has_required_surfaces:
        surfaces = {
            surface_key: acceptance_report[surface_key]
            for surface_key in REQUIRED_SURFACE_CONTRACTS
        }
        surface_source = "acceptance-report"
    else:
        results, live_run_dir = collect_live_results()
        for result in results:
            expect(result.passed is True, f"required interop case {result.case_id} did not pass")
        expect(
            {result.case_id for result in results} == REQUIRED_CASES,
            "live interop case collection drifted from the required case set",
        )
        surfaces = build_live_surfaces(results)
        surface_source = "live-targeted-cases"

    for surface_key, contract_id in REQUIRED_SURFACE_CONTRACTS.items():
        surface = surfaces.get(surface_key)
        expect(
            isinstance(surface, dict),
            f"interop conformance did not publish {surface_key}",
        )
        expect(
            surface.get("contract_id") == contract_id,
            f"interop conformance published the wrong contract id for {surface_key}",
        )

    integration_surface_comparison = "not-available"
    if isinstance(integration_report, dict) and integration_report.get("status") == "PASS":
        integration_surfaces_present = all(
            isinstance(integration_report.get(surface_key), dict)
            for surface_key in REQUIRED_SURFACE_CONTRACTS
        )
        if integration_surfaces_present:
            for surface_key, surface in surfaces.items():
                expect(
                    integration_report.get(surface_key) == surface,
                    f"runtime integration report drifted from live interop surface {surface_key}",
                )
            integration_surface_comparison = "matched"
        else:
            integration_surface_comparison = "stale-or-missing"

    package_loading_surface = surfaces[
        "runtime_package_loading_module_identity_semantics_surface"
    ]
    abi_surface = surfaces["runtime_package_loader_bridge_abi_surface"]
    implementation_surface = surfaces[
        "runtime_package_loading_interop_implementation_surface"
    ]
    expect(
        "imported-runtime-packaging-replay"
        in package_loading_surface.get("authoritative_case_ids", []),
        "package-loading module identity surface did not carry the imported-runtime-packaging-replay case",
    )
    expect(
        "runtime-package-loader-bridge-abi" in abi_surface.get("authoritative_case_ids", []),
        "package-loader bridge ABI surface did not carry the runtime-package-loader-bridge-abi case",
    )
    expect(
        "live-package-loading-interop-runtime-implementation"
        in implementation_surface.get("authoritative_case_ids", []),
        "package-loading interop implementation surface did not carry the live implementation case",
    )
    expect(
        implementation_surface.get("authoritative_probe_paths")
        == abi_surface.get("authoritative_probe_paths"),
        "interop implementation surface drifted from the runtime package-loader ABI probe set",
    )

    child_report_paths = []
    if isinstance(acceptance_report, dict):
        child_report_paths.append(repo_rel(ACCEPTANCE_REPORT))
    if isinstance(integration_report, dict):
        child_report_paths.append(repo_rel(INTEGRATION_REPORT))

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_interop_conformance.py",
        "surface_source": surface_source,
        "integration_surface_comparison": integration_surface_comparison,
        "required_case_ids": sorted(REQUIRED_CASES),
        "required_surface_keys": list(REQUIRED_SURFACE_CONTRACTS.keys()),
        "child_report_paths": child_report_paths,
        "live_case_run_dir": live_run_dir,
        "runtime_package_loading_module_identity_semantics_surface": package_loading_surface,
        "runtime_package_loader_bridge_abi_surface": abi_surface,
        "runtime_package_loading_interop_implementation_surface": implementation_surface,
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
