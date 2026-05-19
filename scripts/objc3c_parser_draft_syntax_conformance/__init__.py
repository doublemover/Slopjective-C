"""Owner modules for parser draft syntax conformance evidence."""

from __future__ import annotations

from objc3c_parser_draft_syntax_conformance.cli import main
from objc3c_parser_draft_syntax_conformance.compiler import run_compiler
from objc3c_parser_draft_syntax_conformance.diagnostics import CODE_RE
from objc3c_parser_draft_syntax_conformance.diagnostics import HEADER_RE
from objc3c_parser_draft_syntax_conformance.diagnostics import diagnostic_matches
from objc3c_parser_draft_syntax_conformance.diagnostics import expected_code_header
from objc3c_parser_draft_syntax_conformance.diagnostics import find_parser_manifest
from objc3c_parser_draft_syntax_conformance.diagnostics import parse_replay_key
from objc3c_parser_draft_syntax_conformance.paths import COMPILER
from objc3c_parser_draft_syntax_conformance.paths import CONFORMANCE_MANIFEST
from objc3c_parser_draft_syntax_conformance.paths import CONTRACT_ID
from objc3c_parser_draft_syntax_conformance.paths import JSON_OUT
from objc3c_parser_draft_syntax_conformance.paths import MD_OUT
from objc3c_parser_draft_syntax_conformance.paths import PARSER
from objc3c_parser_draft_syntax_conformance.paths import POSITIVE_FIXTURE
from objc3c_parser_draft_syntax_conformance.paths import README
from objc3c_parser_draft_syntax_conformance.paths import REPORT_DIR
from objc3c_parser_draft_syntax_conformance.paths import ROOT
from objc3c_parser_draft_syntax_conformance.paths import TMP_ROOT
from objc3c_parser_draft_syntax_conformance.paths import read
from objc3c_parser_draft_syntax_conformance.paths import rel
from objc3c_parser_draft_syntax_conformance.publication import write_outputs
from objc3c_parser_draft_syntax_conformance.rendering import render_markdown
from objc3c_parser_draft_syntax_conformance.source_loading import REPLAY_KEY_FIELDS
from objc3c_parser_draft_syntax_conformance.source_loading import REQUIRED_SURFACES
from objc3c_parser_draft_syntax_conformance.summary import build_summary
from objc3c_tooling.cli import add_check_argument
from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs

__all__ = [
    "CODE_RE",
    "COMPILER",
    "CONFORMANCE_MANIFEST",
    "CONTRACT_ID",
    "HEADER_RE",
    "JSON_OUT",
    "MD_OUT",
    "PARSER",
    "POSITIVE_FIXTURE",
    "README",
    "REPLAY_KEY_FIELDS",
    "REPORT_DIR",
    "REQUIRED_SURFACES",
    "ROOT",
    "TMP_ROOT",
    "add_check_argument",
    "build_summary",
    "diagnostic_matches",
    "expected_code_header",
    "expected_json_report",
    "find_parser_manifest",
    "load_json",
    "main",
    "parse_replay_key",
    "read",
    "rel",
    "render_markdown",
    "run_compiler",
    "write_outputs",
    "write_report_outputs",
]
