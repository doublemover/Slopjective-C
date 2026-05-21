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
    REQUIRED_CHECKED_SOURCE_PROOFS,
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
    assert summary["checked_source_proof_count"] == len(REQUIRED_CHECKED_SOURCE_PROOFS)
    assert all(summary["native_checks"].values())


def test_module_interop_requires_checked_source_proofs_for_all_fail_closed_cases() -> None:
    for proof_case in REQUIRED_CHECKED_SOURCE_PROOFS:
        payload = deepcopy(_contract())
        proofs = payload["checked_source_proofs"]
        assert isinstance(proofs, list)
        payload["checked_source_proofs"] = [
            proof for proof in proofs if proof["case"] != proof_case
        ]

        failures, _ = validate_contract_payload(payload)

        assert f"checked source proofs are incomplete: {proof_case}" in failures


def test_module_interop_rejects_checked_source_proof_digest_drift() -> None:
    payload = deepcopy(_contract())
    proofs = payload["checked_source_proofs"]
    assert isinstance(proofs, list)
    proofs[0]["source_sha256"] = "0" * 64

    failures, _ = validate_contract_payload(payload)

    assert (
        "checked source proof digest drifted for missing-module: "
        "native/objc3c/src/pipeline/objc3_module_interop_contract_surface.cpp"
        in failures
    )


def test_module_interop_rejects_checked_source_proof_fragment_drift() -> None:
    payload = deepcopy(_contract())
    proofs = payload["checked_source_proofs"]
    assert isinstance(proofs, list)
    proofs[0]["fragments"] = ["missing-module", "not-present-in-source"]

    failures, _ = validate_contract_payload(payload)

    assert (
        "checked source proof fragments missing for missing-module: "
        "native/objc3c/src/pipeline/objc3_module_interop_contract_surface.cpp: "
        "not-present-in-source"
        in failures
    )


def test_module_interop_rejects_checked_source_proof_diagnostic_drift() -> None:
    payload = deepcopy(_contract())
    proofs = payload["checked_source_proofs"]
    assert isinstance(proofs, list)
    for proof in proofs:
        if proof["case"] == "abi-mismatch":
            proof["diagnostic"] = "O3MOD8163"

    failures, _ = validate_contract_payload(payload)

    assert "checked source proof diagnostic drifted for abi-mismatch" in failures


def test_module_interop_rebuild_key_rejects_import_version_drift() -> None:
    payload = deepcopy(_contract())
    imports = payload["imports"]
    assert isinstance(imports, list)
    imports[0]["metadata_version"] = "2.0.0"

    failures, _ = validate_contract_payload(payload)

    assert replay_key_for_contract(payload) != payload["incremental_rebuild"]["replay_key"]
    assert "deterministic rebuild replay key drifted" in failures


def test_module_interop_rebuild_key_rejects_bridge_metadata_digest_drift() -> None:
    payload = deepcopy(_contract())
    interop = payload["interop"]
    assert isinstance(interop, dict)
    interop["bridge_metadata_digest"] = "0" * 64

    failures, _ = validate_contract_payload(payload)

    assert (
        replay_key_for_contract(payload)
        != payload["incremental_rebuild"]["replay_key"]
    )
    assert "deterministic rebuild replay key drifted" in failures


def test_module_interop_requires_complete_rebuild_invalidation_cases() -> None:
    payload = deepcopy(_contract())
    rebuild = payload["incremental_rebuild"]
    assert isinstance(rebuild, dict)
    rebuild["invalidation_cases"] = [
        case
        for case in rebuild["invalidation_cases"]
        if case["condition"] != "visibility-surface-drift"
    ]

    failures, _ = validate_contract_payload(payload)

    assert "incremental rebuild invalidation cases are incomplete" in failures


def test_module_interop_rejects_invalidation_diagnostic_drift() -> None:
    payload = deepcopy(_contract())
    rebuild = payload["incremental_rebuild"]
    assert isinstance(rebuild, dict)
    for case in rebuild["invalidation_cases"]:
        if case["condition"] == "imported-module-abi-identity-drift":
            case["diagnostic"] = "O3MOD8163"

    failures, _ = validate_contract_payload(payload)

    assert (
        "incremental rebuild invalidation diagnostic drifted for imported-module-abi-identity-drift"
        in failures
    )


def test_module_interop_rejects_invalidation_scope_drift() -> None:
    payload = deepcopy(_contract())
    rebuild = payload["incremental_rebuild"]
    assert isinstance(rebuild, dict)
    for case in rebuild["invalidation_cases"]:
        if case["condition"] == "bridge-metadata-digest-drift":
            case["rebuild_affects"] = ["semantic"]

    failures, _ = validate_contract_payload(payload)

    assert (
        "incremental rebuild invalidation scope drifted for bridge-metadata-digest-drift"
        in failures
    )


def test_module_interop_bridge_digest_proof_requires_rebuild_invalidation() -> None:
    payload = deepcopy(_contract())
    rebuild = payload["incremental_rebuild"]
    assert isinstance(rebuild, dict)
    rebuild["invalidation_cases"] = [
        case
        for case in rebuild["invalidation_cases"]
        if case["condition"] != "bridge-metadata-digest-drift"
    ]

    failures, _ = validate_contract_payload(payload)

    assert (
        "checked source bridge digest proof is not wired to bridge metadata invalidation"
        in failures
    )


def test_module_interop_rejects_mixed_image_loader_metadata_digest_drift() -> None:
    payload = deepcopy(_contract())
    package_metadata = payload["package_metadata"]
    assert isinstance(package_metadata, dict)
    package_metadata["mixed_image_loader_metadata_digest"] = "0" * 64

    failures, _ = validate_contract_payload(payload)

    assert "mixed image loader metadata digest drifted" in failures
    assert "deterministic rebuild replay key drifted" in failures


def test_module_interop_rejects_abi_mismatch_diagnostic_drift() -> None:
    payload = deepcopy(_contract())
    rebuild = payload["incremental_rebuild"]
    assert isinstance(rebuild, dict)
    rebuild["abi_mismatch_diagnostic"] = "O3INT8165"

    failures, _ = validate_contract_payload(payload)

    assert "incremental rebuild ABI mismatch diagnostic drifted" in failures


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


def test_module_interop_rejects_private_reexport_edge() -> None:
    payload = deepcopy(_contract())
    imports = payload["imports"]
    assert isinstance(imports, list)
    imports[1]["reexport"] = True
    graph = payload["dependency_graph"]
    assert isinstance(graph, dict)
    graph["edges"][1]["reexport"] = True
    graph["reexported_modules"].append("FoundationPrivateShims")

    failures, _ = validate_contract_payload(payload)

    assert "reexported imports must be public" in failures


def test_module_interop_rejects_duplicate_export_across_imports() -> None:
    payload = deepcopy(_contract())
    imports = payload["imports"]
    assert isinstance(imports, list)
    imports[2]["exported_symbols"] = ["FNCoreObject"]

    failures, _ = validate_contract_payload(payload)

    assert "exported symbols must be unique across module and imports" in failures


def test_module_interop_rejects_hidden_import_access_becoming_public() -> None:
    payload = deepcopy(_contract())
    access_cases = payload["visibility_access_cases"]
    assert isinstance(access_cases, list)
    for case in access_cases:
        if case["symbol"] == "FNPrivateBridgeShim":
            case["allowed"] = True
            case.pop("diagnostic")

    failures, _ = validate_contract_payload(payload)

    assert "visibility access allowance drifted for FNPrivateBridgeShim" in failures
    assert "hidden access case must fail closed with O3MOD8165 for FNPrivateBridgeShim" in failures


def test_module_interop_private_reexport_proof_requires_hidden_import_rejection() -> None:
    payload = deepcopy(_contract())
    access_cases = payload["visibility_access_cases"]
    assert isinstance(access_cases, list)
    payload["visibility_access_cases"] = [
        case
        for case in access_cases
        if case["provided_by"] != "FoundationPrivateShims"
    ]

    failures, _ = validate_contract_payload(payload)

    assert (
        "checked source private reexport proof has no hidden/private import rejection case"
        in failures
    )


def test_module_interop_rejects_missing_module_graph_diagnostic() -> None:
    payload = deepcopy(_contract())
    graph = payload["dependency_graph"]
    assert isinstance(graph, dict)
    graph["diagnostics"] = [
        entry for entry in graph["diagnostics"] if entry["case"] != "import-cycle"
    ]

    failures, _ = validate_contract_payload(payload)

    assert (
        "dependency graph diagnostics must cover missing modules, cycles, stale metadata, duplicate exports, hidden declarations, and ABI mismatch"
        in failures
    )


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
