from __future__ import annotations

import json

from objc3c_runtime_backed_semantics_closure.inputs import DURABLE_REPLAY_DIRS
from objc3c_runtime_backed_semantics_closure.inputs import HELPER_SYMBOLS
from objc3c_runtime_backed_semantics_closure.inputs import POSITIVE_FIXTURES
from objc3c_runtime_backed_semantics_closure.inputs import SOURCE_TOKENS
from objc3c_runtime_backed_semantics_closure.paths import CONFORMANCE_MANIFEST
from objc3c_runtime_backed_semantics_closure.paths import CONFORMANCE_NEGATIVE
from objc3c_runtime_backed_semantics_closure.paths import CONFORMANCE_POSITIVE
from objc3c_runtime_backed_semantics_closure.paths import CONFORMANCE_README
from objc3c_runtime_backed_semantics_closure.paths import DURABLE_REPLAY_ROOT
from objc3c_runtime_backed_semantics_closure.paths import RUNTIME
from objc3c_runtime_backed_semantics_closure.paths import STRESS_MANIFEST
from objc3c_runtime_backed_semantics_closure.paths import read
from objc3c_runtime_backed_semantics_closure.paths import rel


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
