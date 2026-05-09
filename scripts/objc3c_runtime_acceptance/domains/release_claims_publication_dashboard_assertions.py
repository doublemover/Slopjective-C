"""Assertions for release-claims publication dashboard artifacts."""

from __future__ import annotations

import re
from typing import Any

from ..expectation_matching import expect


def expect_dashboard_schema_surface(
    dashboard: dict[str, Any], required_keys: list[str]
) -> list[str]:
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
    return artifact_paths
