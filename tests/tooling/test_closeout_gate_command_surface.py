from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CLOSEOUT_GATES = (
    ROOT / "scripts" / "check_governance_sustainability_closeout_gate.py",
    ROOT / "scripts" / "check_runtime_corrective_closeout_gate.py",
)


def test_closeout_gates_refresh_repo_superclean_via_public_workflow() -> None:
    for path in CLOSEOUT_GATES:
        text = path.read_text(encoding="utf-8")
        assert "public_workflow_command" in text, path
        assert 'public_workflow_command("check-repo-superclean-surface")' in text, path
        assert (
            'python_script_command("scripts/check_repo_superclean_surface.py")'
            not in text
        ), path
