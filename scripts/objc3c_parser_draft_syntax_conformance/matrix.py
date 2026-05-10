from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_parser_draft_syntax_conformance.compiler import run_compiler
from objc3c_parser_draft_syntax_conformance.diagnostics import diagnostic_matches
from objc3c_parser_draft_syntax_conformance.diagnostics import expected_code_header
from objc3c_parser_draft_syntax_conformance.paths import ROOT
from objc3c_parser_draft_syntax_conformance.paths import TMP_ROOT
from objc3c_parser_draft_syntax_conformance.source_loading import REPLAY_KEY_FIELDS


def build_negative_entries(surface: dict[str, Any]) -> list[dict[str, Any]]:
    return [
        {
            "fixture": surface["negative_fixture"],
            "expected_diagnostic": surface["expected_diagnostic"],
        }
    ] + surface.get("additional_negative_fixtures", [])


def build_negative_result(
    entry: dict[str, Any],
    compiled_negative_cache: dict[str, dict[str, Any]],
) -> dict[str, Any]:
    fixture = ROOT / entry["fixture"]
    if entry["fixture"] not in compiled_negative_cache:
        compiled_negative_cache[entry["fixture"]] = run_compiler(
            fixture, TMP_ROOT / fixture.stem
        )
    run = compiled_negative_cache[entry["fixture"]]
    expected = entry["expected_diagnostic"]
    header_codes = expected_code_header(fixture)
    return {
        "fixture": entry["fixture"],
        "expected": expected,
        "header_codes": header_codes,
        "exit_code": run["exit_code"],
        "observed_diagnostics": [
            {
                "code": diag.get("code"),
                "line": diag.get("line"),
                "column": diag.get("column"),
                "message": diag.get("message"),
            }
            for diag in run["diagnostics"]
        ],
        "header_matches_expected_code": expected["code"] in header_codes,
        "compiled_fail_closed": run["exit_code"] != 0,
        "expected_location_observed": diagnostic_matches(run["diagnostics"], expected),
    }


def build_surface_result(
    surface: dict[str, Any],
    positive_text: str,
    replay_fields: dict[str, int],
    compiled_negative_cache: dict[str, dict[str, Any]],
) -> dict[str, Any]:
    positive_tokens = surface.get("positive_tokens", [])
    positive_token_results = {token: token in positive_text for token in positive_tokens}
    replay_field = REPLAY_KEY_FIELDS.get(surface["id"], "")
    replay_count = replay_fields.get(replay_field, 0)
    negative_results = [
        build_negative_result(entry, compiled_negative_cache)
        for entry in build_negative_entries(surface)
    ]
    return {
        "id": surface["id"],
        "summary_field": surface["summary_field"],
        "positive_tokens": positive_token_results,
        "replay_key_field": replay_field,
        "replay_key_count": replay_count,
        "positive_tokens_present": all(positive_token_results.values()),
        "positive_manifest_count_present": replay_count > 0,
        "negative_results": negative_results,
        "status": "PASS"
        if all(positive_token_results.values())
        and replay_count > 0
        and all(
            result["header_matches_expected_code"]
            and result["compiled_fail_closed"]
            and result["expected_location_observed"]
            for result in negative_results
        )
        else "FAIL",
    }


def build_surface_results(
    surfaces: list[dict[str, Any]],
    positive_text: str,
    replay_fields: dict[str, int],
) -> list[dict[str, Any]]:
    compiled_negative_cache: dict[str, dict[str, Any]] = {}
    return [
        build_surface_result(
            surface,
            positive_text,
            replay_fields,
            compiled_negative_cache,
        )
        for surface in surfaces
    ]


def collect_negative_fixture_paths(surfaces: list[dict[str, Any]]) -> list[Path]:
    return [
        path
        for path in sorted(
            {
                ROOT / result["fixture"]
                for surface in surfaces
                for result in build_negative_entries(surface)
            },
            key=lambda item: item.as_posix(),
        )
    ]
