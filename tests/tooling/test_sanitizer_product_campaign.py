"""Retain all product-owner outcomes with bounded concurrency and exact profiles."""
import threading
import time
from compression import zstd
import hashlib
import json

import pytest

from scripts.objc3c_toolchain.product_evidence import blob_bytes, consolidate_campaign_blobs
from scripts.objc3c_toolchain.sanitizer_products import execute_owners
from scripts.objc3c_toolchain.runtime_instrumentation import used_receipts


def test_parallel_campaign_keeps_failed_owners_and_completes_independent_work(tmp_path):
    active = peak = 0
    completed = []
    lock = threading.Lock()
    def callback(output):
        nonlocal active, peak
        with lock:
            active += 1
            peak = max(peak, active)
        time.sleep(0.01)
        with lock:
            active -= 1
            completed.append(output.name)
        if output.name == 'broken':
            raise ValueError('retained actual product failure')
        return {'status': 'PASS'}
    tasks = [(name, callback) for name in ('first', 'broken', 'third', 'fourth')]
    report = {'owners': []}
    with pytest.raises(ValueError, match='failed owners'):
        execute_owners(tasks, tmp_path, report, 2)
    assert set(completed) == {name for name, _ in tasks}
    assert 1 < peak <= 2
    assert {row['name'] for row in report['owners']} == set(completed)
    assert next(row for row in report['owners'] if row['name'] == 'broken')['error'] == 'retained actual product failure'


def test_campaign_cannot_succeed_with_empty_or_duplicate_owners(tmp_path):
    for tasks in ([], [('same', None), ('same', None)]):
        with pytest.raises(ValueError, match='unique nonempty'):
            execute_owners(tasks, tmp_path, {'owners': []}, 1)


def test_shared_profile_receipts_describe_only_the_owners_actual_optimizations():
    class Profiles:
        def receipts(self):
            return [{'sanitizer': sanitizer, 'optimization': optimization}
                    for sanitizer in ('address,undefined', 'thread') for optimization in ('O0', 'O1', 'O2')]
    assert len(used_receipts(Profiles(), ('O0', 'O2'))) == 4
    assert len(used_receipts(Profiles(), ('O1',))) == 2


def test_campaign_consolidates_duplicate_replay_bytes_without_rewriting_receipts(tmp_path):
    payloads = (b'shared source bytes', b'owner-specific bytes')
    receipts = []
    for owner, selected in (('one', payloads), ('two', payloads[:1])):
        root = tmp_path / owner / 'product-evidence'
        blobs = root / 'blobs';blobs.mkdir(parents=True)
        records = []
        for payload in selected:
            signature = hashlib.sha256(payload).hexdigest()
            packed = zstd.compress(payload)
            path = blobs / (signature+'.zst');path.write_bytes(packed)
            records.append({'sha256': signature, 'bytes': len(payload), 'encoding': 'zstd',
                'stored_bytes': len(packed), 'stored_sha256': hashlib.sha256(packed).hexdigest(),
                'blob': 'blobs/'+path.name})
        receipt = root / 'qualification.json'
        receipt.write_text(json.dumps({'schema': 'objc3c.sanitizer-product-evidence.v1',
            'status': 'PASS', 'records': records}))
        receipts.append((receipt, receipt.read_bytes(), records, selected))
    result = consolidate_campaign_blobs(tmp_path)
    assert result['owners'] == 2 and result['references'] == 2
    assert result['path'] == 'product-evidence-blobs'
    assert result['private_bytes'] > result['stored_bytes']
    assert len(list((tmp_path / 'product-evidence-blobs').iterdir())) == 2
    for receipt, original, records, selected in receipts:
        assert receipt.read_bytes() == original
        assert (receipt.parent / 'blobs').is_symlink()
        assert [blob_bytes(receipt.parent, row) for row in records] == list(selected)
        (receipt.parent / 'blobs').unlink()
        assert [blob_bytes(receipt.parent, row) for row in records] == list(selected)
