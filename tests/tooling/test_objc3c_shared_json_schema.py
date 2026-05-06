from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

from objc3c_shared.json_io import load_json_object, render_json, write_report_json
from objc3c_shared.report_model import report_envelope
from objc3c_shared.schema_registry import schema_path, schema_registry_summary

ROOT = Path(__file__).resolve().parents[2]
VALIDATOR_PATH = ROOT / "scripts" / "validate_capability_docs.py"
ROUTING_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_llvm_capability_routing.cpp"
PARSER_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "json" / "json_parser.cpp"
WRITER_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "json" / "json_writer.cpp"


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
    )

    write_report_json(out, payload, sort_keys=True)

    assert load_json_object(out)["contract_id"] == "objc3c.test.report.v1"
    assert render_json({"b": 2, "a": 1}, sort_keys=True) == '{\n  "a": 1,\n  "b": 2\n}\n'
    assert out.read_text(encoding="utf-8").endswith("\n")


def test_schema_registry_includes_capability_matrix() -> None:
    assert schema_path("objc3c-capability-matrix-v1").as_posix().endswith(
        "docs/support/capability_matrix.schema.json"
    )
    assert "objc3c-capability-matrix-v1" in schema_registry_summary()


def test_capability_docs_validate_against_schema_and_evidence() -> None:
    validator = _load_validator()

    assert validator.main(["--check"]) == 0


def test_native_capability_routing_uses_strict_json_parser() -> None:
    source = ROUTING_SOURCE.read_text(encoding="utf-8")

    assert "io/json/json_parser.h" in source
    assert "ParseJson(text)" in source
    assert "llvm capability summary parse failure: " in source
    assert "ExtractObjectSegment" not in source
    assert "ExtractBoolField" not in source
    assert "ExtractStringField" not in source


def test_native_json_parser_rejects_malformed_internal_reports() -> None:
    source = PARSER_SOURCE.read_text(encoding="utf-8")

    assert "unexpected trailing JSON content" in source
    assert "duplicate JSON object key" in source
    assert "unescaped control character in JSON string" in source
    assert "JSON number has leading zero" in source


def test_native_json_writer_uses_ordered_object_iteration() -> None:
    source = WRITER_SOURCE.read_text(encoding="utf-8")

    assert "for (const auto &[key, item] : value.AsObject())" in source
    assert "WriteJsonString(out, key)" in source
