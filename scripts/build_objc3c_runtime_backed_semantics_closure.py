from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPORT_DIR = ROOT / "reports" / "claimability" / "runtime-backed-semantics-closure"
JSON_OUT = REPORT_DIR / "runtime_backed_semantics_closure_summary.json"
MD_OUT = REPORT_DIR / "runtime_backed_semantics_closure_summary.md"

CONTRACT_ID = "objc3c.runtime.backed.semantics.closure.v1"
ISSUE = "#8017"
COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
SCRATCH = ROOT / "tmp" / "artifacts" / "objc3c-native" / "runtime-backed-semantics-closure"

LOWERING_CONTRACT_H = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h"
LOWERING_CONTRACT_CPP = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
SEMA_PASS_MANAGER = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
STATIC_ANALYSIS = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_static_analysis.cpp"
RUNTIME = ROOT / "native" / "objc3c" / "src" / "runtime" / "objc3_runtime.cpp"
STRESS_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "lowering_runtime_stress_manifest.json"
CONFORMANCE_MANIFEST = ROOT / "tests" / "conformance" / "lowering_abi" / "manifest.json"
CONFORMANCE_README = ROOT / "tests" / "conformance" / "lowering_abi" / "README.md"
CONFORMANCE_POSITIVE = ROOT / "tests" / "conformance" / "lowering_abi" / "RTBACK-8017-01.json"
CONFORMANCE_NEGATIVE = ROOT / "tests" / "conformance" / "lowering_abi" / "RTBACK-8017-02.json"
DURABLE_REPLAY_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "objc3c"

POSITIVE_FIXTURES = {
    "block_arc_autorelease_return": ROOT / "tests" / "tooling" / "fixtures" / "native" / "arc_block_autorelease_return_positive.objc3",
    "arc_weak_autoreleasepool": ROOT / "tests" / "tooling" / "fixtures" / "native" / "reference_counting_weak_autoreleasepool_positive.objc3",
    "error_runtime_bridge_helper": ROOT / "tests" / "tooling" / "fixtures" / "native" / "error_runtime_bridge_helper_positive.objc3",
    "live_error_runtime": ROOT / "tests" / "tooling" / "fixtures" / "native" / "live_error_runtime_integration_positive.objc3",
    "live_continuation_runtime": ROOT / "tests" / "tooling" / "fixtures" / "native" / "live_continuation_runtime_integration_positive.objc3",
    "live_task_runtime": ROOT / "tests" / "tooling" / "fixtures" / "native" / "live_task_runtime_and_executor_implementation_positive.objc3",
    "live_actor_mailbox_runtime": ROOT / "tests" / "tooling" / "fixtures" / "native" / "live_actor_mailbox_runtime_positive.objc3",
}

NEGATIVE_FIXTURES = {
    "escaping_block_actor_handoff": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "escaping_block_with_handoff_rejected.objc3",
        "codes": ["O3S294"],
    },
    "throw_without_throws_or_catch": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "throw_requires_throws_or_catch_negative.objc3",
        "codes": ["O3S274"],
    },
    "task_group_without_scope": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "task_group_without_scope_rejected.objc3",
        "codes": ["O3S228"],
    },
    "actor_hop_without_async": {
        "path": ROOT / "tests" / "tooling" / "fixtures" / "native" / "actor_hop_without_async_rejected.objc3",
        "codes": ["O3S289"],
    },
}

DURABLE_REPLAY_DIRS = [
    "validation_block_literal_capture_contract",
    "validation_block_abi_invoke_trampoline_contract",
    "validation_block_storage_escape_contract",
    "validation_block_copy_dispose_contract",
    "validation_arc_diagnostics_fixit_contract",
    "validation_error_diagnostics_recovery_contract",
    "validation_ns_error_bridging_contract",
    "validation_result_like_lowering_contract",
    "validation_async_continuation_contract",
    "validation_await_lowering_suspension_state_contract",
    "validation_actor_isolation_sendability_contract",
    "validation_task_runtime_interop_cancellation_contract",
    "validation_concurrency_replay_contract",
]

HELPER_SYMBOLS = [
    "objc3_runtime_read_current_property_i32",
    "objc3_runtime_write_current_property_i32",
    "objc3_runtime_exchange_current_property_i32",
    "objc3_runtime_store_thrown_error_i32",
    "objc3_runtime_load_thrown_error_i32",
    "objc3_runtime_bridge_status_error_i32",
    "objc3_runtime_bridge_nserror_error_i32",
    "objc3_runtime_catch_matches_error_i32",
    "objc3_runtime_allocate_async_continuation_i32",
    "objc3_runtime_handoff_async_continuation_to_executor_i32",
    "objc3_runtime_resume_async_continuation_i32",
    "objc3_runtime_spawn_task_i32",
    "objc3_runtime_enter_task_group_scope_i32",
    "objc3_runtime_add_task_group_task_i32",
    "objc3_runtime_wait_task_group_next_i32",
    "objc3_runtime_cancel_task_group_i32",
    "objc3_runtime_task_is_cancelled_i32",
    "objc3_runtime_task_on_cancel_i32",
    "objc3_runtime_executor_hop_i32",
    "objc3_runtime_actor_enter_isolation_thunk_i32",
    "objc3_runtime_actor_enter_nonisolated_i32",
    "objc3_runtime_actor_hop_to_executor_i32",
    "objc3_runtime_actor_record_replay_proof_i32",
    "objc3_runtime_actor_record_race_guard_i32",
    "objc3_runtime_actor_bind_executor_i32",
    "objc3_runtime_actor_mailbox_enqueue_i32",
    "objc3_runtime_actor_mailbox_drain_next_i32",
    "objc3_runtime_load_weak_current_property_i32",
    "objc3_runtime_store_weak_current_property_i32",
    "objc3_runtime_retain_i32",
    "objc3_runtime_release_i32",
    "objc3_runtime_autorelease_i32",
    "objc3_runtime_promote_block_i32",
    "objc3_runtime_invoke_block_i32",
    "objc3_runtime_push_autoreleasepool_scope",
    "objc3_runtime_pop_autoreleasepool_scope",
]

REQUIRED_IR_TOKENS = [
    "runtime_backed_semantics_closure = contract=objc3c.runtime.backed.semantics.closure.v1",
    "runtime_block_byref_forwarding_heap_promotion_ownership_interop",
    "runtime_block_allocation_copy_dispose_invoke_support",
    "runnable_block_execution_matrix",
    "arc_automatic_insertions",
    "arc_cleanup_weak_lifetime_hooks",
    "arc_block_autorelease_return_lowering",
    "runnable_arc_closeout",
    "error_handling_error_runtime_bridge_helper",
    "error_handling_live_error_runtime_integration",
    "concurrency_live_continuation_runtime_integration",
    "concurrency_live_task_runtime_integration",
    "concurrency_task_runtime_hardening",
    "actor_lowering_metadata_contract",
    "concurrency_replay_race_guard_lowering",
]

SOURCE_TOKENS = {
    "lowering_contract_header": {
        LOWERING_CONTRACT_H: [
            "kObjc3RuntimeBackedSemanticsClosureContractId",
            "kObjc3RuntimeBackedSemanticsClosureBlockModel",
            "kObjc3RuntimeBackedSemanticsClosureArcModel",
            "kObjc3RuntimeBackedSemanticsClosureErrorModel",
            "kObjc3RuntimeBackedSemanticsClosureConcurrencyModel",
            "Objc3RuntimeBackedSemanticsClosureSummary",
        ],
    },
    "lowering_contract_cpp": {
        LOWERING_CONTRACT_CPP: [
            "Objc3RuntimeBackedSemanticsClosureSummary()",
            "runtime_helper_count=36",
            "kObjc3RuntimeBackedSemanticsClosureFailClosedModel",
        ],
    },
    "ir_emitter": {
        IR_EMITTER: [
            "runtime_backed_semantics_closure",
            "Objc3RuntimeBackedSemanticsClosureSummary()",
            "Objc3RunnableArcCloseoutSummary()",
        ],
    },
    "sema_pass_manager": {
        SEMA_PASS_MANAGER: [
            "IsEquivalentBlockStorageEscapeSemanticsSummary",
            "IsEquivalentBlockCopyDisposeSemanticsSummary",
            "IsEquivalentRetainReleaseOperationSummary",
            "IsEquivalentWeakUnownedSemanticsSummary",
            "IsEquivalentThrowsPropagationSummary",
            "IsEquivalentAsyncContinuationSummary",
            "IsEquivalentTaskRuntimeCancellationSummary",
            "IsEquivalentActorIsolationSendabilitySummary",
            "IsEquivalentConcurrencyReplayRaceGuardSummary",
        ],
    },
    "semantic_passes": {
        SEMANTIC_PASSES: [
            "ResolveGlobalInitializerValues",
            "ResolveBlockCaptureSemanticType",
            "BuildTaskRuntimeCancellationSummaryFromIntegrationSurface",
        ],
    },
    "static_analysis": {
        STATIC_ANALYSIS: [
            "BlockAlwaysReturns",
        ],
    },
}


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def run_compiler(source: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    return subprocess.run(
        [
            str(COMPILER),
            str(source),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )


def read_diagnostics(path: Path) -> list[dict]:
    if not path.is_file():
        return []
    return json.loads(read(path)).get("diagnostics", [])


def check_source_tokens() -> dict[str, dict[str, bool]]:
    result: dict[str, dict[str, bool]] = {}
    for group, file_tokens in SOURCE_TOKENS.items():
        group_checks: dict[str, bool] = {}
        for path, tokens in file_tokens.items():
            text = read(path)
            for token in tokens:
                group_checks[f"{rel(path)}::{token}"] = token in text
        result[group] = group_checks

    runtime_text = read(RUNTIME)
    result["runtime_helper_implementations"] = {
        symbol: symbol in runtime_text for symbol in HELPER_SYMBOLS
    }
    return result


def compile_positive_fixtures() -> tuple[dict[str, dict], str]:
    compile_results: dict[str, dict] = {}
    ir_chunks: list[str] = []
    for name, fixture in POSITIVE_FIXTURES.items():
        out_dir = SCRATCH / "positive" / name
        result = run_compiler(fixture, out_dir)
        ir_path = out_dir / "module.ll"
        manifest_path = out_dir / "module.manifest.json"
        ir_text = read(ir_path) if ir_path.is_file() else ""
        if ir_text:
            ir_chunks.append(ir_text)
        compile_results[name] = {
            "fixture": rel(fixture),
            "exit_code": result.returncode,
            "compiled": result.returncode == 0,
            "ir_exists": ir_path.is_file(),
            "manifest_exists": manifest_path.is_file(),
        }
    return compile_results, "\n".join(ir_chunks)


def compile_negative_fixtures() -> dict[str, dict]:
    compile_results: dict[str, dict] = {}
    for name, spec in NEGATIVE_FIXTURES.items():
        fixture = spec["path"]
        expected_codes = spec["codes"]
        out_dir = SCRATCH / "negative" / name
        result = run_compiler(fixture, out_dir)
        diagnostics = read_diagnostics(out_dir / "module.diagnostics.json")
        observed_codes = sorted({diagnostic.get("code", "") for diagnostic in diagnostics})
        compile_results[name] = {
            "fixture": rel(fixture),
            "exit_code": result.returncode,
            "rejected": result.returncode != 0,
            "expected_codes": expected_codes,
            "observed_codes": observed_codes,
            "expected_codes_present": all(code in observed_codes for code in expected_codes),
        }
    return compile_results


def check_durable_replay_dirs() -> dict[str, bool]:
    return {
        directory: (DURABLE_REPLAY_ROOT / directory).is_dir()
        for directory in DURABLE_REPLAY_DIRS
    }


def check_stress_manifest() -> dict[str, bool]:
    manifest = json.loads(read(STRESS_MANIFEST))
    compile_cases = set(manifest.get("compile_cases", []))
    return {
        "live_error_runtime_integration_positive": rel(POSITIVE_FIXTURES["live_error_runtime"]) in compile_cases,
        "live_continuation_runtime_integration_positive": rel(POSITIVE_FIXTURES["live_continuation_runtime"]) in compile_cases,
        "live_task_runtime_and_executor_implementation_positive": rel(POSITIVE_FIXTURES["live_task_runtime"]) in compile_cases,
        "arc_block_autorelease_return_positive": rel(POSITIVE_FIXTURES["block_arc_autorelease_return"]) in compile_cases,
    }


def check_conformance_manifest() -> dict[str, bool]:
    manifest = json.loads(read(CONFORMANCE_MANIFEST))
    files = {"RTBACK-8017-01.json", "RTBACK-8017-02.json"}
    return {
        "positive_fixture_exists": CONFORMANCE_POSITIVE.is_file(),
        "negative_fixture_exists": CONFORMANCE_NEGATIVE.is_file(),
        "manifest_references_rtback": any(
            set(group.get("files", [])) >= files
            for group in manifest.get("groups", [])
        ),
        "readme_references_rtback": "RTBACK-8017-01.json" in read(CONFORMANCE_README)
        and "RTBACK-8017-02.json" in read(CONFORMANCE_README),
    }


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


def render_markdown(summary: dict) -> str:
    counts = summary["counts"]
    lines = [
        "# Runtime-Backed Semantics Closure",
        "",
        f"- Issue: `{summary['issue']}`",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Positive executable fixtures: `{counts['positive_fixture_count']}`",
        f"- Negative fail-closed fixtures: `{counts['negative_fixture_count']}`",
        f"- Private runtime helper symbols: `{counts['runtime_helper_symbol_count']}`",
        f"- Durable replay fixture directories: `{counts['durable_replay_fixture_dir_count']}`",
        f"- Scratch output: `{summary['scratch_directory']}` (not source truth)",
        "",
        "## Checks",
        "",
    ]
    for name, value in summary["checks"].items():
        lines.append(f"- `{name}`: `{str(value).lower()}`")
    lines.extend(["", "## Fixture Coverage", ""])
    for name, item in summary["positive_compile"].items():
        lines.append(f"- `{name}`: `{item['fixture']}` compiled=`{str(item['compiled']).lower()}`")
    for name, item in summary["negative_compile"].items():
        lines.append(
            f"- `{name}`: `{item['fixture']}` rejected=`{str(item['rejected']).lower()}` "
            f"codes=`{','.join(item['observed_codes'])}`"
        )
    lines.extend(["", "## Runtime Helper Boundary", ""])
    for symbol, present in summary["runtime_helper_ir_declarations"].items():
        lines.append(f"- `{symbol}`: ir_declared=`{str(present).lower()}`")
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true", help="fail if committed report files are stale")
    args = parser.parse_args()

    summary = build_summary()
    json_text = json.dumps(summary, indent=2, sort_keys=True) + "\n"
    md_text = render_markdown(summary)

    if args.check:
        expected = {
            JSON_OUT: json_text,
            MD_OUT: md_text,
        }
        stale = [
            rel(path)
            for path, text in expected.items()
            if not path.is_file() or read(path) != text
        ]
        if stale:
            print("status: FAIL")
            print("stale reports:")
            for path in stale:
                print(f"- {path}")
            return 1
        print(f"status: {summary['status']}")
        return 0 if summary["status"] == "PASS" else 1

    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json_text, encoding="utf-8")
    MD_OUT.write_text(md_text, encoding="utf-8")
    print(f"status: {summary['status']}")
    print(f"wrote {rel(JSON_OUT)}")
    print(f"wrote {rel(MD_OUT)}")
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
