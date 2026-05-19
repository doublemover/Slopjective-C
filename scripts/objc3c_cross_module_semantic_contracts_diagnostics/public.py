from __future__ import annotations

from objc3c_cross_module_semantic_contracts_diagnostics.catalog import CONFORMANCE_NEGATIVE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import CONFORMANCE_POSITIVE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import CONTRACT_ID
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import FRONTEND_ARTIFACTS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import FRONTEND_PIPELINE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import FRONTEND_TYPES
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import IR_EMITTER_H
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import ISSUE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import LANDED_FLAGS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import LOWERING_CONTRACT
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import NEGATIVE_FIXTURE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import POSITIVE_FIXTURE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import POSITIVE_MIN_COUNTS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import REPLAY_SEGMENTS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SEMA_CONTRACT
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SEMANTIC_MANIFEST
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SEMANTIC_PASSES
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SEMANTIC_PASSES_H
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SEMANTIC_README
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SOURCE_TRUTH_PATHS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import STRESS_MANIFEST
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SUMMARY_FIELDS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import VALIDATION_COMMANDS
from objc3c_cross_module_semantic_contracts_diagnostics.checks import compile_contract_checks
from objc3c_cross_module_semantic_contracts_diagnostics.checks import compile_static_presence
from objc3c_cross_module_semantic_contracts_diagnostics.cli import main
from objc3c_cross_module_semantic_contracts_diagnostics.diagnostics import diagnostic_matches
from objc3c_cross_module_semantic_contracts_diagnostics.diagnostics import find_model
from objc3c_cross_module_semantic_contracts_diagnostics.execution import run_compiler
from objc3c_cross_module_semantic_contracts_diagnostics.paths import COMPILER
from objc3c_cross_module_semantic_contracts_diagnostics.paths import JSON_OUT
from objc3c_cross_module_semantic_contracts_diagnostics.paths import MD_OUT
from objc3c_cross_module_semantic_contracts_diagnostics.paths import REPORT_DIR
from objc3c_cross_module_semantic_contracts_diagnostics.paths import ROOT
from objc3c_cross_module_semantic_contracts_diagnostics.paths import TMP_ROOT
from objc3c_cross_module_semantic_contracts_diagnostics.paths import read
from objc3c_cross_module_semantic_contracts_diagnostics.paths import rel
from objc3c_cross_module_semantic_contracts_diagnostics.rendering import expected_report_outputs
from objc3c_cross_module_semantic_contracts_diagnostics.rendering import render_markdown
from objc3c_cross_module_semantic_contracts_diagnostics.rendering import write_outputs
from objc3c_cross_module_semantic_contracts_diagnostics.summary import build_summary
from objc3c_tooling.cli import add_check_argument
from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs
from objc3c_tooling.validation import contains_all

__all__ = [
    "COMPILER",
    "CONFORMANCE_NEGATIVE",
    "CONFORMANCE_POSITIVE",
    "CONTRACT_ID",
    "FRONTEND_ARTIFACTS",
    "FRONTEND_PIPELINE",
    "FRONTEND_TYPES",
    "IR_EMITTER_H",
    "ISSUE",
    "JSON_OUT",
    "LANDED_FLAGS",
    "LOWERING_CONTRACT",
    "MD_OUT",
    "NEGATIVE_FIXTURE",
    "POSITIVE_FIXTURE",
    "POSITIVE_MIN_COUNTS",
    "REPLAY_SEGMENTS",
    "REPORT_DIR",
    "ROOT",
    "SEMA_CONTRACT",
    "SEMANTIC_MANIFEST",
    "SEMANTIC_PASSES",
    "SEMANTIC_PASSES_H",
    "SEMANTIC_README",
    "SOURCE_TRUTH_PATHS",
    "STRESS_MANIFEST",
    "SUMMARY_FIELDS",
    "TMP_ROOT",
    "VALIDATION_COMMANDS",
    "add_check_argument",
    "build_summary",
    "compile_contract_checks",
    "compile_static_presence",
    "contains_all",
    "diagnostic_matches",
    "expected_json_report",
    "expected_report_outputs",
    "find_model",
    "load_json",
    "main",
    "read",
    "rel",
    "render_markdown",
    "run_compiler",
    "write_outputs",
    "write_report_outputs",
]
