#!/usr/bin/env python3
"""Run mixed-module and import/export differential stress validation."""

from __future__ import annotations

import argparse
import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Callable, Sequence

import check_objc3c_runtime_acceptance as runtime_acceptance


ROOT = Path(__file__).resolve().parents[1]
MANIFEST_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "mixed_module_differential_manifest.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "stress" / "mixed-module-differential-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.mixed-module-differential.summary.v1"

CaseRunner = Callable[[Path, str | None], runtime_acceptance.CaseResult]


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"expected JSON object at {repo_rel(path)}")
    return payload


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest", type=Path, default=MANIFEST_PATH)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--contract-mode", action="store_true")
    return parser.parse_args(argv)


def run_no_clang(
    fn: Callable[[Path], runtime_acceptance.CaseResult],
    run_dir: Path,
    _: str | None,
) -> runtime_acceptance.CaseResult:
    return fn(run_dir)


def run_with_clang(
    fn: Callable[[str, Path], runtime_acceptance.CaseResult],
    run_dir: Path,
    clangxx: str | None,
) -> runtime_acceptance.CaseResult:
    if not clangxx:
        raise RuntimeError("mixed-module differential requires clang++ for the selected case set")
    return fn(clangxx, run_dir)


CASE_RUNNERS: dict[str, CaseRunner] = {
    "cross-module-runtime-package-interop-source-surface": lambda run_dir, clangxx: run_no_clang(
        runtime_acceptance.check_cross_module_runtime_package_interop_source_surface_case,
        run_dir,
        clangxx,
    ),
    "textual-binary-interface-parity-source-surface": lambda run_dir, clangxx: run_no_clang(
        runtime_acceptance.check_textual_binary_interface_parity_source_surface_case,
        run_dir,
        clangxx,
    ),
    "mixed-image-compatibility-interop-semantics": lambda run_dir, clangxx: run_no_clang(
        runtime_acceptance.check_mixed_image_compatibility_interop_semantics_case,
        run_dir,
        clangxx,
    ),
    "imported-runtime-packaging-replay": lambda run_dir, clangxx: run_with_clang(
        runtime_acceptance.check_imported_runtime_packaging_replay_case,
        run_dir,
        clangxx,
    ),
    "c-cpp-swift-bridge-compatibility-semantics": lambda run_dir, clangxx: run_no_clang(
        runtime_acceptance.check_c_cpp_swift_bridge_compatibility_semantics_case,
        run_dir,
        clangxx,
    ),
    "import-version-feature-claim-diagnostics": lambda run_dir, clangxx: run_no_clang(
        runtime_acceptance.check_import_version_feature_claim_diagnostics_case,
        run_dir,
        clangxx,
    ),
    "runtime-packaging-bridge-loader-artifact-surface": lambda run_dir, clangxx: run_no_clang(
        runtime_acceptance.check_runtime_packaging_bridge_loader_artifact_surface_case,
        run_dir,
        clangxx,
    ),
    "mixed-image-package-lowering-bridge-emission": lambda run_dir, clangxx: run_no_clang(
        runtime_acceptance.check_mixed_image_package_lowering_bridge_emission_case,
        run_dir,
        clangxx,
    ),
    "cross-language-replay-import-surface-preservation": lambda run_dir, clangxx: run_no_clang(
        runtime_acceptance.check_cross_language_replay_import_surface_preservation_case,
        run_dir,
        clangxx,
    ),
    "runtime-package-loader-bridge-abi": lambda run_dir, clangxx: run_with_clang(
        runtime_acceptance.check_runtime_package_loader_bridge_abi_case,
        run_dir,
        clangxx,
    ),
    "live-package-loading-interop-runtime-implementation": lambda run_dir, clangxx: run_with_clang(
        runtime_acceptance.check_live_package_loading_interop_runtime_implementation_case,
        run_dir,
        clangxx,
    ),
}

SURFACE_BUILDERS: dict[str, Callable[[list[runtime_acceptance.CaseResult]], dict[str, Any]]] = {
    "runtime_cross_module_package_interop_source_surface": runtime_acceptance.build_runtime_cross_module_package_interop_source_surface,
    "runtime_textual_binary_interface_parity_source_surface": runtime_acceptance.build_runtime_textual_binary_interface_parity_source_surface,
    "runtime_mixed_image_compatibility_interop_semantics_surface": runtime_acceptance.build_runtime_mixed_image_compatibility_interop_semantics_surface,
    "runtime_package_loading_module_identity_semantics_surface": runtime_acceptance.build_runtime_package_loading_module_identity_semantics_surface,
    "runtime_c_cpp_swift_bridge_compatibility_semantics_surface": runtime_acceptance.build_runtime_c_cpp_swift_bridge_compatibility_semantics_surface,
    "runtime_import_version_feature_claim_diagnostics_surface": runtime_acceptance.build_runtime_import_version_feature_claim_diagnostics_surface,
    "runtime_packaging_bridge_loader_artifact_surface": runtime_acceptance.build_runtime_packaging_bridge_loader_artifact_surface,
    "runtime_mixed_image_package_lowering_bridge_emission_surface": runtime_acceptance.build_runtime_mixed_image_package_lowering_bridge_emission_surface,
    "runtime_cross_language_replay_import_surface_preservation_surface": runtime_acceptance.build_runtime_cross_language_replay_import_surface_preservation_surface,
    "runtime_package_loader_bridge_abi_surface": runtime_acceptance.build_runtime_package_loader_bridge_abi_surface,
    "runtime_package_loading_interop_implementation_surface": runtime_acceptance.build_runtime_package_loading_interop_implementation_surface,
}


def load_manifest(path: Path) -> dict[str, Any]:
    payload = load_json(path)
    if payload.get("contract_id") != "objc3c.stress.mixed-module-differential.manifest.v1":
        raise RuntimeError("mixed-module differential manifest contract_id drifted")
    if payload.get("schema_version") != 1:
        raise RuntimeError("mixed-module differential manifest schema_version drifted")
    case_ids = payload.get("case_ids")
    if not isinstance(case_ids, list) or not case_ids:
        raise RuntimeError("mixed-module differential manifest missing case_ids")
    for case_id in case_ids:
        if not isinstance(case_id, str) or case_id not in CASE_RUNNERS:
            raise RuntimeError(f"mixed-module differential manifest references unsupported case {case_id!r}")
    surface_contracts = payload.get("surface_contracts")
    if not isinstance(surface_contracts, dict) or not surface_contracts:
        raise RuntimeError("mixed-module differential manifest missing surface_contracts")
    for surface_key, contract_id in surface_contracts.items():
        if surface_key not in SURFACE_BUILDERS:
            raise RuntimeError(f"mixed-module differential manifest references unsupported surface {surface_key!r}")
        if not isinstance(contract_id, str) or not contract_id:
            raise RuntimeError(f"mixed-module differential manifest surface {surface_key!r} has invalid contract")
    fixture_groups = payload.get("fixture_groups")
    if not isinstance(fixture_groups, list) or not fixture_groups:
        raise RuntimeError("mixed-module differential manifest missing fixture_groups")
    for group in fixture_groups:
        if not isinstance(group, dict):
            raise RuntimeError("mixed-module differential manifest fixture_groups entry was not an object")
        for key in ("group_id", "provider", "consumer"):
            value = group.get(key)
            if not isinstance(value, str) or not value:
                raise RuntimeError(f"mixed-module differential manifest fixture group missing {key}")
            if key != "group_id" and not (ROOT / value).is_file():
                raise RuntimeError(f"mixed-module differential manifest references missing fixture {value}")
    return {
        "case_ids": [str(case_id) for case_id in case_ids],
        "surface_contracts": {str(key): str(value) for key, value in surface_contracts.items()},
        "fixture_groups": fixture_groups,
    }


def run_cases(case_ids: list[str], run_root: Path) -> list[runtime_acceptance.CaseResult]:
    clangxx: str | None = None
    results: list[runtime_acceptance.CaseResult] = []
    for case_id in case_ids:
        if case_id in {
            "imported-runtime-packaging-replay",
            "runtime-package-loader-bridge-abi",
            "live-package-loading-interop-runtime-implementation",
        } and clangxx is None:
            clangxx = runtime_acceptance.find_clangxx()
        result = CASE_RUNNERS[case_id](run_root, clangxx)
        if result.case_id != case_id:
            raise RuntimeError(f"mixed-module differential runner drifted for case {case_id}")
        if result.passed is not True:
            raise RuntimeError(f"mixed-module differential case failed: {case_id}")
        results.append(result)
    return results


def build_surfaces(
    surface_contracts: dict[str, str],
    results: list[runtime_acceptance.CaseResult],
) -> dict[str, dict[str, Any]]:
    surfaces: dict[str, dict[str, Any]] = {}
    for surface_key, contract_id in surface_contracts.items():
        surface = SURFACE_BUILDERS[surface_key](results)
        if surface.get("contract_id") != contract_id:
            raise RuntimeError(f"mixed-module differential surface {surface_key} drifted from contract {contract_id}")
        surfaces[surface_key] = surface
    return surfaces


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    manifest_path = args.manifest.resolve()
    manifest = load_manifest(manifest_path)

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    run_root = ROOT / "tmp" / "artifacts" / "stress" / "mixed-module-differential" / run_id
    run_root.mkdir(parents=True, exist_ok=True)

    results = run_cases(manifest["case_ids"], run_root)
    surfaces = build_surfaces(manifest["surface_contracts"], results)

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "manifest_path": repo_rel(manifest_path),
        "run_root": repo_rel(run_root),
        "case_count": len(results),
        "surface_count": len(surfaces),
        "case_ids": manifest["case_ids"],
        "fixture_groups": manifest["fixture_groups"],
        "case_summaries": [
            {
                "case_id": result.case_id,
                "probe": result.probe,
                "fixture": result.fixture,
                "claim_class": result.claim_class,
                "summary": result.summary,
            }
            for result in results
        ],
        "surfaces": surfaces,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if args.contract_mode:
        sys.stdout.write(json.dumps(payload, indent=2) + "\n")
    else:
        print(f"summary_path: {repo_rel(args.summary_out)}")
        print("objc3c-mixed-module-differential: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
