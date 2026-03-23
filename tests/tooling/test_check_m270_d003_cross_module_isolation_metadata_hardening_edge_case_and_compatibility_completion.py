from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m270_d003_cross_module_isolation_metadata_hardening_edge_case_and_compatibility_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m270" / "M270-D003" / "cross_module_actor_isolation_metadata_summary.json"


def test_m270_d003_checker_passes() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert SUMMARY.exists()
    text = SUMMARY.read_text(encoding="utf-8")
    assert '"ok": true' in text
    assert '"contract_id": "objc3c-cross-module-actor-isolation-metadata-hardening/m270-d003-v1"' in text
