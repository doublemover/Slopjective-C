from __future__ import annotations

import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_objc3c_runnable_interop_conformance.constants import REQUIRED_CASES  # noqa: E402
from objc3c_runtime_acceptance.domains.registration_replay_cases.catalog import (  # noqa: E402
    CASE_ID as MULTI_IMAGE_REGISTRATION_RESET_REPLAY_CASE,
)
from objc3c_runtime_acceptance.runtime_contract_interop import (  # noqa: E402
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROBE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
    INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
    INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
    INTEROP_PACKAGE_LOADER_FAIL_CLOSED_ABI_PROBE,
)
from objc3c_runtime_acceptance.runtime_contract_registration import (  # noqa: E402
    MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
)


CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"

PACKAGE_LOADER_CLAIM = "objc3c.behavior.runtime.interop.package-loader-bridge"
MIXED_IMAGE_REPLAY_CLAIM = "objc3c.behavior.runtime.interop.mixed-image-replay"
PACKAGE_LOADER_EXPANSION_POSITIVE_EVIDENCE = {
    "tests/tooling/fixtures/native/header_module_bridge_provider.objc3",
    "tests/tooling/fixtures/native/interop_semantic_model_positive.objc3",
    "tests/tooling/fixtures/native/foreign_surfaces_provider.objc3",
    "tests/tooling/fixtures/native/cpp_swift_annotation_positive.objc3",
    "tests/conformance/semantic/BRG-78-01.json",
    "tests/conformance/module_roundtrip/BRG-78-MOD-01.json",
}
MIXED_IMAGE_EXPANSION_POSITIVE_EVIDENCE = {
    "tests/tooling/fixtures/native/interop_lowering_abi_contract_positive.objc3",
    "tests/tooling/fixtures/native/foreign_call_and_lifetime_lowering_positive.objc3",
    "tests/tooling/fixtures/native/ffi_metadata_interface_preservation_provider.objc3",
    "tests/conformance/lowering_abi/INT-CXX-05.json",
    "tests/conformance/module_roundtrip/INT-SWIFT-05.json",
    "tests/tooling/fixtures/package_ecosystem/mixed_image_interop_loader_metadata.json",
}
INTEROP_EXPANSION_NEGATIVE_EVIDENCE = {
    "tests/tooling/fixtures/native/recovery/negative/negative_objcxx_swift_bridge_conflicting_metadata.objc3",
    "tests/tooling/fixtures/native/recovery/negative/negative_objcxx_swift_bridge_unsafe_mixed_image.objc3",
}
INTEROP_EXPANSION_MANIFEST_FIXTURES = {
    "tests/tooling/fixtures/native/header_module_bridge_provider.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/interop_semantic_model_positive.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/foreign_surfaces_provider.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/cpp_swift_annotation_positive.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/interop_lowering_abi_contract_positive.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/foreign_call_and_lifetime_lowering_positive.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/ffi_metadata_interface_preservation_provider.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/recovery/negative/negative_objcxx_swift_bridge_conflicting_metadata.objc3": (
        "strict-error",
        "O3PKG8052",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_objcxx_swift_bridge_unsafe_mixed_image.objc3": (
        "strict-error",
        "O3PKG8053",
    ),
}


def _load_json(path: Path) -> dict[str, object]:
    return json.loads(path.read_text(encoding="utf-8"))


def _catalog_rows() -> dict[str, dict[str, object]]:
    catalog = _load_json(CATALOG_PATH)
    return {
        str(row["support_claim"]): row
        for row in catalog["rows"]
        if isinstance(row, dict)
    }


def _manifest_claims() -> dict[str, dict[str, object]]:
    manifest = _load_json(MANIFEST_PATH)
    return {
        str(claim["claim_id"]): claim
        for claim in manifest["support_claims"]
        if isinstance(claim, dict)
    }


def _manifest_fixture_paths() -> set[str]:
    manifest = _load_json(MANIFEST_PATH)
    return {
        str(fixture["path"])
        for fixture in manifest["fixtures"]
        if isinstance(fixture, dict)
    }


def _evidence(row: dict[str, object], key: str) -> set[str]:
    paths = row[key]
    assert isinstance(paths, list)
    return {str(path) for path in paths}


def _runtime_cases(row: dict[str, object]) -> set[str]:
    cases = {str(row["runtime_acceptance_case"])}
    additional_cases = row.get("additional_runtime_acceptance_cases", [])
    assert isinstance(additional_cases, list)
    cases.update(str(case) for case in additional_cases)
    return cases


def test_interop_support_claims_are_manifest_backed() -> None:
    rows = _catalog_rows()
    claims = _manifest_claims()
    manifest_fixtures = _manifest_fixture_paths()

    for claim_id in (PACKAGE_LOADER_CLAIM, MIXED_IMAGE_REPLAY_CLAIM):
        row = rows[claim_id]
        claim = claims[claim_id]
        behavior_fixture = str(claim["behavior_fixture"])

        assert row["owner_phase"] == "runtime"
        assert claim["owner_phase"] == "runtime"
        assert row["runnable_command"] == claim["executable_command"]
        assert behavior_fixture in manifest_fixtures
        assert behavior_fixture in _evidence(row, "positive_evidence")

        for path in _evidence(row, "positive_evidence") | _evidence(row, "negative_evidence"):
            assert not path.startswith("tmp/")
            assert (ROOT / path).is_file(), path


def test_issue_8165_interop_expansion_groundwork_stays_claim_scoped() -> None:
    catalog = _load_json(CATALOG_PATH)
    rows = _catalog_rows()
    manifest = _load_json(MANIFEST_PATH)
    manifest_fixtures = {
        str(fixture["path"]): fixture
        for fixture in manifest["fixtures"]
        if isinstance(fixture, dict)
    }

    assert 8165 in catalog["issue_refs"]

    package_loader_row = rows[PACKAGE_LOADER_CLAIM]
    mixed_image_row = rows[MIXED_IMAGE_REPLAY_CLAIM]
    package_loader_positive = _evidence(package_loader_row, "positive_evidence")
    mixed_image_positive = _evidence(mixed_image_row, "positive_evidence")

    assert PACKAGE_LOADER_EXPANSION_POSITIVE_EVIDENCE <= package_loader_positive
    assert MIXED_IMAGE_EXPANSION_POSITIVE_EVIDENCE <= mixed_image_positive
    assert INTEROP_EXPANSION_NEGATIVE_EVIDENCE <= _evidence(package_loader_row, "negative_evidence")
    assert INTEROP_EXPANSION_NEGATIVE_EVIDENCE <= _evidence(mixed_image_row, "negative_evidence")
    assert {"O3PKG8052", "O3PKG8053"} <= set(package_loader_row["required_diagnostic_codes"])
    assert {"O3PKG8052", "O3PKG8053"} <= set(mixed_image_row["required_diagnostic_codes"])

    for fixture_path, (fixture_kind, diagnostic_code) in INTEROP_EXPANSION_MANIFEST_FIXTURES.items():
        fixture = manifest_fixtures[fixture_path]
        assert fixture["origin"] == "hand-authored"
        assert fixture["owner_phase"] == "runtime"
        assert fixture["behavior_family"] == "interop"
        assert fixture["fixture_kind"] == fixture_kind
        assert fixture["expected_diagnostic_code"] == diagnostic_code
        assert (ROOT / fixture_path).is_file(), fixture_path


def test_package_loader_bridge_row_uses_live_interop_bridge_contracts() -> None:
    row = _catalog_rows()[PACKAGE_LOADER_CLAIM]
    positive = _evidence(row, "positive_evidence")
    negative = _evidence(row, "negative_evidence")

    assert row["capability_id"] == "runtime.interop.package-loader-bridge"
    assert _runtime_cases(row) <= REQUIRED_CASES
    assert {
        "runtime-package-loader-bridge-abi",
        "live-package-loading-interop-runtime-implementation",
    } <= _runtime_cases(row)
    assert {
        INTEROP_BRIDGE_PACKAGING_CONSUMER_FIXTURE,
        INTEROP_BRIDGE_PACKAGING_PROVIDER_FIXTURE,
        INTEROP_BRIDGE_PACKAGING_RUNTIME_ABI_PROBE,
        INTEROP_HEADER_MODULE_BRIDGE_RUNTIME_ABI_PROBE,
    } <= positive
    assert INTEROP_PACKAGE_LOADER_FAIL_CLOSED_ABI_PROBE in negative
    assert {"O3PKG8052", "O3RT004"} <= set(row["required_diagnostic_codes"])


def test_mixed_image_replay_row_uses_import_matrix_and_reset_replay_probes() -> None:
    row = _catalog_rows()[MIXED_IMAGE_REPLAY_CLAIM]
    positive = _evidence(row, "positive_evidence")

    assert row["capability_id"] == "runtime.interop.mixed-image-replay"
    assert {
        "imported-runtime-packaging-replay",
        "mixed-image-interop-semantics",
    } <= REQUIRED_CASES
    assert {
        "imported-runtime-packaging-replay",
        "mixed-image-interop-semantics",
        MULTI_IMAGE_REGISTRATION_RESET_REPLAY_CASE,
    } <= _runtime_cases(row)
    assert {
        IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
        IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
        IMPORTED_RUNTIME_PACKAGING_PROBE,
        MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE,
        "tests/native/runtime/object_model/registration_replay_contract.objc3",
    } <= positive
    assert {"O3S200", "O3RT004"} <= set(row["required_diagnostic_codes"])
