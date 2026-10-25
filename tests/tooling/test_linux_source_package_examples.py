"""Every maintained source workspace must reach the distributed SDK examples."""
import json
from pathlib import Path

from scripts.objc3c_toolchain.package import SOURCE_PACKAGE_EXAMPLES


def test_all_native_source_workspaces_are_distributed():
    root = Path(__file__).resolve().parents[1] / 'clang_foundation'
    workspaces = {path.parent.name for path in root.glob('*/workspace.json')
                  if json.loads(path.read_text())['schema'] == 'objc3c.source-workspace.v4'}
    assert set(SOURCE_PACKAGE_EXAMPLES) == workspaces
    assert len(set(SOURCE_PACKAGE_EXAMPLES.values())) == len(workspaces)
