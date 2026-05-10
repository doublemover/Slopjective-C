from __future__ import annotations

from objc3c_runtime_backed_semantics_closure.compiler import compile_negative_fixtures
from objc3c_runtime_backed_semantics_closure.compiler import compile_positive_fixtures
from objc3c_runtime_backed_semantics_closure.contracts import check_conformance_manifest
from objc3c_runtime_backed_semantics_closure.contracts import check_durable_replay_dirs
from objc3c_runtime_backed_semantics_closure.contracts import check_source_tokens
from objc3c_runtime_backed_semantics_closure.contracts import check_stress_manifest
from objc3c_runtime_backed_semantics_closure.inputs import DURABLE_REPLAY_DIRS
from objc3c_runtime_backed_semantics_closure.inputs import HELPER_SYMBOLS
from objc3c_runtime_backed_semantics_closure.inputs import NEGATIVE_FIXTURES
from objc3c_runtime_backed_semantics_closure.inputs import POSITIVE_FIXTURES
from objc3c_runtime_backed_semantics_closure.inputs import REQUIRED_IR_TOKENS
from objc3c_runtime_backed_semantics_closure.paths import CONTRACT_ID
from objc3c_runtime_backed_semantics_closure.paths import IR_EMITTER
from objc3c_runtime_backed_semantics_closure.paths import ISSUE
from objc3c_runtime_backed_semantics_closure.paths import JSON_OUT
from objc3c_runtime_backed_semantics_closure.paths import LOWERING_CONTRACT_CPP
from objc3c_runtime_backed_semantics_closure.paths import LOWERING_CONTRACT_H
from objc3c_runtime_backed_semantics_closure.paths import MD_OUT
from objc3c_runtime_backed_semantics_closure.paths import RUNTIME
from objc3c_runtime_backed_semantics_closure.paths import SCRATCH
from objc3c_runtime_backed_semantics_closure.paths import SEMA_PASS_MANAGER
from objc3c_runtime_backed_semantics_closure.paths import SEMANTIC_PASSES
from objc3c_runtime_backed_semantics_closure.paths import STATIC_ANALYSIS
from objc3c_runtime_backed_semantics_closure.paths import rel


def build_summary() -> dict:
    positive_compile, aggregate_ir = compile_positive_fixtures()
    negative_compile = compile_negative_fixtures()
    source_checks = check_source_tokens()
    durable_replay_dirs = check_durable_replay_dirs()
    stress_manifest = check_stress_manifest()
    conformance = check_conformance_manifest()
    ir_tokens = {token: token in aggregate_ir for token in REQUIRED_IR_TOKENS}
    helper_ir_declarations = {
        symbol: symbol in aggregate_ir for symbol in HELPER_SYMBOLS
    }

    no_source_truth_under_tmp = all(
        not rel(path).startswith("tmp/")
        for path in [
            *POSITIVE_FIXTURES.values(),
            *(spec["path"] for spec in NEGATIVE_FIXTURES.values()),
            LOWERING_CONTRACT_H,
            LOWERING_CONTRACT_CPP,
            IR_EMITTER,
            SEMA_PASS_MANAGER,
            SEMANTIC_PASSES,
            STATIC_ANALYSIS,
            RUNTIME,
            JSON_OUT,
            MD_OUT,
        ]
    )

    counts = {
        "positive_fixture_count": len(POSITIVE_FIXTURES),
        "negative_fixture_count": len(NEGATIVE_FIXTURES),
        "runtime_helper_symbol_count": len(HELPER_SYMBOLS),
        "required_ir_token_count": len(REQUIRED_IR_TOKENS),
        "durable_replay_fixture_dir_count": len(DURABLE_REPLAY_DIRS),
    }
    checks = {
        "positive_compile": all(
            item["compiled"] and item["ir_exists"] and item["manifest_exists"]
            for item in positive_compile.values()
        ),
        "negative_compile": all(
            item["rejected"] and item["expected_codes_present"]
            for item in negative_compile.values()
        ),
        "required_ir_tokens": all(ir_tokens.values()),
        "runtime_helper_ir_declarations": all(helper_ir_declarations.values()),
        "source_tokens": all(
            value
            for group in source_checks.values()
            for value in group.values()
        ),
        "durable_replay_dirs": all(durable_replay_dirs.values()),
        "stress_manifest": all(stress_manifest.values()),
        "conformance": all(conformance.values()),
        "no_source_truth_under_tmp": no_source_truth_under_tmp,
    }

    return {
        "schema_version": "1.0.0",
        "issue": ISSUE,
        "contract_id": CONTRACT_ID,
        "status": "PASS" if all(checks.values()) else "FAIL",
        "counts": counts,
        "checks": checks,
        "positive_compile": positive_compile,
        "negative_compile": negative_compile,
        "required_ir_tokens": ir_tokens,
        "runtime_helper_ir_declarations": helper_ir_declarations,
        "source_checks": source_checks,
        "durable_replay_dirs": durable_replay_dirs,
        "stress_manifest": stress_manifest,
        "conformance": conformance,
        "scratch_directory": rel(SCRATCH),
        "scratch_is_not_source_truth": True,
    }
