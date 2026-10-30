"""Qualify the isolated compiler implementation, independently of client checks."""
from __future__ import annotations

import argparse
import hashlib
from datetime import datetime, timezone
import json
import os
from pathlib import Path
import shlex
import subprocess
import time

from .binary_packages import digest, write_json
from .core import build_plans
from .frontend_tests import qualify as frontend_tests
from .sanitizer_policy import SanitizerQualification, require_clean_diagnostic, validate_detection

PROFILE = 'compiler-asan-ubsan'
# Assembly has no C/C++ sanitizer instrumentation; these exact upstream kernels
# are still linked and hashed. Do not widen this to arbitrary uninstrumented TUs.
ASSEMBLY = {f'blake3_{name}_x86-64_unix.S' for name in ('sse2', 'sse41', 'avx2', 'avx512')}


def compile_record(row: dict, build: Path, source: Path, *, foundation: bool) -> dict:
    argv = row.get('arguments') or shlex.split(row['command'])
    directory = Path(row['directory']).resolve(strict=True)
    unit = Path(row['file']).resolve(strict=True)
    output = Path(row.get('output') or argv[argv.index('-o') + 1])
    output = (directory / output).resolve(strict=True)
    if not output.is_relative_to(build) or not (unit.is_relative_to(source) or unit.is_relative_to(build)):
        raise ValueError('compiler instrumentation source/object escaped its declared graph')
    record = {'source': str(unit), 'object': str(output), 'argv': argv,
              'sha256': digest(output), 'source_sha256': digest(unit)}
    if unit.suffix == '.S':
        if not foundation or unit.name not in ASSEMBLY or unit.parent.name != 'BLAKE3':
            raise ValueError('unapproved compiler assembly exclusion')
        return {**record, 'classification': 'upstream-assembly-kernel'}
    enabled = {value for arg in argv if arg.startswith('-fsanitize=')
               for value in arg.split('=', 1)[1].split(',')}
    disabled = {value for arg in argv if arg.startswith('-fno-sanitize=')
                for value in arg.split('=', 1)[1].split(',')}
    if (not {'address', 'undefined'}.issubset(enabled) or
            disabled - {'function', 'vptr'} or '-fno-sanitize-recover=undefined' not in argv or
            any(arg.startswith('-fsanitize-recover') for arg in argv) or
            '-fno-omit-frame-pointer' not in argv):
        raise ValueError('compiler object lacks owned nonrecovering instrumentation: '+str(unit))
    if '-gline-tables-only' not in argv and '-g' not in argv:
        raise ValueError('compiler instrumentation lacks source locations')
    for arg in argv:
        if arg.startswith(('-fsanitize-blacklist=', '-fsanitize-ignorelist=')):
            approved = source / 'llvm/utils/sanitizers/ubsan_ignorelist.txt'
            if not foundation or Path(arg.split('=', 1)[1]).resolve(strict=True) != approved:
                raise ValueError('unapproved compiler instrumentation ignorelist')
    return {**record, 'classification': 'precompiled-header' if output.suffix == '.pch' else 'instrumented-object'}


def _without_dependency_flags(argv):
    result = []
    values = iter(argv)
    for arg in values:
        if arg in ('-MD', '-MMD', '-MP'):
            continue
        if arg in ('-MT', '-MQ', '-MF'):
            next(values)
            continue
        result.append(arg)
    return result


def graph_records(build: Path, source: Path, targets: list[str], ninja: Path, environment: dict,
                  *, foundation: bool, run) -> list[dict]:
    """Intersect the actual transitive Ninja graph with compilation ownership.

    This includes build helpers used to generate compiler inputs. It cannot
    silently shrink to whichever object files happen to exist on disk.
    """
    text = run('graph-commands', [ninja, '-C', build, '-t', 'commands', *targets])
    graph = {}
    scans = {}
    for line in text.splitlines():
        argv = shlex.split(line)
        if '-c' not in argv or '-o' not in argv:
            continue
        output = (build / argv[argv.index('-o') + 1]).resolve()
        if Path(argv[0]).name == 'clang-scan-deps':
            # P1689 scans contain a nested compiler -c/-o command but produce
            # dependency JSON, not the named .o. Count only its actual compiler
            # edge below; bind the bootstrap helper and scan output separately.
            if '-format=p1689' not in argv or '--' not in argv or '-MT' not in argv or output in scans:
                raise ValueError('unrecognized or ambiguous compiler dependency scan')
            scanner = Path(argv[0]).resolve(strict=True)
            dependency = (build / argv[argv.index('-MT')+1]).resolve(strict=True)
            if not dependency.is_relative_to(build) or dependency.suffix != '.ddi':
                raise ValueError('compiler dependency scan escaped its owned build output')
            rules = json.loads(dependency.read_text()).get('rules', [])
            if len(rules) != 1 or (build / rules[0]['primary-output']).resolve() != output:
                raise ValueError('compiler dependency scan describes a different object')
            scans[output] = {'tool': str(scanner), 'tool_sha256': digest(scanner),
                             'output': str(dependency), 'sha256': digest(dependency), 'argv': argv}
            continue
        if output in graph:
            raise ValueError('ambiguous compiler graph output')
        graph[output] = argv
    if not graph:
        raise ValueError('compiler graph has no compilation work')
    commands = json.loads((build / 'compile_commands.json').read_text())
    records = {}
    for row in commands:
        argv = row.get('arguments') or shlex.split(row['command'])
        output = (Path(row['directory']) / (row.get('output') or argv[argv.index('-o') + 1])).resolve()
        if output not in graph:
            continue
        actual = _without_dependency_flags(graph[output])
        argv = _without_dependency_flags(argv)
        if (output in records or
                (build / actual[actual.index('-c') + 1]).resolve() != Path(row['file']).resolve() or
                not any(actual[index:index+len(argv)] == argv
                        for index in range(len(actual)-len(argv)+1))):
            raise ValueError('compiler graph differs from compilation ownership')
        records[output] = compile_record(row, build, source, foundation=foundation)
    custom = build / 'custom-compile-records.json'
    if custom.exists():
        if foundation:
            raise ValueError('foundation cannot import facade custom compilation records')
        for row in json.loads(custom.read_text()):
            output = (build / row['output']).resolve(strict=True)
            if output not in graph:
                continue
            actual = graph[output]
            if output in records or (build / actual[actual.index('-c') + 1]).resolve() != Path(row['file']).resolve():
                raise ValueError('custom compiler object differs from its canonical CMake source')
            records[output] = compile_record({**row, 'directory': str(build), 'arguments': actual},
                                             build, source, foundation=False)
    if set(records) != set(graph):
        raise ValueError('compiler graph is missing compilation records')
    if set(scans) - set(records):
        raise ValueError('dependency scanning cannot substitute for a compiler object edge')
    for output, scan in scans.items():
        records[output]['dependency_scan'] = scan
    return list(records.values())


def verify_symbols(records: list[dict], nm: Path, *, run):
    objects = {row['object']: row for row in records if row['classification'] == 'instrumented-object'}
    if not objects:
        raise ValueError('no compiler implementation objects were verified')
    names = list(objects)
    for start in range(0, len(names), 100):
        batch = names[start:start+100]
        symbols = run('object-symbols-'+str(start), [nm, '-A', '--undefined-only', *batch])
        found = {path: set() for path in batch}
        for line in symbols.splitlines():
            path, separator, value = line.partition(':')
            if separator and path in found and ('__asan_' in value or '__ubsan_' in value):
                found[path].add(value.split()[-1])
        for path, values in found.items():
            if not any('__asan_' in value for value in values):
                raise ValueError('compiler object has no ASan symbol evidence: '+path)
            objects[path]['instrumentation_symbols'] = sorted(values)
    if not any('__ubsan_' in symbol for row in objects.values() for symbol in row['instrumentation_symbols']):
        raise ValueError('compiler graph has no UBSan symbol evidence')


def archive_members(path: Path, *, allow_repeated_names=False) -> list[dict]:
    """Hash ordinary GNU/BSD ar members without extracting or executing them."""
    members = []
    names = b''
    length = path.stat().st_size
    with path.open('rb') as stream:
        if stream.read(8) != b'!<arch>\n':
            raise ValueError('compiler qualification requires ordinary, non-thin archives')
        while stream.tell() < length:
            header = stream.read(60)
            if len(header) != 60 or header[58:] != b'`\n':
                raise ValueError('malformed compiler archive header')
            name = header[:16].decode('ascii').rstrip()
            size = int(header[48:58])
            end = stream.tell() + size
            if size < 0 or end > length:
                raise ValueError('truncated compiler archive member')
            if name == '//':
                if size > 16 * 1024 * 1024:
                    raise ValueError('compiler archive name table exceeds budget')
                names = stream.read(size)
            elif name in ('/', '/SYM64/', '__.SYMDEF', '__.SYMDEF SORTED'):
                stream.seek(end)
            else:
                if name.startswith('#1/'):
                    count = int(name[3:])
                    if count < 1 or count > size or count > 65536:
                        raise ValueError('invalid compiler archive BSD name')
                    name = stream.read(count).rstrip(b'\0').decode('utf-8')
                elif name.startswith('/'):
                    offset = int(name[1:])
                    stop = names.find(b'/\n', offset)
                    if offset < 0 or stop < offset:
                        raise ValueError('invalid compiler archive GNU name')
                    name = names[offset:stop].decode('utf-8')
                else:
                    name = name.removesuffix('/')
                if not name or Path(name).name != name:
                    raise ValueError('compiler archive member must be an owned object basename')
                signature = hashlib.sha256()
                remaining = end - stream.tell()
                payload_size = remaining
                while remaining:
                    value = stream.read(min(remaining, 1024 * 1024))
                    if not value:
                        raise ValueError('truncated compiler object payload')
                    signature.update(value)
                    remaining -= len(value)
                members.append({'name': name, 'sha256': signature.hexdigest(), 'bytes': payload_size})
            if size % 2:
                if stream.read(1) != b'\n':
                    raise ValueError('invalid compiler archive alignment')
    if not members or (not allow_repeated_names and len({row['name'] for row in members}) != len(members)):
        raise ValueError('compiler archive has empty or ambiguous object ownership')
    return members


def archive_owners(build: Path, archives: list[Path], ninja: Path, *, run) -> dict[str, list[str]]:
    """Bind each staged/build archive to its ordered canonical Ninja inputs.

    Clang CodeGen/Driver have distinct TUs with repeated object basenames.
    Names alone cannot establish membership; neither can a global bag of valid
    object hashes, which would permit substitution from an unrelated target.
    """
    result = {}
    for index, archive in enumerate(archives):
        compiled = archive if archive.is_relative_to(build) else build / 'lib' / archive.name
        if digest(compiled) != digest(archive):
            raise ValueError('staged compiler archive differs from its canonical build output')
        relative = compiled.relative_to(build).as_posix()
        query = run('archive-inputs-'+str(index), [ninja, '-C', build, '-t', 'query', relative])
        inputs = []
        collecting = False
        for line in query.splitlines():
            if line.startswith('  input: '):
                collecting = True
            elif line.startswith('  outputs:'):
                collecting = False
            elif collecting:
                name = line.strip()
                if name.startswith('|'):
                    continue # CMake's order-only/implicit target dependencies.
                candidate = (build / name).resolve(strict=True)
                if not name.endswith('.o') or not candidate.is_relative_to(build.resolve()):
                    raise ValueError('compiler archive input is not a canonical owned object')
                inputs.append(str(candidate))
        if not inputs or len(inputs) != len(set(inputs)):
            raise ValueError('compiler archive has missing or ambiguous canonical Ninja inputs')
        result[str(archive)] = inputs
    return result


def verify_owned_archives(archives: list[Path], records: list[dict], *, owners=None) -> list[dict]:
    expected = {}
    for record in records:
        if record['classification'] == 'precompiled-header':
            continue
        expected.setdefault(Path(record['object']).name, set()).add(record['sha256'])
    evidence = []
    if not archives:
        raise ValueError('compiler profile has no owned static archives')
    if owners is not None and set(owners) != {str(archive) for archive in archives}:
        raise ValueError('compiler archive ownership inventory differs from the selected archives')
    objects = {row['object']: row for row in records if row['classification'] != 'precompiled-header'}
    for archive in archives:
        members = archive_members(archive, allow_repeated_names=owners is not None)
        for row in members:
            if row['sha256'] not in expected.get(row['name'], set()):
                raise ValueError('compiler archive member differs from verified objects: '+str(archive)+':'+row['name'])
        ownership = {}
        if owners is not None:
            paths = owners[str(archive)]
            if not paths or len(paths) != len(set(paths)) or any(path not in objects for path in paths):
                raise ValueError('compiler archive target contains unverified or duplicate objects')
            actual = [(row['name'], row['sha256']) for row in members]
            intended = [(Path(path).name, objects[path]['sha256']) for path in paths]
            if actual != intended:
                raise ValueError('compiler archive members differ from ordered canonical target inputs')
            ownership = {'objects': paths}
        evidence.append({'path': str(archive), 'sha256': digest(archive), 'members': members, **ownership})
    return evidence


def loaded_graph(binary: Path, loader: Path, libraries: list[Path], *, required=(), run) -> dict:
    output = run('loader', [loader, '--list', binary])
    paths = set()
    for line in output.splitlines():
        value = line.split(' => ', 1)[-1].strip().rsplit(' (0x', 1)[0]
        if value.startswith('/'):
            paths.add(Path(value).resolve(strict=True))
    owned = {path.resolve(strict=True) for path in libraries}
    if not paths or 'not found' in output or not set(required).issubset(paths):
        raise ValueError('compiler executable did not load its required instrumented libraries')
    for path in paths:
        if path.name.startswith(('libLLVM', 'libclang-cpp')) and path not in owned:
            raise ValueError('compiler executable loaded an unselected LLVM/Clang library')
    return {'binary': str(binary), 'sha256': digest(binary),
            'libraries': {str(path): digest(path) for path in sorted(paths)}}


def borrowed_base_pch_matrix(compiler: Path, source: Path, output: Path, *, run):
    """Reuse the authored mandatory-diagnostic oracle across both PCH standards."""
    fixture = source / 'clang/test/ObjC3/Sema/borrowed-base-subobjects.mm'
    header = fixture.parent / 'Inputs/borrowed-base-subobjects.h'
    for standard in ('c++20', 'c++23'):
        for arc in (False, True):
            name = 'borrowed-base-' + standard + ('-arc' if arc else '-mrc')
            pch = output / (name+'.pch')
            flags = ['-cc1', '-std='+standard, '-fblocks', '-fobjc-version=3',
                     '-fobjc-runtime=gnustep-2.2', '-fobjc3-profile=strict-system',
                     *(['-fobjc-arc'] if arc else [])]
            run(name+'-producer', [compiler, *flags, '-x', 'objective-c++-header',
                                   '-emit-pch', header, '-o', pch])
            run(name+'-consumer', [compiler, *flags, '-x', 'objective-c++',
                '-include-pch', pch, '-DFROM_PCH', '-fsyntax-only', '-verify', fixture])
            pch.unlink()


def qualify(repo: Path, output: Path, *, jobs=2, embedding_build: Path | None = None) -> dict:
    if jobs < 1:
        raise ValueError('compiler qualification requires positive build parallelism')
    repo = repo.resolve(strict=True)
    output = output.resolve()
    if output.exists():
        raise ValueError('compiler instrumentation requires a fresh evidence directory')
    plans = build_plans(repo, host='linux-x86_64', instrumentation_profile=PROFILE)
    plan = plans['configure']
    build, stage, source = (Path(plan['paths'][key]).resolve(strict=True) for key in ('build', 'stage', 'source'))
    # Refuse output locations that can overwrite inputs or the ordinary SDK.
    if output == repo or any(output.is_relative_to(path) or path.is_relative_to(output)
                             for path in (build, stage, source, repo / 'native', repo / 'toolchains')):
        raise ValueError('compiler evidence must be disjoint from implementation inputs')
    foundation_report = build / 'foundation-build-report.json'
    completed = json.loads(foundation_report.read_text())
    if (completed.get('status') != 'passed' or completed.get('source_identity') != plan['source_build_id'] or
            {phase['phase'] for phase in completed['phases']} != {'configure', 'build', 'test', 'stage'} or
            any(phase['exit_code'] for phase in completed['phases'])):
        raise ValueError('compiler instrumentation requires a current complete foundation build/test/stage')
    bootstrap = Path(plan['host_environment']['bootstrap_root'])
    facade = output / 'facade'
    if embedding_build is not None:
        facade = embedding_build.resolve(strict=True)
        if (not facade.is_relative_to(repo / 'out') or any(
                facade.is_relative_to(path) or path.is_relative_to(facade)
                for path in (build, stage, source, output))):
            raise ValueError('reused embedding build must be a disjoint owned development output')
        previous = json.loads((facade / 'compiler-instrumentation-plan.json').read_text())
        for key in ('source_build_id', 'build_config_digest'):
            if previous[key] != plan[key]:
                raise ValueError('reused embedding foundation/profile/bootstrap differs: '+key)
        # The strict CMake subprocess environment intentionally omits a caller's
        # optional ccache launcher from PATH. It is not a compiler/ABI identity;
        # all real tools and resources still have to match, and the rebuilt
        # commands, object symbols/bytes and archive graph are checked below.
        if ({**previous['host_environment'], 'ccache_path': None} !=
                {**plan['host_environment'], 'ccache_path': None}):
            raise ValueError('reused embedding foundation/profile/bootstrap differs: host_environment')
    libraries = [stage / 'lib/libclang-cpp.so', stage / 'lib/libLLVM.so']
    policy = SanitizerQualification(bootstrap / 'bin/clang++', bootstrap / 'lib/clang/23',
        output / 'detectors', 'address,undefined', libraries=libraries,
        additional_inputs=[foundation_report, Path(__file__),
            *(repo / name for name in plan['configuration_inputs']),
            *(path for tree in ('native/objc3c', 'tests/clang_foundation', 'tests/tooling/fixtures/clang_foundation')
              for path in (repo / tree).rglob('*')
              if path.is_file() and (path.suffix in ('.cpp', '.h', '.hpp', '.cmake', '.in') or path.name == 'CMakeLists.txt'))])
    environment = dict(policy.environment)
    for key in ('cmake_path', 'ninja_path'):
        environment['PATH'] = str(Path(plan['host_environment'][key]).parent)+os.pathsep+environment['PATH']
    output.mkdir(parents=True)
    receipt = output / 'qualification.json'
    report = {'schema': 'objc3c.compiler-implementation-instrumentation.v1', 'status': 'running',
              'subject': 'compiler-implementation', 'profile': PROFILE,
              'source_build_id': plan['source_build_id'], 'build_config_digest': plan['build_config_digest'],
              'foundation_receipt_sha256': digest(foundation_report),
              'boundaries': plan['instrumentation']['boundaries'],
              'compile_time_exclusions': {
                  'upstream_ubsan_ignorelist': {
                      'path': str(source / 'llvm/utils/sanitizers/ubsan_ignorelist.txt'),
                      'sha256': digest(source / 'llvm/utils/sanitizers/ubsan_ignorelist.txt'),
                      'owner': 'LLVM upstream sanitizer configuration',
                      'scope': 'libstdc++ tree/iterator/vector/shared-pointer header operations',
                      'references': ['GCC #59829', 'GCC #85930', 'LLVM HandleLLVMOptions.cmake'],
                      'revalidate': 'Every upstream/compiler/standard-library upgrade; #8284'},
                  'function_and_vptr': {
                      'owner': 'LLVM upstream Address;Undefined recipe',
                      'reason': 'Erased callback and native type representations use the supported LLVM profile.',
                      'revalidate': 'Every upstream/compiler upgrade; #8284'},
                  'assembly': {
                      'owner': 'LLVM upstream BLAKE3 kernels',
                      'reason': 'Four named assembly kernels have no C/C++ sanitizer instrumentation.',
                      'revalidate': 'Every source/toolchain upgrade; #8284'}},
              'embedding_build': str(facade), 'embedding_build_reused': embedding_build is not None,
              'commands': [], 'foundation_objects': [], 'embedding_objects': [], 'loaded_graphs': {}}

    def run(name, command, *, expected=0, detector_source=None):
        log = output / (name+'.log')
        started = time.monotonic()
        argv = list(map(str, command))
        try:
            with log.open('w') as stream:
                result = subprocess.run(argv, cwd=repo, env=environment,
                    stdout=stream, stderr=subprocess.STDOUT, timeout=900)
            code = result.returncode
        except subprocess.TimeoutExpired:
            code = 124
        report['commands'].append({'name': name, 'argv': argv, 'exit_code': code,
            'seconds': round(time.monotonic()-started, 6), 'log': str(log), 'log_sha256': digest(log)})
        write_json(receipt, report)
        diagnostic = log.read_text(errors='replace')
        if code != expected:
            raise ValueError('compiler instrumentation command failed: '+name)
        if detector_source is None:
            require_clean_diagnostic(diagnostic)
        else:
            validate_detection('asan', detector_source, code, diagnostic, bad=True)
        return diagnostic

    try:
        policy.ensure_controls('O1')
        llvm_library = libraries[1].resolve(strict=True)
        report['loaded_graphs']['foundation-clang'] = loaded_graph(stage / 'bin/clang',
            policy.loader, libraries, required=tuple(path.resolve(strict=True) for path in libraries),
            run=lambda name, cmd: run('foundation-clang-'+name, cmd))
        borrowed_base_pch_matrix(stage / 'bin/clang', source, output, run=run)
        ninja = Path(plan['host_environment']['ninja_path'])
        targets = [target for group in plan['instrumentation']['build_target_groups'] for target in group['targets']] + ['install-distribution']
        report['foundation_objects'] = graph_records(build, source, targets, ninja, environment,
            foundation=True, run=lambda name, cmd: run('foundation-'+name, cmd))
        verify_symbols(report['foundation_objects'], bootstrap / 'bin/llvm-nm',
            run=lambda name, cmd: run('foundation-'+name, cmd))
        archives = sorted(path for path in (stage / 'lib').glob('*.a')
                          if path.name.startswith(('libLLVM', 'libclang')))
        report['foundation_archives'] = verify_owned_archives(archives, report['foundation_objects'],
            owners=archive_owners(build, archives, ninja,
                run=lambda name, cmd: run('foundation-'+name, cmd)))
        run('embedding-configure', ['cmake', '-S', repo / 'native/objc3c', '-B', facade, '-G', 'Ninja',
            '-DCMAKE_CXX_COMPILER='+str(bootstrap / 'bin/clang++'), '-DCMAKE_BUILD_TYPE=Release',
            '-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld', '-DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=lld',
            '-DOBJC3C_CLANG_FOUNDATION_ROOT='+str(stage),
            '-DOBJC3C_COMPILER_INSTRUMENTATION_PROFILE='+PROFILE])
        embedding_tools = ['objc3c', 'objc3c_frontend_v2', 'clang-scan-deps', 'objc3_compiler_process_control']
        run('embedding-tools-build', ['cmake', '--build', facade, '--target', *embedding_tools,
                                     '--parallel', str(jobs)])
        for name in ('objc3c', 'clang-scan-deps', 'objc3_compiler_process_control'):
            report['loaded_graphs'][name] = loaded_graph(facade / 'src/clang_foundation' / name,
                policy.loader, libraries, required=(llvm_library,),
                run=lambda suffix, cmd, name=name: run(name+'-'+suffix, cmd))
        control = facade / 'src/clang_foundation/objc3_compiler_process_control'
        control_source = repo / 'tests/clang_foundation/compiler_instrumentation_process.cpp'
        good = run('compiler-process-good', [control, 'good'])
        bad = run('compiler-process-bad', [control, 'bad'], expected=86, detector_source=control_source)
        if ('objc3:parsed-ast-detector' not in good or 'objc3:parsed-ast-detector' not in bad or
                'HandleTranslationUnit' not in bad):
            raise ValueError('compiler memory control did not execute inside frontend AST processing')
        report['compiler_process_control'] = {'source': str(control_source),
            'source_sha256': digest(control_source), 'binary_sha256': digest(control),
            'status': 'PASS', 'clean_exit': 0, 'detection_exit': 86,
            'class': 'ASan heap-buffer-overflow in frontend AST consumer; no emitted client executable'}
        metadata = json.loads((facade / 'frontend-test-targets.json').read_text())
        binaries = {row['name']: Path(row['executable']) for row in metadata['tests']}
        def run_frontend(name, command):
            if name.endswith('-ctest'):
                test = name[len('frontend-'):-len('-ctest')]
                report['loaded_graphs'][test] = loaded_graph(binaries[test], policy.loader,
                    libraries, run=lambda suffix, cmd: run(name+'-'+suffix, cmd))
            return run(name, command)
        report['frontends'] = frontend_tests(facade, output / 'frontends', run_frontend, jobs=jobs)
        # Serial qualification reclaims passing test executables, but retains
        # their object files and exact binary identities in the nested receipt.
        targets = [row['target'] for row in report['frontends']['tests']] + embedding_tools
        report['embedding_objects'] = graph_records(facade, repo, targets, ninja, environment,
            foundation=False, run=lambda name, cmd: run('embedding-'+name, cmd))
        verify_symbols(report['embedding_objects'], bootstrap / 'bin/llvm-nm',
            run=lambda name, cmd: run('embedding-'+name, cmd))
        archives = sorted((facade / 'src/clang_foundation').rglob('libobjc3*.a'))
        report['embedding_archives'] = verify_owned_archives(archives, report['embedding_objects'],
            owners=archive_owners(facade, archives, ninja,
                run=lambda name, cmd: run('embedding-'+name, cmd)))
        for record in report['foundation_objects'] + report['embedding_objects']:
            if (digest(Path(record['source'])) != record['source_sha256'] or
                    digest(Path(record['object'])) != record['sha256']):
                raise ValueError('compiler graph changed during implementation qualification')
            if scan := record.get('dependency_scan'):
                if digest(Path(scan['tool'])) != scan['tool_sha256'] or digest(Path(scan['output'])) != scan['sha256']:
                    raise ValueError('compiler dependency scanner or result changed during qualification')
        for archive in report['foundation_archives'] + report['embedding_archives']:
            if digest(Path(archive['path'])) != archive['sha256']:
                raise ValueError('staged compiler archive changed during qualification')
        for graph in report['loaded_graphs'].values():
            if any(digest(Path(path)) != value for path, value in graph['libraries'].items()):
                raise ValueError('loaded compiler dependency changed during qualification')
        final = build_plans(repo, host='linux-x86_64', instrumentation_profile=PROFILE)['configure']
        if (final['source_build_id'] != plan['source_build_id'] or
                final['host_environment'] != plan['host_environment']):
            raise ValueError('compiler source/profile/bootstrap changed during qualification')
        report['detectors'] = policy.finalize()
        report['loaded_compiler_libraries'] = {str(path.resolve(strict=True)): digest(path)
                                               for path in libraries}
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        write_json(receipt, report)
    return report


def build_and_qualify(repo: Path, *, profile=PROFILE, fetch=False) -> dict:
    """Opt-in SDK pipeline prerequisite; never package an instrumented SDK."""
    if profile != PROFILE:
        raise ValueError('unknown compiler implementation instrumentation profile')
    from .build import build_foundation
    build_foundation(repo, host='linux-x86_64', fetch=fetch, stage=True,
                     instrumentation_profile=profile)
    name = datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%S.%fZ') + '-' + str(os.getpid())
    output = repo / 'out/compiler-instrumentation' / name
    report = qualify(repo, output)
    receipt = output / 'qualification.json'
    return {'profile': profile, 'subject': report['subject'], 'status': report['status'],
            'receipt': str(receipt), 'sha256': digest(receipt)}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--jobs', type=int, default=2)
    parser.add_argument('--embedding-build', type=Path,
                        help='incrementally rebuild a matching isolated compiler-profile embedding tree')
    args = parser.parse_args(argv)
    if args.jobs < 1:
        parser.error('--jobs must be positive')
    qualify(args.repo, args.output, jobs=args.jobs, embedding_build=args.embedding_build)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
