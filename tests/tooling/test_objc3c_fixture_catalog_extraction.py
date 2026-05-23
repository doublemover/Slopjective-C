from __future__ import annotations

from objc3c_fixture_catalog_behavior import (
    assert_canonical_manifest_keeps_retired_surfaces_non_positive,
    assert_compatibility_named_sources_are_not_positive_contracts,
    assert_large_fixture_surfaces_are_split_by_behavior_owner,
    assert_legacy_positive_residue_is_owned_by_canonical_rejection,
    assert_native_catalog_tracks_behavior_first_boundaries,
    assert_negative_execution_sidecars_are_strict_failures,
    assert_parser_manifest_tracks_strict_rejection_cases,
    assert_positive_execution_sidecars_are_canonical_live_dispatch,
    assert_recovery_readme_documents_phase_boundaries,
    assert_retired_unsupported_feature_claim_duplicates_are_absent,
    assert_runtime_dispatch_symbols_are_only_live_dispatch_contracts,
    assert_throwing_call_requires_try_sidecar_is_compile_rejection,
    assert_unsupported_feature_claim_sidecars_are_compile_rejections,
)
from objc3c_fixture_catalog_fixtures import (
    CANONICAL_MANIFEST,
    NATIVE_CATALOG,
    PARSER_MANIFEST,
    RECOVERY_README,
)
from objc3c_fixture_catalog_json import read_json, read_text


def test_native_fixture_catalog_tracks_behavior_first_boundaries() -> None:
    assert_native_catalog_tracks_behavior_first_boundaries(read_json(NATIVE_CATALOG))


def test_recovery_fixture_readme_documents_phase_boundaries() -> None:
    assert_recovery_readme_documents_phase_boundaries(read_text(RECOVERY_README))


def test_large_fixture_surfaces_are_split_by_behavior_owner() -> None:
    assert_large_fixture_surfaces_are_split_by_behavior_owner(read_json(NATIVE_CATALOG))


def test_legacy_positive_residue_is_owned_by_canonical_rejection() -> None:
    assert_legacy_positive_residue_is_owned_by_canonical_rejection(
        read_json(NATIVE_CATALOG)
    )


def test_canonical_manifest_keeps_retired_surfaces_non_positive() -> None:
    assert_canonical_manifest_keeps_retired_surfaces_non_positive(
        read_json(CANONICAL_MANIFEST)
    )


def test_compatibility_named_sources_are_not_positive_contracts() -> None:
    assert_compatibility_named_sources_are_not_positive_contracts(
        read_json(NATIVE_CATALOG)
    )


def test_parser_conformance_manifest_tracks_strict_rejection_cases() -> None:
    assert_parser_manifest_tracks_strict_rejection_cases(read_json(PARSER_MANIFEST))


def test_unsupported_feature_claim_sidecars_are_compile_rejections() -> None:
    assert_unsupported_feature_claim_sidecars_are_compile_rejections()


def test_throwing_call_requires_try_sidecar_is_compile_rejection() -> None:
    assert_throwing_call_requires_try_sidecar_is_compile_rejection()


def test_retired_unsupported_feature_claim_duplicates_are_absent() -> None:
    assert_retired_unsupported_feature_claim_duplicates_are_absent()


def test_positive_execution_runtime_dispatch_sidecars_are_canonical_live_dispatch() -> None:
    assert_positive_execution_sidecars_are_canonical_live_dispatch()


def test_runtime_dispatch_symbols_are_only_live_dispatch_contracts() -> None:
    assert_runtime_dispatch_symbols_are_only_live_dispatch_contracts(
        read_json(NATIVE_CATALOG)
    )


def test_negative_execution_runtime_dispatch_sidecars_are_strict_failures() -> None:
    assert_negative_execution_sidecars_are_strict_failures()
