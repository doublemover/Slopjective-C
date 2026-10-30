"""Compiler implementation profiles cannot reuse ordinary foundation evidence."""
from copy import deepcopy
import json
from pathlib import Path
import shutil

import pytest

from scripts.objc3c_toolchain import core, host, instrumentation_profiles as profiles

ROOT = Path(__file__).resolve().parents[2]
NAME = 'compiler-asan-ubsan'


@pytest.fixture
def owned(tmp_path):
    for relative in ('toolchains/llvm/instrumentation-profiles.json',
                     'scripts/objc3c_toolchain/instrumentation_profiles.py',
                     'toolchains/llvm/manifest.json'):
        target = tmp_path / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(ROOT / relative, target)
    manifest = json.loads((tmp_path / 'toolchains/llvm/manifest.json').read_text())
    sealed = {'configuration_inputs': {'toolchains/llvm/cmake/ObjC3LLVMCache.cmake': 'cache'},
              'patch_digest': 'a' * 64, 'build_config_digest': 'b' * 64,
              'build_id': 'ordinary-build', 'tree_sha256': 'source-tree'}
    return tmp_path, manifest, sealed


def select(owned):
    repo, manifest, sealed = owned
    return profiles.select(repo, NAME, 'linux-x86_64', manifest['build'], sealed, manifest['upstream'])


def test_profile_identity_is_distinct_stable_and_preserves_ordinary_inputs(owned):
    repo, manifest, sealed = owned
    before = deepcopy((manifest, sealed))
    first = select(owned)
    assert first == select(owned)
    assert (manifest, sealed) == before
    assert first['source_build_id'] != sealed['build_id']
    assert first['build_config_digest'] != sealed['build_config_digest']
    assert first['configuration_inputs'].items() >= sealed['configuration_inputs'].items()
    assert first['cmake']['LLVM_USE_SANITIZER'] == 'Address;Undefined'
    assert first['build']['assertions'] and first['build']['rtti'] and first['build']['eh']
    assert 'clang-cpp' in first['build']['distribution_components']
    owner = repo / 'scripts/objc3c_toolchain/instrumentation_profiles.py'
    owner.write_text(owner.read_text() + '\n# recipe owner changed\n')
    assert select(owned)['build_config_digest'] != first['build_config_digest']
    assert sealed == before[1]


@pytest.mark.parametrize('mutation', ['recovery', 'disabled-asan', 'wrong-subject',
                                     'disabled-assertions', 'unknown-profile', 'duplicate-target'])
def test_invalid_or_weakened_profile_is_rejected(owned, mutation):
    repo, manifest, sealed = owned
    path = repo / 'toolchains/llvm/instrumentation-profiles.json'
    document = json.loads(path.read_text())
    recipe = document['profiles'][NAME]
    name = NAME
    if mutation == 'recovery':
        recipe['cmake']['LLVM_UBSAN_FLAGS'] = '-fsanitize-recover=undefined'
    elif mutation == 'disabled-asan':
        recipe['cmake']['LLVM_USE_SANITIZER'] = 'Undefined'
    elif mutation == 'wrong-subject':
        recipe['subject'] = 'emitted-client'
    elif mutation == 'disabled-assertions':
        manifest['build']['assertions'] = False
    elif mutation == 'unknown-profile':
        name = 'compiler-msan'
    else:
        recipe['build_target_groups'][1]['targets'].append('clang')
    path.write_text(json.dumps(document))
    with pytest.raises(ValueError):
        profiles.select(repo, name, 'linux-x86_64', manifest['build'], sealed, manifest['upstream'])


def test_canonical_planner_isolates_all_profile_outputs_and_stamps(owned, monkeypatch):
    repo, manifest, sealed = owned
    monkeypatch.setattr(core, 'verify_development', lambda *args, **kwargs: sealed)
    monkeypatch.setattr(core, '_load_manifest', lambda *args: (manifest, []))
    monkeypatch.setattr(host, 'linux_environment', lambda *args: {
        'clang_path': '/bootstrap/bin/clang', 'clangxx_path': '/bootstrap/bin/clang++',
        'link_path': '/bootstrap/bin/ld.lld', 'python_executable': '/bootstrap/bin/python',
        'target_triple': 'x86_64-unknown-linux-gnu'})
    ordinary = core.build_plans(repo, host='linux-x86_64')
    instrumented = core.build_plans(repo, host='linux-x86_64', instrumentation_profile=NAME)
    assert ordinary == core.build_plans(repo, host='linux-x86_64')
    for phase in ordinary:
        normal, special = ordinary[phase], instrumented[phase]
        assert normal['paths']['source'] == special['paths']['source']
        for output in ('build', 'stage'):
            assert special['paths'][output] == normal['paths'][output] + '-' + NAME
        assert special['instrumentation']['subject'] == 'compiler-implementation'
        assert normal['source_build_id'] != special['source_build_id']
        assert normal['build_config_digest'] != special['build_config_digest']
        assert special['effective_configuration']['LLVM_USE_SANITIZER'] == 'Address;Undefined'
    config = instrumented['configure']
    assert '-DLLVM_USE_SANITIZER=Address;Undefined' in config['command']
    assert '-DCLANG_OBJC3_CONFIG_DIGEST=' + config['build_config_digest'] in config['command']
    assert all('-compiler-asan-ubsan/' in cmd[0]
               for cmd in instrumented['test']['follow_up_commands'])
