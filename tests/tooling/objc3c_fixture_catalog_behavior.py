from __future__ import annotations

from typing import Any

from objc3c_fixture_catalog_assertions import (
    assert_contains_all,
    assert_live_runtime_dispatch,
    assert_no_compatibility_runtime_dispatch,
    assert_no_offenders,
    assert_owner_split_shape,
    assert_path_exists,
)
from objc3c_fixture_catalog_fixtures import (
    EXPECTED_CANONICAL_POSITIVE_OWNER_PHASES,
    EXPECTED_OWNER_LABELS,
    EXPECTED_OWNER_SPLIT_SURFACES,
    NEGATIVE_EXECUTION,
    PARSER_CORPUS,
    POSITIVE_EXECUTION,
    ROOT,
    native_fixture_root,
)
from objc3c_fixture_catalog_json import read_json, serialize_json


def assert_native_catalog_tracks_behavior_first_boundaries(
    catalog: dict[str, Any],
) -> None:
    assert catalog["catalog"] == "native-fixture-family-boundaries"
    assert (
        catalog["policy"]["positive_fixture_rule"]
        == "canonical Objective-C 3.0 behavior only"
    )
    assert "strict rejection metadata" in catalog["policy"]["negative_fixture_rule"]
    assert "runtime acceptance scripts" in catalog["policy"]["validation_boundary"]

    families = {family["family"]: family for family in catalog["families"]}
    unsupported_claims = families["unsupported_feature_claims"]
    assert unsupported_claims["paths"] == [
        "tests/tooling/fixtures/native/execution/negative"
    ]
    assert "O3S221" in unsupported_claims["hard_cutover_rule"]
    assert "retired route execution paths" in unsupported_claims["hard_cutover_rule"]


def assert_recovery_readme_documents_phase_boundaries(readme: str) -> None:
    assert_contains_all(
        readme,
        [
            "parser-owned positives",
            "sema-owned positives",
            "lowering-owned positives",
            "IR-owned expectations",
            "runtime-owned positives",
            "executable success belongs in `execution/positive`",
            "must remain rejection metadata",
        ],
    )


def assert_large_fixture_surfaces_are_split_by_behavior_owner(
    catalog: dict[str, Any],
) -> None:
    owner_labels = set(catalog["policy"]["behavior_owner_labels"])
    assert owner_labels == EXPECTED_OWNER_LABELS
    assert "large or legacy-looking fixture directories" in catalog["policy"][
        "large_surface_split_rule"
    ]

    owner_splits = {split["surface"]: split for split in catalog["owner_splits"]}
    assert EXPECTED_OWNER_SPLIT_SURFACES <= set(owner_splits)

    observed_owners = set()
    for split in owner_splits.values():
        observed_owners.update(assert_owner_split_shape(split, owner_labels))

    assert EXPECTED_OWNER_LABELS <= observed_owners


def assert_legacy_positive_residue_is_owned_by_canonical_rejection(
    catalog: dict[str, Any],
) -> None:
    owner_splits = {split["surface"]: split for split in catalog["owner_splits"]}
    negative_surface = owner_splits["tests/tooling/fixtures/native/execution/negative"]
    root_surface = owner_splits["tests/tooling/fixtures/native/*.objc3"]

    canonical_rejection_splits = [
        owner_split
        for surface in (negative_surface, root_surface)
        for owner_split in surface["owners"]
        if owner_split["owner"] == "canonical_rejection"
    ]
    assert len(canonical_rejection_splits) == 2

    serialized = serialize_json(canonical_rejection_splits)
    assert "unsupported_feature_claim_*" in serialized
    assert "*compatibility_negative.objc3" in serialized
    assert "non-positive canonical rejection or strict-error contracts" in serialized
    assert "legacy-positive support" in serialized


def assert_canonical_manifest_keeps_retired_surfaces_non_positive(
    manifest: dict[str, Any],
) -> None:
    boundary = manifest["boundary"]
    assert set(boundary["positive_owner_phases"]) == (
        EXPECTED_CANONICAL_POSITIVE_OWNER_PHASES
    )
    assert boundary["canonical_rejection_owner"] == "canonical_rejection"
    assert "canonical rejection candidates" in boundary[
        "legacy_positive_residue_disposition"
    ]

    retired_surface_entries = []
    for fixture in manifest["fixtures"]:
        if fixture["fixture_kind"] == "positive":
            assert fixture["expected_diagnostic_code"] == ""
            assert "retired_surface_tags" not in fixture
        if fixture.get("retired_surface_tags"):
            retired_surface_entries.append(fixture)
            assert fixture["fixture_kind"] != "positive"
            assert fixture["expected_diagnostic_code"]

    assert retired_surface_entries


def assert_compatibility_named_sources_are_not_positive_contracts(
    catalog: dict[str, Any],
) -> None:
    forbidden_tokens = tuple(catalog["policy"]["forbidden_positive_name_tokens"])
    rejection_name_markers = ("negative", "rejected", "unsupported")

    offenders = []
    for source_path in native_fixture_root().rglob("*.objc3"):
        source_name = source_path.name.lower()
        if not any(token in source_name for token in forbidden_tokens):
            continue
        if any(marker in source_name for marker in rejection_name_markers):
            continue
        offenders.append(source_path.relative_to(ROOT).as_posix())

    assert_no_offenders(offenders)


def assert_parser_manifest_tracks_strict_rejection_cases(
    manifest: dict[str, Any],
) -> None:
    cases = {case["id"]: case for case in manifest["cases"]}

    expected_rejections = {
        "A010-C006": ("reject_extern_without_fn.objc3", "O3P100"),
        "A010-C007": ("reject_duplicate_pure_qualifier.objc3", "O3P100"),
        "A010-C008": ("reject_misplaced_pure_after_fn.objc3", "O3P100"),
        "A010-C009": ("reject_duplicate_throws_modifier.objc3", "O3P181"),
    }
    for case_id, (fixture, diagnostic_code) in expected_rejections.items():
        case = cases[case_id]
        assert case["path"] == fixture
        assert case["expect"] == "reject"
        assert case["diagnostic_code"] == diagnostic_code
        assert_path_exists(PARSER_CORPUS / fixture)


def assert_unsupported_feature_claim_sidecars_are_compile_rejections() -> None:
    expected_tokens = {
        "unsupported_feature_claim_throws": (
            "unsupported feature claim: 'throws' is not yet runnable in "
            "Objective-C 3 native mode"
        ),
        "unsupported_feature_claim_arc_parameter_ownership": (
            "unsupported feature claim: ARC ownership qualifiers are not yet runnable "
            "in Objective-C 3 native mode"
        ),
        "unsupported_feature_claim_arc_return_ownership": (
            "unsupported feature claim: ARC ownership qualifiers are not yet runnable "
            "in Objective-C 3 native mode"
        ),
    }
    for basename, message in expected_tokens.items():
        source = NEGATIVE_EXECUTION / f"{basename}.objc3"
        sidecar = read_json(NEGATIVE_EXECUTION / f"{basename}.meta.json")
        assert_path_exists(source)
        assert sidecar["fixture"] == source.name
        assert sidecar["expect_failure"]["stage"] == "compile"
        tokens = sidecar["expect_failure"]["required_diagnostic_tokens"]
        assert "O3S221" in tokens
        assert message in tokens
        assert sidecar["execution"]["requires_live_runtime_dispatch"] is False


def assert_retired_unsupported_feature_claim_duplicates_are_absent() -> None:
    assert list(native_fixture_root().glob("unsupported_feature_claim_*.objc3")) == []


def assert_positive_execution_sidecars_are_canonical_live_dispatch() -> None:
    live_dispatch_sidecars = []
    for sidecar_path in POSITIVE_EXECUTION.glob("*.meta.json"):
        sidecar = read_json(sidecar_path)
        execution = sidecar.get("execution", {})
        serialized = serialize_json(sidecar)

        assert "expect_failure" not in sidecar
        assert_no_compatibility_runtime_dispatch(serialized)
        if execution.get("requires_live_runtime_dispatch", False):
            live_dispatch_sidecars.append(sidecar_path.name)
            assert_live_runtime_dispatch(execution)

    assert live_dispatch_sidecars


def assert_runtime_dispatch_symbols_are_only_live_dispatch_contracts(
    catalog: dict[str, Any],
) -> None:
    expected_rule = (
        "runtime_dispatch_symbol or runtime_dispatch_symbols is only present when "
        "execution.requires_live_runtime_dispatch is true"
    )
    assert (
        catalog["policy"]["runtime_dispatch_symbol_rule"]
        == expected_rule
    )

    offenders = []
    for sidecar_path in (
        *POSITIVE_EXECUTION.glob("*.meta.json"),
        *NEGATIVE_EXECUTION.glob("*.meta.json"),
    ):
        sidecar = read_json(sidecar_path)
        execution = sidecar.get("execution", {})
        has_dispatch_symbol = (
            "runtime_dispatch_symbol" in execution
            or "runtime_dispatch_symbols" in execution
        )
        if has_dispatch_symbol:
            assert_live_runtime_dispatch(execution)
        if has_dispatch_symbol and not execution.get(
            "requires_live_runtime_dispatch",
            False,
        ):
            offenders.append(sidecar_path.relative_to(ROOT).as_posix())

    assert_no_offenders(offenders)


def assert_negative_execution_sidecars_are_strict_failures() -> None:
    runtime_dispatch_sidecars = sorted(
        NEGATIVE_EXECUTION.glob("*runtime_dispatch*.meta.json")
    )
    assert runtime_dispatch_sidecars

    for sidecar_path in runtime_dispatch_sidecars:
        sidecar = read_json(sidecar_path)
        execution = sidecar.get("execution", {})
        tokens = sidecar["expect_failure"]["required_diagnostic_tokens"]
        serialized = serialize_json(sidecar)

        assert sidecar["expect_failure"]["stage"] in {"link", "run"}
        assert_live_runtime_dispatch(execution)
        assert_no_compatibility_runtime_dispatch(serialized)
        runtime_dispatch_symbols = execution.get(
            "runtime_dispatch_symbols",
            [execution.get("runtime_dispatch_symbol", "")],
        )
        if "O3RT002" in tokens:
            continue
        assert all(
            f"link.unresolved_symbol:{symbol}" in tokens
            for symbol in runtime_dispatch_symbols
        )
