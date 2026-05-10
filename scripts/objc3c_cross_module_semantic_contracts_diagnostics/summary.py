from __future__ import annotations

from typing import Any

from objc3c_cross_module_semantic_contracts_diagnostics.catalog import CONTRACT_ID
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import ISSUE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import LANDED_FLAGS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import NEGATIVE_FIXTURE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import POSITIVE_FIXTURE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import POSITIVE_MIN_COUNTS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import REPLAY_SEGMENTS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SOURCE_TRUTH_PATHS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import VALIDATION_COMMANDS
from objc3c_cross_module_semantic_contracts_diagnostics.checks import compile_contract_checks
from objc3c_cross_module_semantic_contracts_diagnostics.checks import compile_static_presence
from objc3c_cross_module_semantic_contracts_diagnostics.diagnostics import find_model
from objc3c_cross_module_semantic_contracts_diagnostics.execution import run_compiler
from objc3c_cross_module_semantic_contracts_diagnostics.paths import TMP_ROOT
from objc3c_cross_module_semantic_contracts_diagnostics.paths import rel


def build_summary() -> dict[str, Any]:
    positive_run = run_compiler(POSITIVE_FIXTURE, TMP_ROOT / "positive")
    negative_run = run_compiler(NEGATIVE_FIXTURE, TMP_ROOT / "negative-duplicate-module")
    model = find_model(positive_run.get("manifest")) if positive_run.get("manifest") else None
    replay_key = str((model or {}).get("replay_key", ""))
    static_presence = compile_static_presence()
    checks = compile_contract_checks(
        model=model,
        replay_key=replay_key,
        positive_run=positive_run,
        negative_run=negative_run,
        static_presence=static_presence,
    )
    status = "PASS" if all(checks.values()) else "FAIL"
    return {
        "contract_id": CONTRACT_ID,
        "issue": ISSUE,
        "status": status,
        "checks": checks,
        "static_presence": static_presence,
        "source_truth_paths": [rel(path) for path in SOURCE_TRUTH_PATHS],
        "positive_fixture": rel(POSITIVE_FIXTURE),
        "negative_fixture": rel(NEGATIVE_FIXTURE),
        "positive_compile": {key: value for key, value in positive_run.items() if key != "manifest"},
        "negative_compile": {key: value for key, value in negative_run.items() if key != "manifest"},
        "cross_module_semantic_contracts_diagnostics_model": model,
        "positive_minimum_counts": POSITIVE_MIN_COUNTS,
        "landed_flags": LANDED_FLAGS,
        "required_replay_key_segments": REPLAY_SEGMENTS,
        "validation_commands": VALIDATION_COMMANDS,
    }
