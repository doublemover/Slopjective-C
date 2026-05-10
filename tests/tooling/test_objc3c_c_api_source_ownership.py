from __future__ import annotations

from objc3c_c_api_source_ownership_behavior import (
    assert_c_api_cpp_pins_core_compile_and_c_only_helper_owners,
    assert_c_api_helper_contract_tracks_result_error_artifact_stage_helpers,
    assert_c_api_source_paths_exist,
    assert_frontend_result_ownership_releases_immutable_owned_strings,
)
from objc3c_c_api_source_ownership_sources import (
    c_api_helper_contract_payload,
    c_api_source_and_squashed_text,
    c_api_source_paths,
    c_api_surface_and_implementation_text,
    frontend_result_ownership_texts,
)


def test_c_api_cpp_pins_core_compile_and_c_only_helper_owners() -> None:
    assert_c_api_source_paths_exist(c_api_source_paths())
    assert_c_api_cpp_pins_core_compile_and_c_only_helper_owners(
        *c_api_source_and_squashed_text()
    )


def test_c_api_helper_contract_fixture_tracks_result_error_artifact_stage_helpers() -> None:
    assert_c_api_helper_contract_tracks_result_error_artifact_stage_helpers(
        *c_api_surface_and_implementation_text(),
        c_api_helper_contract_payload(),
    )


def test_frontend_result_ownership_releases_immutable_owned_strings() -> None:
    assert_frontend_result_ownership_releases_immutable_owned_strings(
        *frontend_result_ownership_texts()
    )
