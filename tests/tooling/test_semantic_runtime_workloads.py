"""Independent runtime-work oracles and actual native public-API mechanism controls."""
import json
import os
from pathlib import Path
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain.semantic_runtime_oracles import (
    FAMILIES, OWNERSHIP_FAMILIES, GENERATED_TASK_FAMILIES, expected_work, runtime_workload_result)

ROOT = Path(__file__).resolve().parents[2]


def result(family, scale=32):
    row = {**expected_work(family, scale), 'nanoseconds': 1}
    if family == 'shared-pool-tasks': row['worker_mask'] = 15
    if family == 'generic-array': row.update(payload_copies=3*16*scale, destructions=3*16*scale)
    return row


@pytest.mark.parametrize('family', FAMILIES)
@pytest.mark.parametrize('fault', ['zero-work', 'checksum', 'leaked-values', 'zero-timer', 'missing', 'boolean'])
def test_partial_work_lifetime_mismatch_and_invalid_timing_are_rejected(family, fault):
    row = result(family)
    if fault == 'zero-work': row['completed_operations'] = 0
    elif fault == 'checksum': row['checksum'] += 1
    elif fault == 'leaked-values': row['live_values'] = 1
    elif fault == 'zero-timer': row['nanoseconds'] = 0
    elif fault == 'missing': row.pop('checksum')
    else: row['live_values'] = False
    with pytest.raises(ValueError): runtime_workload_result(json.dumps(row), family, 32)


def test_generic_array_requires_balanced_and_bounded_native_copy_work():
    for copies, destroys in ((1, 1), (100000, 100000), (1024, 1023)):
        row = {**result('generic-array'), 'payload_copies': copies, 'destructions': destroys}
        with pytest.raises(ValueError, match='bounded chunk'):
            runtime_workload_result(json.dumps(row), 'generic-array', 32)


@pytest.mark.parametrize('key,value', [('worker_mask', 0), ('worker_mask', 16),
    ('worker_mask', True), ('pool_workers', 3)])
def test_shared_pool_requires_admitted_physical_workers_and_four_way_parallel_probe(key, value):
    row = {**result('shared-pool-tasks'), key: value}
    with pytest.raises(ValueError): runtime_workload_result(json.dumps(row), 'shared-pool-tasks', 32)


@pytest.fixture(scope='module', params=('release', 'asan-ubsan'))
def native_workloads(tmp_path_factory, request):
    # These value-only C++ runtime controls need neither a sealed compiler fork
    # nor libobjc. Hosted SDK qualification remains a separate evidence boundary.
    compiler = shutil.which('clang++')
    if not compiler: pytest.skip('Clang unavailable for native public-runtime controls')
    cmake, ninja = shutil.which('cmake'), shutil.which('ninja')
    flags, environment = [], dict(os.environ)
    if request.param == 'asan-ubsan':
        from scripts.objc3c_toolchain.sanitizer_policy import strict_environment
        symbolizer = shutil.which('llvm-symbolizer')
        if not symbolizer: pytest.skip('LLVM symbolizer unavailable for sanitizer runtime controls')
        flags = ['-fsanitize=address,undefined', '-fno-sanitize=function,vptr', '-fno-sanitize-recover=undefined',
                 '-fno-omit-frame-pointer']
        environment = strict_environment(Path(compiler), Path(symbolizer))
    root = tmp_path_factory.mktemp('semantic-runtime-controls')
    try:
        build = root / 'build'
        subprocess.run([cmake, '-S', ROOT / 'native/objc3c/src/runtime', '-B', build,
            '-G', 'Ninja', '-DCMAKE_CXX_COMPILER='+compiler, '-DCMAKE_BUILD_TYPE=Release',
            '-DCMAKE_MAKE_PROGRAM='+ninja, '-DCMAKE_CXX_FLAGS='+' '.join(flags),
            '-DBUILD_TESTING=OFF'], check=True, capture_output=True, env=environment)
        subprocess.run([cmake, '--build', build, '--parallel', '2'],
                       check=True, capture_output=True, env=environment)
        include = root / 'include/objc3c/runtime'
        shutil.copytree(ROOT / 'native/objc3c/src/runtime/public', include)
        fixture = ROOT / 'tests/benchmarks/runtime_workloads.cpp'
        def compile(source, output):
            subprocess.run([compiler, '-std=c++20', '-O2', '-Wall', '-Wextra', '-Werror', *flags,
                '-I'+str(root / 'include'), source, build / 'libobjc3_runtime_services.a',
                '-pthread', '-fuse-ld=lld', '-o', output], check=True, capture_output=True, env=environment)
        binary = root / 'runtime-workloads';compile(fixture, binary)
        yield root, binary, compile, environment
    finally:
        shutil.rmtree(root)


@pytest.mark.parametrize('family', [family for family in FAMILIES if family not in (*OWNERSHIP_FAMILIES, *GENERATED_TASK_FAMILIES)])
@pytest.mark.parametrize('scale', [32, 256, 2048])
def test_real_runtime_workloads_complete_all_work_and_teardown(native_workloads, family, scale):
    _, binary, _, environment = native_workloads
    completed = subprocess.run([binary, family, str(scale), '16', '17'],
                               capture_output=True, text=True, timeout=60, env=environment)
    assert completed.returncode == 0, completed.stderr
    row = runtime_workload_result(completed.stdout, family, scale)
    assert row['live_values'] == 0 and row['nanoseconds'] > 0


@pytest.mark.parametrize('family', [family for family in FAMILIES if family not in (*OWNERSHIP_FAMILIES, *GENERATED_TASK_FAMILIES)])
def test_real_runtime_delay_control_preserves_work_and_adds_bounded_cost(native_workloads, family):
    _, binary, _, environment = native_workloads
    completed = subprocess.run([binary, family, '32', '16', '17', '1'],
                               check=True, capture_output=True, text=True, timeout=60, env=environment)
    row = runtime_workload_result(completed.stdout, family, 32)
    assert row['nanoseconds'] >= 16_000_000


@pytest.mark.parametrize('family,before,after', [
    ('text', 'ledger.checksum += byte;', 'ledger.checksum += byte + 1;'),
    ('generic-array', 'destroy_payload(value);++ledger.operations;', '++ledger.operations;'),
    ('task-group-actor', '++ledger.callbacks;', ''),
    ('task-group-cancel', '++ledger.cancelled;', ''),
    ('worker-tasks', 'frame->counts->callbacks.fetch_add(1, std::memory_order_relaxed);', ''),
    ('shared-pool-tasks', 'frame->counts->callbacks.fetch_add(1, std::memory_order_relaxed);', ''),
])
def test_native_missing_work_mutants_cannot_appear_faster(native_workloads, family, before, after):
    root, _, compile, environment = native_workloads
    source = (ROOT / 'tests/benchmarks/runtime_workloads.cpp').read_text()
    assert source.count(before) == 1
    mutant = root / (family+'.cpp');mutant.write_text(source.replace(before, after))
    binary = root / family;compile(mutant, binary)
    completed = subprocess.run([binary, family, '32', '16', '17'],
                               capture_output=True, text=True, timeout=60, env=environment)
    if completed.returncode == 0:
        with pytest.raises(ValueError): runtime_workload_result(completed.stdout, family, 32)
    else:
        assert completed.returncode == 4
