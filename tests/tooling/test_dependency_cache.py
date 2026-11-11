"""Dependency reuse remains independent of feature commits and rejects changed bytes."""
import json
from pathlib import Path
import shutil

import pytest

from scripts.objc3c_toolchain import dependency_cache as owner

ROOT = Path(__file__).resolve().parents[2]


@pytest.fixture
def repo(tmp_path):
    root = tmp_path / 'repo'
    for name in ('toolchains/llvm/manifest.json', 'toolchains/swift/manifest.json',
                 'scripts/setup_cloud_environment.sh', 'scripts/objc3c_toolchain/cmake_linux.sh',
                 'scripts/objc3c_toolchain_runtime/runtime-profile.json', 'uv.lock', 'package-lock.json'):
        path = root / name;path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(ROOT / name, path)
    return root


def keys(value):
    return {name: entry['key'] for name, entry in value['entries'].items()}


def test_feature_patch_changes_do_not_invalidate_dependency_keys(repo, tmp_path):
    root = tmp_path / 'tools'
    before = keys(owner.layout(repo, root, include_swift=True))
    path = repo / 'toolchains/llvm/manifest.json';value = json.loads(path.read_text())
    value['patches'][0]['sha256'] = '0' * 64
    value['ci_actions']['actions/checkout']['commit'] = '1' * 40
    path.write_text(json.dumps(value))
    assert keys(owner.layout(repo, root, include_swift=True)) == before


def test_tool_update_invalidates_only_its_own_prefix(repo, tmp_path):
    root = tmp_path / 'tools'
    before = keys(owner.layout(repo, root, include_swift=True))
    path = repo / 'toolchains/llvm/manifest.json';value = json.loads(path.read_text())
    value['host_tools']['cmake'] = '4.4.6';path.write_text(json.dumps(value))
    after = keys(owner.layout(repo, root, include_swift=True))
    assert {name for name in before if before[name] != after[name]} == {'cmake'}


def test_package_lock_updates_do_not_redownload_the_python_interpreter(repo, tmp_path):
    root = tmp_path / 'tools'
    native = keys(owner.layout(repo, root));quick = keys(owner.quick_layout(repo, root))
    path = repo / 'uv.lock';path.write_text(path.read_text() + '\n# new resolved dependency\n')
    assert {name for name, value in native.items()
            if keys(owner.layout(repo, root))[name] != value} == {'python'}
    changed = keys(owner.quick_layout(repo, root))
    assert changed['interpreter'] == quick['interpreter']
    assert changed['python_packages'] != quick['python_packages']


def cache_fixture(tmp_path):
    prefix = tmp_path / 'tool';prefix.mkdir()
    binary = prefix / 'binary';binary.write_bytes(b'verified upstream bytes');binary.chmod(0o755)
    (prefix / 'alias').symlink_to('binary')
    plan = {'schema': owner.SCHEMA, 'entries': {'tool': {
        'key': 'exact-pin', 'paths': [str(prefix)], 'seal': str(tmp_path / 'tool-seal.json')}}}
    owner.seal(plan)
    return plan, prefix


@pytest.mark.parametrize('mutation', ['bytes', 'mode', 'missing', 'extra', 'link', 'key'])
def test_restored_prefix_rejects_changed_contents_modes_links_and_identity(tmp_path, mutation):
    plan, prefix = cache_fixture(tmp_path)
    assert owner.admit(plan) == {'status': 'PASS', 'admitted': ['tool']}
    if mutation == 'bytes': (prefix / 'binary').write_bytes(b'corrupt upstream bytes')
    elif mutation == 'mode': (prefix / 'binary').chmod(0o644)
    elif mutation == 'missing': (prefix / 'binary').unlink()
    elif mutation == 'extra': (prefix / 'unexpected').write_text('unadmitted')
    elif mutation == 'link':
        (prefix / 'alias').unlink();(prefix / 'alias').symlink_to(tmp_path)
    else: plan['entries']['tool']['key'] = 'another-version'
    with pytest.raises((ValueError, OSError)):
        owner.admit(plan)


def test_cold_miss_and_restored_unsealed_prefix_are_distinct(tmp_path):
    entry = {'key': 'pin', 'paths': [str(tmp_path / 'tool')], 'seal': str(tmp_path / 'seal.json')}
    plan = {'schema': owner.SCHEMA, 'entries': {'tool': entry}}
    assert owner.admit(plan)['admitted'] == []
    (tmp_path / 'tool').mkdir()
    with pytest.raises(ValueError, match='no admission seal'):
        owner.admit(plan)
