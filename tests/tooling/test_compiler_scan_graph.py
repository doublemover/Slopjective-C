"""CMake P1689 scanning cannot count as object compilation coverage."""
import json
from pathlib import Path
import shlex

import pytest

from scripts.objc3c_toolchain.compiler_instrumentation import graph_records


@pytest.mark.parametrize('fault', ['good', 'scanner-only', 'wrong-output', 'unknown-format',
                                  'missing-command', 'duplicate-compile'])
def test_dependency_scan_is_separate_from_the_real_compiler_edge(tmp_path, fault):
    source = tmp_path / 'source with spaces'; source.mkdir()
    build = tmp_path / 'build with spaces'; build.mkdir()
    unit = source / 'unit.cpp'; unit.write_text('int value;')
    output = build / 'unit.cpp.o'; output.write_bytes(b'verified separately by symbol evidence')
    compiler = tmp_path / 'clang++'; compiler.write_bytes(b'compiler')
    scanner = tmp_path / 'clang-scan-deps'; scanner.write_bytes(b'scanner')
    flags = ['-fsanitize=address,undefined', '-fno-sanitize-recover=undefined',
             '-fno-omit-frame-pointer', '-gline-tables-only']
    actual = [str(compiler), *flags, '-c', str(unit), '-o', str(output)]
    dependency = build / 'unit.cpp.o.ddi'
    dependency.write_text(json.dumps({'rules': [{'primary-output': str(output if fault != 'wrong-output' else build/'other.o')}]}))
    scan = [str(scanner), '-format=p1689' if fault != 'unknown-format' else '-format=unadmitted',
            '--', str(compiler), *flags, '-c', '-o', str(output), '-MT', str(dependency), str(unit)]
    commands = [scan] + ([] if fault == 'scanner-only' else [actual])
    if fault == 'duplicate-compile': commands.append(actual)
    (build / 'compile_commands.json').write_text(json.dumps([] if fault == 'missing-command' else
        [{'directory': str(build), 'file': str(unit), 'arguments': actual}]))
    def run(name, argv):
        return '\n'.join(shlex.join(args) for args in commands)
    if fault != 'good':
        with pytest.raises(ValueError):
            graph_records(build, source, ['unit'], Path('ninja'), {}, foundation=False, run=run)
        return
    records = graph_records(build, source, ['unit'], Path('ninja'), {}, foundation=False, run=run)
    assert len(records) == 1 and records[0]['classification'] == 'instrumented-object'
    assert records[0]['dependency_scan']['output'] == str(dependency)
    assert records[0]['dependency_scan']['tool'] == str(scanner)
    assert records[0]['argv'] == actual
