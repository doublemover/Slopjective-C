#!/usr/bin/env python3
"""Validate runnable metaprogramming conformance against the integrated live workflow."""

from __future__ import annotations

import json
import os
import subprocess
import sys
import tempfile
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

import check_objc3c_runtime_acceptance as runtime_acceptance


ROOT = Path(__file__).resolve().parents[1]
INTEGRATION_SCRIPT = ROOT / "scripts" / "check_objc3c_runtime_architecture_integration.py"
INTEGRATION_REPORT = ROOT / "tmp" / "reports" / "runtime" / "architecture-integration" / "summary.json"
ACCEPTANCE_REPORT = ROOT / "tmp" / "reports" / "runtime" / "acceptance" / "summary.json"
REPORT_PATH = (
    ROOT / "tmp" / "reports" / "runtime" / "runnable-metaprogramming-conformance" / "summary.json"
)
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.metaprogramming.conformance.summary.v1"

REQUIRED_CASES = {
    "metaprogramming-source-surface",
    "metaprogramming-package-provenance-source-surface",
    "metaprogramming-expansion-semantic-model",
    "metaprogramming-derive-property-behavior-semantics",
    "metaprogramming-macro-safety-cache-diagnostics",
    "metaprogramming-lowering-host-cache-surface",
    "metaprogramming-executable-lowering",
    "cross-module-metaprogramming-artifact-preservation",
    "metaprogramming-runtime-abi-cache-surface",
    "live-metaprogramming-cache-runtime-integration",
}

REQUIRED_SURFACE_CONTRACTS = {
    "runtime_metaprogramming_source_surface": (
        "objc3c.runtime.metaprogramming.source.surface.v1"
    ),
    "runtime_metaprogramming_package_provenance_source_surface": (
        "objc3c.runtime.metaprogramming.package.provenance.source.surface.v1"
    ),
    "runtime_metaprogramming_semantics_surface": (
        "objc3c.runtime.metaprogramming.semantics.surface.v1"
    ),
    "runtime_metaprogramming_lowering_host_cache_surface": (
        "objc3c.runtime.metaprogramming.lowering.host.cache.surface.v1"
    ),
    "runtime_cross_module_metaprogramming_artifact_preservation_surface": (
        "objc3c.runtime.cross.module.metaprogramming.artifact.preservation.surface.v1"
    ),
    "runtime_metaprogramming_runtime_abi_cache_surface": (
        "objc3c.runtime.metaprogramming.runtime.abi.cache.surface.v1"
    ),
    "runtime_metaprogramming_cache_runtime_integration_implementation_surface": (
        "objc3c.runtime.metaprogramming.cache.runtime.integration.implementation.surface.v1"
    ),
}


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_json(path: Path) -> dict[str, Any]:
    expect(path.is_file(), f"expected JSON artifact was not published: {repo_rel(path)}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def ensure_case_passed(case_map: dict[str, dict[str, Any]], case_id: str) -> None:
    case = case_map.get(case_id)
    if case is not None:
        expect(case.get("passed") is True, f"required metaprogramming case {case_id} did not pass")
        return

    fallback_root = (
        ROOT / "tmp" / "reports" / "runtime" / "runnable-metaprogramming-conformance" / "live-case"
    )
    fallback_root.mkdir(parents=True, exist_ok=True)
    clangxx = runtime_acceptance.find_clangxx()
    with tempfile.TemporaryDirectory(dir=fallback_root) as tmp_dir:
        run_dir = Path(tmp_dir)
        if case_id == "metaprogramming-source-surface":
            result = runtime_acceptance.check_metaprogramming_source_surface_case(run_dir)
        elif case_id == "metaprogramming-package-provenance-source-surface":
            result = runtime_acceptance.check_metaprogramming_package_provenance_source_surface_case(
                run_dir
            )
        elif case_id == "metaprogramming-expansion-semantic-model":
            result = runtime_acceptance.check_metaprogramming_semantics_case(run_dir)
        elif case_id == "metaprogramming-derive-property-behavior-semantics":
            result = runtime_acceptance.check_metaprogramming_derive_property_behavior_semantics_case(
                run_dir
            )
        elif case_id == "metaprogramming-macro-safety-cache-diagnostics":
            result = runtime_acceptance.check_metaprogramming_macro_safety_cache_diagnostics_case(
                run_dir
            )
        elif case_id == "metaprogramming-lowering-host-cache-surface":
            result = runtime_acceptance.check_metaprogramming_lowering_host_cache_surface_case(run_dir)
        elif case_id == "metaprogramming-executable-lowering":
            result = runtime_acceptance.check_metaprogramming_executable_lowering_case(clangxx, run_dir)
        elif case_id == "cross-module-metaprogramming-artifact-preservation":
            result = runtime_acceptance.check_cross_module_metaprogramming_artifact_preservation_case(
                run_dir
            )
        elif case_id == "metaprogramming-runtime-abi-cache-surface":
            result = runtime_acceptance.check_metaprogramming_runtime_abi_cache_surface_case(
                clangxx, run_dir
            )
        elif case_id == "live-metaprogramming-cache-runtime-integration":
            result = runtime_acceptance.check_live_metaprogramming_cache_runtime_integration_case(
                clangxx, run_dir
            )
        else:
            raise RuntimeError(f"runtime acceptance report did not publish required case {case_id}")
    expect(result.passed is True, f"required metaprogramming case {case_id} did not pass")


def main() -> int:
    skip_integration_rerun = os.environ.get("OBJC3C_SKIP_INTEGRATION_RERUN") == "1"
    if not skip_integration_rerun:
        integration_result = run_capture([sys.executable, str(INTEGRATION_SCRIPT)])
        if integration_result.returncode != 0:
            raise RuntimeError("runtime architecture integration workflow failed")

    integration_report = load_json(INTEGRATION_REPORT)
    acceptance_report = load_json(ACCEPTANCE_REPORT)
    expect(
        integration_report.get("status") == "PASS",
        "runtime architecture integration report did not publish PASS",
    )
    expect(
        acceptance_report.get("status") == "PASS",
        "runtime acceptance report did not publish PASS",
    )

    cases = acceptance_report.get("cases", [])
    expect(isinstance(cases, list), "runtime acceptance report did not publish cases")
    case_map = {
        str(case.get("case_id")): case
        for case in cases
        if isinstance(case, dict) and case.get("case_id") is not None
    }
    for case_id in sorted(REQUIRED_CASES):
        ensure_case_passed(case_map, case_id)

    for surface_key, contract_id in REQUIRED_SURFACE_CONTRACTS.items():
        acceptance_surface = acceptance_report.get(surface_key)
        integration_surface = integration_report.get(surface_key)
        expect(
            isinstance(acceptance_surface, dict),
            f"runtime acceptance report did not publish {surface_key}",
        )
        expect(
            isinstance(integration_surface, dict),
            f"runtime integration report did not publish {surface_key}",
        )
        expect(
            acceptance_surface.get("contract_id") == contract_id,
            f"runtime acceptance report published the wrong contract id for {surface_key}",
        )
        if isinstance(integration_surface, dict):
            expect(
                integration_surface == acceptance_surface,
                f"runtime integration report drifted from acceptance for {surface_key}",
            )
        elif not skip_integration_rerun:
            raise RuntimeError(f"runtime integration report did not publish {surface_key}")

    abi_surface = acceptance_report["runtime_metaprogramming_runtime_abi_cache_surface"]
    implementation_surface = acceptance_report[
        "runtime_metaprogramming_cache_runtime_integration_implementation_surface"
    ]
    expect(
        "metaprogramming-runtime-abi-cache-surface"
        in abi_surface.get("authoritative_case_ids", []),
        "metaprogramming runtime ABI/cache surface did not carry the runtime ABI case",
    )
    expect(
        "live-metaprogramming-cache-runtime-integration"
        in implementation_surface.get("authoritative_case_ids", []),
        "metaprogramming implementation surface did not carry the live cache/runtime integration case",
    )
    expect(
        implementation_surface.get("macro_host_process_cache_integration_snapshot_symbol")
        == abi_surface.get("macro_host_process_cache_integration_snapshot_symbol"),
        "metaprogramming implementation surface drifted from the runtime ABI/cache snapshot symbol",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_metaprogramming_conformance.py",
        "required_case_ids": sorted(REQUIRED_CASES),
        "required_surface_keys": list(REQUIRED_SURFACE_CONTRACTS.keys()),
        "child_report_paths": [
            repo_rel(INTEGRATION_REPORT),
            repo_rel(ACCEPTANCE_REPORT),
        ],
        "metaprogramming_runtime_abi_cache_surface": abi_surface,
        "metaprogramming_cache_runtime_integration_implementation_surface": (
            implementation_surface
        ),
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
