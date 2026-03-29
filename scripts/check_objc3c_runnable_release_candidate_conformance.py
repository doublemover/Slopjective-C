#!/usr/bin/env python3
"""Validate runnable release-candidate conformance against the integrated live workflow."""

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
REPORT_PATH = (
    ROOT / "tmp" / "reports" / "runtime" / "runnable-release-candidate-conformance" / "summary.json"
)
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.release.candidate.conformance.summary.v1"

REQUIRED_CASES = {
    "claimable-surface-residual-non-claimable-gaps-source-surface",
    "strict-profile-feature-claim-source-surface",
    "claimability-semantics-release-policy",
    "strict-profile-claim-implementation",
    "scaffold-retirement-deprecated-sidecar-compatibility-diagnostics",
    "claim-publication-dashboard-schema-surface",
    "final-claim-publication-deprecated-path-shutdown",
    "release-candidate-runtime-claim-abi",
    "final-release-evidence-descaffolding-implementation",
}

REQUIRED_SURFACE_CONTRACTS = {
    "runtime_claimable_surface_residual_non_claimable_gaps_source_surface": (
        "objc3c.runtime.claimable.surface.residual.non.claimable.gaps.source.surface.v1"
    ),
    "runtime_strict_profile_feature_claim_source_surface": (
        "objc3c.runtime.strict.profile.feature.claim.source.surface.v1"
    ),
    "runtime_claimability_semantics_release_policy_surface": (
        "objc3c.runtime.claimability.semantics.release.policy.surface.v1"
    ),
    "runtime_strict_profile_claim_implementation_surface": (
        "objc3c.runtime.strict.profile.claim.implementation.surface.v1"
    ),
    "runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface": (
        "objc3c.runtime.scaffold.retirement.deprecated.sidecar.compatibility.diagnostics.surface.v1"
    ),
    "runtime_claim_publication_dashboard_schema_surface": (
        "objc3c.runtime.claim.publication.dashboard.schema.surface.v1"
    ),
    "runtime_final_claim_publication_deprecated_path_shutdown_surface": (
        "objc3c.runtime.final.claim.publication.deprecated.path.shutdown.surface.v1"
    ),
    "runtime_release_candidate_claim_abi_surface": (
        "objc3c.runtime.release.candidate.claim.abi.surface.v1"
    ),
    "runtime_final_release_evidence_descaffolding_implementation_surface": (
        "objc3c.runtime.final.release.evidence.descaffolding.implementation.surface.v1"
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
    fallback_root = (
        ROOT / "tmp" / "reports" / "runtime" / "runnable-release-candidate-conformance" / "live-case"
    )
    fallback_root.mkdir(parents=True, exist_ok=True)
    clangxx = runtime_acceptance.find_clangxx()
    with tempfile.TemporaryDirectory(dir=fallback_root) as tmp_dir:
        run_dir = Path(tmp_dir)
        results = [
            runtime_acceptance.check_claimable_surface_residual_non_claimable_gaps_source_surface_case(
                run_dir
            ),
            runtime_acceptance.check_strict_profile_feature_claim_source_surface_case(run_dir),
            runtime_acceptance.check_claimability_semantics_release_policy_case(run_dir),
            runtime_acceptance.check_strict_profile_claim_implementation_case(run_dir),
            runtime_acceptance.check_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_case(
                run_dir
            ),
            runtime_acceptance.check_claim_publication_dashboard_schema_surface_case(run_dir),
            runtime_acceptance.check_final_claim_publication_deprecated_path_shutdown_case(run_dir),
            runtime_acceptance.check_release_candidate_runtime_claim_abi_case(clangxx, run_dir),
            runtime_acceptance.check_final_release_evidence_descaffolding_implementation_case(
                clangxx, run_dir
            ),
        ]
        run_dir_rel = repo_rel(run_dir)
    return results, run_dir_rel


def build_live_surfaces(results: list[runtime_acceptance.CaseResult]) -> dict[str, dict[str, Any]]:
    return {
        "runtime_claimable_surface_residual_non_claimable_gaps_source_surface": (
            runtime_acceptance.build_runtime_claimable_surface_residual_non_claimable_gaps_source_surface(
                results
            )
        ),
        "runtime_strict_profile_feature_claim_source_surface": (
            runtime_acceptance.build_runtime_strict_profile_feature_claim_source_surface(results)
        ),
        "runtime_claimability_semantics_release_policy_surface": (
            runtime_acceptance.build_runtime_claimability_semantics_release_policy_surface(results)
        ),
        "runtime_strict_profile_claim_implementation_surface": (
            runtime_acceptance.build_runtime_strict_profile_claim_implementation_surface(results)
        ),
        "runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface": (
            runtime_acceptance.build_runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface(
                results
            )
        ),
        "runtime_claim_publication_dashboard_schema_surface": (
            runtime_acceptance.build_runtime_claim_publication_dashboard_schema_surface(results)
        ),
        "runtime_final_claim_publication_deprecated_path_shutdown_surface": (
            runtime_acceptance.build_runtime_final_claim_publication_deprecated_path_shutdown_surface(
                results
            )
        ),
        "runtime_release_candidate_claim_abi_surface": (
            runtime_acceptance.build_runtime_release_candidate_claim_abi_surface(results)
        ),
        "runtime_final_release_evidence_descaffolding_implementation_surface": (
            runtime_acceptance.build_runtime_final_release_evidence_descaffolding_implementation_surface(
                results
            )
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
            expect(result.passed is True, f"required release-candidate case {result.case_id} did not pass")
        expect(
            {result.case_id for result in results} == REQUIRED_CASES,
            "live release-candidate case collection drifted from the required case set",
        )
        surfaces = build_live_surfaces(results)
        surface_source = "live-targeted-cases"

    for surface_key, contract_id in REQUIRED_SURFACE_CONTRACTS.items():
        surface = surfaces.get(surface_key)
        expect(
            isinstance(surface, dict),
            f"release-candidate conformance did not publish {surface_key}",
        )
        expect(
            surface.get("contract_id") == contract_id,
            f"release-candidate conformance published the wrong contract id for {surface_key}",
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
                    f"runtime integration report drifted from live release-candidate surface {surface_key}",
                )
            integration_surface_comparison = "matched"
        else:
            integration_surface_comparison = "stale-or-missing"

    claim_surface = surfaces["runtime_release_candidate_claim_abi_surface"]
    evidence_surface = surfaces[
        "runtime_final_release_evidence_descaffolding_implementation_surface"
    ]
    strict_claim_surface = surfaces[
        "runtime_strict_profile_claim_implementation_surface"
    ]
    final_publication_surface = surfaces[
        "runtime_final_claim_publication_deprecated_path_shutdown_surface"
    ]
    dashboard_surface = surfaces[
        "runtime_claim_publication_dashboard_schema_surface"
    ]

    expect(
        claim_surface.get("claimed_profile_ids") == strict_claim_surface.get("claimed_profile_ids"),
        "release-candidate claim ABI surface drifted from the strict-profile claim implementation profile set",
    )
    expect(
        claim_surface.get("targeted_profile_ids")
        == ["strict", "strict-concurrency", "strict-system"],
        "release-candidate claim ABI surface drifted from the targeted strict profile set",
    )
    expect(
        evidence_surface.get("runtime_release_candidate_claim_abi_surface_contract_id")
        == claim_surface.get("contract_id"),
        "final release evidence implementation surface drifted from the release-candidate claim ABI contract",
    )
    expect(
        dashboard_surface.get("dashboard_schema_path")
        == "schemas/objc3-conformance-dashboard-status-v1.schema.json",
        "claim dashboard schema surface drifted from the live dashboard schema path",
    )
    expect(
        final_publication_surface.get("deprecated_sidecar_filenames")
        == [
            "module.objc3-release-runtime-claim-matrix.json",
            "module.objc3-dashboard-ready-summary.json",
            "module.objc3-toolchain-runtime-ga-operations-scaffold.json",
        ],
        "final claim publication surface drifted from the retired sidecar inventory",
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
        "runner_path": "scripts/check_objc3c_runnable_release_candidate_conformance.py",
        "surface_source": surface_source,
        "integration_surface_comparison": integration_surface_comparison,
        "required_case_ids": sorted(REQUIRED_CASES),
        "required_surface_keys": list(REQUIRED_SURFACE_CONTRACTS.keys()),
        "child_report_paths": child_report_paths,
        "live_case_run_dir": live_run_dir,
        "runtime_release_candidate_claim_abi_surface": claim_surface,
        "runtime_final_release_evidence_descaffolding_implementation_surface": evidence_surface,
        "runtime_strict_profile_claim_implementation_surface": strict_claim_surface,
        "runtime_final_claim_publication_deprecated_path_shutdown_surface": (
            final_publication_surface
        ),
        "runtime_claim_publication_dashboard_schema_surface": dashboard_surface,
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
