from __future__ import annotations

from typing import Any

from objc3c_effects_ownership_semantic_model.compiler import find_effects_model
from objc3c_effects_ownership_semantic_model.compiler import run_compiler
from objc3c_effects_ownership_semantic_model.contracts import CONTRACT_ID
from objc3c_effects_ownership_semantic_model.contracts import ISSUE
from objc3c_effects_ownership_semantic_model.contracts import LANDED_FLAGS
from objc3c_effects_ownership_semantic_model.contracts import POSITIVE_MIN_COUNTS
from objc3c_effects_ownership_semantic_model.contracts import RUNTIME_REPLAY_SEGMENTS
from objc3c_effects_ownership_semantic_model.contracts import VALIDATION_COMMANDS
from objc3c_effects_ownership_semantic_model.inputs import build_static_presence
from objc3c_effects_ownership_semantic_model.inputs import load_semantic_inputs
from objc3c_effects_ownership_semantic_model.inputs import source_truth_paths
from objc3c_effects_ownership_semantic_model.paths import NEGATIVE_FIXTURE
from objc3c_effects_ownership_semantic_model.paths import POSITIVE_FIXTURE
from objc3c_effects_ownership_semantic_model.paths import TMP_ROOT
from objc3c_effects_ownership_semantic_model.paths import rel
from objc3c_effects_ownership_semantic_model.validation import build_checks


def build_summary() -> dict[str, Any]:
    positive_run = run_compiler(POSITIVE_FIXTURE, TMP_ROOT / "positive")
    negative_run = run_compiler(NEGATIVE_FIXTURE, TMP_ROOT / "negative-async-throws")
    model = find_effects_model(positive_run.get("manifest")) if positive_run.get("manifest") else None
    replay_key = str((model or {}).get("replay_key", ""))

    semantic_inputs = load_semantic_inputs()
    static_presence = build_static_presence()
    truth_paths = source_truth_paths()
    checks = build_checks(
        positive_run=positive_run,
        negative_run=negative_run,
        model=model,
        replay_key=replay_key,
        semantic_inputs=semantic_inputs,
        static_presence=static_presence,
        source_truth_paths=truth_paths,
    )
    status = "PASS" if all(checks.values()) else "FAIL"
    return {
        "contract_id": CONTRACT_ID,
        "issue": ISSUE,
        "status": status,
        "checks": checks,
        "static_presence": static_presence,
        "source_truth_paths": [rel(path) for path in truth_paths],
        "positive_fixture": rel(POSITIVE_FIXTURE),
        "negative_fixture": rel(NEGATIVE_FIXTURE),
        "positive_compile": {key: value for key, value in positive_run.items() if key != "manifest"},
        "negative_compile": {key: value for key, value in negative_run.items() if key != "manifest"},
        "effects_ownership_semantic_model": model,
        "positive_minimum_counts": POSITIVE_MIN_COUNTS,
        "landed_flags": LANDED_FLAGS,
        "required_replay_key_segments": RUNTIME_REPLAY_SEGMENTS,
        "validation_commands": VALIDATION_COMMANDS,
    }
