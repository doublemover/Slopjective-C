from __future__ import annotations

import json
import sys
from copy import deepcopy
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPTS_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from objc3c_shared.schema_registry import validate_registered_schema  # noqa: E402
from check_objc3c_standalone_textual_interface_payload import (  # noqa: E402
    PUBLIC_VALIDATE_COMMAND,
    REQUIRED_NEGATIVE_CASE_IDS,
    REQUIRED_SOURCE_TRUTH_POLICY,
    negative_payload_cases,
    validate_payload,
)

TEXTUAL_INTERFACE_SCHEMA_ID = "objc3c-standalone-textual-interface-payload-v1"
TEXTUAL_INTERFACE_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "module_interfaces"
    / "standalone_textual_interface_payload.json"
)
RUNTIME_EXECUTABLE_SCHEMA_ID = "objc3c-advanced-runtime-executable-contract-v1"
RUNTIME_EXECUTABLE_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "advanced_runtime_closure"
    / "executable_runtime_contract_surfaces.json"
)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def source_text(relative_path: str) -> str:
    path = ROOT / relative_path
    assert path.is_file(), relative_path
    return path.read_text(encoding="utf-8")


def assert_checked_repo_path(relative_path: str) -> None:
    assert not relative_path.startswith(("tmp/", "temp/", "generated/", "build/", "dist/"))
    assert (ROOT / relative_path).exists(), relative_path


def assert_textual_interface_fail_closed(payload: dict[str, Any]) -> None:
    failures = textual_interface_contract_failures(payload)
    assert failures == []


def textual_interface_contract_failures(payload: dict[str, Any]) -> list[str]:
    failures: list[str] = []
    roundtrip = payload["interface_roundtrip"]
    if roundtrip["parse_status"] != "supported":
        failures.append("interface importer parse support missing")
    if roundtrip["semantic_equivalence_status"] != "supported":
        failures.append("interface semantic roundtrip support missing")
    if roundtrip["drift_diagnostic"] != "O3IFC8238":
        failures.append("interface drift diagnostic changed")
    if "modules.standalone-textual-interface-payload" not in payload["capability_requirements"]:
        failures.append("standalone textual interface capability requirement missing")
    if "npm run objc3c -- compile-objc3c <input.objc3>" not in payload["public_commands"]:
        failures.append("public compile command missing")
    if PUBLIC_VALIDATE_COMMAND not in payload["public_commands"]:
        failures.append("public textual interface validation command missing")
    for declaration in payload["declarations"]:
        source_path = str(declaration["source_anchor"]["path"])
        if source_path.startswith(("tmp/", "temp/", "generated/", "build/", "dist/")):
            failures.append(f"non-durable declaration source anchor: {source_path}")
        elif not (ROOT / source_path).exists():
            failures.append(f"missing declaration source anchor: {source_path}")
    return failures


def test_standalone_textual_interface_payload_fixture_is_schema_backed() -> None:
    payload = load_json(TEXTUAL_INTERFACE_FIXTURE)

    validate_registered_schema(payload, TEXTUAL_INTERFACE_SCHEMA_ID)
    summary = validate_payload(payload)
    assert payload["payload_kind"] == "objc3c.standalone_textual_interface_payload.v1"
    assert summary["status"] == "PASS"
    assert summary["declaration_count"] == 2
    assert payload["source_counts"] == {
        "globals": 0,
        "protocols": 0,
        "interfaces": 1,
        "implementations_reserved": 1,
        "functions": 1,
    }
    assert set(payload["issue_refs"]) == {8238, 8208, 8233, 8234}
    assert payload["source_truth_policy"] == REQUIRED_SOURCE_TRUTH_POLICY
    assert {row["case_id"] for row in payload["negative_cases"]} == REQUIRED_NEGATIVE_CASE_IDS
    assert_textual_interface_fail_closed(payload)


def test_textual_interface_runtime_source_is_registered_and_emitted() -> None:
    artifact_source = source_text(
        "native/objc3c/src/artifacts/objc3_frontend_textual_interface_payload_artifact.cpp"
    )
    driver_source = source_text(
        "native/objc3c/src/driver/objc3_driver_frontend_interface_payload_artifact_publication.cpp"
    )
    bundle_source = source_text(
        "native/objc3c/src/artifacts/objc3_frontend_artifact_bundle_publication.cpp"
    )
    importer_header = source_text(
        "native/objc3c/src/artifacts/objc3_frontend_textual_interface_payload_import.h"
    )
    importer_source = source_text(
        "native/objc3c/src/artifacts/objc3_frontend_textual_interface_payload_import.cpp"
    )
    schema_registry_source = source_text("scripts/objc3c_shared/schema_registry.py")
    schema_table_source = source_text(
        "native/objc3c/src/artifacts/json/artifact_schema_contract_table.cpp"
    )

    assert "PublishRegisteredArtifactJson(request)" in artifact_source
    assert "\"source_truth_policy\"" in artifact_source
    assert "\"negative_cases\"" in artifact_source
    assert "\"local_temp_source_truth_allowed\"" in artifact_source
    assert "BuildObjc3StandaloneTextualInterfacePayloadArtifact(" in bundle_source
    assert "ValidateObjc3StandaloneTextualInterfacePayloadImport(" in importer_header
    assert "ParseJson(payload_json)" in importer_source
    assert "ValidateSourceTruthPolicy(payload, result)" in importer_source
    assert "ValidateNegativeCases(payload, result)" in importer_source
    assert "source_counts drift" in importer_source
    assert "interface import must carry package lock/trust identity" in importer_source
    assert "WriteStandaloneTextualInterfacePayloadArtifact(" in driver_source
    assert "io/objc3_artifact_writers.h" in driver_source
    assert ".interface-payload.json" in source_text(
        "native/objc3c/src/io/objc3_runtime_artifact_paths.cpp"
    )
    assert TEXTUAL_INTERFACE_SCHEMA_ID in schema_registry_source
    assert TEXTUAL_INTERFACE_SCHEMA_ID in schema_table_source


def test_textual_interface_reserved_roundtrip_is_rejected_by_contract_guard() -> None:
    payload = deepcopy(load_json(TEXTUAL_INTERFACE_FIXTURE))
    payload["interface_roundtrip"]["parse_status"] = "reserved-importer-not-landed"
    payload["interface_roundtrip"]["semantic_equivalence_status"] = "reserved-importer-not-landed"

    validate_registered_schema(payload, TEXTUAL_INTERFACE_SCHEMA_ID)
    assert textual_interface_contract_failures(payload) == [
        "interface importer parse support missing",
        "interface semantic roundtrip support missing",
    ]


def test_textual_interface_import_negative_cases_fail_closed() -> None:
    payload = load_json(TEXTUAL_INTERFACE_FIXTURE)
    failures = negative_payload_cases(payload)

    assert set(failures) == {
        "stale-schema",
        "unlocked-import",
        "hidden-declaration",
        "count-drift",
        "reserved-roundtrip",
        "typed-throws-abi-lowering",
        "typed-throws-interface-contract-drift",
        "value-optional-lowering",
        "value-optional-layout-drift",
    }
    assert "schema validation failed at schema_version" in failures["stale-schema"]
    assert "lock_identity must be package lock/trust identity" in failures["unlocked-import"]
    assert "schema validation failed at declarations.0.kind" in failures["hidden-declaration"]
    assert "source_counts.interfaces expected 1, saw 0" in failures["count-drift"]
    assert "parse_status must be supported" in failures["reserved-roundtrip"]
    assert (
        "runtime_execution_claimed must be false for nonthrowing effects"
        in failures["typed-throws-abi-lowering"]
    )
    assert (
        "typed_throws.declared_error_type must match effects.declared_error_type"
        in failures["typed-throws-interface-contract-drift"]
    )
    assert (
        "supported_runtime_payload_forms must be ['i32', 'bool', 'id']"
        in failures["value-optional-lowering"]
    )
    assert "abi_layout_id expected" in failures["value-optional-layout-drift"]


def test_runtime_executable_contract_fixture_is_schema_backed() -> None:
    payload = load_json(RUNTIME_EXECUTABLE_FIXTURE)

    validate_registered_schema(payload, RUNTIME_EXECUTABLE_SCHEMA_ID)
    assert payload["dependency_issue_ref"] == 8213
    assert payload["support_boundary"]["typed_dispatch_required"] is True
    assert payload["support_boundary"]["native_registration_required"] is True
    assert payload["support_boundary"]["executable_runtime_required"] is True
    assert payload["support_boundary"]["compatibility_shims"] is False
    assert {surface["issue_ref"] for surface in payload["surfaces"]} == {8214, 8215, 8216, 8217}
    for surface in payload["surfaces"]:
        assert surface["support_status"] == "supported"
        assert surface["typed_dispatch_required"] is True
        assert surface["native_registration_required"] is True
        assert surface["executable_runtime_required"] is True
        assert surface["fail_closed_required"] is True
        assert surface["compatibility_shim_allowed"] is False
        assert_checked_repo_path(str(surface["primary_fixture"]))
        assert_checked_repo_path(str(surface["negative_fixture"]))
        for source_anchor in surface["source_anchors"]:
            assert_checked_repo_path(str(source_anchor))


def test_runtime_executable_public_api_matches_fixture_contract() -> None:
    payload = load_json(RUNTIME_EXECUTABLE_FIXTURE)
    runtime_contract = payload["runtime_public_contract"]
    header = source_text(str(runtime_contract["header"]))
    implementation = source_text(str(runtime_contract["implementation"]))
    runtime_cmake = source_text("native/objc3c/src/runtime/CMakeLists.txt")
    public_api = source_text("native/objc3c/src/runtime/public/objc3_runtime_api.h")
    docs = source_text("docs/runbooks/objc3c_advanced_runtime_executable_contract.md")

    assert runtime_contract["abi_version_macro"] in header
    assert runtime_contract["snapshot_type"] in header
    for symbol in runtime_contract["copy_symbols"]:
        assert str(symbol) in header
        assert str(symbol) in implementation
    assert "OBJC3_RUNTIME_EXECUTABLE_CONTRACT_SUPPORT_REJECTED" in implementation
    assert "compatibility_shim_allowed = 0" in implementation
    assert "public/objc3_runtime_executable_contract.cpp" in runtime_cmake
    assert "objc3_runtime_executable_contract.h" in public_api
    assert "compatibility shims" in docs


def test_runtime_executable_negative_fixtures_are_diagnostic_marked() -> None:
    payload = load_json(RUNTIME_EXECUTABLE_FIXTURE)

    for negative in payload["negative_fixtures"]:
        fixture = ROOT / str(negative["fixture"])
        assert fixture.is_file()
        text = fixture.read_text(encoding="utf-8")
        assert str(negative["expected_diagnostic"]) in text
        assert str(negative["unsupported_boundary"]) not in ("", "supported")
