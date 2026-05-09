"""Release Claims publication dashboard runtime acceptance cases."""

from __future__ import annotations

import json
import re
from pathlib import Path

from ..case_result import CaseResult
from ..commands import run
from ..core import (
    NATIVE_EXE,
    RELEASE_CLAIMABLE_SURFACE_FIXTURE,
    ROOT,
    compile_fixture_with_args,
    expect,
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
    expect(
        all(key in dashboard for key in required_keys),
        "expected dashboard artifact to publish every required top-level schema field",
    )
    expect(
        dashboard.get("schema_id") == "objc3-conformance-dashboard-status/v1"
        and dashboard.get("schema_version") == 1
        and dashboard.get("dashboard_version") == "0.11.0"
        and dashboard.get("release_label") == "v0.11"
        and dashboard.get("status") == "pass",
        "expected dashboard artifact to preserve the live schema identity and release status surface",
    )
    expect(
        [entry.get("profile_id") for entry in dashboard.get("profiles", [])]
        == ["core", "strict", "strict-concurrency", "strict-system"],
        "expected dashboard artifact to publish one schema-shaped profile row for each claimed profile",
    )
    expect(
        [entry.get("dependency_id") for entry in dashboard.get("dependencies", [])]
        == ["B-04", "B-10", "B-11", "B-12"],
        "expected dashboard artifact to publish the schema-shaped dependency inventory",
    )
    artifact_paths = [entry.get("artifact_path") for entry in dashboard.get("artifacts", [])]
    expect(
        artifact_paths
        == [
            "module.objc3-conformance-report.json",
            "module.objc3-conformance-publication.json",
            "module.objc3-conformance-validation.json",
            "module.objc3-release-evidence-operation.json",
        ],
        "expected dashboard artifact to preserve the live emitted artifact refs behind the schema surface",
    )
    expect(
        all(
            isinstance(entry.get("file_sha256"), str)
            and re.fullmatch(r"[a-f0-9]{64}", entry["file_sha256"])
            for entry in dashboard.get("artifacts", [])
        ),
        "expected dashboard artifact to publish schema-shaped lowercase 64-hex file digests for each emitted artifact ref",
    )
    expect(
        dashboard.get("summary", {}).get("profile_counts")
        == {"pass": 4, "fail": 0, "blocked": 0, "incomplete": 0}
        and dashboard.get("summary", {}).get("dependency_counts")
        == {"pass": 4, "fail": 0, "blocked": 0, "stale": 0, "missing": 0},
        "expected dashboard artifact to publish deterministic schema-shaped summary counts",
    )
    expect(
        dashboard.get("refresh", {}).get("trigger") == "manual-replay"
        and dashboard.get("refresh", {}).get("stale_dependency_ids") == []
        and dashboard.get("refresh", {}).get("escalation_state") == "none",
        "expected dashboard artifact to publish deterministic refresh telemetry",
    )
    expect(
        len(dashboard.get("change_history", [])) == 1
        and dashboard["change_history"][0].get("change_kind") == "refresh-only",
        "expected dashboard artifact to publish deterministic change history over the schema surface",
    )

    return CaseResult(
        case_id="claim-publication-dashboard-schema-surface",
        probe="compile-validate-and-inspect-schema-shaped-dashboard-artifact",
        fixture=RELEASE_CLAIMABLE_SURFACE_FIXTURE,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "dashboard_schema_id": dashboard.get("schema_id"),
            "artifact_paths": artifact_paths,
            "profile_ids": [entry.get("profile_id") for entry in dashboard.get("profiles", [])],
        },
    )


__all__ = [
    "check_claim_publication_dashboard_schema_surface_case",
]
