from __future__ import annotations

from copy import deepcopy
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_module_abi_interop_completion_contract import (  # noqa: E402
    CONTRACT_PATH,
    build_summary,
    validate_contract_payload,
)
from objc3c_shared.json_io import load_json_object  # noqa: E402


def _contract() -> dict[str, object]:
    return load_json_object(CONTRACT_PATH)


def test_module_abi_interop_completion_contract_passes() -> None:
    summary = build_summary()

    assert summary["status"] == "PASS"
    assert summary["issues"] == ["#8163", "#8165", "#8173"]
    assert summary["source_anchor_count"] == 11
    assert summary["module"]["module_name"] == "FoundationNext.VisibilityBridge"
    assert summary["module"]["public_reexport_count"] == 1
    assert summary["module"]["private_import_count"] == 1
    assert summary["module"]["hidden_rejection_count"] == 2
    assert summary["interop"]["runtime_bridge_surface_count"] == 4
    assert summary["interop"]["package_bridge_surface_count"] == 4
    assert summary["interop"]["package_mixed_image_count"] == 3
    assert summary["abi_governance"]["compatibility_case_count"] == 4
    assert "O3ABI8173" in summary["diagnostic_codes"]


def test_module_abi_interop_rejects_stale_source_anchor_digest() -> None:
    payload = deepcopy(_contract())
    anchor = payload["source_anchors"][0]  # type: ignore[index]
    anchor["sha256"] = "0" * 64

    failures, _ = validate_contract_payload(payload)

    assert "source anchor digest drifted for native/objc3c/src/pipeline/objc3_module_interop_contract_surface.h" in failures


def test_module_abi_interop_rejects_missing_source_fragment() -> None:
    payload = deepcopy(_contract())
    anchor = payload["source_anchors"][1]  # type: ignore[index]
    anchor["fragments"] = ["BuildObjc3ModuleInteropRebuildKey", "compatibility shim fallback"]

    failures, _ = validate_contract_payload(payload)

    assert (
        "source anchor fragments missing for native/objc3c/src/pipeline/objc3_module_interop_contract_surface.cpp: compatibility shim fallback"
        in failures
    )


def test_module_abi_interop_rejects_hidden_visibility_boundary_drift() -> None:
    payload = deepcopy(_contract())
    module = payload["module_semantics"]  # type: ignore[index]
    module["required_private_imports"] = []

    failures, _ = validate_contract_payload(payload)

    assert "module private import boundary drifted" in failures


def test_module_abi_interop_rejects_rebuild_key_digest_drift() -> None:
    payload = deepcopy(_contract())
    module = payload["module_semantics"]  # type: ignore[index]
    module["rebuild_key_sha256"] = "0" * 64

    failures, _ = validate_contract_payload(payload)

    assert "deterministic rebuild key digest drifted" in failures


def test_module_abi_interop_rejects_reserved_foreign_lane_promotion() -> None:
    payload = deepcopy(_contract())
    interop = payload["interop_metadata"]  # type: ignore[index]
    interop["required_reserved_languages"] = ["cpp"]
    interop["required_supported_languages"] = ["c", "objc2", "swift"]

    failures, _ = validate_contract_payload(payload)

    assert "module interop supported-language boundary drifted" in failures
    assert "module interop reserved-language boundary drifted" in failures


def test_module_abi_interop_rejects_missing_unsupported_runtime_topology() -> None:
    payload = deepcopy(_contract())
    interop = payload["interop_metadata"]  # type: ignore[index]
    interop["required_unsupported_topologies"] = [
        "objc2-source-compatibility",
        "swift-full-abi-callable-import",
        "cpp-template-instantiation-import",
        "unchecked-abi-alignment-fallback",
        "foreign-abi-autoload-fallback",
    ]

    failures, _ = validate_contract_payload(payload)

    assert "runtime unsupported topology rejections are incomplete: foreign-abi-autoload-fallback" in failures


def test_module_abi_interop_rejects_ungoverned_abi_transition() -> None:
    payload = deepcopy(_contract())
    abi = payload["abi_governance"]  # type: ignore[index]
    abi["required_compatibility_transitions"] = [
        "public-symbol-removal-without-deprecation-window",
        "signature-change-without-major-line",
        "package-abi-identity-drift",
        "unsupported-downgrade-route",
        "silent-layout-change-without-review",
    ]

    failures, _ = validate_contract_payload(payload)

    assert "ABI governance compatibility cases lost required transitions" in failures


def test_module_abi_interop_completion_contract_cli_accepts_default_fixture() -> None:
    result = subprocess.run(
        [
            sys.executable,
            str(ROOT / "scripts" / "check_module_abi_interop_completion_contract.py"),
        ],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )

    assert result.returncode == 0
    assert "objc3c-module-abi-interop-completion-contract: PASS" in result.stdout
    assert result.stderr == ""
