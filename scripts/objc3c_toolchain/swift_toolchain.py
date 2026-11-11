"""Install/admit the signed, version-pinned independent Linux Swift consumer."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import platform
import shutil
import subprocess
import tarfile
import tempfile
from urllib.request import urlopen

from .binary_packages import digest, write_json


def policy(repo):
    path = repo / 'toolchains/swift/manifest.json'
    value = json.loads(path.read_text())
    if (value.get('schema') != 'objc3c.swift-consumer-toolchain.v1' or
            value.get('host') != 'debian13-x86_64' or value.get('version') != '6.4.2' or
            value.get('signing_fingerprint') != '52BB7E3DE28A71BE22EC05FFEF80A866B47A981F' or
            not value['archive_url'].startswith('https://download.swift.org/') or
            digest(repo / value['public_key']) != value['public_key_sha256']):
        raise ValueError('unreviewed Swift consumer toolchain/key policy')
    return value, hashlib.sha256(json.dumps(value, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def environment(root):
    return {'PATH': str(root / 'usr/bin')+os.pathsep+os.defpath, 'LC_ALL': 'C',
            'SOURCE_DATE_EPOCH': '0', **{key: os.environ[key] for key in ('HOME', 'TMPDIR') if key in os.environ}}


def require_host():
    release = platform.freedesktop_os_release()
    if (platform.system() != 'Linux' or platform.machine() != 'x86_64' or
            release.get('ID') != 'debian' or release.get('VERSION_ID') != '13'):
        raise ValueError('Swift consumer requires the admitted Debian 13 Linux x86-64 host')
    return {'system': platform.system(), 'machine': platform.machine(), 'os_release': release}


def wanted(name, pin):
    path = PurePosixPath(name)
    if path.is_absolute() or '..' in path.parts:
        raise ValueError('Swift archive member escaped its relative root')
    if not path.parts or path.parts[0] != pin['archive_root']:
        raise ValueError('Swift archive has a different release root')
    relative = PurePosixPath(*path.parts[1:]).as_posix()
    if any(relative == root or relative.startswith(root+'/') for root in pin['excluded_directories']):
        return None
    return relative if (relative in ['usr/bin/'+name for name in pin['bin']] or
        relative in pin['library_files'] or
        any(relative == root or relative.startswith(root+'/') for root in pin['library_directories']) or
        'LICENSE' in path.name or 'NOTICE' in path.name) else None


def download(url, path, *, limit):
    with urlopen(url, timeout=60) as response, path.open('wb') as stream:
        count = 0
        while block := response.read(1024**2):
            count += len(block)
            if count > limit:
                raise ValueError('Swift download exceeded its pinned byte limit')
            stream.write(block)


def verify_archive(repo, archive, signature, work, pin):
    if archive.stat().st_size != pin['archive_bytes'] or digest(archive) != pin['archive_sha256']:
        raise ValueError('Swift consumer archive differs from its version pin')
    if digest(signature) != pin['signature_sha256']:
        raise ValueError('Swift consumer signature differs from its version pin')
    keyring = work / 'gnupg';keyring.mkdir(mode=0o700)
    gpg = shutil.which('gpg', path=os.defpath)
    if not gpg:
        raise ValueError('GnuPG is required to verify the pinned Swift release')
    commands = [[gpg, '--batch', '--homedir', keyring, '--import', repo / pin['public_key']],
                [gpg, '--batch', '--homedir', keyring, '--status-fd', '1', '--verify', signature, archive]]
    records = []
    for index, argv in enumerate(commands):
        result = subprocess.run(list(map(str, argv)), capture_output=True, text=True, timeout=120,
                                env={'PATH': os.defpath, 'LC_ALL': 'C'})
        log = work / ('signature-'+str(index)+'.log');log.write_text(result.stdout+result.stderr)
        records.append({'argv': list(map(str, argv)), 'exit_code': result.returncode,
                        'log': log.name, 'log_sha256': digest(log)})
        if result.returncode:
            raise ValueError('Swift release signature verification failed')
    if '[GNUPG:] VALIDSIG '+pin['signing_fingerprint']+' ' not in (work / 'signature-1.log').read_text():
        raise ValueError('Swift release used a different signing fingerprint')
    return {'verifier': str(gpg), 'sha256': digest(Path(gpg)), 'commands': records}


def verify(repo, root):
    require_host()
    pin, pin_hash = policy(repo)
    root = root.resolve(strict=True)
    receipt = json.loads((root / 'objc3-swift-consumer.json').read_text())
    if (receipt.get('schema') != 'objc3c.swift-consumer-install.v1' or
            receipt.get('pin_sha256') != pin_hash or not receipt.get('files') or
            receipt.get('archive_sha256') != pin['archive_sha256']):
        raise ValueError('Swift consumer seal differs from its current version pin')
    actual = {str(path.relative_to(root)) for path in root.rglob('*') if path.is_file() or path.is_symlink()}
    if actual != {*receipt['files'], 'objc3-swift-consumer.json'}:
        raise ValueError('Swift consumer installation inventory changed')
    for relative, row in receipt['files'].items():
        path = root / relative
        if row.get('link') is not None:
            if not path.is_symlink() or os.readlink(path) != row['link'] or not path.resolve(strict=True).is_relative_to(root):
                raise ValueError('Swift consumer link changed or escaped')
        elif path.is_symlink() or digest(path) != row['sha256'] or (path.stat().st_mode & 0o777) != row['mode']:
            raise ValueError('Swift consumer file changed: '+relative)
    selected = {name: row for name, row in receipt['files'].items()
                if name not in ('signature-0.log', 'signature-1.log')}
    if hashlib.sha256(json.dumps(selected, sort_keys=True, separators=(',', ':')).encode()).hexdigest() != pin['selected_manifest_sha256']:
        raise ValueError('Swift consumer file inventory differs from the signed publisher pin')
    return receipt


def install(repo, root, *, archive=None):
    repo, root = repo.resolve(strict=True), root.resolve()
    require_host()
    pin, pin_hash = policy(repo)
    if root.exists():
        return verify(repo, root)
    root.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='objc3-swift-download-') as scratch:
        work = Path(scratch)
        if archive is None:
            archive = work / 'swift.tar.gz'
            download(pin['archive_url'], archive, limit=pin['archive_bytes'])
        else:
            archive = archive.resolve(strict=True)
        signature = work / 'swift.tar.gz.sig'
        download(pin['archive_url']+'.sig', signature, limit=4096)
        proof = verify_archive(repo, archive, signature, work, pin)
        with tempfile.TemporaryDirectory(prefix='.swift-install-', dir=root.parent) as staging:
            destination = Path(staging)
            retained = 0
            with tarfile.open(archive, 'r|gz') as bundle:
                for member in bundle:
                    relative = wanted(member.name, pin)
                    if relative is None:
                        continue
                    if member.islnk() or not (member.isfile() or member.isdir() or member.issym()):
                        raise ValueError('unapproved Swift archive file type')
                    retained += member.size
                    if retained > 1024**3:
                        raise ValueError('selected Swift consumer files exceed the 1 GiB install budget')
                    bundle.extract(member.replace(name=relative), destination, filter='data')
            for name in pin['bin']:
                if not (destination / 'usr/bin' / name).is_file():
                    raise ValueError('selected Swift tool was not installed: '+name)
            for index in range(2):
                shutil.copyfile(work / ('signature-'+str(index)+'.log'), destination / ('signature-'+str(index)+'.log'))
            files = {str(path.relative_to(destination)):
                {'link': os.readlink(path)} if path.is_symlink() else {'sha256': digest(path), 'mode': path.stat().st_mode & 0o777}
                for path in sorted(destination.rglob('*')) if path.is_file() or path.is_symlink()}
            receipt = {'schema': 'objc3c.swift-consumer-install.v1', 'pin_sha256': pin_hash,
                'archive_sha256': pin['archive_sha256'], 'signature_verification': proof,
                'selected_bytes': retained, 'files': files}
            selected = {name: row for name, row in files.items() if name not in ('signature-0.log', 'signature-1.log')}
            if hashlib.sha256(json.dumps(selected, sort_keys=True, separators=(',', ':')).encode()).hexdigest() != pin['selected_manifest_sha256']:
                raise ValueError('Swift archive extraction differs from its signed publisher inventory')
            write_json(destination / 'objc3-swift-consumer.json', receipt)
            os.rename(destination, root)
    return verify(repo, root)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--root', type=Path, required=True)
    parser.add_argument('--archive', type=Path)
    args = parser.parse_args()
    result = install(args.repo, args.root, archive=args.archive)
    print(json.dumps({'status': 'PASS', 'selected_bytes': result['selected_bytes']}))


if __name__ == '__main__':
    main()
