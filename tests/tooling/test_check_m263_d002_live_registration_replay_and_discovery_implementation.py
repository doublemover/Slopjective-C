from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MODULE_PATH = (
    ROOT / "scripts" / "check_m263_d002_live_registration_replay_and_discovery_implementation.py"
)
SPEC = importlib.util.spec_from_file_location("m263_d002_checker", MODULE_PATH)
assert SPEC and SPEC.loader
MODULE = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = MODULE
SPEC.loader.exec_module(MODULE)


def test_models_and_cases_are_wired() -> None:
    assert (
        MODULE.CONTRACT_ID
        == "objc3c-runtime-live-registration-discovery-replay/m263-d002-v1"
    )
    assert MODULE.REPLAY_REGISTERED_IMAGES_SYMBOL == (
        "objc3_runtime_replay_registered_images_for_testing"
    )
    assert MODULE.RESET_REPLAY_SNAPSHOT_SYMBOL == (
        "objc3_runtime_copy_reset_replay_state_for_testing"
    )
    assert len(MODULE.DYNAMIC_CASES) == 2


def test_summary_path_is_under_tmp_reports() -> None:
    assert str(MODULE.DEFAULT_SUMMARY_OUT).replace("\\", "/").startswith(
        "tmp/reports/m263/M263-D002/"
    )


def test_snippet_ids_are_unique() -> None:
    groups = (
        MODULE.EXPECTATIONS_SNIPPETS,
        MODULE.PACKET_SNIPPETS,
        MODULE.NATIVE_DOC_SNIPPETS,
        MODULE.LOWERING_SPEC_SNIPPETS,
        MODULE.METADATA_SPEC_SNIPPETS,
        MODULE.INTERNAL_HEADER_SNIPPETS,
        MODULE.RUNTIME_SOURCE_SNIPPETS,
        MODULE.PROCESS_CPP_SNIPPETS,
        MODULE.PROBE_SNIPPETS,
        MODULE.PACKAGE_SNIPPETS,
    )
    seen: set[str] = set()
    for group in groups:
        for snippet in group:
            assert snippet.check_id not in seen
            seen.add(snippet.check_id)
