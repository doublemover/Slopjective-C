"""Report writers for runtime acceptance."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.summary_owner_contracts import (
    REPORT_ASSEMBLY_OWNER_CONTRACT,
)


def with_report_persistence_contract(payload: dict[str, Any]) -> dict[str, Any]:
    owned_payload = dict(payload)
    owned_payload["report_persistence_owner_contract"] = (
        REPORT_ASSEMBLY_OWNER_CONTRACT.payload()
    )
    return owned_payload


def write_json_report(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        json.dumps(with_report_persistence_contract(payload), indent=2) + "\n",
        encoding="utf-8",
    )


__all__ = [
    "REPORT_ASSEMBLY_OWNER_CONTRACT",
    "with_report_persistence_contract",
    "write_json_report",
]
