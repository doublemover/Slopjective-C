$ErrorActionPreference = "Stop"

function Write-Objc3cNativeJsonArtifactFile {
  param(
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    $Payload,
    [int]$Depth = 8
  )

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }

  $leaf = Split-Path -Leaf $OutputPath
  $tempPath = Join-Path $parent ('.' + $leaf + '.' + [Guid]::NewGuid().ToString('N') + '.tmp')
  $json = $Payload | ConvertTo-Json -Depth $Depth
  Set-Content -LiteralPath $tempPath -Value $json -Encoding utf8
  $overwriteMoveMethod = [System.IO.File].GetMethod("Move", [Type[]]@([string], [string], [bool]))
  $maxAttempts = 12
  for ($attempt = 1; $attempt -le $maxAttempts; $attempt++) {
    try {
      if ($null -ne $overwriteMoveMethod) {
        [System.IO.File]::Move($tempPath, $OutputPath, $true)
      }
      else {
        [System.IO.File]::Copy($tempPath, $OutputPath, $true)
        Remove-Item -LiteralPath $tempPath -Force
      }
      return
    }
    catch {
      if ($attempt -eq $maxAttempts) {
        throw
      }
      Start-Sleep -Milliseconds 100
    }
  }
}

function Get-Objc3cNativeRepoRelativePath {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RootPath,
    [Parameter(Mandatory = $true)]
    [string]$TargetPath
  )

  $resolvedRoot = (Resolve-Path -LiteralPath $RootPath).Path.TrimEnd('\', '/')
  if (Test-Path -LiteralPath $TargetPath) {
    $resolvedTarget = (Resolve-Path -LiteralPath $TargetPath).Path
  } else {
    $resolvedTarget = [System.IO.Path]::GetFullPath($TargetPath)
  }
  $rootUri = [System.Uri]::new(($resolvedRoot + '\'))
  $targetUri = [System.Uri]::new($resolvedTarget)
  $relative = [System.Uri]::UnescapeDataString($rootUri.MakeRelativeUri($targetUri).ToString())
  return $relative.Replace('\', '/')
}

function Get-Objc3cNativeFileSha256Hex {
  param(
    [Parameter(Mandatory = $true)]
    [string]$Path
  )

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "cannot hash missing file: $Path"
  }

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  $stream = [System.IO.File]::OpenRead($Path)
  try {
    $hashBytes = $sha256.ComputeHash($stream)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $stream.Dispose()
    $sha256.Dispose()
  }
}

Export-ModuleMember -Function @(
  "Write-Objc3cNativeJsonArtifactFile",
  "Get-Objc3cNativeRepoRelativePath",
  "Get-Objc3cNativeFileSha256Hex"
)
