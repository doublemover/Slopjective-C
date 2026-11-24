"""Merge exact-source product partitions into the canonical campaign."""
from __future__ import annotations

import json
from pathlib import Path
import shutil

from scripts.objc3c_toolchain.binary_packages import write_json
from scripts.objc3c_toolchain.product_evidence import blob_bytes
from scripts.objc3c_toolchain.sanitizer_partitions import (
    GROUPS, OWNERS, PARTITION_TASKS, PARTITIONS, SOURCE_FILES, owner_tasks, select,
)
from .evidence import SCHEMA, artifact, digest, native_files, suite_results


def _copy(source: Path, target: Path) -> None:
    target.parent.mkdir(parents=True, exist_ok=True)
    if target.exists():
        if not target.is_file() or target.is_symlink() or digest(source) != digest(target):
            raise ValueError('sanitizer partitions disagree on a shared artifact: '+str(target))
    else:
        try:
            target.hardlink_to(source)
        except OSError:
            shutil.copyfile(source, target)


def merge(plan: dict, partials: Path, output: Path, *, repo: Path | None = None) -> Path | None:
    found = {}
    for path in partials.rglob('result.json'):
        document = json.loads(path.read_text())
        group = document.get('release_matrix_group', '')
        if group.startswith('asan-products-partition-'):
            if group in found:
                raise ValueError('duplicate sanitizer partition receipt: '+group)
            found[group] = (path.parent, document)
    if not found:
        return None
    if set(found) != set(GROUPS):
        raise ValueError('sanitizer partition union is incomplete or contains an unknown partition')
    if any(json.loads(path.read_text()).get('release_matrix_group') == 'asan-products'
           for path in partials.rglob('result.json')):
        raise ValueError('whole and partitioned sanitizer campaigns cannot be mixed')
    output.mkdir(parents=True, exist_ok=False)
    suite = output / 'sanitizer-products'
    campaign = suite / 'campaign'
    store = campaign / 'product-evidence-blobs'
    store.mkdir(parents=True)
    product = source_inputs = None
    owners, profiles, commands, proof = {}, {}, [], []
    elapsed = private_bytes = peak = 0
    partition_profiles = []
    for index, group in enumerate(GROUPS):
        root, receipt = found[group]
        if (receipt.get('schema') != SCHEMA or receipt.get('status') != 'PASS' or
                receipt.get('revision') != plan['revision'] or receipt.get('plan_sha256') != plan['sha256'] or
                receipt.get('suite_partition') != ['sanitizer-products'] or
                receipt.get('sanitizer_product_partition') != index):
            raise ValueError('sanitizer partition is failed, stale or substituted: '+group)
        rows = receipt.get('suites', [])
        if len(rows) != 1 or rows[0].get('id') != 'sanitizer-products' or rows[0].get('status') != 'PASS':
            raise ValueError('sanitizer partition suite inventory changed: '+group)
        row = rows[0]
        executions = suite_results(root, 'sanitizer-products')
        report = json.loads((root / 'sanitizer-products/campaign/qualification.json').read_text())
        if report.get('partition_index') != index or row.get('executions') != executions:
            raise ValueError('sanitizer partition execution identity changed: '+group)
        expected = set(select(index))
        if set(owners) & expected or {entry['name'] for entry in executions} != expected:
            raise ValueError('sanitizer partition owners overlap or differ from their catalogue')
        if product is None:
            product, source_inputs = receipt.get('product'), report.get('source_inputs')
        if not product or receipt.get('product') != product or not source_inputs or report.get('source_inputs') != source_inputs:
            raise ValueError('sanitizer partitions used different SDK/compiler source identities')
        if repo is not None:
            normalized = {}
            for source_name, signature in source_inputs.items():
                relative = source_name if source_name.startswith('scripts/') else 'scripts/'+source_name.rsplit('/scripts/', 1)[-1]
                if relative not in SOURCE_FILES or relative in normalized:
                    raise ValueError('sanitizer partition source inventory is not canonical')
                source = repo / relative
                if not source.is_file() or source.is_symlink() or digest(source) != signature:
                    raise ValueError('sanitizer partition source inputs differ from the checked-out revision')
                normalized[relative] = signature
            if set(normalized) != set(SOURCE_FILES):
                raise ValueError('sanitizer partition source inventory is incomplete')
        inventory = {path.relative_to(root).as_posix() for path in native_files(root / 'sanitizer-products')}
        if {item['path'] for item in row.get('artifacts', [])} != inventory:
            raise ValueError('sanitizer partition artifact inventory is incomplete')
        for item in row['artifacts']:
            source = root / item['path']
            if artifact(root, source) != item:
                raise ValueError('sanitizer partition artifact changed')
            relative = source.relative_to(root / 'sanitizer-products')
            parts = relative.parts
            if parts[:2] == ('campaign', 'product-evidence-blobs'):
                target = store / parts[-1]
            elif len(parts) > 1 and parts[0] == 'campaign' and parts[1] in expected:
                target = suite / relative
            elif parts[0] == 'foundations':
                target = suite / relative
            else:
                target = campaign / 'partition-evidence' / str(index) / relative
            _copy(source, target)
        owners.update({entry['name']: entry for entry in executions})
        for profile in report['runtime_profiles']:
            profiles.setdefault((profile['sanitizer'], profile['optimization']), profile)
        partition_profiles.append({'partition': index, 'profiles': report['runtime_profiles']})
        private_bytes += report['product_evidence_store']['private_bytes']
        elapsed += float(receipt.get('elapsed_seconds', 0))
        peak = max(peak, int(receipt.get('peak_child_rss_kib', 0)))
        commands.extend(receipt.get('commands', []))
        proof.append({'group': group, 'result_sha256': digest(root / 'result.json'),
                      'campaign_sha256': digest(root / 'sanitizer-products/campaign/qualification.json'),
                      'owners': list(select(index))})
    if set(owners) != set(PARTITION_TASKS):
        raise ValueError('sanitizer disjoint union differs from the complete transport catalogue')
    complete = []
    for name in OWNERS:
        tasks = owner_tasks(name)
        if len(tasks) == 1:
            complete.append(owners[name])
            continue
        selected = [owners[task] for task in tasks]
        complete.append({'name': name, 'status': 'PASS', 'transport_results': selected,
                         'seconds': sum(row['seconds'] for row in selected)})
    receipts = sorted(campaign.glob('**/product-evidence/qualification.json'))
    verified = set()
    def verify(value, root):
        if isinstance(value, dict):
            if 'blob' in value:
                key = (str(root), *(value.get(name) for name in
                    ('blob', 'sha256', 'bytes', 'encoding', 'stored_bytes', 'stored_sha256')))
                if key not in verified:
                    blob_bytes(root, value)
                    verified.add(key)
            for child in value.values():
                verify(child, root)
        elif isinstance(value, list):
            for child in value:
                verify(child, root)
    for path in receipts:
        verify(json.loads(path.read_text()), path.parent)
    stored_bytes = sum(path.stat().st_size for path in store.iterdir())
    store_report = {'schema': 'objc3c.product-evidence-shared-store.v1', 'owners': len(receipts),
        'references': len(list(store.iterdir())), 'private_bytes': private_bytes,
        'stored_bytes': stored_bytes, 'deduplicated_bytes': private_bytes-stored_bytes,
        'path': 'product-evidence-blobs'}
    write_json(campaign / 'partition-union.json', {'status': 'PASS', 'partitions': proof,
        'product': product, 'source_inputs': source_inputs, 'owners': list(OWNERS),
        'partition_tasks': list(PARTITION_TASKS)})
    write_json(campaign / 'qualification.json', {'schema': 'objc3c.sanitizer-products-campaign.v1',
        'status': 'PASS', 'owners': complete, 'expected_owners': list(OWNERS),
        'partition_index': None, 'partition_count': len(PARTITIONS), 'owner_catalogue': list(OWNERS),
        'partition_tasks': list(PARTITION_TASKS),
        'runtime_profiles': list(profiles.values()), 'partition_runtime_profiles': partition_profiles,
        'source_inputs': source_inputs, 'product_evidence_store': store_report})
    executions = suite_results(output, 'sanitizer-products')
    combined = {'schema': SCHEMA, 'status': 'PASS', 'revision': plan['revision'], 'plan_sha256': plan['sha256'],
        'release_matrix_group': 'asan-products', 'suite_partition': ['sanitizer-products'],
        'product': product, 'commands': commands, 'elapsed_seconds': elapsed,
        'peak_child_rss_kib': peak, 'suites': [{'id': 'sanitizer-products', 'status': 'PASS',
            'executions': executions, 'artifacts': [artifact(output, path) for path in native_files(suite)]}]}
    write_json(output / 'result.json', combined)
    return output
