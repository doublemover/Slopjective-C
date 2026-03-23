from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m270_c002_actor_thunk_hop_and_isolation_lowering_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m270" / "M270-C002" / "actor_lowering_runtime_summary.json"


def test_m270_c002_checker_passes() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert SUMMARY.exists()
    text = SUMMARY.read_text(encoding="utf-8")
    assert '"ok": true' in text
    assert '"contract_id": "objc3c-part7-actor-thunk-hop-and-isolation-lowering/m270-c002-v1"' in text
