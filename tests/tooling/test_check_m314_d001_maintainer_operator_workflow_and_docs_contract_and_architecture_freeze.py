from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CONTRACT_JSON = ROOT / "spec" / "planning" / "compiler" / "m314" / "m314_d001_maintainer_operator_workflow_and_docs_contract_and_architecture_freeze_contract.json"


def test_contract_points_at_maintainer_runbook() -> None:
    payload = json.loads(CONTRACT_JSON.read_text(encoding="utf-8"))
    assert payload["canonical_docs"]["maintainer_runbook"] == "docs/runbooks/objc3c_maintainer_workflows.md"


def test_contract_keeps_native_objc3c_as_supported_compiler_root() -> None:
    payload = json.loads(CONTRACT_JSON.read_text(encoding="utf-8"))
    assert payload["supported_compiler_root"] == "native/objc3c"
