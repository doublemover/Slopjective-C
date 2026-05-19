"""Shared JSON and text file helper exports."""

from __future__ import annotations

from objc3c_shared.json_io import (
    JsonSchemaValidationError,
    canonical_json,
    load_json_any,
    load_json_array,
    load_json_object,
    load_json_with_schema,
    load_optional_json_object,
    render_json,
    require_json_object,
    validate_json_schema,
    write_json_file,
    write_report_json,
    write_text_file,
)

__all__ = [
    "canonical_json",
    "JsonSchemaValidationError",
    "load_json_any",
    "load_json_array",
    "load_json_object",
    "load_json_with_schema",
    "load_optional_json_object",
    "render_json",
    "require_json_object",
    "validate_json_schema",
    "write_json_file",
    "write_report_json",
    "write_text_file",
]
