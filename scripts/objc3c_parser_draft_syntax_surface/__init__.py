"""Owner modules for parser draft syntax surface evidence."""

from __future__ import annotations

from objc3c_parser_draft_syntax_surface.aggregation import build_checks
from objc3c_parser_draft_syntax_surface.aggregation import build_summary
from objc3c_parser_draft_syntax_surface.aggregation import section_passed
from objc3c_parser_draft_syntax_surface.cli import main
from objc3c_parser_draft_syntax_surface.constants import CONTRACT_ID
from objc3c_parser_draft_syntax_surface.constants import POSITIVE_TOKENS
from objc3c_parser_draft_syntax_surface.constants import REQUIRED_ARTIFACT_FIELDS
from objc3c_parser_draft_syntax_surface.constants import REQUIRED_AST_FIELDS
from objc3c_parser_draft_syntax_surface.constants import REQUIRED_CONTRACT_ANCHORS
from objc3c_parser_draft_syntax_surface.constants import REQUIRED_PARSER_ANCHORS
from objc3c_parser_draft_syntax_surface.constants import VALIDATION_COMMANDS
from objc3c_parser_draft_syntax_surface.models import DraftSyntaxSurfaceSources
from objc3c_parser_draft_syntax_surface.paths import ARTIFACTS
from objc3c_parser_draft_syntax_surface.paths import AST
from objc3c_parser_draft_syntax_surface.paths import AST_BUILDER_HEADER
from objc3c_parser_draft_syntax_surface.paths import AST_BUILDER_SOURCE
from objc3c_parser_draft_syntax_surface.paths import JSON_OUT
from objc3c_parser_draft_syntax_surface.paths import MD_OUT
from objc3c_parser_draft_syntax_surface.paths import NEGATIVE_FIXTURE
from objc3c_parser_draft_syntax_surface.paths import PARSER
from objc3c_parser_draft_syntax_surface.paths import PARSER_CONTRACT
from objc3c_parser_draft_syntax_surface.paths import POSITIVE_FIXTURE
from objc3c_parser_draft_syntax_surface.paths import README
from objc3c_parser_draft_syntax_surface.paths import REPORT_DIR
from objc3c_parser_draft_syntax_surface.paths import ROOT
from objc3c_parser_draft_syntax_surface.paths import read
from objc3c_parser_draft_syntax_surface.paths import rel
from objc3c_parser_draft_syntax_surface.reporting import expected_reports
from objc3c_parser_draft_syntax_surface.reporting import write_outputs
from objc3c_parser_draft_syntax_surface.rendering import render_markdown
from objc3c_parser_draft_syntax_surface.source_extraction import load_draft_syntax_surface_sources
from objc3c_parser_draft_syntax_surface.source_extraction import static_source_truth_paths
from objc3c_tooling.cli import add_check_argument
from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs
from objc3c_tooling.validation import contains_all

__all__ = [
    "ARTIFACTS",
    "AST",
    "AST_BUILDER_HEADER",
    "AST_BUILDER_SOURCE",
    "CONTRACT_ID",
    "DraftSyntaxSurfaceSources",
    "JSON_OUT",
    "MD_OUT",
    "NEGATIVE_FIXTURE",
    "PARSER",
    "PARSER_CONTRACT",
    "POSITIVE_FIXTURE",
    "POSITIVE_TOKENS",
    "README",
    "REPORT_DIR",
    "REQUIRED_ARTIFACT_FIELDS",
    "REQUIRED_AST_FIELDS",
    "REQUIRED_CONTRACT_ANCHORS",
    "REQUIRED_PARSER_ANCHORS",
    "ROOT",
    "VALIDATION_COMMANDS",
    "add_check_argument",
    "build_checks",
    "build_summary",
    "contains_all",
    "expected_json_report",
    "expected_reports",
    "load_draft_syntax_surface_sources",
    "main",
    "read",
    "rel",
    "render_markdown",
    "section_passed",
    "static_source_truth_paths",
    "write_outputs",
    "write_report_outputs",
]
