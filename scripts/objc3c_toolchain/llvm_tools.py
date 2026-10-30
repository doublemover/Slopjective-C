"""Admit development LLVM tools from the sealed compiler's actual build graph.

No PATH tool, version-only match, stage mutation or SDK rebuild is permitted.
The bundle inventory has its own identity, separate from the foundation ABI.
"""
from __future__ import annotations

import hashlib
import json
from pathlib import Path
import struct

from .binary_packages import digest
from .build import verify_effective_configuration
from .compiler_instrumentation import (archive_owners, graph_records, loaded_graph,
    verify_owned_archives, verify_symbols)
from .core import build_plans
from .sanitizer_policy import strict_environment


def identity(value: dict) -> str:
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def runpath(data: bytes) -> tuple[int, str]:
    """Locate only DT_RUNPATH in a Linux ELF64 image (no code/data masking)."""
    if data[:6] != b'\x7fELF\x02\x01' or len(data) < 64:
        raise ValueError('development tools require little-endian ELF64')
    offset = struct.unpack_from('<Q', data, 40)[0]
    size, count = struct.unpack_from('<HH', data, 58)
    if size != 64 or not count or offset+size*count > len(data):
        raise ValueError('invalid ELF section inventory')
    sections = [struct.unpack_from('<IIQQQQIIQQ', data, offset+size*index) for index in range(count)]
    found = []
    for section in sections:
        if section[1] != 6:
            continue
        if section[6] >= len(sections) or section[5] % 16 or section[4]+section[5] > len(data):
            raise ValueError('invalid ELF dynamic section')
        strings = sections[section[6]]
        for index in range(section[4], section[4]+section[5], 16):
            tag, value = struct.unpack_from('<QQ', data, index)
            if tag == 29:
                start = strings[4]+value
                end = data.find(b'\0', start, strings[4]+strings[5])
                if value >= strings[5] or end < start:
                    raise ValueError('invalid ELF RUNPATH string')
                found.append((start, data[start:end].decode('ascii')))
    if len(found) != 1:
        raise ValueError('expected one owned ELF RUNPATH')
    return found[0]


def verify_installation(canonical: Path, installed: Path, build: Path) -> dict:
    """CMake edits RUNPATH at install time; admit exactly that byte transition."""
    old, new = canonical.read_bytes(), installed.read_bytes()
    position, before = runpath(old)
    destination, after = runpath(new)
    expected = '$ORIGIN/../lib:$ORIGIN/../lib/x86_64-unknown-linux-gnu'
    if (len(old) != len(new) or position != destination or after != expected or
            before not in (expected+':', expected+':'+str(build / 'lib')+':')):
        raise ValueError('unowned LLVM install-time ELF RUNPATH transition')
    # The overwritten tail must be zeros, not arbitrary hidden changes. Every
    # other byte (including the ELF dynamic tables and code) must be identical.
    canonical_bytes = old[:position]+(after.encode()+b'\0').ljust(len(before)+1, b'\0')+old[position+len(before)+1:]
    if canonical_bytes != new:
        raise ValueError('installed LLVM differs outside its exact CMake RUNPATH edit')
    return {'build_sha256': hashlib.sha256(old).hexdigest(), 'installed_sha256': hashlib.sha256(new).hexdigest(),
            'runpath_before': before, 'runpath_after': after, 'status': 'PASS'}


def selected(repo: Path) -> tuple[dict, dict, dict]:
    inventory = repo / 'toolchains/llvm/assurance-tools.json'
    document = json.loads(inventory.read_text())
    if (document.get('schema') != 'objc3c.llvm-development-tools.v1' or
            document.get('host') != 'linux-x86_64' or
            document.get('foundation_profile') != 'compiler-asan-ubsan' or
            document.get('build_targets') != ['opt', 'llvm-dis', 'llvm-reduce'] or
            document.get('foundation_tools') != ['clang', 'clang++', 'llc', 'llvm-dwarfdump', 'ld.lld'] or
            document.get('pipelines') != ['default<O0>', 'default<O2>']):
        raise ValueError('unreviewed LLVM development tool inventory')
    plan = build_plans(repo, host=document['host'],
                       instrumentation_profile=document['foundation_profile'])['configure']
    build, stage = (Path(plan['paths'][key]).resolve(strict=True) for key in ('build', 'stage'))
    completed = json.loads((build / 'foundation-build-report.json').read_text())
    if (completed.get('status') != 'passed' or completed.get('source_identity') != plan['source_build_id'] or
            {row['phase'] for row in completed['phases']} != {'configure', 'build', 'test', 'stage'} or
            any(row['exit_code'] for row in completed['phases'])):
        raise ValueError('development tools require the complete matching foundation')
    verify_effective_configuration(plan)
    tools = {name: (build / 'bin' / name).resolve(strict=True) for name in document['build_targets']}
    for name in document['foundation_tools']:
        # Invocation basename selects clang++ and ld.lld driver behavior.
        # Hash/loader checks still follow symlinks to the exact binary bytes.
        tools[name] = stage / 'bin' / name
        canonical = (build / 'bin' / name).resolve(strict=True)
        verify_installation(canonical, tools[name], build)
    # CMake changes only the owned RUNPATH during installation. Code and every
    # other byte must match; both actual loader graphs are independently bound.
    for name in ('libLLVM.so', 'libclang-cpp.so'):
        verify_installation(build / 'lib' / name, stage / 'lib' / name, build)
    bootstrap = Path(plan['host_environment']['bootstrap_root'])
    environment = strict_environment(tools['clang'], bootstrap / 'bin/llvm-symbolizer')
    return plan, tools, environment


def verify_receipt(evidence: dict, *, plan: dict, tools: dict, repo: Path):
    if (evidence.get('status') != 'PASS' or
            evidence.get('identity') != identity({key: value for key, value in evidence.items() if key != 'identity'}) or
            evidence.get('source_build_id') != plan['source_build_id'] or
            evidence.get('build_config_digest') != plan['build_config_digest'] or
            evidence.get('owner_sha256') != digest(Path(__file__)) or
            evidence.get('inventory_sha256') != digest(repo / 'toolchains/llvm/assurance-tools.json') or
            set(evidence.get('tools', {})) != set(tools) or not evidence.get('inputs')):
        raise ValueError('LLVM tool receipt differs from its current source/configuration/inventory owner')
    for path, signature in evidence['inputs'].items():
        if digest(Path(path)) != signature:
            raise ValueError('LLVM tool graph input changed: '+path)
    for name, tool in evidence['tools'].items():
        if tool['path'] != str(tools[name]) or digest(tools[name]) != tool['sha256']:
            raise ValueError('LLVM tool selection changed: '+name)
        for path, signature in tool['loaded_graph']['libraries'].items():
            if digest(Path(path)) != signature:
                raise ValueError('LLVM tool loaded library changed: '+path)
    for row in evidence['objects']:
        if digest(Path(row['object'])) != row['sha256'] or digest(Path(row['source'])) != row['source_sha256']:
            raise ValueError('LLVM tool object/source changed: '+row['object'])
    for row in evidence['archives']:
        if digest(Path(row['path'])) != row['sha256']:
            raise ValueError('LLVM tool archive changed: '+row['path'])


def qualify(repo: Path, *, run, receipt: Path | None = None) -> tuple[dict, dict, dict]:
    plan, tools, environment = selected(repo)
    if receipt is not None:
        evidence = json.loads(receipt.read_text())
        verify_receipt(evidence, plan=plan, tools=tools, repo=repo)
        return evidence, tools, environment
    build, stage, source = (Path(plan['paths'][key]).resolve(strict=True)
                             for key in ('build', 'stage', 'source'))
    inventory = repo / 'toolchains/llvm/assurance-tools.json'
    document = json.loads(inventory.read_text())
    targets = document['build_targets'] + ['clang', 'llc', 'llvm-dwarfdump', 'lld']
    ninja = Path(plan['host_environment']['ninja_path'])
    records = graph_records(build, source, targets, ninja, environment, foundation=True,
                            run=lambda name, cmd: run('tools-'+name, cmd))
    verify_symbols(records, Path(plan['host_environment']['bootstrap_root']) / 'bin/llvm-nm',
                   run=lambda name, cmd: run('tools-'+name, cmd))
    # Query only the actual transitive graph, never all archives left in a tree.
    commands = run('tools-archive-graph', [ninja, '-C', build, '-t', 'commands', *targets])
    import shlex
    archives = set()
    for line in commands.splitlines():
        argv = shlex.split(line)
        for index, arg in enumerate(argv[:-1]):
            if Path(arg).name == 'llvm-ar' and argv[index+1] in ('qc', 'Dqc'):
                archives.add((build / argv[index+2]).resolve(strict=True))
    archives = sorted(archives)
    owned = verify_owned_archives(archives, records, owners=archive_owners(build, archives, ninja,
        run=lambda name, cmd: run('tools-'+name, cmd)))
    graphs, versions = {}, {}
    loader = Path('/lib64/ld-linux-x86-64.so.2').resolve(strict=True)
    for name, tool in tools.items():
        library_root = build if name in document['build_targets'] else stage
        libraries = [(library_root / 'lib' / name).resolve(strict=True)
                     for name in ('libLLVM.so', 'libclang-cpp.so')]
        # LLD is statically linked; all other tools use LLVM (Clang also uses
        # clang-cpp). The graph rejects unselected LLVM DSOs for either route.
        required = () if name == 'ld.lld' else (libraries[0],)
        graphs[name] = loaded_graph(tool, loader, libraries, required=required,
            run=lambda suffix, cmd, name=name: run('tools-'+name+'-'+suffix, cmd))
        versions[name] = run('tools-'+name+'-version', [tool, '--version'])
    evidence = {'schema': 'objc3c.llvm-development-tools-receipt.v1', 'status': 'PASS',
        'source_build_id': plan['source_build_id'], 'build_config_digest': plan['build_config_digest'],
        'inventory_sha256': digest(inventory), 'owner_sha256': digest(Path(__file__)),
        'foundation_receipt_sha256': digest(build / 'foundation-build-report.json'),
        'configuration': verify_effective_configuration(plan),
        'install_transitions': {name: verify_installation(build / name, stage / name, build)
            for name in [*('bin/'+name for name in document['foundation_tools']),
                         'lib/libLLVM.so', 'lib/libclang-cpp.so']},
        'tools': {name: {'path': str(path), 'sha256': digest(path), 'version': versions[name],
                         'loaded_graph': graphs[name]} for name, path in tools.items()},
        'objects': records, 'archives': owned,
        'inputs': {str(path): digest(path) for path in [
            *(Path(__file__).with_name(name+'.py') for name in
              ('llvm_tools', 'compiler_instrumentation', 'core', 'build', 'sanitizer_policy')),
            inventory, build / 'build.ninja', build / 'compile_commands.json', build / 'CMakeCache.txt',
            build / 'foundation-build-report.json']}}
    evidence['identity'] = identity(evidence)
    return evidence, tools, environment
