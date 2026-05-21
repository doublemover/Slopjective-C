from __future__ import annotations

from copy import deepcopy
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_objc3c_module_interop_contracts import (  # noqa: E402
    CONTRACT_PATH,
    PUBLIC_COMMAND,
    REQUIRED_LANGUAGES,
    build_summary,
    replay_key_for_contract,
    validate_contract_payload,
)
from objc3c_shared.json_io import load_json_object  # noqa: E402


def _contract() -> dict[str, object]:
    return load_json_object(CONTRACT_PATH)


def test_module_interop_contract_summary_passes() -> None:
    summary = build_summary()

    assert summary["status"] == "PASS"
    assert summary["issues"] == [8163, 8165]
    assert summary["public_command"] == PUBLIC_COMMAND
    assert summary["foreign_languages"] == sorted(REQUIRED_LANGUAGES)
    assert summary["bridge_surface_count"] == 4
    assert summary["supported_bridge_surface_count"] == 2
    assert summary["reserved_bridge_surface_count"] == 2
    assert all(summary["native_checks"].values())


def test_module_interop_rebuild_key_rejects_import_version_drift() -> None:
    payload = deepcopy(_contract())
    imports = payload["imports"]
    assert isinstance(imports, list)
    imports[0]["metadata_version"] = "2.0.0"

    failures, _ = validate_contract_payload(payload)

    assert replay_key_for_contract(payload) != payload["incremental_rebuild"]["replay_key"]
    assert "deterministic rebuild replay key drifted" in failures


def test_module_interop_rejects_missing_reserved_swift_bridge_surface() -> None:
    payload = deepcopy(_contract())
    interop = payload["interop"]
    assert isinstance(interop, dict)
    interop["foreign_surfaces"] = [
        surface
        for surface in interop["foreign_surfaces"]
        if surface["language"] != "swift"
    ]

    failures, _ = validate_contract_payload(payload)

    assert "foreign surfaces must cover C, ObjC2, Swift, and C++ exactly" in failures
    assert "Swift and C++ interop lanes must remain reserved until executable proof lands" in failures
    assert "deterministic rebuild replay key drifted" in failures


def test_module_interop_rejects_unsupported_lane_claimed_without_evidence() -> None:
    payload = deepcopy(_contract())
    interop = payload["interop"]
    assert isinstance(interop, dict)
    for surface in interop["foreign_surfaces"]:
        if surface["language"] == "swift":
            surface["supported"] = True
            surface["support_state"] = "supported"

    failures, _ = validate_contract_payload(payload)

    assert "only C and ObjC2 interop lanes may be supported by current evidence" in failures
    assert "Swift and C++ interop lanes must remain reserved until executable proof lands" in failures


def test_module_interop_rejects_objc2_source_compatibility_acceptance() -> None:
    payload = deepcopy(_contract())
    interop = payload["interop"]
    assert isinstance(interop, dict)
    for surface in interop["foreign_surfaces"]:
        if surface["language"] == "objc2":
            surface["source_syntax_policy"] = "objc2-source-compatible"

    failures, _ = validate_contract_payload(payload)

    assert "ObjC2 bridge must be metadata-only and reject retired syntax" in failures


def test_module_interop_contract_cli_accepts_default_fixture() -> None:
    result = subprocess.run(
        [sys.executable, str(ROOT / "scripts" / "check_objc3c_module_interop_contracts.py")],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )

    assert result.returncode == 0
    assert "objc3c-module-interop-contracts: PASS" in result.stdout
    assert result.stderr == ""
