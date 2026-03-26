from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m318_b001_governance_and_budget_enforcement_policy_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m318" / "M318-B001" / "governance_budget_policy_summary.json"


def test_m318_b001_checker_passes_and_writes_summary() -> None:
    result = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-governance-anti-noise-budget-policy/m318-b001-v1"
    assert payload["budget_family_ownership"]["public_command_surface"]["existing_owner_issue"] == "M314-C003"
    assert payload["budget_family_ownership"]["validation_growth"]["existing_owner_issue"] == "M313-B005"
    assert payload["budget_family_ownership"]["source_hygiene_and_residue"]["existing_owner_issue"] == "M315-D002"
