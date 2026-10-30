"""Run the real regressions with one deliberately broken test-only variant."""
from pathlib import Path
import shutil
import os
import signal
import subprocess

import pytest

ROOT = Path(__file__).resolve().parents[2]
COMPILER = ROOT / 'out/toolchains/objc3-development/stage-linux-x86_64/bin/clang++'
LIBRARY = ROOT / 'out/toolchains/objc3-facade/build-linux-x86_64/src/clang_foundation/runtime/libobjc3_runtime_services.a'
pytestmark = pytest.mark.skipif(not COMPILER.is_file() or not LIBRARY.is_file(),
                                reason='requires a built native runtime and qualified foundation')


@pytest.mark.parametrize('mutant', ['map-count', 'token-rollback'])
def test_semantic_oracle_detects_mutant(tmp_path, mutant):
    includes = tmp_path / 'include'
    includes.mkdir()
    if mutant == 'map-count':
        source = ROOT / 'native/objc3c/tests/stdlib_map_count_test.cpp'
        original = source.read_text()
        changed = original.replace('counts[index] == index + 1', 'counts[index] == index + 2')
        assert changed != original
        broken = tmp_path / 'broken.cpp'
        broken.write_text(changed)
    else:
        source = ROOT / 'native/objc3c/tests/opaque_storage_transaction_test.cpp'
        header = ROOT / 'native/objc3c/src/runtime/stdlib/opaque_service_storage.h'
        original = header.read_text()
        changed = original.replace('tokens_.pop_back();', '(void)tokens_;')
        assert changed != original
        override = includes / 'runtime/stdlib/opaque_service_storage.h'
        override.parent.mkdir(parents=True)
        override.write_text(changed)
        broken = source
    for failure in (False, True):
        binary = tmp_path / ('bad' if failure else 'good')
        command = [str(COMPILER), '--no-default-config', '-std=c++20', '-O2', '-UNDEBUG',
                   *(['-I'+str(includes)] if failure else []),
                   '-I'+str(ROOT / 'native/objc3c/src'), str(broken if failure else source),
                   str(LIBRARY), *(['-Wl,--wrap=_Znwm'] if mutant == 'token-rollback' else []),
                   '-pthread', '-fuse-ld=lld', '-o', str(binary)]
        subprocess.run(command, check=True, capture_output=True, text=True, timeout=60)
        result = subprocess.run([str(binary)], capture_output=True, text=True, timeout=20)
        assert result.returncode == (-signal.SIGABRT if failure else 0), result.stderr
        if failure:
            expected = 'map-count contract failed' if mutant == 'map-count' else 'registry.TokenCount() == 0'
            assert expected in result.stderr


@pytest.mark.parametrize('mutant', ['index-rollback', 'index-repair', 'index-collisions'])
def test_i32_index_oracle_detects_mutant(tmp_path, mutant):
    source = ROOT / 'native/objc3c/src/runtime/stdlib/collections_service.cpp'
    original = source.read_text()
    replacements = {
        'index-rollback': ('catch (...) { values.pop_back(); throw; }', 'catch (...) { throw; }'),
        'index-repair': ('index.find(key(values[i]))->second = i;', '(void)key;'),
        'index-collisions': ('return std::hash<std::int32_t>{}(value);', 'return 0;'),
    }
    before, after = replacements[mutant]
    assert before in original
    changed = tmp_path / 'collections.cpp'
    changed.write_text(original.replace(before, after))
    for failure in (False, True):
        binary = tmp_path / ('bad' if failure else 'good')
        command = [str(COMPILER), '--no-default-config', '-std=c++20', '-O2', '-UNDEBUG',
                   '-DOBJC3_RUNTIME_I32_INDEX_TESTING', '-I'+str(ROOT / 'native/objc3c/src'),
                   str(ROOT / 'native/objc3c/tests/stdlib_map_count_test.cpp'),
                   str(changed if failure else source), '-Wl,--wrap=_Znwm',
                   '-pthread', '-fuse-ld=lld', '-o', str(binary)]
        subprocess.run(command, check=True, capture_output=True, text=True, timeout=60)
        result = subprocess.run([str(binary)], capture_output=True, text=True, timeout=20)
        assert result.returncode == (-signal.SIGABRT if failure else 0), result.stderr
        if failure:
            assert 'map-count contract failed' in result.stderr


@pytest.mark.parametrize('mutant', ['text-validation', 'text-copy', 'text-builder-rollback'])
def test_text_construction_oracle_detects_mutant(tmp_path, mutant):
    source = ROOT / 'native/objc3c/src/runtime/stdlib/text_service.cpp'
    original = source.read_text()
    replacements = {
        'text-validation': (
            'void *handle = TextV2Storage().Store(std::move(value), kTextV2Kind);',
            'int count = 0; TryDecodeUtf8Scalars(value->bytes.data(), value->bytes.size(), count, nullptr);\n'
            '  void *handle = TextV2Storage().Store(std::move(value), kTextV2Kind);'),
        'text-copy': (
            'void *handle = TextV2Storage().Store(std::move(value), kTextV2Kind);',
            'auto redundant = value->bytes; value->bytes.swap(redundant);\n'
            '  void *handle = TextV2Storage().Store(std::move(value), kTextV2Kind);'),
        'text-builder-rollback': (
            'ReserveTextGrowth(value.scalar_offsets, value.scalar_offsets.size() + offset_count - 1,\n'
            '                           kTextV2MaxBytes + 1);', '(void)offset_count;'),
    }
    before, after = replacements[mutant]
    assert before in original
    changed = tmp_path / 'text.cpp'
    changed.write_text(original.replace(before, after))
    for failure in (False, True):
        binary = tmp_path / ('bad' if failure else 'good')
        command = [str(COMPILER), '--no-default-config', '-std=c++20', '-O2', '-UNDEBUG',
                   '-DOBJC3_TEXT_CONSTRUCTION_TESTING', '-I'+str(ROOT / 'native/objc3c/src'),
                   str(ROOT / 'native/objc3c/tests/text_scalar_boundary_test.cpp'),
                   str(changed if failure else source), '-Wl,--wrap=_Znwm',
                   '-pthread', '-fuse-ld=lld', '-o', str(binary)]
        subprocess.run(command, check=True, capture_output=True, text=True, timeout=60)
        result = subprocess.run([str(binary)], capture_output=True, text=True, timeout=20)
        assert result.returncode == (-signal.SIGABRT if failure else 0), result.stderr
        if failure:
            assert 'Assertion' in result.stderr


@pytest.mark.parametrize('mutant', ['group-depth', 'failed-wake'])
def test_runtime_bounds_oracle_detects_mutant(tmp_path, mutant):
    runtime = ROOT / 'native/objc3c/src/runtime'
    sources = [runtime / name for name in (
        'services/error_service.cpp', 'services/carrier_service.cpp',
        'concurrency/service_executor.cpp', 'concurrency/service_task.cpp',
        'concurrency/service_async_join.cpp', 'concurrency/service_group.cpp',
        'concurrency/service_actor.cpp', 'concurrency/service_public.cpp')]
    owner = runtime / ('concurrency/service_group.cpp' if mutant == 'group-depth'
                       else 'concurrency/service_task.cpp')
    original = owner.read_text()
    before, after = (
        ('parent && parent->depth >= OBJC3_TASK_GROUP_MAX_DEPTH', 'false')
        if mutant == 'group-depth' else ('cancel_task(current);', '(void)current;'))
    assert before in original
    changed = tmp_path / owner.name
    changed.write_text(original.replace(before, after))
    for failure in (False, True):
        binary = tmp_path / ('bad' if failure else 'good')
        command = [str(COMPILER), '--no-default-config', '-std=c++20', '-O2', '-UNDEBUG',
                   '-I'+str(ROOT / 'native/objc3c/src'), '-I'+str(runtime / 'concurrency'),
                   '-I'+str(runtime / 'public'),
                   str(ROOT / 'native/objc3c/tests/runtime_lifetime_test.cpp'),
                   *map(str, (changed if failure and source == owner else source for source in sources)),
                   '-pthread', '-fuse-ld=lld', '-o', str(binary)]
        subprocess.run(command, check=True, capture_output=True, text=True, timeout=90)
        result = subprocess.run([str(binary)], capture_output=True, text=True, timeout=20)
        assert result.returncode == (-signal.SIGABRT if failure else 0), result.stderr
        if failure:
            assert ('OBJC3_SERVICE_INVALID_ARGUMENT' if mutant == 'group-depth'
                    else 'destroyed == tasks.size()') in result.stderr


@pytest.mark.skipif(not os.environ.get('OBJC3_TEST_OBJC_RUNTIME'),
                    reason='requires an explicit current GNUstep runtime for the object-lifetime control')
def test_actor_generation_oracle_detects_delayed_cleanup_mutant(tmp_path):
    objc = Path(os.environ['OBJC3_TEST_OBJC_RUNTIME']).resolve(strict=True)
    source = ROOT / 'native/objc3c/src/runtime/concurrency/service_actor_object.cpp'
    original = source.read_text()
    before = ' &&\n      found->second.lifetime == identity'
    assert before in original
    changed = tmp_path / 'service_actor_object.cpp'
    changed.write_text(original.replace(before, ''))
    for failure in (False, True):
        binary = tmp_path / ('bad' if failure else 'good')
        owner_object = tmp_path / ('bad.o' if failure else 'good.o')
        subprocess.run([str(COMPILER), '--no-default-config', '-std=c++20', '-O2',
                        '-I'+str(ROOT / 'native/objc3c/src'),
                        '-I'+str(ROOT / 'native/objc3c/src/runtime/concurrency'),
                        '-c', str(changed if failure else source), '-o', str(owner_object)],
                       check=True, capture_output=True, text=True, timeout=60)
        command = [str(COMPILER), '--no-default-config', '-std=c++20', '-O2', '-UNDEBUG',
                   '-fcoro-aligned-allocation', '-fblocks', '-fobjc-version=3',
                   '-DOBJC3_ACTOR_LIFETIME_INTERNAL_TESTING',
                   '-fobjc-runtime=gnustep-2.2', '-fobjc-arc',
                   '-I'+str(ROOT / 'native/objc3c/src'),
                   '-I'+str(ROOT / 'native/objc3c/src/runtime/concurrency'),
                   '-I'+str(ROOT / 'native/objc3c/src/runtime/public'), '-x', 'objective-c++',
                   str(ROOT / 'tests/clang_foundation/native_async/provider.cpp'),
                   str(ROOT / 'tests/clang_foundation/native_async/actor_lifetime_control.mm'),
                   '-x', 'none', str(owner_object),
                   str(LIBRARY), str(objc), '-Wl,--wrap=_Znwm',
                   '-pthread', '-fuse-ld=lld', '-o', str(binary)]
        subprocess.run(command, check=True, capture_output=True, text=True, timeout=90)
        result = subprocess.run([str(binary)], env={**os.environ, 'LD_LIBRARY_PATH': str(objc.parent)},
                                capture_output=True, text=True, timeout=20)
        assert result.returncode == (-signal.SIGABRT if failure else 0), result.stderr
        if failure:
            assert 'actor_object_lifetime_reuse' in result.stderr
