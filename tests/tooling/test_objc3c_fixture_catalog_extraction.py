from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NATIVE_CATALOG = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "fixture_family_catalog.json"
)
PARSER_CORPUS = ROOT / "tests" / "tooling" / "fixtures" / "parser_conformance_corpus"
PARSER_MANIFEST = PARSER_CORPUS / "manifest.json"
NEGATIVE_EXECUTION = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "negative"
)
POSITIVE_EXECUTION = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "positive"
)


def _read_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


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
