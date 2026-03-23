from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m270_c003_replay_proof_and_race_guard_artifact_integration_core_feature_expansion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m270" / "M270-C003" / "actor_replay_race_guard_summary.json"


def test_m270_c003_checker_passes() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert SUMMARY.exists()
    text = SUMMARY.read_text(encoding="utf-8")
    assert '"ok": true' in text
    assert '"contract_id": "objc3c-part7-actor-replay-proof-and-race-guard-integration/m270-c003-v1"' in text
