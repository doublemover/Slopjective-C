"""Verify canonical source/object/archive coverage, including false-evidence controls."""
from pathlib import Path
import json
import os
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain import runtime_instrumentation as profiles

ROOT = Path(__file__).resolve().parents[2]
COMPILER_ROOT = ROOT / 'out/toolchains/objc3-development/stage-linux-x86_64'
RESOURCE = Path(os.environ.get('OBJC3_TEST_SANITIZER_RESOURCE', str(COMPILER_ROOT / 'lib/clang/23')))
OBJC = Path(os.environ.get('OBJC3_TEST_OBJC_RUNTIME_ASAN', '/not-configured'))
READY = (COMPILER_ROOT / 'bin/clang').is_file() and OBJC.is_file() and (RESOURCE.parents[2] / 'bin/llvm-symbolizer').is_file()


def test_unqualified_or_wrong_native_runtime_is_rejected(tmp_path):
    library = tmp_path / 'build/stage/lib/libobjc.so'
    library.parent.mkdir(parents=True)
    library.write_bytes(b'not a runtime')
    report = library.parents[2] / 'build-report.json'
    report.write_text(json.dumps({'status': 'passed', 'configuration': {'sanitizer': 'thread'},
                                 'stage': str(library.parent.parent)}))
    with pytest.raises(ValueError, match='matching qualified instrumentation'):
        profiles._matching_objc_runtime(library, 'address,undefined', tmp_path / 'nm', environment={})


@pytest.mark.skipif(not READY, reason='requires native tools, resources and an explicit qualified ASan GNUstep variant')
@pytest.mark.parametrize('mutant', ['disabled-owned-source', 'missing-compile-command', 'substituted-archive-member'])
def test_canonical_runtime_profile_rejects_false_instrumentation_evidence(tmp_path, monkeypatch, mutant):
    repo = tmp_path / 'repo'
    source = repo / 'native/objc3c/src/runtime'
    shutil.copytree(ROOT / 'native/objc3c/src/runtime', source)
    if mutant == 'disabled-owned-source':
        with (source / 'CMakeLists.txt').open('a') as stream:
            stream.write('\nset_property(SOURCE services/error_service.cpp APPEND PROPERTY COMPILE_OPTIONS -fno-sanitize=all)\n')
    output = repo / 'out/instrumented'
    owner = profiles.RuntimeProfiles(repo, COMPILER_ROOT, RESOURCE, output, jobs=2)
    original_run = subprocess.run
    tampered = []

    def run(argv, **kwargs):
        result = original_run(argv, **kwargs)
        command = list(map(str, argv))
        if result.returncode == 0 and '--build' in command and not tampered:
            build = Path(command[command.index('--build')+1])
            if mutant == 'missing-compile-command':
                path = build / 'compile_commands.json'
                rows = json.loads(path.read_text())
                path.write_text(json.dumps(rows[1:]))
                tampered.append(True)
            if mutant == 'substituted-archive-member':
                plain_source = tmp_path / 'plain.cpp'
                plain_source.write_text('int ordinary_uninstrumented_member() { return 0; }\n')
                plain = tmp_path / 'service_public.cpp.o'
                original_run([owner.tools['compiler'], '--no-default-config', '-O2', '-c',
                              plain_source, '-o', plain], check=True, capture_output=True)
                original_run([owner.tools['ar'], 'r', build / 'libobjc3_runtime_services.a', plain],
                             check=True, capture_output=True)
                tampered.append(True)
        return result

    monkeypatch.setattr(profiles.subprocess, 'run', run)
    expected = {'disabled-owned-source': 'lacks its instrumentation',
                'missing-compile-command': 'omit or add canonical target sources',
                'substituted-archive-member': 'differs from its instrumented object'}[mutant]
    with pytest.raises(ValueError, match=expected):
        owner.get('address,undefined', 'O2', objc_runtime=OBJC)
    receipt = output / 'address-undefined-O2/qualification.json'
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and 'archive_sha256' not in report
    if mutant != 'disabled-owned-source':
        assert tampered == [True]


def test_dependency_symbol_probe_uses_owned_environment(tmp_path, monkeypatch):
    library = tmp_path / 'build/stage/lib/libobjc.so'
    library.parent.mkdir(parents=True)
    library.write_bytes(b'fixture-only library')
    (library.parents[2] / 'build-report.json').write_text(json.dumps({
        'status': 'passed', 'configuration': {'sanitizer': 'address,undefined'},
        'stage': str(library.parent.parent)}))
    monkeypatch.setenv('LD_PRELOAD', '/unapproved')
    monkeypatch.setenv('ASAN_OPTIONS', 'detect_leaks=0')
    environment = profiles.strict_environment(tmp_path / 'clang', tmp_path / 'symbolizer')
    calls = []
    def probe(argv, **kwargs):
        calls.append(argv)
        assert kwargs['env'] == environment and 'LD_PRELOAD' not in kwargs['env']
        assert 'detect_leaks=1' in kwargs['env']['ASAN_OPTIONS']
        return ' U __asan_init\n U __ubsan_handle_type_mismatch_v1\n'
    monkeypatch.setattr(profiles.subprocess, 'check_output', probe)
    profiles._matching_objc_runtime(library, 'address,undefined', tmp_path / 'nm', environment=environment)
    assert len(calls) == 1
