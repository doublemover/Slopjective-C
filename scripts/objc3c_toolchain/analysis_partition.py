"""Stable bounded partitions of the complete dependency-selected TU inventory."""
import re

FULL_PARTITIONS = 8


def partition_spec(index=0, count=1):
    if (type(index) is not int or type(count) is not int or count not in (1, 2, 4, 8) or
            not 0 <= index < count):
        raise ValueError('analysis requires a valid bounded partition')
    return {'index': index, 'count': count}


def partition_rows(rows, partition=None):
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
    return [row for row in rows if int(row['identity'], 16) % partition['count'] == partition['index']]
