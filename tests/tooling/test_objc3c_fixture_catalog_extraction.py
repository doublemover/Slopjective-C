from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NATIVE_CATALOG = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "fixture_family_catalog.json"
)
CANONICAL_MANIFEST = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
PARSER_CORPUS = ROOT / "tests" / "tooling" / "fixtures" / "parser_conformance_corpus"
PARSER_MANIFEST = PARSER_CORPUS / "manifest.json"
NEGATIVE_EXECUTION = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "negative"
)
POSITIVE_EXECUTION = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "positive"
)
RECOVERY_README = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "README.md"
)
EXPECTED_OWNER_LABELS = {
    "parser",
    "sema",
    "lowering",
    "runtime",
    "e2e",
    "canonical_rejection",
}
EXPECTED_CANONICAL_POSITIVE_OWNER_PHASES = {
    "parser",
    "sema",
    "lowering",
    "ir",
    "runtime",
    "e2e",
}
EXPECTED_OWNER_SPLIT_SURFACES = {
    "tests/tooling/fixtures/native/recovery/positive",
    "tests/tooling/fixtures/native/execution/positive",
    "tests/tooling/fixtures/native/execution/negative",
    "tests/tooling/fixtures/native/*.objc3",
}


def _read_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def _read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_native_fixture_catalog_tracks_behavior_first_boundaries() -> None:
    catalog = _read_json(NATIVE_CATALOG)
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
    assert "fallback execution paths" in unsupported_claims["hard_cutover_rule"]


def test_recovery_fixture_readme_documents_phase_boundaries() -> None:
    readme = _read_text(RECOVERY_README)
    for required in (
        "parser-owned positives",
        "sema-owned positives",
        "lowering-owned positives",
        "runtime-owned positives",
        "executable success belongs in `execution/positive`",
        "must remain rejection metadata",
    ):
        assert required in readme


def test_large_fixture_surfaces_are_split_by_behavior_owner() -> None:
    catalog = _read_json(NATIVE_CATALOG)
    owner_labels = set(catalog["policy"]["behavior_owner_labels"])
    assert owner_labels == EXPECTED_OWNER_LABELS
    assert "large or legacy-looking fixture directories" in catalog["policy"][
        "large_surface_split_rule"
    ]

    owner_splits = {split["surface"]: split for split in catalog["owner_splits"]}
    assert EXPECTED_OWNER_SPLIT_SURFACES <= set(owner_splits)

    observed_owners = set()
    for split in owner_splits.values():
        assert split["surface_kind"]
        assert split["split_rule"]
        assert split["owners"]
        for owner_split in split["owners"]:
            assert owner_split["owner"] in owner_labels
            assert owner_split["selectors"]
            assert owner_split["disposition"]
            observed_owners.add(owner_split["owner"])

    assert EXPECTED_OWNER_LABELS <= observed_owners


def test_legacy_positive_residue_is_owned_by_canonical_rejection() -> None:
    catalog = _read_json(NATIVE_CATALOG)
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

    serialized = json.dumps(canonical_rejection_splits, sort_keys=True)
    assert "unsupported_feature_claim_*" in serialized
    assert "*compatibility_negative.objc3" in serialized
    assert "non-positive canonical rejection or strict-error contracts" in serialized
    assert "legacy-positive support" in serialized


def test_canonical_manifest_keeps_retired_surfaces_non_positive() -> None:
    manifest = _read_json(CANONICAL_MANIFEST)
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


def test_compatibility_named_sources_are_not_positive_contracts() -> None:
    catalog = _read_json(NATIVE_CATALOG)
    native_root = ROOT / "tests" / "tooling" / "fixtures" / "native"
    forbidden_tokens = tuple(catalog["policy"]["forbidden_positive_name_tokens"])
    rejection_name_markers = ("negative", "rejected", "unsupported")

    offenders = []
    for source_path in native_root.rglob("*.objc3"):
        source_name = source_path.name.lower()
        if not any(token in source_name for token in forbidden_tokens):
            continue
        if any(marker in source_name for marker in rejection_name_markers):
            continue
        offenders.append(source_path.relative_to(ROOT).as_posix())

    assert offenders == []


def test_parser_conformance_manifest_tracks_strict_rejection_cases() -> None:
    manifest = _read_json(PARSER_MANIFEST)
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
        assert (PARSER_CORPUS / fixture).is_file()


def test_unsupported_feature_claim_sidecars_are_compile_rejections() -> None:
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
        sidecar = _read_json(NEGATIVE_EXECUTION / f"{basename}.meta.json")
        assert source.is_file()
        assert sidecar["fixture"] == source.name
        assert sidecar["expect_failure"]["stage"] == "compile"
        tokens = sidecar["expect_failure"]["required_diagnostic_tokens"]
        assert "O3S221" in tokens
        assert message in tokens
        assert sidecar["execution"]["requires_live_runtime_dispatch"] is False


def test_retired_unsupported_feature_claim_duplicates_are_absent() -> None:
    native_root = ROOT / "tests" / "tooling" / "fixtures" / "native"
    assert list(native_root.glob("unsupported_feature_claim_*.objc3")) == []


def test_positive_execution_runtime_dispatch_sidecars_are_canonical_live_dispatch() -> None:
    live_dispatch_sidecars = []
    for sidecar_path in POSITIVE_EXECUTION.glob("*.meta.json"):
        sidecar = _read_json(sidecar_path)
        execution = sidecar.get("execution", {})
        serialized = json.dumps(sidecar, sort_keys=True)

        assert "expect_failure" not in sidecar
        assert "objc3_msgsend_i32" not in serialized
        assert "compatibility_runtime_dispatch_symbol" not in serialized
        if execution.get("requires_live_runtime_dispatch", False):
            live_dispatch_sidecars.append(sidecar_path.name)
            assert execution["runtime_dispatch_symbol"] == "objc3_runtime_dispatch_i32"

    assert live_dispatch_sidecars


def test_runtime_dispatch_symbols_are_only_live_dispatch_contracts() -> None:
    catalog = _read_json(NATIVE_CATALOG)
    assert (
        catalog["policy"]["runtime_dispatch_symbol_rule"]
        == "runtime_dispatch_symbol is only present when execution.requires_live_runtime_dispatch is true"
    )

    offenders = []
    for sidecar_path in (
        *POSITIVE_EXECUTION.glob("*.meta.json"),
        *NEGATIVE_EXECUTION.glob("*.meta.json"),
    ):
        sidecar = _read_json(sidecar_path)
        execution = sidecar.get("execution", {})
        has_dispatch_symbol = "runtime_dispatch_symbol" in execution
        if has_dispatch_symbol:
            assert execution["runtime_dispatch_symbol"] == "objc3_runtime_dispatch_i32"
        if has_dispatch_symbol and not execution.get("requires_live_runtime_dispatch", False):
            offenders.append(sidecar_path.relative_to(ROOT).as_posix())

    assert offenders == []


def test_negative_execution_runtime_dispatch_sidecars_are_strict_failures() -> None:
    runtime_dispatch_sidecars = sorted(NEGATIVE_EXECUTION.glob("*runtime_dispatch*.meta.json"))
    assert runtime_dispatch_sidecars

    for sidecar_path in runtime_dispatch_sidecars:
        sidecar = _read_json(sidecar_path)
        execution = sidecar.get("execution", {})
        tokens = sidecar["expect_failure"]["required_diagnostic_tokens"]
        serialized = json.dumps(sidecar, sort_keys=True)

        assert sidecar["expect_failure"]["stage"] in {"link", "run"}
        assert execution["runtime_dispatch_symbol"] == "objc3_runtime_dispatch_i32"
        assert execution["requires_live_runtime_dispatch"] is True
        assert "objc3_msgsend_i32" not in serialized
        assert "compatibility_runtime_dispatch_symbol" not in serialized
        assert (
            "O3RT002" in tokens
            or "link.unresolved_symbol:objc3_runtime_dispatch_i32" in tokens
        )
