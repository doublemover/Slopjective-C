"""Native ARC/MRC ownership mechanisms and independent completed-work controls."""
import json
import os
from pathlib import Path
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain.semantic_runtime_oracles import (
    OWNERSHIP_FAMILIES, expected_work, ownership_flags, runtime_workload_result)

ROOT = Path(__file__).resolve().parents[2]


def runtime_prefix():
    if explicit := os.environ.get('OBJC3_TEST_OWNERSHIP_RUNTIME'):
        return Path(explicit).resolve(strict=True)
    receipt = ROOT / 'out/ownership-runtime-preparation.json'
    if receipt.is_file():
        result = json.loads(receipt.read_text())
        if result.get('status') == 'passed': return Path(result['stage']).resolve(strict=True)
    pytest.skip('requires an explicit pinned GNUstep runtime for native ownership controls; full native SDK lane owns execution')


@pytest.fixture(scope='module', params=('release', 'asan-ubsan'))
def native_ownership(tmp_path_factory, request):
    compiler = shutil.which('clang++')
    if not compiler: pytest.skip('Clang unavailable for native ownership controls')
    runtime = runtime_prefix()
    root = tmp_path_factory.mktemp('native-ownership-controls')
    flags, environment = [], dict(os.environ)
    if request.param == 'asan-ubsan':
        from scripts.objc3c_toolchain.sanitizer_policy import strict_environment
        symbolizer = shutil.which('llvm-symbolizer')
        if not symbolizer: pytest.skip('symbolizer unavailable for native ownership controls')
        flags = ['-fsanitize=address,undefined', '-fno-sanitize-recover=undefined', '-fno-omit-frame-pointer']
        environment = strict_environment(Path(compiler), Path(symbolizer), library_dirs=(runtime / 'lib',))
    try:
        def compile(source, binary, family):
            # This overlap verifies native ownership mechanisms with ordinary
            # Clang. Production benchmark compilation always requires ObjC3;
            # these controls do not qualify an unpatched compiler for ObjC3.
            native_flags = [flag for flag in ownership_flags(family) if flag != '-fobjc-version=3']
            subprocess.run([compiler, '--no-default-config', '-std=c++20', '-O2', '-Wall', '-Wextra', '-Werror',
                *native_flags, *flags, '-I'+str(runtime / 'include'), source, '-x', 'none', runtime / 'lib/libobjc.so',
                '-fuse-ld=lld', '-pthread', '-Wl,-rpath,'+str(runtime / 'lib'), '-o', binary],
                check=True, capture_output=True, env=environment, timeout=60)
        binaries = {}
        for family in OWNERSHIP_FAMILIES:
            binary = root / family;compile(ROOT / 'tests/benchmarks/ownership_workloads.mm', binary, family)
            binaries[family] = binary
        yield root, binaries, compile, environment
    finally:
        shutil.rmtree(root)


@pytest.mark.parametrize('family', OWNERSHIP_FAMILIES)
@pytest.mark.parametrize('scale', [32, 256, 2048])
def test_native_owned_aliases_block_captures_destruction_and_weak_zeroing_complete(native_ownership, family, scale):
    _, binaries, _, environment = native_ownership
    result = subprocess.run([binaries[family], str(scale), '16', '17'], check=True,
                            capture_output=True, text=True, timeout=30, env=environment)
    observed = runtime_workload_result(result.stdout, family, scale)
    assert observed['live_values'] == 0
    assert observed['allocations'] == observed['destructions'] == observed['weak_zeroed'] == 16*scale
    assert observed['retained_aliases'] == observed['released_aliases'] == observed['block_invocations'] == 16*scale


@pytest.mark.parametrize('family', OWNERSHIP_FAMILIES)
def test_native_ownership_delay_control_keeps_all_work_and_adds_bounded_cost(native_ownership, family):
    _, binaries, _, environment = native_ownership
    result = subprocess.run([binaries[family], '32', '16', '17', '1'], check=True,
                            capture_output=True, text=True, timeout=30, env=environment)
    assert runtime_workload_result(result.stdout, family, 32)['nanoseconds'] >= 16_000_000


@pytest.mark.parametrize('family', OWNERSHIP_FAMILIES)
@pytest.mark.parametrize('fault', ['checksum', 'weak-observation', 'partial-rounds'])
def test_sanitizer_clean_wrong_owned_value_work_does_not_look_faster(native_ownership, family, fault):
    root, _, compile, environment = native_ownership
    source = (ROOT / 'tests/benchmarks/ownership_workloads.mm').read_text()
    if fault == 'checksum':
        assert source.count('checksum += object->value;') == 2
        source = source.replace('checksum += object->value;', 'checksum += object->value + 1;')
    elif fault == 'weak-observation':
        assert source.count('++weak_zeroed;++operations;') == 1
        source = source.replace('++weak_zeroed;++operations;', '++operations;')
    else:
        assert source.count('round < rounds;') == 1
        source = source.replace('round < rounds;', 'round + 1 < rounds;')
    mutant = root / (family+'-'+fault+'.mm');mutant.write_text(source)
    binary = root / (family+'-'+fault);compile(mutant, binary, family)
    result = subprocess.run([binary, '32', '16', '17'], check=True,
                            capture_output=True, text=True, timeout=30, env=environment)
    with pytest.raises(ValueError): runtime_workload_result(result.stdout, family, 32)


@pytest.mark.parametrize('family', OWNERSHIP_FAMILIES)
@pytest.mark.parametrize('field', ['allocations', 'destructions', 'retained_aliases', 'released_aliases',
                                  'block_copies', 'block_invocations', 'weak_zeroed'])
def test_ownership_ledger_requires_each_independent_lifetime_boundary(family, field):
    row = {**expected_work(family, 32), 'nanoseconds': 1};row[field] -= 1
    with pytest.raises(ValueError): runtime_workload_result(json.dumps(row), family, 32)
