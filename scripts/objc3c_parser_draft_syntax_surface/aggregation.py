from __future__ import annotations

from typing import Any

from objc3c_parser_draft_syntax_surface.constants import CONTRACT_ID
from objc3c_parser_draft_syntax_surface.constants import POSITIVE_TOKENS
from objc3c_parser_draft_syntax_surface.constants import REQUIRED_ARTIFACT_FIELDS
from objc3c_parser_draft_syntax_surface.constants import REQUIRED_AST_FIELDS
from objc3c_parser_draft_syntax_surface.constants import REQUIRED_CONTRACT_ANCHORS
from objc3c_parser_draft_syntax_surface.constants import REQUIRED_PARSER_ANCHORS
from objc3c_parser_draft_syntax_surface.constants import VALIDATION_COMMANDS
from objc3c_parser_draft_syntax_surface.models import DraftSyntaxSurfaceSources
from objc3c_parser_draft_syntax_surface.paths import NEGATIVE_FIXTURE
from objc3c_parser_draft_syntax_surface.paths import POSITIVE_FIXTURE
from objc3c_parser_draft_syntax_surface.paths import rel
from objc3c_parser_draft_syntax_surface.source_extraction import load_draft_syntax_surface_sources
from objc3c_tooling.validation import contains_all


def section_passed(values: dict[str, bool]) -> bool:
    return all(values.values())


def build_checks(sources: DraftSyntaxSurfaceSources) -> dict[str, dict[str, bool]]:
    return {
        "ast_fields": contains_all(sources.ast_text, REQUIRED_AST_FIELDS),
        "parser_anchors": contains_all(sources.parser_text, REQUIRED_PARSER_ANCHORS),
        "ast_builder": {
            "header_declares_summary_setter": "SetDraftSyntaxSurfaceSummary" in sources.builder_header_text,
            "source_writes_summary_to_program": "draft_syntax_surface_summary =" in sources.builder_source_text,
        },
        "parser_contract": contains_all(sources.parser_contract_text, REQUIRED_CONTRACT_ANCHORS),
        "artifact_manifest": contains_all(sources.artifacts_text, REQUIRED_ARTIFACT_FIELDS),
        "positive_fixture": contains_all(sources.positive_text, POSITIVE_TOKENS),
        "negative_fixture": {
            "negative_fixture_exists": NEGATIVE_FIXTURE.is_file(),
            "expects_o3p341": "Expected diagnostic code(s): O3P341" in sources.negative_text,
            "malformed_macro_payload": "objc_macro(\"Trace\")" in sources.negative_text,
        },
        "readme": {
            "references_positive_fixture": rel(POSITIVE_FIXTURE) in sources.readme_text,
            "references_negative_fixture": rel(NEGATIVE_FIXTURE) in sources.readme_text,
            "references_issue": "issue #8011" in sources.readme_text,
        },
    }


def build_summary() -> dict[str, Any]:
    sources = load_draft_syntax_surface_sources()
    checks = build_checks(sources)
    no_tmp_source_truth = all(not rel(path).startswith("tmp/") for path in sources.source_truth_paths)
    status = (
        "PASS"
        if all(section_passed(section) for section in checks.values()) and no_tmp_source_truth
        else "FAIL"
    )
    return {
        "contract_id": CONTRACT_ID,
        "issue": "#8011",
        "status": status,
        "source_truth_paths": [rel(path) for path in sources.source_truth_paths],
        "no_tmp_source_truth": no_tmp_source_truth,
        "checks": checks,
        "positive_fixture": rel(POSITIVE_FIXTURE),
        "negative_fixture": rel(NEGATIVE_FIXTURE),
        "validation_commands": list(VALIDATION_COMMANDS),
    }
