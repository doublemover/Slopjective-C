"""Stable cost-balanced partitions of the dependency-selected TU inventory."""
from pathlib import Path
import re

FULL_PARTITIONS = 8


def balance_indices(durations, count):
    """Place the longest tasks first on the least loaded worker."""
    loads = [0] * count
    bins = [[] for _ in loads]
    for item in sorted(range(len(durations)), key=lambda index: -durations[index]):
        index = min(range(count), key=loads.__getitem__)
        loads[index] += durations[item]
        bins[index].append(item)
    return bins


def partition_spec(index=0, count=1):
    if (type(index) is not int or type(count) is not int or count not in (1, 2, 4, 8) or
            not 0 <= index < count):
        raise ValueError('analysis requires a valid bounded partition')
    return {'index': index, 'count': count}


def partition_rows(rows, partition=None, *, seconds=None):
    partition = partition_spec() if partition is None else partition
    if type(partition) is not dict or set(partition) != {'index','count'}:
        raise ValueError('analysis partition requires index and count')
    if partition != partition_spec(partition['index'], partition['count']):
        raise ValueError('analysis partition contains unknown fields')
    identities = [row['identity'] for row in rows]
    if (len(set(identities)) != len(identities) or
            any(not isinstance(value, str) or not re.fullmatch('[0-9a-f]{64}', value)
                for value in identities)):
        raise ValueError('analysis partition requires unique canonical TU identities')
    if seconds is None:
        seconds = {'default': 1}
    if (not isinstance(seconds, dict) or 'default' not in seconds or
            any(type(value) is not int or value <= 0 for value in seconds.values())):
        raise ValueError('analysis partition costs must be positive seconds')
    if partition['count'] == 1:
        return rows
    canonical = sorted(rows, key=lambda row: row['identity'])
    durations = [seconds.get(Path(row.get('file', '')).name, seconds['default']) for row in canonical]
    bins = balance_indices(durations, partition['count'])
    return [canonical[index] for index in bins[partition['index']]]
