from __future__ import annotations

from objc3c_runtime_backed_semantics_closure.cli import main
from objc3c_runtime_backed_semantics_closure.contracts import check_conformance_manifest
from objc3c_runtime_backed_semantics_closure.contracts import check_durable_replay_dirs
from objc3c_runtime_backed_semantics_closure.contracts import check_source_tokens
from objc3c_runtime_backed_semantics_closure.contracts import check_stress_manifest
from objc3c_runtime_backed_semantics_closure.compiler import compile_negative_fixtures
from objc3c_runtime_backed_semantics_closure.compiler import compile_positive_fixtures
from objc3c_runtime_backed_semantics_closure.compiler import read_diagnostics
from objc3c_runtime_backed_semantics_closure.compiler import run_compiler
from objc3c_runtime_backed_semantics_closure.inputs import DURABLE_REPLAY_DIRS
from objc3c_runtime_backed_semantics_closure.inputs import HELPER_SYMBOLS
from objc3c_runtime_backed_semantics_closure.inputs import NEGATIVE_FIXTURES
from objc3c_runtime_backed_semantics_closure.inputs import POSITIVE_FIXTURES
from objc3c_runtime_backed_semantics_closure.inputs import REQUIRED_IR_TOKENS
from objc3c_runtime_backed_semantics_closure.inputs import SOURCE_TOKENS
from objc3c_runtime_backed_semantics_closure.paths import COMPILER
from objc3c_runtime_backed_semantics_closure.paths import CONFORMANCE_MANIFEST
from objc3c_runtime_backed_semantics_closure.paths import CONFORMANCE_NEGATIVE
from objc3c_runtime_backed_semantics_closure.paths import CONFORMANCE_POSITIVE
from objc3c_runtime_backed_semantics_closure.paths import CONFORMANCE_README
from objc3c_runtime_backed_semantics_closure.paths import CONTRACT_ID
from objc3c_runtime_backed_semantics_closure.paths import DURABLE_REPLAY_ROOT
from objc3c_runtime_backed_semantics_closure.paths import IR_EMITTER
from objc3c_runtime_backed_semantics_closure.paths import ISSUE
from objc3c_runtime_backed_semantics_closure.paths import JSON_OUT
from objc3c_runtime_backed_semantics_closure.paths import LOWERING_CONTRACT_CPP
from objc3c_runtime_backed_semantics_closure.paths import LOWERING_CONTRACT_H
from objc3c_runtime_backed_semantics_closure.paths import MD_OUT
from objc3c_runtime_backed_semantics_closure.paths import REPORT_DIR
from objc3c_runtime_backed_semantics_closure.paths import ROOT
from objc3c_runtime_backed_semantics_closure.paths import RUNTIME
from objc3c_runtime_backed_semantics_closure.paths import SCRATCH
from objc3c_runtime_backed_semantics_closure.paths import SEMA_PASS_MANAGER
from objc3c_runtime_backed_semantics_closure.paths import SEMANTIC_PASSES
from objc3c_runtime_backed_semantics_closure.paths import STATIC_ANALYSIS
from objc3c_runtime_backed_semantics_closure.paths import STRESS_MANIFEST
from objc3c_runtime_backed_semantics_closure.paths import read
from objc3c_runtime_backed_semantics_closure.paths import rel
from objc3c_runtime_backed_semantics_closure.reporting import render_markdown
from objc3c_runtime_backed_semantics_closure.summary import build_summary

__all__ = [
    "COMPILER",
    "CONFORMANCE_MANIFEST",
    "CONFORMANCE_NEGATIVE",
    "CONFORMANCE_POSITIVE",
    "CONFORMANCE_README",
    "CONTRACT_ID",
    "DURABLE_REPLAY_DIRS",
    "DURABLE_REPLAY_ROOT",
    "HELPER_SYMBOLS",
    "IR_EMITTER",
    "ISSUE",
    "JSON_OUT",
    "LOWERING_CONTRACT_CPP",
    "LOWERING_CONTRACT_H",
    "MD_OUT",
    "NEGATIVE_FIXTURES",
    "POSITIVE_FIXTURES",
    "REPORT_DIR",
    "REQUIRED_IR_TOKENS",
    "ROOT",
    "RUNTIME",
    "SCRATCH",
    "SEMA_PASS_MANAGER",
    "SEMANTIC_PASSES",
    "SOURCE_TOKENS",
    "STATIC_ANALYSIS",
    "STRESS_MANIFEST",
    "build_summary",
    "check_conformance_manifest",
    "check_durable_replay_dirs",
    "check_source_tokens",
    "check_stress_manifest",
    "compile_negative_fixtures",
    "compile_positive_fixtures",
    "main",
    "read",
    "read_diagnostics",
    "rel",
    "render_markdown",
    "run_compiler",
]


if __name__ == "__main__":
    raise SystemExit(main())
