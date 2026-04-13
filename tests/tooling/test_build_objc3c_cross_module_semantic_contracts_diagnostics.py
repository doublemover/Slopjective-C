import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "build_objc3c_cross_module_semantic_contracts_diagnostics.py"
REPORT = ROOT / "reports" / "claimability" / "cross-module-semantic-contracts-diagnostics" / "cross_module_semantic_contracts_diagnostics_summary.json"


def test_cross_module_semantic_contracts_diagnostics_summary_is_fresh_and_passing() -> None:
    completed = subprocess.run(
        ["python", str(SCRIPT), "--check"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    summary = json.loads(REPORT.read_text(encoding="utf-8"))
    assert summary["status"] == "PASS"
    model = summary["cross_module_semantic_contracts_diagnostics_model"]
    assert model["contract_id"] == "objc3c.cross_module.semantic.contracts.diagnostics.closure.v1"
    assert model["deterministic"] is True
    assert model["ready_for_lowering_and_runtime"] is True
    assert model["module_import_graph_sites"] >= 4
    assert model["cross_module_conformance_sites"] >= 4
    assert model["interop_import_module_annotation_sites"] >= 1
    assert model["contract_violation_sites"] == 0
