from __future__ import annotations

from copy import deepcopy
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_runtime_import_interop_bridge_metadata import (  # noqa: E402
    BRIDGE_MEMBER,
    validate_import_surface,
)
from objc3c_shared.json_io import load_json_object  # noqa: E402

FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "runtime_import_interop_bridge_metadata"
CHECKER = ROOT / "scripts" / "check_runtime_import_interop_bridge_metadata.py"


def _run_checker(fixture_name: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(CHECKER), str(FIXTURE_ROOT / fixture_name)],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )


def test_runtime_import_interop_bridge_metadata_accepts_canonical_surface() -> None:
    result = _run_checker("valid_bridge_surface.json")

    assert result.returncode == 0
    assert result.stderr == ""


def test_runtime_import_interop_bridge_metadata_rejects_tampered_surface() -> None:
    result = _run_checker("tampered_bridge_surface.json")

    assert result.returncode == 1
    assert "header artifact path traverses directories" in result.stderr
    assert "active bridge packet has no C++ or Swift-facing metadata" in result.stderr


def test_runtime_import_interop_bridge_metadata_derives_callable_count_from_surfaces() -> None:
    payload = deepcopy(load_json_object(FIXTURE_ROOT / "valid_bridge_surface.json"))
    bridge = payload[BRIDGE_MEMBER]
    assert isinstance(bridge, dict)
    bridge["local_foreign_callable_count"] = 99

    failures = validate_import_surface(payload)

    assert "active bridge packet callable count is not derived from bridge surfaces" in failures


def test_runtime_import_interop_bridge_metadata_rejects_fallback_topology_promotion() -> None:
    payload = deepcopy(load_json_object(FIXTURE_ROOT / "valid_bridge_surface.json"))
    bridge = payload[BRIDGE_MEMBER]
    assert isinstance(bridge, dict)
    unsupported = bridge["unsupported_topologies"]
    assert isinstance(unsupported, list)
    unsupported[0]["fallback_allowed"] = True
    unsupported[1]["public_state"] = "reserved"

    failures = validate_import_surface(payload)

    assert "objc2-source-compatibility unsupported topology allows fallback" in failures
    assert "swift-full-abi-callable-import unsupported topology must be rejected" in failures


def test_runtime_import_interop_bridge_metadata_rejects_duplicate_unsupported_topology() -> None:
    payload = deepcopy(load_json_object(FIXTURE_ROOT / "valid_bridge_surface.json"))
    bridge = payload[BRIDGE_MEMBER]
    assert isinstance(bridge, dict)
    unsupported = bridge["unsupported_topologies"]
    assert isinstance(unsupported, list)
    unsupported.append(deepcopy(unsupported[0]))

    failures = validate_import_surface(payload)

    assert "active bridge packet has duplicated unsupported topology rejection metadata" in failures
