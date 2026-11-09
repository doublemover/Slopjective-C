"""Retain actual sanitizer product commands, inputs, artifacts and loader routes.

Detector controls remain owned by sanitizer_policy. This owner records the
project process, including failures, before its temporary workspace disappears.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil

if __package__:
    from .binary_packages import digest, write_json
    from .sdk_example import dependencies
    from .llvm_ir_execution import command
    from .sanitizer_policy import require_clean_diagnostic
else:
    from binary_packages import digest, write_json
    from sdk_example import dependencies
    from llvm_ir_execution import command
    from sanitizer_policy import require_clean_diagnostic

SOURCE = {'.c', '.cc', '.cpp', '.cxx', '.m', '.mm', '.objc3', '.h', '.hpp', '.inc', '.def', '.modulemap', '.json', '.objc3i', '.py'}
ARTIFACT = {'.o', '.pch', '.pcm', '.a', '.d', '.bc'}
OUTPUT_FLAGS = {'-o', '--emit-object', '--emit-interface', '--emit-dependency-record'}


def consolidate_campaign_blobs(campaign: Path) -> dict:
    """Deduplicate completed per-owner blob stores without changing receipts.

    Product owners deliberately retain exact replay bytes, but a campaign can
    execute the same headers, compiler inputs and runtime products many times.
    ZIP artifacts do not deduplicate identical files. Replace each completed
    owner's private ``blobs`` directory with a link to one campaign-owned store
    so durable evidence contains each content-addressed byte sequence once.
    """
    campaign = campaign.resolve(strict=True)
    receipts = sorted(campaign.glob('*/product-evidence/qualification.json'))
    if not receipts:
        raise ValueError('product campaign has no completed evidence receipts')
    store = campaign / 'product-evidence-blobs'
    store.mkdir()
    private_bytes = 0
    references = set()
    for receipt in receipts:
        document = json.loads(receipt.read_text())
        if (document.get('schema') != 'objc3c.sanitizer-product-evidence.v1' or
                document.get('status') != 'PASS'):
            raise ValueError('product campaign contains incomplete evidence')
        private = receipt.parent / 'blobs'
        if private.is_symlink() or not private.is_dir():
            raise ValueError('product campaign private blob store is missing or linked')
        for source in sorted(private.iterdir()):
            if (not source.is_file() or source.is_symlink() or
                    not re.fullmatch(r'[0-9a-f]{64}\.zst', source.name)):
                raise ValueError('product campaign private blob store contains an invalid entry')
            private_bytes += source.stat().st_size
            target = store / source.name
            if target.exists():
                if (not target.is_file() or target.is_symlink() or
                        target.stat().st_size != source.stat().st_size or digest(target) != digest(source)):
                    raise ValueError('content-addressed product blob collision')
            else:
                try:
                    target.hardlink_to(source)
                except OSError:
                    shutil.copyfile(source, target)
            references.add(source.name)
        shutil.rmtree(private)
        private.symlink_to(Path('../..') / store.name, target_is_directory=True)
        # Exercise every rewritten route after the old private store is gone.
        def verify(value):
            if isinstance(value, dict):
                if 'blob' in value:
                    blob_bytes(receipt.parent, value)
                for child in value.values():
                    verify(child)
            elif isinstance(value, list):
                for child in value:
                    verify(child)
        verify(document)
    stored_bytes = sum(path.stat().st_size for path in store.iterdir())
    if {path.name for path in store.iterdir()} != references:
        raise ValueError('shared product blob store differs from receipt references')
    return {'schema': 'objc3c.product-evidence-shared-store.v1',
            'owners': len(receipts), 'references': len(references),
            'private_bytes': private_bytes, 'stored_bytes': stored_bytes,
            'deduplicated_bytes': private_bytes - stored_bytes,
            'path': store.name}


def phase_of(name, argv):
    if name.endswith(('-execute', '-execution')):
        return 'execute'
    if (Path(str(argv[0])).name in ('nm', 'readelf', 'llvm-nm', 'llvm-readobj',
            'llvm-dwarfdump', 'lldb', 'lldb-23') or '--list' in argv):
        return 'inspect'
    return 'build'


class ProductCommands:
    def __init__(self, output: Path, *, sources=(), budget=1024**3):
        self.output = output.resolve()
        self.output.mkdir(parents=True, exist_ok=False)
        (self.output / 'blobs').mkdir()
        self.budget, self.retained_bytes = budget, 0
        self.signatures = {}
        self.blobs = {}
        self.fixed, self.external = {}, {}
        self.execution_inputs = {}
        self.report = {'schema': 'objc3c.sanitizer-product-evidence.v1', 'status': 'running',
            'configurations': {}, 'source_inputs': {}, 'commands': [], 'external_inputs': {},
            'retained_byte_limit': budget, 'execution_replay': 'Fresh-directory native execution; compilation commands and exact source/artifact bytes are retained separately.'}
        for source in (*sources, Path(__file__), Path(__file__).with_name('llvm_ir_execution.py'),
                       Path(__file__).with_name('sdk_example.py')):
            root = Path(source).resolve(strict=True)
            paths = sorted(root.rglob('*')) if root.is_dir() else [root]
            for path in paths:
                if path.is_file() and (path.suffix in SOURCE or path.name == 'module.modulemap'):
                    self.fixed[str(path)] = self.retain(path)
        self.report['source_inputs'] = self.fixed
        self.save()

    @property
    def receipt(self):
        return self.output / 'qualification.json'

    def save(self):
        self.report['retained_bytes'] = self.retained_bytes
        self.report['external_inputs'] = self.external
        self.report['execution_inputs'] = self.execution_inputs
        write_json(self.receipt, self.report)

    def retain(self, path):
        # Ordinary installed SDK helpers also import this owner. Only sanitizer
        # evidence production/replay requires the development Python zstd API.
        from compression import zstd

        path = Path(path)
        signature = digest(path)
        size = path.stat().st_size
        blob = self.output / 'blobs' / (signature+'.zst')
        if signature not in self.blobs:
            if size > 64 * 1024**2:
                raise ValueError('sanitizer product evidence exceeds its retained artifact budget')
            packed = zstd.compress(path.read_bytes(), level=3)
            if len(packed) > min(64 * 1024**2, self.budget - self.retained_bytes):
                raise ValueError('sanitizer product evidence exceeds its retained artifact budget')
            blob.write_bytes(packed)
            if hashlib.sha256(zstd.decompress(packed)).hexdigest() != signature or digest(path) != signature:
                raise ValueError('product input changed while retaining evidence: '+str(path))
            self.retained_bytes += len(packed)
            self.blobs[signature] = {'sha256': signature, 'bytes': size, 'encoding': 'zstd',
                'stored_bytes': len(packed), 'stored_sha256': digest(blob), 'blob': 'blobs/'+blob.name}
        return {**self.blobs[signature], 'mode': path.stat().st_mode & 0o777}

    def signature(self, path, *, force=False):
        path = Path(path)
        stat = path.stat()
        stamp = (stat.st_dev, stat.st_ino, stat.st_size, stat.st_mtime_ns, stat.st_ctime_ns)
        previous = self.signatures.get(str(path))
        if force or previous is None or previous[0] != stamp:
            previous = (stamp, digest(path))
            self.signatures[str(path)] = previous
        return previous[1]

    def immutable(self, paths, *, execution=True):
        for value in paths:
            path = Path(value).resolve(strict=True)
            signature = self.signature(path)
            if str(path) in self.external and self.external[str(path)] != signature:
                raise ValueError('sanitizer product execution route changed: '+str(path))
            self.external[str(path)] = signature
            if execution:
                self.execution_inputs[str(path)] = signature

    def check(self, *, force=False):
        if any(not Path(path).is_file() or self.signature(Path(path), force=force) != row['sha256']
               for path, row in self.fixed.items()):
            raise ValueError('product fixture/header inputs changed during qualification')
        if any(not Path(path).is_file() or self.signature(Path(path), force=force) != signature
               for path, signature in self.external.items()):
            raise ValueError('sanitizer product execution route changed')

    def policy(self, gate):
        gate._check_immutable()
        if not gate._controls:
            raise ValueError('project execution requires successful detector controls first')
        for record in gate._controls.values():
            if digest(Path(record['path'])) != record['sha256']:
                raise ValueError('product detector-control receipt changed')
        if any(self.signature(Path(path)) != signature for path, signature in gate.inputs.items()):
            raise ValueError('product sanitizer compiler/resource/runtime input changed')
        self.immutable(gate.inputs, execution=False)
        self.immutable(path for path in gate.inputs if Path(path).suffix not in SOURCE)
        document = gate.receipt()
        identity = hashlib.sha256(json.dumps(document, sort_keys=True).encode()).hexdigest()
        if identity not in self.report['configurations']:
            log = self.output / ('compiler-version-'+identity[:16]+'.log')
            version = command([gate.compiler, '--no-default-config', '--version'], cwd=self.output,
                environment=gate.environment, log=log, timeout=30)
            if version['exit_code'] or version['timed_out']:
                raise ValueError('product compiler version probe failed')
            version['text'] = log.read_text(errors='replace')
            document['compiler_version'] = version
            self.report['configurations'][identity] = document
        return identity

    def view(self, work):
        files, links = {}, {}
        for path in sorted(work.rglob('*')):
            if path.is_symlink():
                links[str(path)] = str(path.resolve(strict=True))
            elif path.is_file() and (path.suffix in SOURCE | ARTIFACT or path.name == 'module.modulemap'):
                files[str(path)] = self.retain(path)
        return {'files': files, 'symlinks': links}

    def loader(self, gate, name, binary, work, required):
        log = self.output / (name+'-loader.log')
        row = command([gate.loader, '--list', binary], cwd=work,
            environment=gate.environment, log=log, timeout=30)
        text = log.read_text(errors='replace')
        if row['exit_code'] or row['timed_out'] or 'not found' in text:
            raise ValueError('actual product loader failed: '+name)
        loaded = {}
        expected = {path.resolve(strict=True) for path in gate.libraries if '.so' in path.name}
        for line in text.splitlines():
            value = line.split(' => ', 1)[-1].strip().rsplit(' (0x', 1)[0]
            if value.startswith('/'):
                path = Path(value).resolve(strict=True)
                if (path.name.startswith('libobjc.so') and expected and
                        any(item.name.startswith('libobjc.so') for item in expected) and path not in expected):
                    raise ValueError('product loaded an unselected Objective-C runtime')
                loaded[str(path)] = self.signature(path)
        if not loaded or not {str(Path(path).resolve(strict=True)) for path in required}.issubset(loaded):
            raise ValueError('actual product did not load its required runtime route')
        self.immutable(loaded)
        return {**row, 'libraries': loaded, 'selected_shared_not_loaded':
                sorted(str(path) for path in expected if str(path) not in loaded)}

    def run(self, gate, name, argv, *, cwd=None, log=None, expected=0, phase=None,
            required_libraries=None, timeout=180):
        if not re.fullmatch(r'[A-Za-z0-9_.+-]+', name) or any(row['name'] == name for row in self.report['commands']):
            raise ValueError('product command requires a unique bounded name')
        work = Path(cwd or Path.cwd()).resolve(strict=True)
        args = list(map(str, argv))
        tool = Path(shutil.which(args[0], path=gate.environment['PATH']) or args[0]).absolute()
        args[0] = str(tool)
        phase = phase or phase_of(name, args)
        row = {'name': name, 'phase': phase, 'expected_exit_code': expected, 'status': 'running'}
        self.report['commands'].append(row)
        log = Path(log or self.output / (name+'.log')).resolve()
        try:
            configuration = self.policy(gate)
            self.check()
            if phase != 'execute':
                self.immutable([tool])
            row.update(configuration=configuration, inputs=self.view(work) if work != self.output.parent else {'files': {}, 'symlinks': {}})
            sources = []
            for index, arg in enumerate(args[1:], 1):
                if arg.startswith('-'):
                    continue
                path = (work / arg).resolve()
                if path.is_file() and index > 0 and args[index-1] not in OUTPUT_FLAGS | {'-include', '-include-pch', '-imacros'}:
                    if path.suffix in SOURCE or (path.suffix in ARTIFACT and str(path) not in gate.inputs):
                        row['inputs']['files'][str(path)] = self.retain(path)
                        if path.suffix in {'.c', '.cc', '.cpp', '.cxx', '.m', '.mm', '.objc3', '.h', '.hpp', '.objc3i'}:
                            sources.append(path)
                if args[index-1] == '--module-search-path' and path.is_dir():
                    view = self.view(path)
                    for key in ('files', 'symlinks'):
                        row['inputs'][key].update(view[key])
            # Actual compiler depfiles bind the complete header closure, including
            # system headers. Multi-TU builds must use separate object commands.
            depfile = None
            if phase == 'build' and sources:
                if ('-fsanitize='+gate.sanitizer not in args or
                        any(arg.startswith('-fno-sanitize=') for arg in args) or
                        ('undefined' in gate.sanitizer and '-fno-sanitize-recover=undefined' not in args)):
                    raise ValueError('product compilation weakened its selected sanitizer flags')
                if '-resource-dir' not in args or Path(args[args.index('-resource-dir')+1]).resolve() != gate.resource:
                    raise ValueError('product compilation selected the wrong sanitizer resource directory')
            if phase == 'build' and sources and expected == 0:
                if len(sources) != 1:
                    raise ValueError('sanitizer product compilation must own one translation unit')
                depfile = self.output / (name+'.d')
                args += ['-MD', '-MF', str(depfile), '-MT', 'product']
            if phase == 'build' and '-c' not in args and '-o' in args:
                shared = {str(path.resolve(strict=True)) for path in gate.libraries if '.so' in path.name}
                args = [part for arg in args for part in
                    (['-Wl,--no-as-needed', arg, '-Wl,--as-needed'] if arg in shared or
                        (arg == '-lobjc' and any('libobjc.so' in path for path in shared)) else [arg])]
            if phase in ('build', 'execute'):
                required = tuple(path for path in gate.libraries if '.so' in path.name) if phase == 'execute' else ()
                if required_libraries is not None:
                    required = required_libraries
                row['required_shared_libraries'] = list(map(str, required))
                row['loaded_graph'] = self.loader(gate, name, tool, work, required)
            if phase == 'execute':
                row['executable'] = self.retain(tool)
            row.update(command(args, cwd=work, environment=gate.environment, log=log, timeout=timeout))
            row['signal'] = -row['exit_code'] if row['exit_code'] < 0 else None
            row['environment'] = dict(gate.environment)
            self.save()
            if row['timed_out'] or row['exit_code'] != expected:
                raise ValueError('product command failed: '+name+'; see '+str(log))
            require_clean_diagnostic(log.read_text(errors='replace'))
            if depfile is not None:
                if not depfile.is_file():
                    raise ValueError('successful product compilation omitted its header dependency evidence')
                headers = {}
                virtual_headers = {}
                for path in dependencies(depfile, work):
                    if path.is_file():
                        headers[str(path)] = self.retain(path)
                        if not path.is_relative_to(work):
                            self.immutable([path], execution=False)
                    elif '__objc3_imports' in path.parts:
                        try:
                            module = bytes.fromhex(path.name.removeprefix('m-').removesuffix('.objc3i')).decode('utf-8')
                            matches = {}
                            for name, metadata in row['inputs']['files'].items():
                                if name.endswith('.objc3i.json'):
                                    document = json.loads(blob_bytes(self.output, metadata))
                                    artifact = name.removesuffix('.json')
                                    if document.get('module', {}).get('canonical_name') == module and artifact in row['inputs']['files']:
                                        original = row['inputs']['files'][artifact]
                                        matches[original['sha256']] = {'artifact': artifact, **original}
                            if len(matches) != 1:
                                raise ValueError('virtual interface has missing or conflicting retained module ownership')
                            virtual_headers[str(path)] = next(iter(matches.values()))
                        except (ValueError, UnicodeError) as error:
                            raise ValueError('product virtual header lacks exact retained interface evidence') from error
                    else:
                        raise ValueError('product dependency has no retained physical input: '+str(path))
                row['headers'] = headers
                row['virtual_headers'] = virtual_headers
                row['dependency_file'] = self.retain(depfile)
            row['outputs'] = {}
            for index, arg in enumerate(args[:-1]):
                if arg in OUTPUT_FLAGS:
                    path = (work / args[index+1]).resolve()
                    for product in (path, path.with_suffix(path.suffix+'.json')):
                        if product.is_file():
                            row['outputs'][str(product)] = self.retain(product)
            row['after'] = self.view(work) if work != self.output.parent else {'files': {}, 'symlinks': {}}
            row['log_relative'] = os.path.relpath(log, self.output)
            self.policy(gate)
            self.check()
            row['status'] = 'PASS'
        except BaseException as error:
            row.update(status='FAIL', error=str(error))
            self.report.update(status='FAIL', error=str(error))
            raise
        finally:
            self.save()
        return row

    def record(self, gate, name, argv, rows, **configuration):
        start = len(self.report['commands'])
        try:
            return self.run(gate, name, argv, **configuration)
        finally:
            rows.extend(self.report['commands'][start:])

    def finalize(self):
        try:
            self.check(force=True)
            if (not self.report['commands'] or any(row['status'] != 'PASS' for row in self.report['commands']) or
                    not any(row['phase'] == 'execute' for row in self.report['commands'])):
                raise ValueError('incomplete sanitizer product execution evidence')
            for row in self.report['commands']:
                if digest(Path(row['log'])) != row['log_sha256']:
                    raise ValueError('product diagnostic artifact changed')
            for configuration in self.report['configurations'].values():
                version = configuration['compiler_version']
                if digest(Path(version['log'])) != version['log_sha256']:
                    raise ValueError('product compiler diagnostic artifact changed')
            for record in self.blobs.values():
                blob_bytes(self.output, record)
        except (OSError, ValueError) as error:
            self.report.update(status='FAIL', error=str(error))
            self.save()
            raise
        self.report['status'] = 'PASS'
        self.save()
        return {'path': str(self.receipt), 'sha256': digest(self.receipt),
                'commands': len(self.report['commands']), 'retained_bytes': self.retained_bytes}


def blob_bytes(root: Path, record):
    from compression import zstd

    root = root.resolve(strict=True)
    relative = Path(record['blob'])
    if (relative.parent != Path('blobs') or
            not re.fullmatch(r'[0-9a-f]{64}\.zst', relative.name)):
        raise ValueError('retained product input/artifact changed or escaped its evidence root')
    logical_store = root / 'blobs'
    expected_store = root.parent.parent / 'product-evidence-blobs'
    if logical_store.is_symlink():
        if (root.name != 'product-evidence' or not expected_store.is_dir() or expected_store.is_symlink() or
                expected_store.resolve(strict=True) != expected_store or
                logical_store.resolve(strict=True) != expected_store.resolve(strict=True)):
            raise ValueError('retained product shared blob store escaped its campaign')
        store = logical_store.resolve(strict=True)
    elif (not logical_store.exists() and root.name == 'product-evidence' and
            expected_store.is_dir() and not expected_store.is_symlink() and
            expected_store.resolve(strict=True) == expected_store):
        # upload-artifact retains the campaign store but does not promise to
        # preserve directory symlinks. The receipt's content-addressed name is
        # therefore resolved against the only admitted campaign store after an
        # artifact is extracted.
        store = expected_store.resolve(strict=True)
    elif logical_store.resolve(strict=True) != logical_store:
        raise ValueError('retained product private blob store escaped its evidence root')
    else:
        store = logical_store
    blob = store / relative.name
    if (not blob.is_file() or blob.is_symlink() or record.get('encoding') != 'zstd' or
            record['bytes'] > 64 * 1024**2 or blob.stat().st_size != record['stored_bytes'] or
            blob.stat().st_size > 64 * 1024**2 or digest(blob) != record['stored_sha256']):
        raise ValueError('retained product input/artifact changed or escaped its evidence root')
    try:
        with zstd.ZstdFile(blob, 'rb') as stream:
            value = stream.read(record['bytes']+1)
    except (OSError, zstd.ZstdError) as error:
        raise ValueError('retained product input/artifact changed') from error
    if len(value) != record['bytes'] or hashlib.sha256(value).hexdigest() != record['sha256']:
        raise ValueError('retained product input/artifact changed')
    return value


def replay(receipt: Path, name: str, output: Path):
    """Replay an admitted native execution with retained bytes in a fresh cwd."""
    document = json.loads(receipt.read_text())
    if document.get('schema') != 'objc3c.sanitizer-product-evidence.v1' or document.get('status') != 'PASS':
        raise ValueError('native execution replay requires a complete product receipt')
    matches = [row for row in document['commands'] if row['name'] == name and row['phase'] == 'execute']
    if len(matches) != 1 or output.exists():
        raise ValueError('select one native execution and a fresh replay directory')
    row = matches[0]
    for path, signature in document['execution_inputs'].items():
        if not Path(path).is_file() or digest(Path(path)) != signature:
            raise ValueError('immutable replay execution route changed: '+path)
    root = receipt.parent.resolve(strict=True)
    checked = set()
    def verify_blobs(value):
        if isinstance(value, dict):
            if 'blob' in value and 'sha256' in value:
                # Only blob_bytes owns private/shared-store resolution. Uploads
                # can omit the private directory symlink after consolidation.
                # Cache the complete record so conflicting metadata is checked.
                key = json.dumps(value, sort_keys=True, separators=(',', ':'))
                if key not in checked:
                    blob_bytes(root, value)
                    checked.add(key)
            for item in value.values():
                verify_blobs(item)
        elif isinstance(value, list):
            for item in value:
                verify_blobs(item)
    verify_blobs(document)
    for command_row in document['commands']:
        log = (root / command_row['log_relative']).resolve(strict=True)
        if not log.is_relative_to(root.parent) or digest(log) != command_row['log_sha256']:
            raise ValueError('retained replay diagnostic changed')
    def restore(record, path):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(blob_bytes(root, record))
        path.chmod(record['mode'])
    output.mkdir(parents=True)
    original = Path(row['cwd'])
    for path, record in row['inputs']['files'].items():
        source = Path(path)
        if source.is_relative_to(original):
            destination = (output / source.relative_to(original)).resolve()
            if not destination.is_relative_to(output.resolve()):
                raise ValueError('native replay input escaped its fresh output')
            restore(record, destination)
    binary = output / 'replay-program'
    restore(row['executable'], binary)
    argv = [str(binary), *(arg.replace(str(original), str(output)) for arg in row['argv'][1:])]
    result = command(argv, cwd=output, environment=row['environment'], log=output / 'execution.log', timeout=180)
    result.update(subject='native-execution-replay', original_command=name, receipt_sha256=digest(receipt), status='FAIL')
    if not result['timed_out'] and result['exit_code'] == row['expected_exit_code']:
        require_clean_diagnostic((output / 'execution.log').read_text(errors='replace'))
        result['status'] = 'PASS'
    write_json(output / 'qualification.json', result)
    if result['status'] != 'PASS':
        raise ValueError('retained native execution did not reproduce its expected disposition')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--receipt', type=Path, required=True)
    parser.add_argument('--command', required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = replay(args.receipt.resolve(strict=True), args.command, args.output.resolve())
    print(json.dumps({'status': result['status']}))


if __name__ == '__main__':
    main()
