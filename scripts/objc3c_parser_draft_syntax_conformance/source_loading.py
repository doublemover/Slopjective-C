from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_parser_draft_syntax_conformance.paths import CONFORMANCE_MANIFEST
from objc3c_parser_draft_syntax_conformance.paths import PARSER
from objc3c_parser_draft_syntax_conformance.paths import POSITIVE_FIXTURE
from objc3c_parser_draft_syntax_conformance.paths import README
from objc3c_parser_draft_syntax_conformance.paths import read
from objc3c_tooling.json_io import load_json_any as load_json

REQUIRED_SURFACES = [
    "block_literal",
    "try_expression",
    "throw_statement",
    "do_catch",
    "error_catch_clause",
    "error_bridge_payload",
    "error_foreign_boundary",
    "error_nested_cleanup_marker",
    "throws_callable",
    "async_callable",
    "await_expression",
    "actor_interface",
    "macro_attribute",
    "macro_package",
    "macro_provenance",
    "macro_cache_key",
    "macro_sandbox_policy",
    "property_behavior",
    "property_attribute_metadata",
    "property_accessor_metadata",
    "property_synthesis_metadata",
    "property_reflection_input",
    "property_ownership_nullability",
    "interop_attribute",
    "interop_import_module",
    "interop_swift_annotation",
    "interop_cxx_annotation",
    "interop_header_import",
    "interop_error_bridge",
]
REPLAY_KEY_FIELDS = {
    "block_literal": "blocks",
    "try_expression": "try",
    "throw_statement": "throw",
    "do_catch": "do_catch",
    "error_catch_clause": "error_catch_clauses",
    "error_bridge_payload": "error_bridge_payloads",
    "error_foreign_boundary": "error_foreign_boundaries",
    "error_nested_cleanup_marker": "error_nested_cleanup_markers",
    "throws_callable": "throws_callables",
    "async_callable": "async_callables",
    "await_expression": "await",
    "actor_interface": "actors",
    "macro_attribute": "macro_attrs",
    "macro_package": "macro_packages",
    "macro_provenance": "macro_provenance",
    "macro_cache_key": "macro_cache_keys",
    "macro_sandbox_policy": "macro_sandbox_policies",
    "property_behavior": "property_behaviors",
    "property_attribute_metadata": "property_attrs",
    "property_accessor_metadata": "property_accessor_selectors",
    "property_synthesis_metadata": "property_synthesis_metadata",
    "property_reflection_input": "property_reflection_inputs",
    "property_ownership_nullability": "property_ownership_nullability",
    "interop_attribute": "interop_attrs",
    "interop_import_module": "interop_import_modules",
    "interop_swift_annotation": "interop_swift_annotations",
    "interop_cxx_annotation": "interop_cxx_annotations",
    "interop_header_import": "interop_header_imports",
    "interop_error_bridge": "interop_error_bridges",
}


def load_draft_syntax_sources() -> dict[str, Any]:
    return {
        "manifest": load_json(CONFORMANCE_MANIFEST),
        "positive_text": read(POSITIVE_FIXTURE),
        "parser_text": read(PARSER),
        "readme_text": read(README),
    }


def build_surface_index(surfaces: list[dict[str, Any]]) -> dict[str, dict[str, Any]]:
    return {surface["id"]: surface for surface in surfaces}


def static_source_truth_paths() -> list[Path]:
    return [
        CONFORMANCE_MANIFEST,
        POSITIVE_FIXTURE,
        PARSER,
        README,
    ]
