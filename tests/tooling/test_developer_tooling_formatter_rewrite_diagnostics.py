from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_developer_tooling_diagnostic_quality import build_diagnostic_quality_summary
from format_objc3c_source import build_format_summary_for_source
from rewrite_objc3c_source import build_rewrite_for_source, build_rule_set


FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "developer_tooling"


def load_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def test_formatter_formats_canonical_objc3_subset_and_fails_closed() -> None:
    source_text = (FIXTURE_ROOT / "objc3_syntax_messy.objc3").read_text(
        encoding="utf-8"
    )
    expected_text = (FIXTURE_ROOT / "objc3_syntax_formatted.objc3").read_text(
        encoding="utf-8"
    )

    formatted_text, summary = build_format_summary_for_source(
        "tests/tooling/fixtures/developer_tooling/objc3_syntax_messy.objc3",
        source_text,
        "tmp/formatted.objc3",
    )

    assert formatted_text == expected_text
    assert summary["supported"] is True
    assert summary["support_class"] == "canonical-objc3-source-formatting"
    assert {
        "objc-interface-declaration",
        "actor-declaration",
        "await-expression",
        "message-send-or-indexing",
        "objc-string-literal",
    }.issubset(set(summary["feature_ids"]))

    bad_text = (FIXTURE_ROOT / "objc3_syntax_unbalanced_negative.objc3").read_text(
        encoding="utf-8"
    )
    _, bad_summary = build_format_summary_for_source(
        "tests/tooling/fixtures/developer_tooling/objc3_syntax_unbalanced_negative.objc3",
        bad_text,
        "tmp/bad-formatted.objc3",
    )
    assert bad_summary["supported"] is False
    assert bad_summary["support_class"] == "fail-closed"
    assert bad_summary["diagnostics"]


def test_safe_source_rewrite_skips_strings_comments_and_publishes_token_edits() -> None:
    source_text = (FIXTURE_ROOT / "rewrite_legacy_aliases.objc3").read_text(
        encoding="utf-8"
    )
    expected_text = (
        FIXTURE_ROOT / "rewrite_legacy_aliases_expected.objc3"
    ).read_text(encoding="utf-8")
    rules = build_rule_set(
        ["legacy-literal-aliases"], ["legacyValue=canonicalValue"]
    )

    rewritten_text, summary = build_rewrite_for_source(
        "tests/tooling/fixtures/developer_tooling/rewrite_legacy_aliases.objc3",
        source_text,
        "tmp/rewrite.objc3",
        rules,
    )

    assert rewritten_text == expected_text
    assert summary["supported"] is True
    assert summary["support_class"] == "deterministic-safe-rewrite"
    assert summary["active_rule_ids"] == ["legacy-literal-aliases", "rename-symbol"]
    assert summary["edit_count"] == 5
    assert "YES NO NULL must stay text" in rewritten_text
    assert "// YES NO NULL must stay comment text" in rewritten_text
    assert all(edit["range"]["start"]["line"] > 0 for edit in summary["edits"])


def test_diagnostic_quality_gate_covers_taxonomy_and_machine_fixits() -> None:
    contract = load_json(FIXTURE_ROOT / "diagnostic_quality_contract.json")

    summary = build_diagnostic_quality_summary(contract)

    assert summary["ok"] is True
    assert summary["diagnostic_entry_count"] >= contract["minimum_diagnostic_count"]
    assert (
        summary["machine_applicable_fixit_count"]
        >= contract["minimum_machine_applicable_fixit_count"]
    )
    assert (
        summary["recovery_diagnostic_count"]
        >= contract["minimum_recovery_diagnostic_count"]
    )
    assert summary["missing_required_fixit_codes"] == []
    assert summary["missing_required_recovery_case_ids"] == []
    assert summary["missing_required_recovery_phases"] == []
    assert summary["checks"]["structured_recovery_payloads_valid"] is True
    assert summary["deterministic_digest"]
