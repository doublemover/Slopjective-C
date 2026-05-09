from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

SCRIPTS_ROOT = Path(__file__).resolve().parents[2] / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "build_objc3c_claimability_dashboard_release_blocker_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "build_objc3c_claimability_dashboard_release_blocker_contract", SCRIPT_PATH
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/build_objc3c_claimability_dashboard_release_blocker_contract.py"
    )
builder = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = builder
SPEC.loader.exec_module(builder)


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def test_dashboard_release_blocker_contract_summary(tmp_path: Path) -> None:
    json_out = tmp_path / "dashboard_release_blocker_contract_summary.json"
    md_out = tmp_path / "dashboard_release_blocker_contract_summary.md"

    code = builder.main(["--summary-json", str(json_out), "--summary-md", str(md_out)])

    assert code == 0
    payload = read_json(json_out)
    assert payload["contract_id"] == builder.SUMMARY_CONTRACT_ID
    assert payload["status"] == "PASS"
    assert payload["dashboard_release_blocker_projection"]["blocker"] == (
        "claimability-dashboard-not-production-strength"
    )
    assert payload["checks"]["dashboard_script_consumes_projection"] is True
    assert payload["checks"]["release_blocker_script_emits_projection"] is True
    assert payload["checks"]["projection_requires_public_summary_decision_fields"] is True
    assert payload["checks"]["projection_has_source_owned_decision_fields"] is True
    assert payload["checks"]["source_truth_excludes_tmp"] is True


def test_dashboard_release_blocker_contract_check_mode_fails_on_drift(
    tmp_path: Path,
    capsys: object,
) -> None:
    json_out = tmp_path / "summary.json"
    md_out = tmp_path / "summary.md"
    json_out.write_text("{}\n", encoding="utf-8")
    md_out.write_text("# stale\n", encoding="utf-8")

    code = builder.main(
        ["--summary-json", str(json_out), "--summary-md", str(md_out), "--check"]
    )

    assert code == 1
    captured = capsys.readouterr()
    assert "dashboard release-blocker contract output drift" in captured.err
