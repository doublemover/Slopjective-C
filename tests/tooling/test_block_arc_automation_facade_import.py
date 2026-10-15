from __future__ import annotations

import os
from pathlib import Path
import subprocess
import sys


def test_facade_import_survives_immutable_alias_reported_as_class() -> None:
    repo = Path(__file__).resolve().parents[2]
    env = dict(os.environ)
    env["PYTHONPATH"] = str(repo / "scripts") + os.pathsep + env.get("PYTHONPATH", "")
    script = r"""
import inspect
_original = inspect.isclass
inspect.isclass = lambda value: _original(value) or type(value).__name__ == "GenericAlias"
from objc3c_runtime_acceptance.domains.block_arc_automation_artifacts import (
    BlockArcAutomationArtifacts,
    ManifestSurface,
    NegativeBatch,
    load_block_arc_automation_artifacts,
)
assert callable(load_block_arc_automation_artifacts)
assert isinstance(BlockArcAutomationArtifacts, type)
assert type(ManifestSurface).__name__ == "GenericAlias"
assert type(NegativeBatch).__name__ == "GenericAlias"
"""
    completed = subprocess.run(
        [sys.executable, "-c", script],
        cwd=repo,
        env=env,
        text=True,
        capture_output=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
