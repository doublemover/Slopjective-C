"""Archive byte ownership remains independent of compiler flag evidence."""
import hashlib

import pytest

from scripts.objc3c_toolchain.compiler_instrumentation import archive_members, verify_owned_archives


def member(name, data):
    header = f"{name:<16}{0:<12}{0:<6}{0:<6}{100644:<8}{len(data):<10}\x60\n".encode()
    assert len(header) == 60
    return header + data + (b"\n" if len(data) % 2 else b"")


def write_archive(tmp_path, contents):
    path = tmp_path / 'owned.a'
    path.write_bytes(b'!<arch>\n' + contents)
    return path


def test_gnu_long_name_and_symbol_tables_preserve_object_bytes(tmp_path):
    name = 'a-long-native-translation-unit.cpp.o'
    payload = b'object payload'
    path = write_archive(tmp_path, member('/', b'symbol table') +
                         member('//', (name+'/\n').encode()) + member('/0', payload))
    expected = hashlib.sha256(payload).hexdigest()
    assert archive_members(path) == [{'name': name, 'sha256': expected, 'bytes': len(payload)}]
    record = {'object': '/build/'+name, 'sha256': expected, 'classification': 'instrumented-object'}
    assert verify_owned_archives([path], [record])[0]['members'][0]['sha256'] == expected
    path.write_bytes(b'!<arch>\n'+member('//', (name+'/\n').encode())+member('/0', b'ordinary object'))
    with pytest.raises(ValueError, match='differs from verified objects'):
        verify_owned_archives([path], [record])


def test_bsd_long_name_is_outside_object_payload(tmp_path):
    name = b'a-bsd-native-translation-unit.cpp.o'
    payload = b'compiler object'
    path = write_archive(tmp_path, member('#1/'+str(len(name)), name+payload))
    assert archive_members(path) == [{'name': name.decode(),
        'sha256': hashlib.sha256(payload).hexdigest(), 'bytes': len(payload)}]


@pytest.mark.parametrize('contents', [b'', member('unit.o/', b'a')*2,
    member('/8', b'object'), member('../escape.o/', b'object'),
    member('#1/100', b'object'), member('unit.o/', b'object')[:-2], b'bad header'])
def test_malformed_or_ambiguous_archive_cannot_qualify(tmp_path, contents):
    path = write_archive(tmp_path, contents)
    with pytest.raises(ValueError):
        archive_members(path)


def test_thin_archive_does_not_bypass_byte_ownership(tmp_path):
    path = tmp_path / 'thin.a'
    path.write_bytes(b'!<thin>\n')
    with pytest.raises(ValueError, match='non-thin'):
        archive_members(path)
