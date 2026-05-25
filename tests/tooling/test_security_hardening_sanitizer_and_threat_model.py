from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_workflow.actions.release_governance_security_targets import (
    CHECK_SECURITY_LANGUAGE_RUNTIME_THREAT_MODEL,
    CHECK_SECURITY_SANITIZER_VALIDATION,
    SECURITY_HARDENING_VALIDATION_CHILD_ACTIONS,
)
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


ROOT = Path(__file__).resolve().parents[2]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening"


def load_fixture(name: str) -> dict[str, object]:
    return json.loads((FIXTURE_ROOT / name).read_text(encoding="utf-8"))


def test_sanitizer_validation_contract_pins_asan_ubsan_runtime_and_compiler_surfaces() -> None:
    contract = load_fixture("sanitizer_validation_contract.json")
    registered = set(public_workflow_action_names())

    assert contract["public_action"] == CHECK_SECURITY_SANITIZER_VALIDATION
    assert CHECK_SECURITY_SANITIZER_VALIDATION in registered
    assert CHECK_SECURITY_SANITIZER_VALIDATION in SECURITY_HARDENING_VALIDATION_CHILD_ACTIONS
    assert contract["report_contract"]["native_sanitizer_execution_claimed"] is False  # type: ignore[index]

    matrix = {
        (entry["sanitizer"], entry["surface"])
        for entry in contract["coverage_matrix"]  # type: ignore[index]
    }
    assert matrix == {
        ("ASan", "native_runtime"),
        ("ASan", "native_compiler"),
        ("UBSan", "native_runtime"),
        ("UBSan", "native_compiler"),
    }

    application_surfaces = {
        entry["surface"]
        for entry in contract["target_applications"]  # type: ignore[index]
    }
    assert application_surfaces == {
        "native_runtime",
        "native_compiler_frontend",
        "native_compiler_cli",
        "native_compiler_c_api_runner",
    }

    package_variants = {
        variant["variant_id"]: variant
        for variant in contract["runtime_package_variants"]  # type: ignore[index]
    }
    assert package_variants["objc3c.toolchain.sanitizer.address"]["issue_ref"] == 8230
    assert package_variants["objc3c.toolchain.sanitizer.undefined"]["issue_ref"] == 8231
    assert all(
        variant["claim_state"] == "reserved"
        and variant["native_package_execution_claimed"] is False
        and variant["unsupported_behavior"] == "fail-closed"
        and variant["metadata_freshness_guard"]["generated_metadata_allowed"] is False
        and variant["metadata_freshness_guard"]["blocks_publication_on_stale"] is True
        and variant["install_guard"]["missing_runtime_behavior"]
        == "fail-closed-before-package-install"
        and variant["install_guard"]["stale_package_metadata_behavior"]
        == "fail-closed-before-publication"
        and variant["package_runtime_contract"]["runtime_probe_required"] is True
        and variant["package_runtime_contract"]["default_release_channel_allowed"]
        is False
        and variant["package_runtime_contract"]["report_artifact_support_truth"]
        is False
        and variant["package_runtime_contract"][
            "mixed_release_sanitizer_runtime_behavior"
        ]
        == "fail-closed"
        for variant in package_variants.values()
    )
    assert package_variants["objc3c.toolchain.sanitizer.address"][
        "runtime_library_contract"
    ]["runtime_library_ids"] == ["objc3-runtime", "clang_rt.asan"]
    assert package_variants["objc3c.toolchain.sanitizer.undefined"][
        "runtime_library_contract"
    ]["runtime_library_ids"] == ["objc3-runtime", "clang_rt.ubsan"]

    asan_contract = package_variants["objc3c.toolchain.sanitizer.address"][
        "package_runtime_contract"
    ]
    ubsan_contract = package_variants["objc3c.toolchain.sanitizer.undefined"][
        "package_runtime_contract"
    ]
    assert asan_contract["package_layout_contract"]["runtime_library_manifest_path"] == (  # type: ignore[index]
        "share/objc3c/sanitizer/asan-runtime-libraries.json"
    )
    assert ubsan_contract["package_layout_contract"]["runtime_library_manifest_path"] == (  # type: ignore[index]
        "share/objc3c/sanitizer/ubsan-runtime-libraries.json"
    )
    assert set(asan_contract["package_layout_contract"]["runtime_library_required_entries"]) == {  # type: ignore[index]
        "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.dll",
        "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.lib",
        "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib",
    }
    assert set(ubsan_contract["package_layout_contract"]["runtime_library_required_entries"]) == {  # type: ignore[index]
        "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone-x86_64.lib",
        "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone_cxx-x86_64.lib",
    }
    for runtime_contract in (asan_contract, ubsan_contract):
        receipt = runtime_contract["install_receipt_contract"]  # type: ignore[index]
        native_execution = receipt["native_execution_contract"]
        assert {
            "runtime_library_manifest_path",
            "runtime_library_manifest_digest",
            "runtime_library_artifacts",
            "missing_runtime_behavior",
        } <= set(receipt["required_fields"])
        assert "runtime_library_artifacts" in native_execution["native_execution_record_fields"]
        assert {
            "runtime_library_manifest_path",
            "runtime_library_artifacts",
        } <= set(runtime_contract["runtime_probe_contract"]["probe_inputs"])  # type: ignore[index]


def test_language_runtime_threat_model_links_macro_runtime_compiler_evidence() -> None:
    contract = load_fixture("language_runtime_threat_model_backlog.json")
    registered = set(public_workflow_action_names())

    assert contract["public_action"] == CHECK_SECURITY_LANGUAGE_RUNTIME_THREAT_MODEL
    assert CHECK_SECURITY_LANGUAGE_RUNTIME_THREAT_MODEL in registered
    assert CHECK_SECURITY_LANGUAGE_RUNTIME_THREAT_MODEL in (
        SECURITY_HARDENING_VALIDATION_CHILD_ACTIONS
    )

    evidence_roots = contract["evidence_roots"]  # type: ignore[index]
    assert evidence_roots["macro_supply_chain_trust_registry"] == (  # type: ignore[index]
        "tests/tooling/fixtures/security_hardening/macro_supply_chain_trust_registry.json"
    )
    assert evidence_roots["runtime_hardening_contract"] == (  # type: ignore[index]
        "tests/tooling/fixtures/security_hardening/runtime_hardening_contract.json"
    )
    assert evidence_roots["sanitizer_validation_contract"] == (  # type: ignore[index]
        "tests/tooling/fixtures/security_hardening/sanitizer_validation_contract.json"
    )

    categories = {
        threat["category"]
        for threat in contract["threats"]  # type: ignore[index]
    }
    assert categories == set(contract["required_categories"])  # type: ignore[arg-type]


def test_security_hardening_source_and_workflow_surfaces_own_tail_actions() -> None:
    source_surface = load_fixture("source_surface.json")
    workflow_surface = load_fixture("workflow_surface.json")
    tail_actions = {
        CHECK_SECURITY_SANITIZER_VALIDATION,
        CHECK_SECURITY_LANGUAGE_RUNTIME_THREAT_MODEL,
    }

    assert tail_actions <= set(source_surface["public_actions"])  # type: ignore[arg-type]
    assert tail_actions <= set(source_surface["owner_policy"]["owned_actions"])  # type: ignore[index]
    assert tail_actions <= set(workflow_surface["required_actions"])  # type: ignore[arg-type]
    assert tail_actions <= set(workflow_surface["owner_policy"]["owned_actions"])  # type: ignore[index]
    assert tail_actions <= set(workflow_surface["validation_child_actions"])  # type: ignore[arg-type]
    assert tail_actions <= set(workflow_surface["owner_policy"]["workflow_child_actions"])  # type: ignore[index]
