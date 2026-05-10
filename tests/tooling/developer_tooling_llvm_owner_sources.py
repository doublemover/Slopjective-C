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
        "llc": {"found": True},
        "llc_features": {"supports_filetype_obj": True},
    }


def hosted_probe_without_object_emission() -> tuple[int, dict[str, object]]:
    return (
        1,
        {
            "mode": "objc3c-llvm-capabilities-v2",
            "ok": True,
            "clang": {"found": True},
            "llc": {"found": True},
            "llc_features": {"supports_filetype_obj": False},
        },
    )


def hosted_summary_without_clang() -> dict[str, object]:
    return {
        "mode": "objc3c-llvm-capabilities-v2",
        "ok": True,
        "clang": {"found": False},
        "llc": {"found": True},
        "llc_features": {"supports_filetype_obj": True},
    }
