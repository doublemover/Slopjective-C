from __future__ import annotations

from copy import deepcopy
from pathlib import Path
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_objc3c_interop_surface_policy import (  # noqa: E402
    POLICY_PATH,
    RESERVED_LANE_IDS,
    SUPPORTED_LANE_IDS,
    validate_policy_payload,
)
from objc3c_shared.json_io import load_json_object  # noqa: E402


def _policy() -> dict[str, object]:
    return load_json_object(POLICY_PATH)


def _lane(payload: dict[str, object], lane_id: str) -> dict[str, object]:
    lanes = payload["lanes"]
    assert isinstance(lanes, list)
    for lane in lanes:
        assert isinstance(lane, dict)
        if lane["lane_id"] == lane_id:
            return lane
    raise AssertionError(f"missing lane {lane_id}")


def test_interop_surface_policy_accepts_truthful_slice() -> None:
    payload = _policy()

    failures = validate_policy_payload(payload)

    assert failures == []
    states = {
        lane["lane_id"]: lane["public_state"]
        for lane in payload["lanes"]
        if isinstance(lane, dict)
    }
    assert {lane_id for lane_id, state in states.items() if state == "supported"} == SUPPORTED_LANE_IDS
    assert {lane_id for lane_id, state in states.items() if state == "reserved"} == RESERVED_LANE_IDS


def test_interop_surface_policy_rejects_swift_full_abi_promotion() -> None:
    payload = deepcopy(_policy())
    lane = _lane(payload, "swift.full-abi-callable-import")
    lane["public_state"] = "supported"
    lane["support_claim"] = "objc3c.behavior.runtime.interop.swift-full-abi"
    lane["public_commands"] = ["npm run objc3c -- validate-interop-conformance"]
    lane["evidence_anchors"] = [
        "tests/tooling/fixtures/native/cpp_swift_annotation_positive.objc3"
    ]

    failures = validate_policy_payload(payload)

    assert "swift.full-abi-callable-import must remain reserved" in failures
    assert "Swift full ABI callable import must stay reserved" in failures


def test_interop_surface_policy_rejects_objc2_source_compatibility() -> None:
    payload = deepcopy(_policy())
    lane = _lane(payload, "objc2.retired-source-compatibility")
    lane["public_state"] = "supported"
    lane["support_claim"] = "objc3c.behavior.language.interop.objc2-source-compatible"
    lane["public_commands"] = ["npm run objc3c -- validate-migration-workflow"]

    failures = validate_policy_payload(payload)

    assert "objc2.retired-source-compatibility must remain rejected" in failures
    assert "Objective-C 2 source compatibility must stay rejected" in failures


def test_interop_surface_policy_rejects_supported_lane_without_evidence() -> None:
    payload = deepcopy(_policy())
    lane = _lane(payload, "cpp.annotation-metadata")
    lane["evidence_anchors"] = []
    lane["public_commands"] = []

    failures = validate_policy_payload(payload)

    assert "cpp.annotation-metadata supported lane needs replayable public command evidence" in failures
    assert "cpp.annotation-metadata supported lane needs checked evidence anchors" in failures


def test_interop_surface_policy_rejects_tamper_fixture_drift() -> None:
    payload = deepcopy(_policy())
    bridge_policy = payload["bridge_metadata_policy"]
    assert isinstance(bridge_policy, dict)
    bridge_policy["tampered_runtime_import_surface_fixture"] = bridge_policy["runtime_import_surface_fixture"]

    failures = validate_policy_payload(payload)

    assert any(
        failure.startswith("tampered bridge fixture no longer proves fail-closed message:")
        for failure in failures
    )


def test_interop_surface_policy_cli_accepts_default_fixture() -> None:
    result = subprocess.run(
        [sys.executable, str(ROOT / "scripts" / "check_objc3c_interop_surface_policy.py")],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )

    assert result.returncode == 0
    assert "objc3c-interop-surface-policy: PASS" in result.stdout
    assert result.stderr == ""
