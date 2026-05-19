from __future__ import annotations

from pathlib import Path

from hard_cutover_gate_support import write_fixture
from scripts.source_hygiene.scanner import build_report


def assert_projected_retired_behavior_claims_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/support/capability_matrix.md",
        "Future compatibility aliases will be supported after closeout.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "projected-retired-behavior-claim"
    )
    assert report["active_findings"][0]["residue_class"] == "projected-behavior-claim"


def assert_backward_compatible_alias_claims_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/support/capability_matrix.md",
        "Backward-compatible aliases remain available.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "backward-compatible-alias-wording"
    )
    assert report["active_findings"][0]["residue_class"] == "alias-residue"


def assert_public_compatibility_shim_support_claims_rejected(
    tmp_path: Path,
) -> None:
    write_fixture(
        tmp_path / "docs/support/capability_matrix.md",
        "Compatibility shim support remains accepted.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert {
        finding["pattern_id"] for finding in report["active_findings"]
    } >= {"public-compatibility-shim-support-claim", "shim-wording"}


def assert_public_migration_lane_support_claims_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/support/capability_matrix.md",
        "Migration-lane support remains enabled.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "public-migration-lane-support-claim"
    )
    assert report["active_findings"][0]["residue_class"] == "shim-fallback-language"


def assert_legacy_compatibility_public_text_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/support/capability_matrix.md",
        "Legacy compatibility text remains authoritative.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "legacy-compatibility-text"
    assert report["active_findings"][0]["residue_class"] == "legacy-compatibility-text"
