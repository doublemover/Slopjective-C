from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

import pytest

from objc3c_shared import json_io as shared_json_io
from objc3c_shared.json_io import (
    JsonSchemaValidationError,
    load_json_object,
    load_json_with_schema,
    render_json,
    validate_json_schema,
    write_json_file,
    write_report_json,
)
from objc3c_shared.report_model import report_envelope, validate_report_envelope
from objc3c_shared.schema_registry import (
    load_schema,
    schema_ids,
    schema_path,
    schema_registry_summary,
    validate_registered_schema,
)
from objc3c_tooling import json_io as tooling_json_io

ROOT = Path(__file__).resolve().parents[2]
VALIDATOR_PATH = ROOT / "scripts" / "validate_capability_docs.py"
ROUTING_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_llvm_capability_routing.cpp"
CAPABILITY_SUMMARY_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_llvm_capability_summary.cpp"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "json" / "json_parser.cpp"
PARSER_COMPLETION_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "json" / "json_parser_completion.cpp"
PARSER_OBJECT_MEMBER_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "json" / "json_parser_object_container_member.cpp"
PARSER_NUMBER_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "json" / "json_parser_number_mantissa_integer_zero_token.cpp"
PARSER_STRING_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "json" / "json_parser_string_token_character.cpp"
WRITER_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "json" / "json_writer.cpp"
WRITER_VALUE_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "json" / "json_value_writer.cpp"
WRITER_CONTAINER_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "json" / "json_value_container_writer.cpp"


def _load_validator():
    spec = importlib.util.spec_from_file_location("validate_capability_docs", VALIDATOR_PATH)
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_shared_json_helpers_are_deterministic(tmp_path: Path) -> None:
    out = tmp_path / "report.json"
    payload = report_envelope(
        contract_id="objc3c.test.report.v1",
        status="PASS",
        generated_by="tests/tooling/test_objc3c_shared_json_schema.py",
        payload={"b": 2, "a": 1},
        schema_id="objc3c.test.report.schema.v1",
    )

    validate_report_envelope(payload, require_schema_id=True)
    write_report_json(out, payload)

    assert load_json_object(out)["contract_id"] == "objc3c.test.report.v1"
    assert render_json({"b": 2, "a": 1}, sort_keys=True) == '{\n  "a": 1,\n  "b": 2\n}\n'
    assert out.read_text(encoding="utf-8").endswith("\n")


def test_tooling_json_io_facade_routes_to_shared_helpers() -> None:
    assert tooling_json_io.canonical_json is shared_json_io.canonical_json
    assert tooling_json_io.load_json_object is shared_json_io.load_json_object
    assert tooling_json_io.write_json_file is shared_json_io.write_json_file
    assert tooling_json_io.write_report_json is shared_json_io.write_report_json


def test_shared_json_schema_validation_blocks_invalid_reports(tmp_path: Path) -> None:
    schema = {
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "type": "object",
        "required": ["contract_id", "status"],
        "properties": {
            "contract_id": {"type": "string"},
            "status": {"enum": ["PASS", "FAIL"]},
        },
        "additionalProperties": False,
    }
    schema_file = tmp_path / "report.schema.json"
    report_file = tmp_path / "report.json"
    write_json_file(schema_file, schema, sort_keys=True)
    write_report_json(
        report_file,
        {"contract_id": "objc3c.test", "status": "PASS"},
        schema=schema,
    )

    assert load_json_with_schema(report_file, schema_file) == {
        "contract_id": "objc3c.test",
        "status": "PASS",
    }
    with pytest.raises(JsonSchemaValidationError, match=r"schema validation failed at status"):
        validate_json_schema(
            {"contract_id": "objc3c.test", "status": "UNKNOWN"},
            schema,
            label="test report",
        )


def test_report_envelope_rejects_invalid_shape() -> None:
    with pytest.raises(ValueError, match="report status must be PASS or FAIL"):
        report_envelope(
            contract_id="objc3c.test.report.v1",
            status="UNKNOWN",
            generated_by="tests/tooling/test_objc3c_shared_json_schema.py",
            payload={},
        )
    with pytest.raises(ValueError, match="report payload must be an object"):
        validate_report_envelope(
            {
                "contract_id": "objc3c.test.report.v1",
                "status": "PASS",
                "generated_by": "tests/tooling/test_objc3c_shared_json_schema.py",
                "payload": [],
            }
        )


def test_schema_registry_includes_capability_truth_schemas() -> None:
    capability_truth_schemas = {
        "objc3c-capability-matrix-v1": (
            "schemas/objc3c-capability-matrix-v1.schema.json",
            ROOT / "docs" / "support" / "capability_matrix.json",
        ),
        "objc3c-capability-evidence-map-v1": (
            "schemas/objc3c-capability-evidence-map-v1.schema.json",
            ROOT / "docs" / "support" / "evidence_map.json",
        ),
    }
    summary = schema_registry_summary()

    for schema_id, (expected_path, payload_path) in capability_truth_schemas.items():
        assert schema_id in schema_ids()
        assert schema_path(schema_id).as_posix().endswith(expected_path)
        assert load_schema(schema_id)["type"] == "object"
        validate_registered_schema(load_json_object(payload_path), schema_id)
        assert summary[schema_id] == expected_path


def test_schema_registry_includes_conformance_evidence_schemas() -> None:
    conformance_evidence_schemas = {
        "objc3-conformance-dashboard-status-v1": "schemas/objc3-conformance-dashboard-status-v1.schema.json",
        "objc3-conformance-evidence-bundle-v1": "schemas/objc3-conformance-evidence-bundle-v1.schema.json",
        "objc3-runtime-2025Q4-manifest": "schemas/objc3-runtime-2025Q4.manifest.schema.json",
        "objc3-abi-2025Q4": "schemas/objc3-abi-2025Q4.schema.json",
    }
    summary = schema_registry_summary()

    for schema_id, expected_path in conformance_evidence_schemas.items():
        schema = load_schema(schema_id)
        assert schema_id in schema_ids()
        assert schema_path(schema_id).as_posix().endswith(expected_path)
        assert schema["$schema"] == "https://json-schema.org/draft/2020-12/schema"
        assert summary[schema_id] == expected_path


def test_schema_registry_includes_release_adoption_schemas() -> None:
    release_adoption_schemas = {
        "objc3c-abi-api-governance-v1": "schemas/objc3c-abi-api-governance-v1.schema.json",
        "objc3c-adoption-legibility-evidence-v1": "schemas/objc3c-adoption-legibility-evidence-v1.schema.json",
        "objc3c-long-horizon-operations-evidence-v1": "schemas/objc3c-long-horizon-operations-evidence-v1.schema.json",
        "objc3c-upgrade-support-report-v1": "schemas/objc3c-upgrade-support-report-v1.schema.json",
        "objc3c-update-manifest-v1": "schemas/objc3c-update-manifest-v1.schema.json",
        "objc3c-release-channel-operations-v1": "schemas/objc3c-release-channel-operations-v1.schema.json",
        "objc3c-application-architecture-evidence-summary-v1": "schemas/objc3c-application-architecture-evidence-summary-v1.schema.json",
        "objc3c-artifact-authenticity-v1": "schemas/objc3c-artifact-authenticity-v1.schema.json",
        "source-hygiene-hard-cutover-report-v1": "schemas/source-hygiene-hard-cutover-report-v1.schema.json",
        "objc3c-package-channels-manifest-v1": "schemas/objc3c-package-channels-manifest-v1.schema.json",
        "objc3c-package-manifest-v1": "schemas/objc3c-package-manifest-v1.schema.json",
        "objc3c-package-lock-v1": "schemas/objc3c-package-lock-v1.schema.json",
        "objc3c-package-offline-mirror-index-v1": "schemas/objc3c-package-offline-mirror-index-v1.schema.json",
        "objc3c-package-local-registry-index-v1": "schemas/objc3c-package-local-registry-index-v1.schema.json",
        "objc3c-package-install-receipt-v1": "schemas/objc3c-package-install-receipt-v1.schema.json",
        "objc3c-platform-support-matrix-v1": "schemas/objc3c-platform-support-matrix-v1.schema.json",
        "objc3c-compiler-throughput-summary-v1": "schemas/objc3c-compiler-throughput-summary-v1.schema.json",
        "objc3c-developer-tooling-editor-surface-v1": "schemas/objc3c-developer-tooling-editor-surface-v1.schema.json",
        "objc3c-developer-tooling-schema-surface-summary-v1": "schemas/objc3c-developer-tooling-schema-surface-summary-v1.schema.json",
        "objc3c-performance-telemetry-v1": "schemas/objc3c-performance-telemetry-v1.schema.json",
        "objc3c-full-envelope-dashboard-summary-v1": "schemas/objc3c-full-envelope-dashboard-summary-v1.schema.json",
    }
    summary = schema_registry_summary()

    for schema_id, expected_path in release_adoption_schemas.items():
        schema = load_schema(schema_id)
        assert schema_id in schema_ids()
        assert schema_path(schema_id).as_posix().endswith(expected_path)
        assert schema["$schema"] == "https://json-schema.org/draft/2020-12/schema"
        assert summary[schema_id] == expected_path


def test_capability_docs_validate_against_schema_and_evidence() -> None:
    validator = _load_validator()

    assert validator.main(["--check"]) == 0


def test_native_capability_routing_uses_strict_json_parser() -> None:
    source = ROUTING_SOURCE.read_text(encoding="utf-8")
    summary_source = CAPABILITY_SUMMARY_SOURCE.read_text(encoding="utf-8")

    assert "TryLoadObjc3LLVMCapabilitySummary" in source
    assert "io/json/json_parser.h" in summary_source
    assert "ParseJson(text)" in summary_source
    assert "llvm capability summary parse failure: " in summary_source
    assert "ExtractObjectSegment" not in summary_source
    assert "ExtractBoolField" not in summary_source
    assert "ExtractStringField" not in summary_source


def test_native_json_parser_rejects_malformed_internal_reports() -> None:
    source = PARSER_SOURCE.read_text(encoding="utf-8")
    completion_source = PARSER_COMPLETION_SOURCE.read_text(encoding="utf-8")
    object_member_source = PARSER_OBJECT_MEMBER_SOURCE.read_text(encoding="utf-8")
    number_source = PARSER_NUMBER_SOURCE.read_text(encoding="utf-8")
    string_source = PARSER_STRING_SOURCE.read_text(encoding="utf-8")

    assert "ParseJsonDocument(text)" in source
    assert "unexpected trailing JSON content" in completion_source
    assert "duplicate JSON object key" in object_member_source
    assert "unescaped control character in JSON string" in string_source
    assert "JSON number has leading zero" in number_source


def test_native_json_writer_uses_ordered_object_iteration() -> None:
    source = WRITER_SOURCE.read_text(encoding="utf-8")
    value_source = WRITER_VALUE_SOURCE.read_text(encoding="utf-8")
    container_source = WRITER_CONTAINER_SOURCE.read_text(encoding="utf-8")

    assert "WriteJsonValue(out, value)" in source
    assert "WriteJsonObjectValue(out, value.AsObject())" in value_source
    assert "for (const auto &[key, item] : object)" in container_source
    assert "WriteJsonString(out, key)" in container_source
