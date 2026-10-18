from copy import deepcopy
from pathlib import Path

import pytest

from scripts.objc3c_native_validation.worker import validate_editor_toolchain


def context(tmp_path: Path):
    root = tmp_path / 'sdk'
    names = {'clang': 'clang', 'clangd': 'clangd', 'clang_format': 'clang-format'}
    tools = {name: str(root / 'bin' / filename) for name, filename in names.items()}
    inputs = [{'path': path, 'sha256': 'a' * 64} for path in tools.values()]
    snapshot = {'sdk_build_id': 'qualified-sdk', 'target_triple': 'x86_64-unknown-linux-gnu',
                'resource_dir': str(root / 'lib/clang/23'), 'tools': tools, 'input_files': inputs}
    report = {'status': 'passed', 'build_identity': snapshot['sdk_build_id'],
              'target_triple': snapshot['target_triple'], 'resource_dir': snapshot['resource_dir'],
              'tools': {filename: {'path': tools[name], 'sha256': 'a' * 64,
                                   'version': 'same reported LLVM version'}
                        for name, filename in names.items()}}
    return snapshot, report


def test_editor_results_bind_the_installed_sdk_and_native_target(tmp_path):
    snapshot, report = context(tmp_path)
    validate_editor_toolchain(report, snapshot)


@pytest.mark.parametrize('changed', ['target', 'resources', 'build_tree', 'binary_contents', 'missing_hash'])
def test_version_strings_cannot_admit_other_editor_inputs(tmp_path, changed):
    snapshot, original = context(tmp_path)
    report = deepcopy(original)
    if changed == 'target':
        report['target_triple'] = 'x86_64-pc-windows-msvc'
    elif changed == 'resources':
        report['resource_dir'] = str(tmp_path / 'other/lib/clang/23')
    elif changed == 'build_tree':
        report['tools']['clangd']['path'] = str(tmp_path / 'build/bin/clangd')
    elif changed == 'binary_contents':
        report['tools']['clangd']['sha256'] = 'b' * 64
    else:
        snapshot['input_files'] = []
    with pytest.raises(ValueError):
        validate_editor_toolchain(report, snapshot)
