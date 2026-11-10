"""Compiler implementation profiles cannot reuse ordinary foundation evidence."""
from copy import deepcopy
import json
from pathlib import Path
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain import core, host, instrumentation_profiles as profiles

ROOT = Path(__file__).resolve().parents[2]
NAME = 'compiler-asan-ubsan'
FUZZ_NAME = 'compiler-fuzz-asan-ubsan'
COVERAGE_NAME = 'compiler-source-coverage'


@pytest.mark.parametrize('shared', [True, False])
def test_native_clang_link_selection_executes_exactly_the_selected_implementation(tmp_path, shared):
    compiler = shutil.which('clang++')
    if not compiler: pytest.skip('requires a native C++ compiler')
    source = tmp_path / 'source';source.mkdir()
    (source / 'shared.cpp').write_text('extern "C" int selected() { return 42; }\n')
    (source / 'component.cpp').write_text('extern "C" int selected() { return 17; }\n')
    (source / 'facade.cpp').write_text('extern "C" int selected(); int facade() { return selected(); }\n')
    (source / 'main.cpp').write_text('int facade(); int main() { return facade() == '+
                                    str(42 if shared else 17)+' ? 0 : 1; }\n')
    policy = ROOT / 'native/objc3c/cmake/Objc3ClangLink.cmake'
    cmake = ('cmake_minimum_required(VERSION 3.24)\nproject(link_control CXX)\n'
        'include("'+policy.as_posix()+'")\n'
        'add_library(clang-cpp SHARED shared.cpp)\n'
        'set_target_properties(clang-cpp PROPERTIES OUTPUT_NAME objc3_link_control_clang)\n'
        'add_library(clangTooling STATIC component.cpp)\n'
        'add_library(objc3c_clang_foundation STATIC facade.cpp)\n'
        'objc3c_link_clang_components(objc3c_clang_foundation clangTooling)\n'
        'add_executable(consumer main.cpp)\n'
        'target_link_libraries(consumer PRIVATE objc3c_clang_foundation)\n'
        'objc3c_link_clang_components(consumer clangTooling)\n'
        'objc3c_verify_clang_linkage()\n')
    (source / 'CMakeLists.txt').write_text(cmake)
    build = tmp_path / 'build'
    configure = ['cmake', '-S', str(source), '-B', str(build), '-G', 'Ninja',
                 '-DCMAKE_CXX_COMPILER='+compiler, '-DOBJC3C_LINK_CLANG_CPP='+('ON' if shared else 'OFF')]
    subprocess.run(configure, check=True, capture_output=True, timeout=60)
    subprocess.run(['cmake', '--build', build, '--target', 'consumer'],
                   check=True, capture_output=True, timeout=60)
    subprocess.run([build / 'consumer'], check=True, capture_output=True, timeout=10)
    if shared:
        # Reintroduce the actual mixed-link mistake. Reject it during configure,
        # even when a non-sanitized link would otherwise silently choose a symbol.
        (source / 'CMakeLists.txt').write_text(cmake.replace('objc3c_verify_clang_linkage()',
            'target_link_libraries(consumer PRIVATE clangTooling)\nobjc3c_verify_clang_linkage()'))
        rejected = subprocess.run(configure, capture_output=True, text=True, timeout=60)
        assert rejected.returncode != 0 and 'mixes shared clang-cpp' in rejected.stderr


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
    assert {'clang-cpp', 'LLVM', 'llvm-nm'} <= set(first['build']['distribution_components'])
    owner = repo / 'scripts/objc3c_toolchain/instrumentation_profiles.py'
    owner.write_text(owner.read_text() + '\n# recipe owner changed\n')
    assert select(owned)['build_config_digest'] != first['build_config_digest']
    assert sealed == before[1]


def test_fuzzer_profile_instruments_the_compiler_graph_without_linking_an_engine_main(owned):
    repo, manifest, sealed = owned
    profile = profiles.select(repo, FUZZ_NAME, 'linux-x86_64', manifest['build'], sealed, manifest['upstream'])
    assert profile['recipe']['kind'] == 'fuzzer'
    assert profile['recipe']['fuzzer_flags'] == ['-fsanitize=fuzzer-no-link', '-gline-tables-only']
    assert profile['cmake']['LLVM_USE_SANITIZER'] == 'Address;Undefined'
    assert '-fsanitize=fuzzer-no-link' in profile['cmake']['CMAKE_CXX_FLAGS_RELEASE']
    assert '-fsanitize=fuzzer' not in profile['cmake']['CMAKE_CXX_FLAGS_RELEASE'].split()
    assert {'clang', 'clang-cpp', 'LLVM'} <= set(profile['build']['build_targets'])


def test_fuzzer_and_source_coverage_profiles_reject_weakened_instrumentation(owned):
    repo, manifest, sealed = owned
    path = repo / 'toolchains/llvm/instrumentation-profiles.json'
    document = json.loads(path.read_text())
    document['profiles'][FUZZ_NAME]['cmake']['CMAKE_CXX_FLAGS_RELEASE'] = '-O1 -gline-tables-only'
    path.write_text(json.dumps(document))
    with pytest.raises(ValueError, match='fuzzer profile'):
        profiles.select(repo, FUZZ_NAME, 'linux-x86_64', manifest['build'], sealed, manifest['upstream'])
    shutil.copyfile(ROOT / 'toolchains/llvm/instrumentation-profiles.json', path)
    document = json.loads(path.read_text())
    document['profiles'][COVERAGE_NAME]['cmake']['CMAKE_SHARED_LINKER_FLAGS'] = ''
    path.write_text(json.dumps(document))
    with pytest.raises(ValueError, match='source coverage'):
        profiles.select(repo, COVERAGE_NAME, 'linux-x86_64', manifest['build'], sealed, manifest['upstream'])


def test_sanitizer_profile_rejects_a_built_tool_omitted_from_the_stage(owned):
    repo, manifest, sealed = owned
    path = repo / 'toolchains/llvm/instrumentation-profiles.json'
    document = json.loads(path.read_text())
    document['profiles'][NAME]['additional_distribution_components'].remove('llvm-nm')
    path.write_text(json.dumps(document))
    with pytest.raises(ValueError, match='distribution omits.*llvm-nm'):
        profiles.select(repo, NAME, 'linux-x86_64', manifest['build'], sealed, manifest['upstream'])


def test_source_coverage_profile_owns_lit_support_tools(owned):
    repo, manifest, sealed = owned
    profile = profiles.select(repo, COVERAGE_NAME, 'linux-x86_64', manifest['build'], sealed, manifest['upstream'])
    assert 'llvm-config' in profile['build']['build_targets']
    assert 'llvm-config' in profile['build']['distribution_components']
    assert 'c-index-test' in profile['build']['build_targets']
    assert all('-Wl,--build-id=sha1' in profile['cmake'][key]
               for key in ('CMAKE_EXE_LINKER_FLAGS', 'CMAKE_SHARED_LINKER_FLAGS',
                           'CMAKE_MODULE_LINKER_FLAGS'))


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
    assert instrumented['test']['command_environment']['LD_LIBRARY_PATH'] == (
        instrumented['test']['paths']['build'] + '/lib')
