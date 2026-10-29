"""Run the real regressions with one deliberately broken test-only variant."""
from pathlib import Path
import shutil
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
