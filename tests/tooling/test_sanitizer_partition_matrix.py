"""Defend distributed owner selection, identity, completeness and byte replay."""
from compression import zstd
import hashlib
import json
from pathlib import Path
import shutil
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import sanitizer_products as products
from scripts.objc3c_toolchain.sanitizer_partitions import (
    CASE_RECEIPT, GROUPS, OWNERS, PARTITION_TASKS, PARTITIONS, SOURCE_FILES, SPLIT_FEATURES,
    TASK_OWNERS, evidence_owners, feature_cases, feature_receipt, feature_scope, owner_tasks, profiles, select,
)
from scripts.objc3c_assurance import evidence
from scripts.objc3c_assurance.sanitizer_matrix import merge


def test_owner_partitions_are_a_complete_disjoint_union_and_use_only_needed_profiles():
    assigned = [owner for index in range(len(PARTITIONS)) for owner in select(index)]
    assert len(PARTITIONS) == 10
    assert len(assigned) == len(set(assigned)) == len(PARTITION_TASKS) == 16
    assert {TASK_OWNERS[task] for task in assigned} == set(select()) == set(OWNERS)
    assert len(OWNERS) == 13
    assert len(profiles(select())) == 6
    assert [len(profiles(part)) for part in PARTITIONS] == [4, 1, 1, 2, 1, 1, 4, 1, 1, 1]
    assert sum(evidence_owners(part) for part in PARTITIONS) == 17
    assert evidence_owners(select()) == 14
    for bad in (-1, len(PARTITIONS), True, '0'):
        with pytest.raises(ValueError, match='partition'):
            select(bad)


@pytest.mark.parametrize('owner', SPLIT_FEATURES)
def test_feature_transports_are_the_exact_disjoint_local_case_inventory(owner):
    source, pch = owner_tasks(owner)
    all_cases = set(feature_cases(feature_scope(owner)))
    source_cases, pch_cases = (set(feature_cases(feature_scope(task))) for task in (source, pch))
    assert len(source_cases) == len(pch_cases) == 60
    assert source_cases.isdisjoint(pch_cases)
    assert source_cases | pch_cases == all_cases


def test_qualification_selects_exact_owners_without_building_unused_profiles(tmp_path, monkeypatch):
    repo = tmp_path / 'repo';repo.mkdir()
    sdk = tmp_path / 'sdk';(sdk / 'bin').mkdir(parents=True)
    for name in ('clang', 'clang++', 'ld.lld', 'llvm-readobj', 'llvm-dwarfdump'):
        (sdk / 'bin' / name).touch()
    (sdk / 'share/objc3c/examples/ownership-projection').mkdir(parents=True)
    (sdk / 'objc3-sdk.json').write_text('{}')
    resource = tmp_path / 'resource';resource.mkdir()
    facade = tmp_path / 'facade';facade.touch()
    for name in SOURCE_FILES:
        source = repo / name;source.parent.mkdir(parents=True, exist_ok=True);source.write_bytes(b'sealed source')
    monkeypatch.setattr(products, 'campaign_sources', lambda root: [root / name for name in SOURCE_FILES])
    monkeypatch.setattr(products, 'verify_sdk', lambda _: None)
    monkeypatch.setattr(products, 'consolidate_campaign_blobs', lambda _: {})
    requested = set()
    class Profiles:
        def __init__(self, *args):
            pass
        def get(self, mode, opt):
            requested.add((mode, opt))
            return SimpleNamespace(archive=tmp_path / 'runtime.a', objc_runtime=tmp_path / 'libobjc.so')
        def receipts(self):
            return [{'sanitizer': mode, 'optimization': opt} for mode, opt in sorted(requested)]
    monkeypatch.setattr(products, 'RuntimeProfiles', Profiles)
    executed = []
    def execute(tasks, output, report, jobs):
        executed.extend(name for name, _ in tasks)
        report['owners'] = [{'name': name, 'status': 'PASS'} for name, _ in tasks]
    monkeypatch.setattr(products, 'execute_owners', execute)
    for index in range(len(PARTITIONS)):
        requested.clear();executed.clear()
        selected = select(index)
        result = products.qualify(repo, sdk, sdk, facade if any(name.startswith('factories-') for name in selected) else None,
            resource, repo / 'out' / str(index), partition=index)
        assert result['status'] == 'PASS' and set(executed) == set(selected)
        assert requested == set(profiles(selected))
    with pytest.raises(ValueError, match='instrumented facade'):
        products.qualify(repo, sdk, sdk, None, resource, repo / 'out/missing-facade', partition=6)


def feature_batch(output, task):
    names = feature_cases(feature_scope(task))
    payload = b'passing case transcript\n'
    signature = hashlib.sha256(payload).hexdigest()
    for name in names:
        (output / (name+'-execute.log')).write_bytes(payload)
    report = {'cases': [{'name': name, 'status': 'pass'} for name in names],
              'commands': [{'name': name+'-execute', 'status': 'PASS', 'phase': 'execute',
                            'exit_code': 0, 'timed_out': False, 'log_sha256': signature} for name in names],
              'status': 'pass', 'sanitizer': feature_scope(task)['sanitizer']}
    (output / 'batch.json').write_text(json.dumps(report))
    (output / CASE_RECEIPT).write_text(json.dumps(feature_receipt(task, output, report)))


def partition_receipts(tmp_path):
    root = tmp_path / 'partials';root.mkdir()
    plan = {'revision': {'tested': 'f'*40}, 'sha256': 'e'*64}
    payload = b'exact retained native artifact bytes'
    signature = hashlib.sha256(payload).hexdigest();packed = zstd.compress(payload)
    record = {'sha256': signature, 'bytes': len(payload), 'encoding': 'zstd',
        'stored_bytes': len(packed), 'stored_sha256': hashlib.sha256(packed).hexdigest(),
        'blob': 'blobs/'+signature+'.zst'}
    for index, group in enumerate(GROUPS):
        output = root / group;campaign = output / 'sanitizer-products/campaign'
        store = campaign / 'product-evidence-blobs';store.mkdir(parents=True)
        (store / (signature+'.zst')).write_bytes(packed)
        for owner in select(index):
            scopes = ('c-frame-bridge/address-undefined', 'c-frame-bridge/thread') if owner == 'c-frame-bridge' else (owner,)
            for scope in scopes:
                receipt = campaign / scope / 'product-evidence/qualification.json'
                receipt.parent.mkdir(parents=True)
                receipt.write_text(json.dumps({'schema': 'objc3c.sanitizer-product-evidence.v1',
                    'status': 'PASS', 'records': [record]}))
            if owner.startswith('features-'):
                feature_batch(campaign / owner, owner)
        count = evidence_owners(select(index))
        qualification = {'status': 'PASS', 'owners': [{'name': name, 'status': 'PASS', 'seconds': 1.0} for name in select(index)],
            'expected_owners': list(select(index)), 'partition_index': index, 'partition_count': len(PARTITIONS),
            'owner_catalogue': list(OWNERS), 'source_inputs': {name: hashlib.sha256(b'sealed source').hexdigest() for name in SOURCE_FILES},
            'runtime_profiles': [{'sanitizer': mode, 'optimization': opt} for mode, opt in profiles(select(index))],
            'product_evidence_store': {'schema': 'objc3c.product-evidence-shared-store.v1',
                'path': 'product-evidence-blobs', 'owners': count, 'references': 1,
                'private_bytes': len(packed)*count, 'stored_bytes': len(packed),
                'deduplicated_bytes': len(packed)*(count-1)}}
        (campaign / 'qualification.json').write_text(json.dumps(qualification))
        report = {'schema': evidence.SCHEMA, 'status': 'PASS', 'revision': plan['revision'],
            'plan_sha256': plan['sha256'], 'release_matrix_group': group,
            'suite_partition': ['sanitizer-products'], 'sanitizer_product_partition': index,
            'product': {'source_build_id': 'source-build', 'seal_sha256': 'c'*64, 'archive_sha256': 'b'*64},
            'suites': [{'id': 'sanitizer-products', 'status': 'PASS',
                'executions': evidence.suite_results(output, 'sanitizer-products'),
                'artifacts': [evidence.artifact(output, path) for path in evidence.native_files(output / 'sanitizer-products')]}]}
        (output / 'result.json').write_text(json.dumps(report))
    return root, plan, record


def test_terminal_union_retains_each_real_replay_byte_once_without_changing_owner_receipts(tmp_path):
    root, plan, record = partition_receipts(tmp_path)
    original = {path.relative_to(root).as_posix(): path.read_bytes()
                for path in root.glob('**/product-evidence/qualification.json')}
    output = merge(plan, root, tmp_path / 'merged')
    assert len(evidence.suite_results(output, 'sanitizer-products')) == 13
    campaign = output / 'sanitizer-products/campaign'
    assert len(list((campaign / 'product-evidence-blobs').iterdir())) == 1
    assert len(json.loads((campaign / 'partition-union.json').read_text())['partitions']) == len(PARTITIONS)
    result = json.loads((campaign / 'qualification.json').read_text())
    assert result['partition_tasks'] == list(PARTITION_TASKS)
    for row in result['owners']:
        if row['name'] in SPLIT_FEATURES:
            assert tuple(part['name'] for part in row['transport_results']) == owner_tasks(row['name'])
    from scripts.objc3c_toolchain.product_evidence import blob_bytes
    for path in campaign.glob('**/product-evidence/qualification.json'):
        assert path.read_bytes() in original.values()
        assert blob_bytes(path.parent, record) == b'exact retained native artifact bytes'


@pytest.mark.parametrize('mutation', ['missing', 'duplicate', 'product', 'source', 'index', 'blob'])
def test_terminal_union_rejects_incomplete_substituted_or_changed_evidence(tmp_path, mutation):
    root, plan, record = partition_receipts(tmp_path)
    first = root / GROUPS[0]
    if mutation == 'missing':
        shutil.rmtree(root / GROUPS[-1])
    elif mutation == 'duplicate':
        shutil.copytree(first, root / 'duplicate' / GROUPS[0])
    elif mutation == 'product':
        path = first / 'result.json';data = json.loads(path.read_text());data['product']['seal_sha256'] = 'd'*64;path.write_text(json.dumps(data))
    elif mutation in ('source', 'index'):
        path = first / 'sanitizer-products/campaign/qualification.json';data = json.loads(path.read_text())
        if mutation == 'source':
            data['source_inputs'][SOURCE_FILES[0]] = 'd'*64
        else:
            data['partition_index'] = 1
        path.write_text(json.dumps(data))
    else:
        (first / 'sanitizer-products/campaign/product-evidence-blobs' / Path(record['blob']).name).write_bytes(b'changed')
    with pytest.raises(ValueError):
        merge(plan, root, tmp_path / 'merged')


@pytest.mark.parametrize('fault', ('wrong-transport', 'missing', 'duplicate', 'failed', 'wrong-profile',
                                   'changed-batch', 'changed-log'))
def test_terminal_requires_every_passing_case_in_each_assigned_transport(tmp_path, fault):
    root, plan, _ = partition_receipts(tmp_path)
    task = 'features-address-undefined-O2-source'
    group = GROUPS[next(index for index, part in enumerate(PARTITIONS) if task in part)]
    path = root / group / 'sanitizer-products/campaign' / task / CASE_RECEIPT
    receipt = json.loads(path.read_text())
    if fault == 'wrong-transport':
        receipt['transports'] = ['pch']
    elif fault == 'missing':
        receipt['cases'].pop()
    elif fault == 'duplicate':
        receipt['cases'][1] = receipt['cases'][0]
    elif fault == 'failed':
        receipt['cases'][0]['status'] = 'FAIL'
    elif fault == 'wrong-profile':
        receipt['sanitizer'] = 'thread'
    elif fault == 'changed-batch':
        (path.parent / 'batch.json').write_text('{}')
    else:
        (path.parent / (receipt['cases'][0]['name']+'-execute.log')).write_text('substituted output')
    path.write_text(json.dumps(receipt))
    with pytest.raises(ValueError, match='sanitizer feature transport or case identity|case outcome or log identity'):
        merge(plan, root, tmp_path / 'merged')


def test_folded_logical_owner_requires_both_passing_transport_results(tmp_path):
    root, plan, _ = partition_receipts(tmp_path)
    merged = merge(plan, root, tmp_path / 'merged')
    path = merged / 'sanitizer-products/campaign/qualification.json'
    receipt = json.loads(path.read_text())
    owner = next(row for row in receipt['owners'] if row['name'] == SPLIT_FEATURES[0])
    owner['transport_results'][1]['status'] = 'FAIL'
    path.write_text(json.dumps(receipt))
    with pytest.raises(ValueError, match='incomplete transport union'):
        evidence.suite_results(merged, 'sanitizer-products')


def test_core_aggregate_consumes_the_canonical_union_and_removes_temporary_merge_tree(tmp_path, monkeypatch):
    from scripts.objc3c_assurance import release_matrix
    partials, plan, _ = partition_receipts(tmp_path)
    repo = tmp_path / 'repo';(repo / 'out').mkdir(parents=True)
    for name in SOURCE_FILES:
        source = repo / name;source.parent.mkdir(parents=True, exist_ok=True);source.write_bytes(b'sealed source')
    monkeypatch.setattr(release_matrix, 'validate', lambda _: None)
    monkeypatch.setattr(release_matrix, '_require_checkout', lambda *_: None)
    monkeypatch.setattr(release_matrix, 'selected_suites', lambda *_: [{'id': 'sanitizer-products'}])
    monkeypatch.setattr(release_matrix, 'GROUPS', {'asan-products': ('sanitizer-products',)})
    monkeypatch.setattr(release_matrix, 'foundation_receipts', lambda *_: [])
    monkeypatch.setattr(release_matrix, 'snapshot', lambda **_: {'status': 'PASS'})
    admitted = []
    monkeypatch.setattr(release_matrix, 'aggregate', lambda *args, **kwargs: admitted.append(kwargs['shard']))
    output = repo / 'out/results'
    result = release_matrix.aggregate_core(repo, plan, partials, output)
    assert result['status'] == 'PASS' and admitted == ['core']
    assert len(result['suites']) == 1 and len(result['suites'][0]['executions']) == 13
    assert (output / 'sanitizer-products/campaign/partition-union.json').is_file()
    assert not (output / '_sanitizer_union').exists()
    assert len(evidence.suite_results(output, 'sanitizer-products')) == 13


def test_workflow_enrolls_every_partition_and_the_existing_schedule_owner():
    repo = Path(__file__).resolve().parents[2]
    workflow = (repo / '.github/workflows/native-assurance.yml').read_text()
    consumer = workflow.split('  instrumented-product-consumers:', 1)[1].split('\n  independent-native:', 1)[0]
    line = next(line.strip() for line in consumer.splitlines() if line.strip().startswith('group: ['))
    groups = [value.strip() for value in line.removeprefix('group: [').removesuffix(']').split(',')]
    assert groups == [*GROUPS, 'runtime-schedules']
    assert 'fail-fast: true' in consumer
    gate = workflow.split('  gate:', 1)[1]
    assert 'instrumented-product-consumers' in gate
    assert "INSTRUMENTED_CONSUMERS_RESULT: ${{ needs.instrumented-product-consumers.result }}" in gate
    assert gate.index('release_matrix aggregate') < gate.index('verify --sharded')


def test_campaign_sources_cannot_claim_a_different_checkout(tmp_path):
    root = Path(__file__).resolve().parents[2]
    assert {path.relative_to(root).as_posix() for path in products.campaign_sources(root)} == set(SOURCE_FILES)
    for name in SOURCE_FILES:
        source = tmp_path / name;source.parent.mkdir(parents=True, exist_ok=True);source.write_bytes(b'another checkout')
    with pytest.raises(ValueError, match='another source checkout'):
        products.campaign_sources(tmp_path)
