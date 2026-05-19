"""Release Claims publication dashboard runtime acceptance cases."""

from __future__ import annotations

import json
from pathlib import Path

from ..expectation_matching import expect
from ..case_result import CaseResult
from ..fixture_compilation import compile_fixture_with_args
from ..paths import NATIVE_EXE, ROOT
from ..process_execution import run
from .release_claims_publication_dashboard_assertions import (
    expect_dashboard_schema_surface,
)
from .release_claims_owner_contracts import release_claims_case_summary
from ..runtime_contract_release import (
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
)


def check_claim_publication_dashboard_schema_surface_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "claim-publication-dashboard-schema-surface"
    fixture = ROOT / Path(RELEASE_CLAIMABLE_SURFACE_FIXTURE)
    compile_dir = case_dir / "compile"
    compile_fixture_with_args(fixture, compile_dir)
    report_path = compile_dir / "module.objc3-conformance-report.json"

    validate_dir = case_dir / "validate"
    validate_dir.mkdir(parents=True, exist_ok=True)
    validation = run(
        [
            str(NATIVE_EXE),
            "--validate-objc3-conformance",
            str(report_path),
            "--out-dir",
            str(validate_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "json",
        ]
    )
    expect(
        validation.returncode == 0,
        "expected conformance validation to publish the dashboard artifact successfully",
    )

    dashboard_path = validate_dir / "module.objc3-dashboard-status.json"
    dashboard = json.loads(dashboard_path.read_text(encoding="utf-8"))
    schema = json.loads(
        (ROOT / "schemas" / "objc3-conformance-dashboard-status-v1.schema.json").read_text(
            encoding="utf-8"
        )
    )
    required_keys = schema.get("required", [])
    artifact_paths = expect_dashboard_schema_surface(dashboard, required_keys)

    return CaseResult(
        case_id="claim-publication-dashboard-schema-surface",
        probe="compile-validate-and-inspect-schema-shaped-dashboard-artifact",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=release_claims_case_summary(
            "claim-publication-dashboard-schema-surface",
            {
                "dashboard_schema_id": dashboard.get("schema_id"),
                "artifact_paths": artifact_paths,
                "profile_ids": [
                    entry.get("profile_id") for entry in dashboard.get("profiles", [])
                ],
            },
        ),
    )


__all__ = [
    "check_claim_publication_dashboard_schema_surface_case",
]
