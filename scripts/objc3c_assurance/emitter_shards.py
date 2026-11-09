"""Verify that bounded coverage shards execute the entire emitter catalogue once."""
import hashlib
import json
from pathlib import Path
import re
import xml.etree.ElementTree as ET

from .evidence import junit

CTEST_OUTPUT_LIMIT = 16 * 1024**2


def shard_count(policy: dict) -> int:
    count = policy.get('emitter_shards')
    if type(count) is not int or not 8 <= count <= 64:
        raise ValueError('emitter coverage requires 8..64 bounded shards')
    selected = [name for name in policy.get('frontend_tests', [])
                if name.startswith('objc3_interface_emitter_coverage_')]
    if selected != [f'objc3_interface_emitter_coverage_{index}' for index in range(count)]:
        raise ValueError('emitter coverage policy must select every shard exactly once')
    return count


SHARDS = shard_count(json.loads((Path(__file__).resolve().parents[2] /
    'toolchains/llvm/source-coverage.json').read_text()))
EMITTER_TESTS = tuple(f'objc3_interface_emitter_coverage_{index}' for index in range(SHARDS))


def emitter_partition(root: Path, suites: list[dict]) -> dict:
    catalogue = None
    binaries = set()
    for index, name in enumerate(EMITTER_TESTS):
        rows = [row for row in suites if row.get('component') == 'frontend' and row.get('name') == name]
        if len(rows) != 1 or rows[0].get('status') != 'PASS':
            raise ValueError('emitter coverage shard is missing or duplicated')
        binaries.add(rows[0]['binary_sha256'])
        path = root / 'suites' / ('frontend-'+name) / 'tests.xml'
        junit(path, expected={name})
        text = '\n'.join(node.text or '' for node in ET.parse(path).iter('system-out'))
        entries = re.findall(r'^CATALOG (\d+) (\w+)$', text, re.M)
        names = [name for ordinal, name in entries]
        if (not entries or [int(ordinal) for ordinal, _ in entries] != list(range(len(entries))) or
                len(names) != len(set(names)) or len(names) < SHARDS or
                re.findall(r'^SHARD (\d+)/(\d+)$', text, re.M) != [(str(index), str(SHARDS))] or
                re.findall(r'^CASE (\w+)$', text, re.M) != names[index::SHARDS] or
                re.findall(r'^PASS (\w+)$', text, re.M) != names[index::SHARDS] or
                re.search(r'^(?:FAIL |PARTIAL )', text, re.M)):
            raise ValueError('emitter coverage shard omitted, duplicated or filtered cases')
        if catalogue is not None and names != catalogue:
            raise ValueError('emitter coverage shards have different catalogues')
        catalogue = names
    if len(binaries) != 1:
        raise ValueError('emitter coverage shards executed different binaries')
    return {'shards': SHARDS, 'cases': len(catalogue),
            'catalogue_sha256': hashlib.sha256(('\n'.join(catalogue)+'\n').encode()).hexdigest()}
