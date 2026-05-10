from __future__ import annotations

from objc3c_c_api_runner_extraction_behavior import (
    assert_bonus_experience_boundary_surface,
    assert_c_gate_compile_path,
    assert_contract_tracks_summary_ownership_fields,
    assert_fail_closed_stage_and_result_accessor_drift,
    assert_observability_surface,
    assert_playground_repro_surface,
    assert_runtime_inspector_and_dump_flags,
    assert_summary_and_cli_contract,
)
from objc3c_c_api_runner_extraction_json import load_c_api_runner_contract
from objc3c_c_api_runner_extraction_sources import runner_source_text


def test_c_api_runner_uses_c_gate_compile_path() -> None:
    assert_c_gate_compile_path(runner_source_text())


def test_c_api_runner_reports_summary_and_cli_contract() -> None:
    assert_summary_and_cli_contract(runner_source_text())


def test_c_api_runner_reports_observability_surface() -> None:
    assert_observability_surface(runner_source_text())


def test_c_api_runner_fails_closed_on_stage_and_result_accessor_drift() -> None:
    assert_fail_closed_stage_and_result_accessor_drift(runner_source_text())


def test_c_api_runner_contract_fixture_tracks_summary_ownership_fields() -> None:
    assert_contract_tracks_summary_ownership_fields(
        runner_source_text(),
        load_c_api_runner_contract(),
    )


def test_c_api_runner_reports_runtime_inspector_and_dump_flags() -> None:
    assert_runtime_inspector_and_dump_flags(runner_source_text())


def test_c_api_runner_reports_bonus_experience_boundary_surface() -> None:
    assert_bonus_experience_boundary_surface(runner_source_text())


def test_c_api_runner_reports_playground_repro_surface() -> None:
    assert_playground_repro_surface(runner_source_text())
