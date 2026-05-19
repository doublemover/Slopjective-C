from __future__ import annotations

import json
from pathlib import Path

from hard_cutover_gate_support import write_fixture
from scripts.source_hygiene.gate_contracts import (
    HARD_CUTOVER_GATE_ID,
    REQUIRED_RESIDUE_CLASSES,
    RETIRED_ALLOWLIST_REPORT_FIELDS,
)
from scripts.source_hygiene.generated_reports import (
    GENERATED_TRUTH_BOUNDARY_CLAIM_POLICY,
    GENERATED_TRUTH_BOUNDARY_CONTRACT_ID,
)
from scripts.source_hygiene.hard_cutover_gate import (
    HARD_CUTOVER_GATE_CLI_CONTRACT_ID,
    HARD_CUTOVER_GATE_SUMMARY_FIELDS,
    format_gate_summary,
    hard_cutover_gate_cli_contract_payload,
)
from scripts.source_hygiene.owners import (
    SOURCE_HYGIENE_BLOCKER_METADATA_OWNER,
    SOURCE_HYGIENE_GENERATED_REPORT_OWNER,
    SOURCE_HYGIENE_PATTERN_OWNER,
    SOURCE_HYGIENE_SCAN_ROOT_OWNER,
)
from scripts.source_hygiene.patterns import FORBIDDEN_PATTERN_GROUPS, FORBIDDEN_PATTERNS
from scripts.source_hygiene.report_writer import (
    REPORT_SUMMARY_FIELDS,
    REPORT_WRITER_CONTRACT_ID,
    report_writer_contract_payload,
)
from scripts.source_hygiene.roots import (
    DEFAULT_SCAN_ROOTS,
    SOURCE_HYGIENE_ROOTS_CONTRACT_ID,
)
from scripts.source_hygiene.scan_config import SOURCE_HYGIENE_SCAN_CONFIG_CONTRACT_ID
from scripts.source_hygiene.scanner import build_report, write_reports
from scripts.source_hygiene.violations import SOURCE_HYGIENE_VIOLATION_CONTRACT_ID


def assert_default_roots_cover_public_command_truth_surfaces() -> None:
    assert "README.md" in DEFAULT_SCAN_ROOTS
    assert "CONTRIBUTING.md" in DEFAULT_SCAN_ROOTS
    assert "docs" in DEFAULT_SCAN_ROOTS
    assert "showcase" in DEFAULT_SCAN_ROOTS
    assert "spec" in DEFAULT_SCAN_ROOTS
    assert "stdlib" in DEFAULT_SCAN_ROOTS
    assert "site" in DEFAULT_SCAN_ROOTS
    assert "tests" in DEFAULT_SCAN_ROOTS


def assert_policy_data_covers_closure_residue_classes(tmp_path: Path) -> None:
    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())
    residue_classes = {
        pattern["residue_class"] for pattern in report["forbidden_patterns"]
    }

    assert set(REQUIRED_RESIDUE_CLASSES) <= residue_classes
    assert {
        pattern["gate_contract"] for pattern in report["forbidden_patterns"]
    } == {HARD_CUTOVER_GATE_ID}


def assert_pattern_groups_are_reported_as_owner_contracts(tmp_path: Path) -> None:
    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())
    report_groups = report["forbidden_pattern_groups"]
    config_groups = report["scan_config_contract"]["pattern_groups"]

    assert report_groups == config_groups
    assert len({group.group_id for group in FORBIDDEN_PATTERN_GROUPS}) == len(
        FORBIDDEN_PATTERN_GROUPS
    )
    assert [
        pattern.pattern_id
        for group in FORBIDDEN_PATTERN_GROUPS
        for pattern in group.patterns
    ] == [pattern.pattern_id for pattern in FORBIDDEN_PATTERNS]
    assert {
        group["group_id"]: group["owner_surface"] for group in report_groups
    } == {
        group.group_id: group.owner_surface for group in FORBIDDEN_PATTERN_GROUPS
    }
    assert all(group["pattern_count"] > 0 for group in report_groups)


def assert_report_declares_allowlist_free_contract(tmp_path: Path) -> None:
    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["gate_contract"]["gate_id"] == HARD_CUTOVER_GATE_ID
    assert report["gate_contract"]["closure_issues"] == ["8149", "8150"]
    assert (
        report["gate_contract"]["retired_allowlist_report_fields"]
        == list(RETIRED_ALLOWLIST_REPORT_FIELDS)
    )
    for retired_key in RETIRED_ALLOWLIST_REPORT_FIELDS:
        assert retired_key not in report
        assert retired_key not in report["stats"]


def assert_report_declares_source_owned_contracts(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/support/capability_matrix.md",
        "Backward-compatible aliases remain available.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())
    owner_contract = report["owner_contract"]
    scan_config_contract = report["scan_config_contract"]

    assert owner_contract["scan_root_owner"]["owner_id"] == SOURCE_HYGIENE_SCAN_ROOT_OWNER
    assert scan_config_contract["contract_id"] == SOURCE_HYGIENE_SCAN_CONFIG_CONTRACT_ID
    assert scan_config_contract["path_scope_is_fail_closed"] is True
    assert scan_config_contract["compiled_patterns_are_case_insensitive"] is True
    assert scan_config_contract["scan_roots"] == report["scan_roots"]
    assert scan_config_contract["pattern_count"] == len(report["forbidden_patterns"])
    assert report["roots_contract"]["contract_id"] == SOURCE_HYGIENE_ROOTS_CONTRACT_ID
    assert report["roots_contract"]["default_scan_roots"] == list(DEFAULT_SCAN_ROOTS)
    assert report["roots_contract"]["tmp_and_report_outputs_excluded"] is True
    assert (
        report["roots_contract"]["canonical_rejection_registry_excludes_are_scoped"]
        is True
    )
    assert report["violation_contract"]["contract_id"] == (
        SOURCE_HYGIENE_VIOLATION_CONTRACT_ID
    )
    assert report["violation_contract"]["pattern_owner_required"] is True
    assert owner_contract["pattern_owner"]["owner_id"] == SOURCE_HYGIENE_PATTERN_OWNER
    assert (
        owner_contract["generated_report_owner"]["owner_id"]
        == SOURCE_HYGIENE_GENERATED_REPORT_OWNER
    )
    assert owner_contract["generated_report_owner"]["generated_boundary_policy"] == {
        "boundary_contract": GENERATED_TRUTH_BOUNDARY_CONTRACT_ID,
        "claim_policy": GENERATED_TRUTH_BOUNDARY_CLAIM_POLICY,
        "generated_output_is_claim_source": False,
    }
    assert (
        owner_contract["blocker_metadata"]["blocker_owner"]
        == SOURCE_HYGIENE_BLOCKER_METADATA_OWNER
    )
    assert report["active_findings"][0]["pattern_owner"] == SOURCE_HYGIENE_PATTERN_OWNER
    assert "pattern_owner_surface" in report["active_findings"][0]
    assert {
        boundary["owner_id"] for boundary in report["generated_truth_boundaries"]
    } == {SOURCE_HYGIENE_GENERATED_REPORT_OWNER}
    assert {
        boundary["contract_id"] for boundary in report["generated_truth_boundaries"]
    } == {GENERATED_TRUTH_BOUNDARY_CONTRACT_ID}
    assert {
        boundary["generated_output_is_claim_source"]
        for boundary in report["generated_truth_boundaries"]
    } == {False}
    assert {
        boundary["claim_policy"] for boundary in report["generated_truth_boundaries"]
    } == {GENERATED_TRUTH_BOUNDARY_CLAIM_POLICY}


def assert_report_matches_schema_shape(tmp_path: Path) -> None:
    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())
    json_path = tmp_path / "tmp/reports/source_hygiene/hard-cutover/report.json"
    text_path = tmp_path / "tmp/reports/source_hygiene/hard-cutover/report.txt"

    write_reports(report, json_path, text_path)
    payload = json.loads(json_path.read_text(encoding="utf-8"))
    text = text_path.read_text(encoding="utf-8")
    writer_contract = report_writer_contract_payload()

    assert payload["schema_version"] == "source-hygiene-hard-cutover-report-v1"
    assert isinstance(payload["forbidden_patterns"], list)
    assert isinstance(payload["active_findings"], list)
    assert payload["stats"]["active_finding_count"] == 0
    assert writer_contract["contract_id"] == REPORT_WRITER_CONTRACT_ID
    assert writer_contract["summary_fields"] == list(REPORT_SUMMARY_FIELDS)
    assert text.startswith("schema_version:")
    assert [line.split(":", 1)[0] for line in text.splitlines()[:5]] == list(
        REPORT_SUMMARY_FIELDS
    )


def assert_cli_summary_declares_owner_contract(tmp_path: Path) -> None:
    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())
    contract = hard_cutover_gate_cli_contract_payload()
    summary = format_gate_summary(
        report=report,
        json_path=tmp_path / "tmp/reports/source_hygiene/hard-cutover/report.json",
        text_path=tmp_path / "tmp/reports/source_hygiene/hard-cutover/report.txt",
        root=tmp_path,
    )

    assert contract["contract_id"] == HARD_CUTOVER_GATE_CLI_CONTRACT_ID
    assert contract["summary_fields"] == list(HARD_CUTOVER_GATE_SUMMARY_FIELDS)
    assert [line.split(":", 1)[0] for line in summary] == list(
        HARD_CUTOVER_GATE_SUMMARY_FIELDS
    )
    assert summary[0] == (
        "source_hygiene_cli_contract: source-hygiene-hard-cutover-cli-v1"
    )
    assert summary[-1] == "generated_truth_boundary_findings: 0"
