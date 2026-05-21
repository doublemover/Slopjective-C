from __future__ import annotations

import copy
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))
CASE_MATRIX = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "release_foundation"
    / "abi_api_governance_cases.json"
)

from objc3c_release_manifest import abi_api_drift
from objc3c_release_manifest.model import PackageAssembly, PayloadEntry
from objc3c_release_manifest.validate import validate_release_inputs
from objc3c_shared.json_io import load_json_object, write_json_file


def _governance() -> dict[str, object]:
    return load_json_object(abi_api_drift.GOVERNANCE_MANIFEST)


def _case(case_id: str) -> dict[str, object]:
    matrix = load_json_object(CASE_MATRIX)
    assert matrix["contract_id"] == "objc3c.release.foundation.abi_api_governance.case_matrix.v1"
    for section in ("positive_cases", "negative_cases"):
        for entry in matrix[section]:  # type: ignore[index]
            if entry["case_id"] == case_id:
                return entry
    raise AssertionError(f"missing ABI/API governance case {case_id}")


def _run_with_governance(
    tmp_path: Path,
    governance: dict[str, object],
) -> tuple[int, dict[str, object]]:
    governance_path = tmp_path / "abi_api_governance.json"
    summary_path = tmp_path / "abi-api-drift-summary.json"
    write_json_file(governance_path, governance, sort_keys=True)
    return abi_api_drift.run_check(
        governance_path=governance_path,
        summary_path=summary_path,
    )


def test_release_abi_api_drift_gate_accepts_checked_in_governance(tmp_path: Path) -> None:
    case = _case("canonical-governance")
    rc, summary = _run_with_governance(tmp_path, _governance())

    assert rc == 0
    assert summary["status"] == "PASS"
    assert summary["failure_count"] == 0
    assert summary["source_manifest_count"] == 7
    assert summary["public_api_symbol_count"] == case["expected_public_api_symbol_count"]
    assert summary["runtime_abi_symbol_count"] == case["expected_runtime_abi_symbol_count"]
    assert summary["frontend_c_api_symbol_count"] == case["expected_frontend_c_api_symbol_count"]
    assert summary["governed_symbol_surface_count"] == case["expected_governed_symbol_surface_count"]
    assert summary["compiler_artifact_schema_count"] == case["expected_compiler_artifact_schema_count"]
    assert summary["package_abi_identity"] == {
        "identity": "objc3-abi-2025Q4",
        "governance_manifest": "tests/tooling/fixtures/release_foundation/abi_api_governance.json",
        "governance_schema": "schemas/objc3c-abi-api-governance-v1.schema.json",
        "supported_major_line": 1,
        "current_minor_line": 0,
        "compatibility_class": "package-lockfile-abi",
        "stdlib_public_api_symbol_count": 83,
        "stdlib_runtime_abi_symbol_count": 36,
        "frontend_c_api_symbol_count": 33,
    }


def test_release_abi_api_drift_gate_rejects_runtime_surface_class_drift(tmp_path: Path) -> None:
    case = _case("runtime-surface-class-drift")
    governance = _governance()
    concurrency_runtime = next(
        entry
        for entry in governance["stdlib_runtime_abi"]  # type: ignore[index]
        if entry["module"] == "objc3.concurrency"
    )
    concurrency_runtime["compatibility_class"] = "stable-public-api"

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        case["expected_failure"] in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_unblocked_downgrade_route(
    tmp_path: Path,
) -> None:
    case = _case("unsupported-downgrade-route-not-release-blocking")
    governance = _governance()
    blocked = governance["allowed_transition_policy"]["release_blocked_transitions"]  # type: ignore[index]
    blocked.remove("unsupported-downgrade-route")

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        case["expected_failure"] in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_package_abi_identity_count_drift(
    tmp_path: Path,
) -> None:
    case = _case("package-abi-identity-count-drift")
    governance = _governance()
    identity = governance["package_abi_identity"]["lockfile_abi_identity"]  # type: ignore[index]
    identity["stdlib_runtime_abi_symbol_count"] = 35

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        case["expected_failure"] in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_package_lock_schema_abi_identity_drift(
    tmp_path: Path,
) -> None:
    case = _case("package-lock-schema-abi-identity-drift")
    governance = _governance()
    surfaces = governance["compiler_artifact_compatibility"]["artifact_schema_surfaces"]  # type: ignore[index]
    package_identity_surface = next(
        surface
        for surface in surfaces
        if surface["surface_id"] == "package-lockfile-abi-identity"
    )
    package_identity_surface["expected_identity"] = "objc3-abi-broken"

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        case["expected_failure"] in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_unreviewed_public_symbol_addition(
    tmp_path: Path,
) -> None:
    governance = _governance()
    core_surface = governance["stdlib_public_api"][0]  # type: ignore[index]
    del core_surface["symbols"]["objc3_core_array_count"]  # type: ignore[index]

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert summary["status"] == "FAIL"
    assert any(
        "unreviewed public-api symbol addition in objc3.core" in failure
        and "objc3_core_array_count" in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_missing_inventory_module_governance(
    tmp_path: Path,
) -> None:
    governance = _governance()
    governance["stdlib_public_api"] = [  # type: ignore[index]
        entry
        for entry in governance["stdlib_public_api"]  # type: ignore[index]
        if entry["module"] != "objc3.system"
    ]

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        "ABI/API governance missing stdlib_public_api modules: objc3.system"
        in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_live_frontend_header_drift(
    tmp_path: Path,
) -> None:
    governance = _governance()
    frontend_c_api = governance["frontend_c_api"]  # type: ignore[index]
    frontend_c_api["required_helpers"] = [  # type: ignore[index]
        *frontend_c_api["required_helpers"],  # type: ignore[index]
        "objc3c_frontend_c_missing_live_helper",
    ]

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        "frontend C API live headers missing required helpers" in failure
        and "objc3c_frontend_c_missing_live_helper" in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_unreviewed_public_symbol_removal(
    tmp_path: Path,
) -> None:
    governance = _governance()
    core_surface = governance["stdlib_public_api"][0]  # type: ignore[index]
    core_surface["symbols"]["objc3_core_not_real"] = "fn objc3_core_not_real()"  # type: ignore[index]

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        "unreviewed public-api symbol removal in objc3.core" in failure
        and "objc3_core_not_real" in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_signature_drift(tmp_path: Path) -> None:
    governance = _governance()
    core_surface = governance["stdlib_public_api"][0]  # type: ignore[index]
    core_surface["symbols"]["objc3_core_array_count"] = (  # type: ignore[index]
        "fn objc3_core_array_count(count: i32, extra: i32)"
    )

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        "public-api signature drifted for objc3.core.objc3_core_array_count"
        in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_private_helper_graduation_without_policy(
    tmp_path: Path,
) -> None:
    governance = _governance()
    module_policies = governance["module_policies"]  # type: ignore[index]
    core_policy = copy.deepcopy(module_policies["objc3.core"])  # type: ignore[index]
    core_policy["allowed_public_prefixes"] = ["objc3_promoted_"]
    core_policy["allowed_public_symbols"] = []
    module_policies["objc3.core"] = core_policy  # type: ignore[index]

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        "private helper graduation lacks policy for objc3.core.objc3_core_array_count"
        in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_abi_api_drift_gate_rejects_compatibility_window_violation(
    tmp_path: Path,
) -> None:
    governance = _governance()
    compatibility_policy = governance["compatibility_window_policy"]  # type: ignore[index]
    compatibility_policy["expected_supported_major_line"] = 2  # type: ignore[index]

    rc, summary = _run_with_governance(tmp_path, governance)

    assert rc == 1
    assert any(
        "supported major line drifted" in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_release_manifest_input_validation_rejects_failed_abi_api_gate(
    tmp_path: Path,
) -> None:
    package_root = tmp_path / "package"
    package_root.mkdir()
    (package_root / "repo-superclean.json").write_text("{}", encoding="utf-8")
    manifest_path = package_root / "manifest.json"
    manifest_path.write_text("{}", encoding="utf-8")
    entry = PayloadEntry(
        path="artifacts/bin/objc3c.exe",
        sha256="0" * 64,
        byte_count=1,
        component_group="native-binaries",
    )
    assembly = PackageAssembly(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest={
            "repo_superclean_surface": "repo-superclean.json",
            "copied_file_count": 1,
        },
        entries=(entry,),
        payload_digest="a" * 64,
    )
    failed_summary = tmp_path / "abi-api-drift-summary.json"
    write_json_file(
        failed_summary,
        {
            "contract_id": abi_api_drift.SUMMARY_CONTRACT_ID,
            "status": "FAIL",
            "failure_count": 1,
        },
    )

    with pytest.raises(RuntimeError, match="ABI/API drift gate did not pass"):
        validate_release_inputs(
            first=assembly,
            second=assembly,
            payload_policy={"required_payload_prefixes": ["artifacts/bin/"]},
            abi_api_drift_summary_path=failed_summary,
        )


def test_release_manifest_input_validation_reports_added_payload_entries(
    tmp_path: Path,
) -> None:
    package_root = tmp_path / "package"
    package_root.mkdir()
    (package_root / "repo-superclean.json").write_text("{}", encoding="utf-8")
    manifest_path = package_root / "manifest.json"
    manifest_path.write_text("{}", encoding="utf-8")
    binary_entry = PayloadEntry(
        path="artifacts/bin/objc3c.exe",
        sha256="0" * 64,
        byte_count=1,
        component_group="native-binaries",
    )
    late_source_entry = PayloadEntry(
        path="scripts/late_source.py",
        sha256="1" * 64,
        byte_count=2,
        component_group="scripts-and-runbooks",
    )
    first = PackageAssembly(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest={
            "repo_superclean_surface": "repo-superclean.json",
            "copied_file_count": 1,
        },
        entries=(binary_entry,),
        payload_digest="a" * 64,
    )
    second = PackageAssembly(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest={
            "repo_superclean_surface": "repo-superclean.json",
            "copied_file_count": 2,
        },
        entries=(binary_entry, late_source_entry),
        payload_digest="b" * 64,
    )
    passing_summary = tmp_path / "abi-api-drift-summary.json"
    write_json_file(
        passing_summary,
        {
            "contract_id": abi_api_drift.SUMMARY_CONTRACT_ID,
            "status": "PASS",
            "failure_count": 0,
        },
    )

    with pytest.raises(
        RuntimeError,
        match=(
            "added payload entries after first package run: "
            "scripts/late_source.py"
        ),
    ):
        validate_release_inputs(
            first=first,
            second=second,
            payload_policy={"required_payload_prefixes": ["artifacts/bin/"]},
            abi_api_drift_summary_path=passing_summary,
        )


def test_release_manifest_input_validation_reports_removed_payload_entries(
    tmp_path: Path,
) -> None:
    package_root = tmp_path / "package"
    package_root.mkdir()
    (package_root / "repo-superclean.json").write_text("{}", encoding="utf-8")
    manifest_path = package_root / "manifest.json"
    manifest_path.write_text("{}", encoding="utf-8")
    binary_entry = PayloadEntry(
        path="artifacts/bin/objc3c.exe",
        sha256="0" * 64,
        byte_count=1,
        component_group="native-binaries",
    )
    removed_doc_entry = PayloadEntry(
        path="docs/runbooks/release.md",
        sha256="1" * 64,
        byte_count=2,
        component_group="scripts-and-runbooks",
    )
    first = PackageAssembly(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest={
            "repo_superclean_surface": "repo-superclean.json",
            "copied_file_count": 2,
        },
        entries=(binary_entry, removed_doc_entry),
        payload_digest="a" * 64,
    )
    second = PackageAssembly(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest={
            "repo_superclean_surface": "repo-superclean.json",
            "copied_file_count": 1,
        },
        entries=(binary_entry,),
        payload_digest="b" * 64,
    )
    passing_summary = tmp_path / "abi-api-drift-summary.json"
    write_json_file(
        passing_summary,
        {
            "contract_id": abi_api_drift.SUMMARY_CONTRACT_ID,
            "status": "PASS",
            "failure_count": 0,
        },
    )

    with pytest.raises(
        RuntimeError,
        match=(
            "removed payload entries after first package run: "
            "docs/runbooks/release.md"
        ),
    ):
        validate_release_inputs(
            first=first,
            second=second,
            payload_policy={"required_payload_prefixes": ["artifacts/bin/"]},
            abi_api_drift_summary_path=passing_summary,
        )


def test_release_manifest_input_validation_reports_changed_payload_entries(
    tmp_path: Path,
) -> None:
    package_root = tmp_path / "package"
    package_root.mkdir()
    (package_root / "repo-superclean.json").write_text("{}", encoding="utf-8")
    manifest_path = package_root / "manifest.json"
    manifest_path.write_text("{}", encoding="utf-8")
    first_entry = PayloadEntry(
        path="artifacts/bin/objc3c.exe",
        sha256="0" * 64,
        byte_count=1,
        component_group="native-binaries",
    )
    second_entry = PayloadEntry(
        path="artifacts/bin/objc3c.exe",
        sha256="1" * 64,
        byte_count=1,
        component_group="native-binaries",
    )
    first = PackageAssembly(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest={
            "repo_superclean_surface": "repo-superclean.json",
            "copied_file_count": 1,
        },
        entries=(first_entry,),
        payload_digest="a" * 64,
    )
    second = PackageAssembly(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest={
            "repo_superclean_surface": "repo-superclean.json",
            "copied_file_count": 1,
        },
        entries=(second_entry,),
        payload_digest="b" * 64,
    )
    passing_summary = tmp_path / "abi-api-drift-summary.json"
    write_json_file(
        passing_summary,
        {
            "contract_id": abi_api_drift.SUMMARY_CONTRACT_ID,
            "status": "PASS",
            "failure_count": 0,
        },
    )

    with pytest.raises(
        RuntimeError,
        match=(
            "changed payload entry digests after first package run: "
            "artifacts/bin/objc3c.exe"
        ),
    ):
        validate_release_inputs(
            first=first,
            second=second,
            payload_policy={"required_payload_prefixes": ["artifacts/bin/"]},
            abi_api_drift_summary_path=passing_summary,
        )


def test_release_manifest_input_validation_rejects_wrong_abi_api_summary_contract(
    tmp_path: Path,
) -> None:
    package_root = tmp_path / "package"
    package_root.mkdir()
    (package_root / "repo-superclean.json").write_text("{}", encoding="utf-8")
    manifest_path = package_root / "manifest.json"
    manifest_path.write_text("{}", encoding="utf-8")
    entry = PayloadEntry(
        path="artifacts/bin/objc3c.exe",
        sha256="0" * 64,
        byte_count=1,
        component_group="native-binaries",
    )
    assembly = PackageAssembly(
        package_root=package_root,
        manifest_path=manifest_path,
        package_manifest={
            "repo_superclean_surface": "repo-superclean.json",
            "copied_file_count": 1,
        },
        entries=(entry,),
        payload_digest="a" * 64,
    )
    wrong_contract_summary = tmp_path / "abi-api-drift-summary.json"
    write_json_file(
        wrong_contract_summary,
        {
            "contract_id": "objc3c.release.foundation.other.v1",
            "status": "PASS",
            "failure_count": 0,
        },
    )

    with pytest.raises(RuntimeError, match="ABI/API drift gate summary contract drifted"):
        validate_release_inputs(
            first=assembly,
            second=assembly,
            payload_policy={"required_payload_prefixes": ["artifacts/bin/"]},
            abi_api_drift_summary_path=wrong_contract_summary,
        )
