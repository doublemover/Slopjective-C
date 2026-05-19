$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$compileIoScriptRoot = Split-Path -Parent $PSScriptRoot
$hashIoModule = Join-Path $compileIoScriptRoot "objc3c_native_compile_provenance/hash_io.psm1"
if (!(Test-Path -LiteralPath $hashIoModule -PathType Leaf)) {
  Write-Error "native compile hash IO helper missing at $hashIoModule"
  exit 2
}
Import-Module $hashIoModule -Force -DisableNameChecking

function Get-Objc3cNativeCompileDirectoryDigest {
  param([string]$Path)

  if ([string]::IsNullOrWhiteSpace($Path)) {
    return ""
  }
  if (!(Test-Path -LiteralPath $Path -PathType Container)) {
    return ""
  }

  $resolvedRoot = (Resolve-Path -LiteralPath $Path).Path
  $files = Get-ChildItem -LiteralPath $Path -Recurse -File | Sort-Object -Property FullName
  $rows = New-Object System.Collections.Generic.List[string]
  foreach ($file in $files) {
    $relativePath = $file.FullName.Substring($resolvedRoot.Length).TrimStart('\', '/').Replace('\', '/')
    $fileHash = Get-FileSha256Hex -Path $file.FullName
    $rows.Add($relativePath + ":" + $fileHash)
  }

  $payloadText = [string]::Join("`n", $rows.ToArray())
  $payloadBytes = [System.Text.Encoding]::UTF8.GetBytes($payloadText)
  return Get-Sha256HexFromBytes -Bytes $payloadBytes
}

function Copy-Objc3cNativeCompileDirectoryContents {
  param(
    [string]$SourceRoot,
    [string]$DestinationRoot
  )

  if (!(Test-Path -LiteralPath $SourceRoot -PathType Container)) {
    return
  }

  New-Item -ItemType Directory -Force -Path $DestinationRoot | Out-Null
  $resolvedSourceRoot = (Resolve-Path -LiteralPath $SourceRoot).Path
  $files = Get-ChildItem -LiteralPath $SourceRoot -Recurse -File | Sort-Object -Property FullName

  foreach ($file in $files) {
    $relativePath = $file.FullName.Substring($resolvedSourceRoot.Length).TrimStart('\', '/')
    $destination = Join-Path $DestinationRoot $relativePath
    $parent = Split-Path -Parent $destination
    if (![string]::IsNullOrWhiteSpace($parent)) {
      New-Item -ItemType Directory -Force -Path $parent | Out-Null
    }
    Copy-Item -LiteralPath $file.FullName -Destination $destination -Force
  }
}

Export-ModuleMember -Function @(
  "Copy-Objc3cNativeCompileDirectoryContents",
  "Get-Objc3cNativeCompileDirectoryDigest"
)
