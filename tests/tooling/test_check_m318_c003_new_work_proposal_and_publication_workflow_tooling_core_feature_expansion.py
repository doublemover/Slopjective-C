from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
TOOL = ROOT / "tmp" / "github-publish" / "m318_governance" / "publish_new_work_proposal.py"
TEMPLATE = ROOT / "tmp" / "planning" / "m318_governance" / "new_work_proposal_template.json"
FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "m318_governance" / "new_work_proposal_sample.json"
OUTPUT_DIR = ROOT / "tmp" / "reports" / "m318" / "governance" / "pytest_new_work_proposal"


def test_render_only_publication_tool_generates_expected_outputs() -> None:
    completed = subprocess.run(
        [
            sys.executable,
            str(TOOL),
            "--proposal",
            str(FIXTURE),
            "--template",
            str(TEMPLATE),
            "--output-dir",
            str(OUTPUT_DIR),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout

    summary = json.loads((OUTPUT_DIR / "publication_summary.json").read_text(encoding="utf-8"))
    payload = json.loads((OUTPUT_DIR / "issue_payload.json").read_text(encoding="utf-8"))
    body = (OUTPUT_DIR / "issue_body.md").read_text(encoding="utf-8")

    assert summary["ok"] is True
    assert summary["publication_mode"] == "render-only"
    assert payload["title"] == "[M400][Lane-A][A001] Example proposal publication boundary - Contract and architecture freeze"
    assert "## Validation posture" in body
    assert "<!-- EXECUTION-ORDER-START -->" in body
