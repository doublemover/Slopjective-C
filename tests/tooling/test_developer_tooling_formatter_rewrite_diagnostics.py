from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

import check_developer_tooling_diagnostic_quality as diagnostic_quality
from format_objc3c_source import build_format_summary_for_source
from rewrite_objc3c_source import build_rewrite_for_source, build_rule_set


FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "developer_tooling"


def load_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def write_json(path: Path, payload: dict[str, object]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


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

    summary = diagnostic_quality.build_diagnostic_quality_summary(contract)

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
    assert summary["checks"]["native_recovery_fixtures_within_root"] is True
    assert summary["checks"]["native_recovery_fixture_sources_match"] is True
    assert summary["checks"]["native_recovery_fixture_expected_codes_match"] is True
    assert summary["deterministic_digest"]


def test_diagnostic_quality_gate_fails_closed_on_stale_native_recovery_fixture(
    tmp_path: Path, monkeypatch
) -> None:
    monkeypatch.setattr(diagnostic_quality, "ROOT", tmp_path)
    diagnostic_root = tmp_path / "tests" / "conformance" / "diagnostics"
    fixture_root = (
        tmp_path / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative"
    )
    case_source = "module FixtureTruth;\n\nfn main() -> i32 {\n  return 1;\n}\n"
    stale_fixture_source = "module StaleFixture;\n\nfn main() -> i32 {\n  return 1;\n}\n"
    fixture = fixture_root / "stale_recovery_fixture.objc3"
    fixture.parent.mkdir(parents=True, exist_ok=True)
    fixture.write_text(
        stale_fixture_source + "\n// Expected diagnostic code(s): O3P100.\n",
        encoding="utf-8",
    )
    write_json(
        diagnostic_root / "CASE.json",
        {
            "id": "STALE-RECOVERY-FIXTURE",
            "source": case_source,
            "expect": {
                "parse": "reject",
                "diagnostics": [
                    {
                        "code": "O3P104",
                        "severity": "error",
                        "phase": "parse",
                        "category": "parsing",
                        "span": {
                            "start": {"line": 4, "column": 3},
                            "end": {"line": 4, "column": 9},
                        },
                        "message": "missing ';' after return",
                        "explanation": "Synthetic stale-fixture guard case.",
                        "fixits": [],
                        "recovery": {
                            "strategy": "parser-statement-boundary-synchronization",
                            "boundary": "next statement token",
                            "deterministic": True,
                            "accepts_invalid_program": False,
                            "recovery_counts_as_success": False,
                            "native_fixture": (
                                "tests/tooling/fixtures/native/recovery/negative/"
                                "stale_recovery_fixture.objc3"
                            ),
                            "expected_native_code": "O3P104",
                        },
                    }
                ],
            },
        },
    )
    write_json(
        diagnostic_root / "manifest.json",
        {
            "schema_version": "1.0.0",
            "groups": [{"name": "stale", "files": ["CASE.json"]}],
        },
    )

    summary = diagnostic_quality.build_diagnostic_quality_summary(
        {
            "contract_id": "stale-fixture-test",
            "surface_kind": "diagnostic-quality-test",
            "manifest_path": "tests/conformance/diagnostics/manifest.json",
            "native_recovery_fixture_root": (
                "tests/tooling/fixtures/native/recovery/negative"
            ),
            "minimum_diagnostic_count": 1,
            "minimum_machine_applicable_fixit_count": 0,
            "minimum_recovery_diagnostic_count": 1,
            "minimum_native_recovery_fixture_count": 1,
            "required_fixit_codes": [],
            "required_recovery_case_ids": ["STALE-RECOVERY-FIXTURE"],
            "required_recovery_phases": ["parse"],
        }
    )

    assert summary["ok"] is False
    assert summary["checks"]["native_recovery_fixture_sources_match"] is False
    assert summary["checks"]["native_recovery_fixture_expected_codes_match"] is False
    failures = "\n".join(str(failure) for failure in summary["failures"])
    assert "native_fixture source does not match diagnostic source" in failures
    assert "native_fixture header does not include expected_native_code O3P104" in failures
