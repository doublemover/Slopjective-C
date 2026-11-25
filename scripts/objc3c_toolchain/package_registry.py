"""Publish and resolve exact immutable native package locks over static HTTP."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path
import tempfile
from urllib.parse import urljoin, urlsplit
from urllib.request import urlopen

if __package__:
    from . import binary_packages as packages
else:
    import binary_packages as packages

SCHEMA = 'objc3c.package-registry.v1'
ENTRY = {'archive_sha256', 'archive_bytes', 'sdk_seal', 'target', 'optimization', 'modules'}


def load_index(data: bytes) -> dict:
    index = json.loads(data)
    packages.fields(index, {'schema', 'packages'}, 'package registry')
    if index['schema'] != SCHEMA or not isinstance(index['packages'], dict):
        raise ValueError('unsupported package registry schema')
    for signature, entry in index['packages'].items():
        packages.check_hash(signature)
        packages.fields(entry, ENTRY, 'package registry entry')
        packages.check_hash(entry['archive_sha256'])
        packages.check_hash(entry['sdk_seal'])
        if (type(entry['archive_bytes']) is not int or entry['archive_bytes'] < 1 or
                not isinstance(entry['target'], str) or not entry['target'] or
                entry['optimization'] not in ('O0', 'O2') or
                not isinstance(entry['modules'], dict) or not entry['modules'] or
                any(not packages.NAME.fullmatch(name) or not isinstance(identity, str) or not identity.strip()
                    for name, identity in entry['modules'].items())):
            raise ValueError('invalid package registry compatibility or module identity')
    return index


def package_entry(inspection: dict) -> dict:
    return {**{name: inspection[name] for name in ('sdk_seal', 'target', 'optimization')},
            'modules': {row['module']['canonical_name']: row['module']['package_identity']
                        for row in inspection['modules']}}


def publish(source: Path, expected: str, directory: Path) -> dict:
    directory = directory.resolve()
    packages.disjoint(directory, source.resolve(strict=True))
    entry = package_entry(packages.inspect(source, expected, []))
    index_path = directory / 'index.json'
    with packages.lock(directory, '.registry.lock'), tempfile.TemporaryDirectory(dir=directory) as scratch:
        index = load_index(index_path.read_bytes()) if index_path.exists() else {'schema': SCHEMA, 'packages': {}}
        temporary = Path(scratch) / 'package.zip'
        exported = packages.export_archive(source, expected, temporary)
        entry.update(archive_sha256=exported['archive_sha256'], archive_bytes=temporary.stat().st_size)
        previous = index['packages'].setdefault(expected, entry)
        if previous != entry:
            raise ValueError('immutable registry entry differs from its existing archive lock')
        archive = directory / 'archives' / (entry['archive_sha256'] + '.zip')
        archive.parent.mkdir(exist_ok=True)
        if archive.exists() or archive.is_symlink():
            if archive.is_symlink() or not archive.is_file() or packages.digest(archive) != entry['archive_sha256']:
                raise ValueError('existing registry archive differs from its content lock')
        else:
            temporary.rename(archive)
        packages.write_json(index_path, index, compact=True)
        return {'status': 'PASS', 'index': str(index_path), 'index_sha256': packages.digest(index_path),
                'content_sha256': expected, 'archive_sha256': entry['archive_sha256'], 'archive': str(archive)}


def http_url(url: str) -> str:
    parts = urlsplit(url)
    if parts.scheme not in ('https', 'http') or not parts.hostname or parts.username or parts.password or parts.fragment:
        raise ValueError('package registry requires an HTTP(S) index URL without credentials or a fragment')
    return url


def verify_entry(inspected: dict, entry: dict) -> None:
    if package_entry(inspected) != {key: entry[key] for key in ENTRY - {'archive_bytes', 'archive_sha256'}}:
        raise ValueError('registry compatibility or module inventory differs from the verified package')


def fetch(sdk: Path, index_url: str, index_sha256: str, expected: str, modules: list,
          store: Path, *, offline: bool = False) -> dict:
    http_url(index_url)
    packages.check_hash(index_sha256)
    packages.check_hash(expected)
    sdk, store = sdk.resolve(strict=True), store.resolve()
    packages.disjoint(store, sdk)
    sdk_data = packages.sdk_identity(sdk)
    cached_index = store / '.registry' / (index_sha256 + '.json')
    installed = store / expected
    if cached_index.exists():
        if cached_index.is_symlink() or not cached_index.is_file() or packages.digest(cached_index) != index_sha256:
            raise ValueError('cached registry index differs from the requested lock')
        data = cached_index.read_bytes()
    elif offline:
        raise ValueError('offline registry replay requires the pinned index in the cache')
    else:
        with urlopen(index_url) as response:
            http_url(response.geturl())
            data = response.read()
    if hashlib.sha256(data).hexdigest() != index_sha256:
        raise ValueError('registry index SHA-256 differs from the requested lock')
    index = load_index(data)
    entry = index['packages'].get(expected)
    if entry is None:
        raise ValueError('registry does not contain the requested package content lock')
    if not modules or any(entry['modules'].get(name) != identity for name, identity in modules):
        raise ValueError('registry entry does not match the requested module identity locks')
    if entry['target'] != sdk_data['target'] or entry['sdk_seal'] != sdk_data['seal_sha256']:
        raise ValueError('registry entry requires the exact producing SDK seal and target')

    cache_hit = installed.exists() or installed.is_symlink()
    if cache_hit:
        verify_entry(packages.inspect(installed, expected, []), entry)
    elif offline:
        raise ValueError('offline registry replay requires the locked package in the cache')
    else:
        store.parent.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix='.package-fetch-', dir=store.parent) as temporary:
            archive = Path(temporary) / 'package.zip'
            url = urljoin(index_url, 'archives/' + entry['archive_sha256'] + '.zip')
            with urlopen(url) as response, archive.open('xb') as target:
                http_url(response.geturl())
                received = 0
                while chunk := response.read(1024 * 1024):
                    received += len(chunk)
                    if received > entry['archive_bytes']:
                        raise ValueError('registry archive exceeds the pinned byte count')
                    target.write(chunk)
            if received != entry['archive_bytes'] or packages.digest(archive) != entry['archive_sha256']:
                raise ValueError('registry archive differs from its size or SHA-256 lock')
            verify_entry(packages.inspect(archive, expected, []), entry)
            packages.install(archive, expected, store)
    if not cached_index.exists():
        cached_index.parent.mkdir(parents=True, exist_ok=True)
        with tempfile.NamedTemporaryFile(dir=cached_index.parent, delete=False) as stream:
            temporary = Path(stream.name)
            try:
                stream.write(data)
                stream.close()
                temporary.replace(cached_index)
            finally:
                temporary.unlink(missing_ok=True)
    return {'status': 'PASS', 'index_sha256': index_sha256, 'content_sha256': expected,
            'archive_sha256': entry['archive_sha256'], 'package': str(installed), 'cache_hit': cache_hit}
