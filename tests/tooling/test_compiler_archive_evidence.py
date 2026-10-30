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


def test_repeated_basenames_need_exact_target_ownership_not_a_global_hash_bag(tmp_path):
    first, second, unrelated = b'first native object', b'second native object', b'other target object'
    path = write_archive(tmp_path, member('unit.cpp.o/', first)+member('unit.cpp.o/', second))
    records = [{'object': '/build/'+directory+'/unit.cpp.o', 'classification': 'instrumented-object',
                'sha256': hashlib.sha256(data).hexdigest()}
               for directory, data in [('first', first), ('second', second), ('unrelated', unrelated)]]
    owners = {str(path): [row['object'] for row in records[:2]]}
    with pytest.raises(ValueError, match='ambiguous'):
        verify_owned_archives([path], records)
    assert len(verify_owned_archives([path], records, owners=owners)[0]['members']) == 2
    path.write_bytes(b'!<arch>\n'+member('unit.cpp.o/', first)+member('unit.cpp.o/', unrelated))
    with pytest.raises(ValueError, match='canonical target inputs'):
        verify_owned_archives([path], records, owners=owners)
    path.write_bytes(b'!<arch>\n'+member('unit.cpp.o/', first))
    with pytest.raises(ValueError, match='canonical target inputs'):
        verify_owned_archives([path], records, owners=owners)


def test_native_ninja_archive_inputs_disambiguate_repeated_source_names(tmp_path):
    import json
    import shlex
    import subprocess
    from pathlib import Path
    from scripts.objc3c_toolchain.compiler_instrumentation import archive_owners
    source = tmp_path / 'source'; source.mkdir()
    build = tmp_path / 'build with spaces'
    for name, value in [('first', 1), ('second', 2)]:
        (source / name).mkdir()
        (source / name / 'unit.cpp').write_text('int '+name+'() { return '+str(value)+'; }')
    (source / 'CMakeLists.txt').write_text('cmake_minimum_required(VERSION 3.24)\nproject(archive_control CXX)\n'
        'set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\nadd_library(owned STATIC first/unit.cpp second/unit.cpp)\n')
    subprocess.run(['cmake','-S',str(source),'-B',str(build),'-G','Ninja'], check=True, capture_output=True)
    subprocess.run(['cmake','--build',str(build)], check=True, capture_output=True)
    records = []
    for row in json.loads((build / 'compile_commands.json').read_text()):
        argv = shlex.split(row['command'])
        obj = (build / argv[argv.index('-o')+1]).resolve()
        records.append({'object': str(obj), 'sha256': hashlib.sha256(obj.read_bytes()).hexdigest(),
                        'classification': 'instrumented-object'})
    archive = build / 'libowned.a'
    def run(name, argv):
        return subprocess.check_output(list(map(str, argv)), text=True)
    owners = archive_owners(build, [archive], Path('ninja'), run=run)
    result = verify_owned_archives([archive], records, owners=owners)
    assert len(result[0]['members']) == 2 and {row['name'] for row in result[0]['members']} == {'unit.cpp.o'}
    owners[str(archive)].reverse()
    with pytest.raises(ValueError, match='canonical target inputs'):
        verify_owned_archives([archive], records, owners=owners)
