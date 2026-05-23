from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from scripts.objc3c_workflow.actions import (
    developer_tooling_llvm_hosted,
    developer_tooling_llvm_parity,
    hosted_llvm_summary,
)

ROOT = Path(__file__).resolve().parents[2]
OWNER_CONTRACT_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "hosted_llvm_tool_capability_owner_contract.json"
)


def load_owner_contract_fixture() -> dict[str, Any]:
    return json.loads(OWNER_CONTRACT_FIXTURE.read_text(encoding="utf-8"))


def capable_llvm_summary() -> dict[str, object]:
    return {
        "mode": "objc3c-llvm-capabilities-v2",
        "ok": True,
        "clang": {"found": True},
        "clangxx": {"found": True},
        "llc": {"found": True},
        "llvm_ar": {"found": True},
        "llvm_config": {"found": True},
        "llc_features": {"supports_filetype_obj": True},
        "llvm_config_features": {"headers_libraries_discovered": True},
        "toolchain_identity": {"claimable": True},
        "llvm_support_matrix": {
            "native_object_emission_contract": {
                "status": "native_object_emission_supported"
            }
        },
    }


def hosted_probe_without_object_emission() -> tuple[int, dict[str, object]]:
    return (
        1,
        {
            "mode": "objc3c-llvm-capabilities-v2",
            "ok": False,
            "clang": {"found": True},
            "clangxx": {"found": True},
            "llc": {
                "found": False,
                "diagnostic": "llc executable not found: llc",
            },
            "llvm_ar": {"found": True},
            "llvm_config": {"found": True},
            "llc_features": {"supports_filetype_obj": False},
            "llvm_config_features": {"headers_libraries_discovered": True},
            "toolchain_identity": {"claimable": False},
            "llvm_support_matrix": {
                "native_object_emission_contract": {
                    "status": "native_object_emission_missing_llc"
                }
            },
            "failures": [
                "llc executable not found: llc",
                "sema/type-system parity capability unavailable: llc executable missing",
                (
                    "capability demo compatibility: capability demo compatibility "
                    "requires sema/type-system parity to stay ready"
                ),
            ],
            "capability_demo_compatibility": {
                "failures": [
                    "capability demo compatibility requires sema/type-system parity to stay ready"
                ]
            },
        },
    )


def hosted_probe_with_capability_truth_drift() -> tuple[int, dict[str, object]]:
    return (
        1,
        {
            "mode": "objc3c-llvm-capabilities-v2",
            "ok": False,
            "clang": {"found": True},
            "clangxx": {"found": True},
            "llc": {"found": True},
            "llvm_ar": {"found": True},
            "llvm_config": {"found": True},
            "llc_features": {"supports_filetype_obj": True},
            "llvm_config_features": {"headers_libraries_discovered": True},
            "toolchain_identity": {"claimable": True},
            "llvm_support_matrix": {
                "native_object_emission_contract": {
                    "status": "native_object_emission_supported"
                }
            },
            "capability_demo_compatibility": {
                "failures": ["story capability drift detected for signalMesh"]
            },
        },
    )


def hosted_summary_without_clang() -> dict[str, object]:
    return {
        "mode": "objc3c-llvm-capabilities-v2",
        "ok": True,
        "clang": {"found": False},
        "clangxx": {"found": True},
        "llc": {"found": True},
        "llvm_ar": {"found": True},
        "llvm_config": {"found": True},
        "llc_features": {"supports_filetype_obj": True},
        "llvm_config_features": {"headers_libraries_discovered": True},
        "toolchain_identity": {"claimable": True},
        "llvm_support_matrix": {
            "native_object_emission_contract": {
                "status": "native_object_emission_supported"
            }
        },
    }


def hosted_summary_without_llc() -> dict[str, object]:
    return {
        "mode": "objc3c-llvm-capabilities-v2",
        "ok": False,
        "clang": {"found": True},
        "clangxx": {"found": True},
        "llc": {"found": False},
        "llvm_ar": {"found": True},
        "llvm_config": {"found": True},
        "llc_features": {"supports_filetype_obj": False},
        "llvm_config_features": {"headers_libraries_discovered": True},
        "toolchain_identity": {"claimable": False},
        "llvm_support_matrix": {
            "native_object_emission_contract": {
                "status": "native_object_emission_missing_llc"
            }
        },
    }
