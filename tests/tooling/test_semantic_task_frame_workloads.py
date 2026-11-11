"""Independent generated-frame oracles and explicit sealed-SDK native controls."""
import json
import os
from pathlib import Path
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain.semantic_runtime_oracles import (
    GENERATED_TASK_FAMILIES, completed_task_count, expected_work, runtime_workload_result,
    task_frame_control_result)

ROOT = Path(__file__).resolve().parents[2]


@pytest.mark.parametrize('family', GENERATED_TASK_FAMILIES)
@pytest.mark.parametrize('field', ['frame_leases_created', 'destructions', 'resumes', 'completed_tasks',
                                  'cancelled', 'checksum', 'completed_operations'])
def test_frame_oracle_requires_final_destruction_typed_completion_and_full_work(family, field):
    row = {**expected_work(family, 32), 'nanoseconds': 1000}
    assert completed_task_count(row) == 16*32
    row[field] += 1
    with pytest.raises(ValueError): runtime_workload_result(json.dumps(row), family, 32)


@pytest.mark.parametrize('family', GENERATED_TASK_FAMILIES)
def test_generated_frames_distinguish_successful_payloads_from_cancelled_results(family):
    row = {**expected_work(family, 256), 'nanoseconds': 1}
    assert row['callbacks'] == 0 and completed_task_count(row) == 16*256
    assert row['resumes'] == 2*row['completed_tasks']
    assert row['frame_leases_created'] == row['destructions'] == row['completed_tasks']
    assert row['checksum'] == 16*sum(range(17, 17+256))*(1 if family.endswith('-cancel') else 2)
    assert runtime_workload_result(json.dumps(row), family, 256) == row


@pytest.mark.parametrize('family', GENERATED_TASK_FAMILIES)
@pytest.mark.parametrize('fault,field', [('checksum', 'checksum'), ('frame-destruction', 'destructions'),
                                       ('completion', 'completed_tasks')])
def test_frame_controls_require_the_intended_clean_wrong_code_and_reject_other_failures(family, fault, field):
    correct = {**expected_work(family, 32), 'nanoseconds': 1000}
    wrong = dict(correct);wrong[field] = wrong[field]+512 if fault == 'checksum' else 0
    assert task_frame_control_result(json.dumps(wrong), family, fault) == wrong
    unrelated = dict(wrong);unrelated['live_values'] = 1
    for value in (correct, unrelated, {}, {**wrong, 'nanoseconds': False}):
        with pytest.raises(ValueError): task_frame_control_result(json.dumps(value), family, fault)


@pytest.fixture(scope='module')
def sdk_frames(tmp_path_factory):
    selected = os.environ.get('OBJC3_TEST_SEMANTIC_SDK')
    if not selected:
        pytest.skip('requires an explicit sealed ObjC3 SDK; current bootstrap lacks the production type-identity builtin')
    from scripts.objc3c_toolchain.package import verify_sdk
    from scripts.objc3c_toolchain.sdk_example import environment
    sdk = Path(selected).resolve(strict=True);manifest = verify_sdk(sdk)
    assert manifest['target'] == 'x86_64-unknown-linux-gnu'
    root = tmp_path_factory.mktemp('native-generated-frame-controls')
    env = environment(sdk)
    try:
        def compile(source, binary):
            obj = binary.with_suffix('.o')
            subprocess.run([sdk / 'bin/clang++', '--no-default-config', '-std=c++20', '-O2',
                '-fcoro-aligned-allocation', '-Wall', '-Wextra', '-Werror', '-I'+str(sdk / 'include'),
                '-c', source, '-o', obj], check=True, capture_output=True, env=env, timeout=120)
            subprocess.run([sdk / 'bin/clang++', '--no-default-config', '-fuse-ld=lld', obj,
                sdk / 'lib/libobjc3_runtime_services.a', sdk / 'lib/libobjc.so', '-pthread',
                '-Wl,-rpath,'+str(sdk / 'lib'), '-o', binary], check=True, capture_output=True, env=env, timeout=120)
        binary = root / 'frames';compile(ROOT / 'tests/benchmarks/task_frame_workloads.cpp', binary)
        yield root, binary, compile, env
    finally:
        shutil.rmtree(root)
        assert verify_sdk(sdk)['seal_sha256'] == manifest['seal_sha256']


@pytest.mark.parametrize('family', GENERATED_TASK_FAMILIES)
@pytest.mark.parametrize('scale', [32, 256, 2048])
def test_sdk_generated_frames_complete_and_destroy_before_group_quiescence(sdk_frames, family, scale):
    _, binary, _, env = sdk_frames
    result = subprocess.run([binary, family, str(scale), '16', '17'], check=True,
                            capture_output=True, text=True, env=env, timeout=60)
    row = runtime_workload_result(result.stdout, family, scale)
    assert row['live_values'] == 0


@pytest.mark.parametrize('family', GENERATED_TASK_FAMILIES)
def test_sdk_frame_delay_control_preserves_all_frame_and_result_work(sdk_frames, family):
    _, binary, _, env = sdk_frames
    result = subprocess.run([binary, family, '32', '16', '17', '1'], check=True,
                            capture_output=True, text=True, env=env, timeout=60)
    assert runtime_workload_result(result.stdout, family, 32)['nanoseconds'] >= 16_000_000


@pytest.mark.parametrize('family', GENERATED_TASK_FAMILIES)
@pytest.mark.parametrize('fault', ['checksum', 'frame-destruction', 'completion'])
def test_sdk_frame_semantic_mutants_cannot_pass_with_fast_clean_exits(sdk_frames, family, fault):
    root, _, compile, env = sdk_frames
    source = (ROOT / 'tests/benchmarks/task_frame_workloads.cpp').read_text()
    before, after = {
        'checksum': ('ledger.checksum += value;', 'ledger.checksum += value + 1;'),
        'frame-destruction': ('++ledger.destroys;++ledger.operations;', '++ledger.operations;'),
        'completion': ('++ledger.completed;++ledger.operations;', '++ledger.operations;'),
    }[fault]
    assert source.count(before) == 1
    mutant = root / (family+'-'+fault+'.cpp');mutant.write_text(source.replace(before, after))
    binary = root / (family+'-'+fault);compile(mutant, binary)
    result = subprocess.run([binary, family, '32', '16', '17'], check=True,
                            capture_output=True, text=True, env=env, timeout=60)
    with pytest.raises(ValueError): runtime_workload_result(result.stdout, family, 32)
