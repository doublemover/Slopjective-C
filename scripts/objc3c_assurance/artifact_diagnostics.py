"""Inspect large retained artifacts using bounded HTTP ranges, without extraction."""
from __future__ import annotations
import io
import json
import os
import urllib.error
import urllib.request
import zipfile


class NoRedirect(urllib.request.HTTPRedirectHandler):
    def redirect_request(self, *args):
        return None


class RemoteZip(io.RawIOBase):
    def __init__(self, url):
        self.url, self.position, self.transferred = url, 0, 0
        with urllib.request.urlopen(urllib.request.Request(url, method='HEAD'), timeout=30) as response:
            self.size = int(response.headers['Content-Length'])

    def seekable(self):
        return True

    def seek(self, offset, whence=0):
        self.position = offset + (self.position if whence == 1 else self.size if whence == 2 else 0)
        return self.position

    def tell(self):
        return self.position

    def read(self, size=-1):
        size = min(self.size - self.position, size if size >= 0 else self.size)
        if size <= 0:
            return b''
        if self.transferred + size > 256 * 1024**2:
            raise ValueError('artifact inspection exceeds its bounded transfer budget')
        request = urllib.request.Request(self.url, headers={
            'Range': f'bytes={self.position}-{self.position + size - 1}'})
        with urllib.request.urlopen(request, timeout=30) as response:
            if response.status != 206:
                raise ValueError('artifact server must honor byte ranges')
            data = response.read(size + 1)
        if len(data) != size:
            raise ValueError('artifact byte range is incomplete')
        self.transferred += size
        self.position += size
        return data


def main():
    artifact = os.environ['ARTIFACT_ID']
    if not artifact.isdecimal():
        raise ValueError('artifact ID must be numeric')
    request = urllib.request.Request(
        f'https://api.github.com/repos/{os.environ["GITHUB_REPOSITORY"]}/actions/artifacts/{artifact}/zip',
        headers={'Authorization': 'Bearer ' + os.environ['GH_TOKEN']})
    try:
        urllib.request.build_opener(NoRedirect).open(request, timeout=30)
    except urllib.error.HTTPError as error:
        if error.code != 302:
            raise
        url = error.headers['Location']
    else:
        raise ValueError('artifact endpoint omitted its download redirect')
    remote = RemoteZip(url)
    with zipfile.ZipFile(remote) as archive:
        entries = archive.infolist()
        print(json.dumps({'files': len(entries), 'bytes': sum(row.file_size for row in entries),
                          'largest': [(row.filename, row.file_size) for row in
                                      sorted(entries, key=lambda row: row.file_size, reverse=True)[:40]]}))
        for row in entries:
            if (row.filename.endswith('/result.json') or row.filename.endswith('/campaign/qualification.json')) and row.file_size < 2 * 1024**2:
                document = json.loads(archive.read(row))
                print(json.dumps({'path': row.filename, 'receipt': document}))
    print(json.dumps({'transferred_bytes': remote.transferred}))


if __name__ == '__main__':
    main()
