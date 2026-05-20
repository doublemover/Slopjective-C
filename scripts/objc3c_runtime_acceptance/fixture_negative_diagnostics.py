"""Negative fixture diagnostic contracts for runtime acceptance."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from time import perf_counter
from typing import Any

from .expectation_matching import expect
from .fixture_compile_runner import run_fixture_compile
from .progress_format import repo_display_path
from .progress_format import round_seconds


@dataclass(frozen=True)
class NegativeDiagnosticExpectation:
    key: str
    fixture: Path
    expected_snippets: list[str]
    expected_codes: list[str]
    extra_args: list[str] | None = None
    allow_missing_structured_diagnostics: bool = False


def _structured_diagnostic_locations(diagnostics: list[Any]) -> list[dict[str, Any]]:
    locations: list[dict[str, Any]] = []
    for diagnostic in diagnostics:
        if not isinstance(diagnostic, dict):
            continue
        code = diagnostic.get("code")
        if not isinstance(code, str):
            continue
        line = diagnostic.get("line", 0)
        column = diagnostic.get("column", 0)
        message = diagnostic.get("message", "")
        locations.append(
            {
                "code": code,
                "line": int(line) if isinstance(line, (int, float)) else 0,
                "column": int(column) if isinstance(column, (int, float)) else 0,
                "message": message if isinstance(message, str) else "",
            }
        )
    return locations


def _fixture_sidecar_metadata(fixture: Path) -> dict[str, Any]:
    sidecar_path = fixture.with_name(f"{fixture.stem}.meta.json")
    if not sidecar_path.is_file():
        return {}
    sidecar = json.loads(sidecar_path.read_text(encoding="utf-8"))
    expect(
        sidecar.get("fixture") == fixture.name,
        f"negative fixture sidecar {sidecar_path} does not point at {fixture.name}",
    )
    failure = sidecar.get("expect_failure", {})
    tokens = failure.get("required_diagnostic_tokens", [])
    expect(
        isinstance(tokens, list) and all(isinstance(token, str) for token in tokens),
        f"negative fixture sidecar {sidecar_path} does not publish required diagnostic tokens",
    )
    return {
        "sidecar": repo_display_path(sidecar_path),
        "sidecar_stage": failure.get("stage", ""),
        "sidecar_required_diagnostic_tokens": tokens,
    }


def compile_fixture_expect_failure(
    fixture: Path,
    out_dir: Path,
    *,
    expected_snippets: list[str],
    expected_codes: list[str],
    extra_args: list[str] | None = None,
    allow_missing_structured_diagnostics: bool = False,
) -> dict[str, Any]:
    result, _ = run_fixture_compile(
        fixture,
        out_dir,
        extra_args=extra_args,
        write_provenance=False,
    )
    if result.returncode == 0:
        raise RuntimeError(f"fixture compile unexpectedly succeeded for {fixture}")
    diagnostics_txt_path = out_dir / "module.diagnostics.txt"
    diagnostics_json_path = out_dir / "module.diagnostics.json"
    if not diagnostics_txt_path.is_file():
        raise RuntimeError(
            f"failed compile for {fixture} did not publish {diagnostics_txt_path}"
        )
    if not diagnostics_json_path.is_file():
        raise RuntimeError(
            f"failed compile for {fixture} did not publish {diagnostics_json_path}"
        )
    diagnostics_text = diagnostics_txt_path.read_text(encoding="utf-8")
    if diagnostics_text == "" and result.stderr:
        diagnostics_text = result.stderr
    diagnostics_payload = json.loads(diagnostics_json_path.read_text(encoding="utf-8"))
    diagnostics = diagnostics_payload.get("diagnostics", [])
    if allow_missing_structured_diagnostics:
        expect(
            isinstance(diagnostics, list),
            f"failed compile for {fixture} did not publish a diagnostics list",
        )
    else:
        expect(
            isinstance(diagnostics, list) and diagnostics,
            f"failed compile for {fixture} did not publish structured diagnostics",
        )
    for snippet in expected_snippets:
        expect(
            snippet in diagnostics_text,
            f"failed compile for {fixture} did not publish expected diagnostic snippet: {snippet}",
        )
    observed_codes = {
        diagnostic.get("code")
        for diagnostic in diagnostics
        if isinstance(diagnostic, dict) and isinstance(diagnostic.get("code"), str)
    }
    for expected_code in expected_codes:
        expect(
            expected_code in observed_codes,
            f"failed compile for {fixture} did not publish expected diagnostic code {expected_code}",
        )
    diagnostic_locations = _structured_diagnostic_locations(diagnostics)
    return {
        "returncode": result.returncode,
        "diagnostic_count": len(diagnostics),
        "diagnostic_codes": sorted(observed_codes),
        "diagnostic_locations": diagnostic_locations,
        "stderr": result.stderr,
        "diagnostics_path": repo_display_path(diagnostics_json_path),
        "sidecar_metadata": _fixture_sidecar_metadata(fixture),
    }


def compile_negative_diagnostic_batch(
    *,
    case_id: str,
    out_dir: Path,
    expectations: list[NegativeDiagnosticExpectation],
) -> dict[str, Any]:
    out_dir.mkdir(parents=True, exist_ok=True)
    started_at = perf_counter()
    results: list[dict[str, Any]] = []
    for expectation in expectations:
        fixture_started_at = perf_counter()
        negative_result = compile_fixture_expect_failure(
            expectation.fixture,
            out_dir / expectation.key,
            expected_snippets=expectation.expected_snippets,
            expected_codes=expectation.expected_codes,
            extra_args=expectation.extra_args,
            allow_missing_structured_diagnostics=(
                expectation.allow_missing_structured_diagnostics
            ),
        )
        results.append(
            {
                "key": expectation.key,
                "fixture": repo_display_path(expectation.fixture),
                "expected_codes": list(expectation.expected_codes),
                "expected_snippets": list(expectation.expected_snippets),
                "diagnostic_codes": negative_result["diagnostic_codes"],
                "diagnostic_count": negative_result["diagnostic_count"],
                "diagnostic_locations": negative_result["diagnostic_locations"],
                "diagnostics": negative_result["diagnostics_path"],
                "returncode": negative_result["returncode"],
                "duration_seconds": round_seconds(perf_counter() - fixture_started_at),
                **negative_result["sidecar_metadata"],
            }
        )
    return {
        "contract_id": "objc3c.runtime.acceptance.negative.diagnostics.batch.v1",
        "case_id": case_id,
        "batch_out_dir": repo_display_path(out_dir),
        "fixture_count": len(results),
        "total_seconds": round_seconds(perf_counter() - started_at),
        "results": results,
        "preserves_per_fixture_expected_diagnostic_codes": True,
        "preserves_per_fixture_expected_diagnostic_snippets": True,
        "fail_closed_on_unexpected_success": True,
        "fail_closed_on_missing_structured_diagnostics": True,
    }


__all__ = [
    "NegativeDiagnosticExpectation",
    "compile_fixture_expect_failure",
    "compile_negative_diagnostic_batch",
]
