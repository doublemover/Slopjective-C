$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Resolve-RepoBoundPath {
  param(
    [string]$RepoRoot,
    [string]$RelativeOrAbsolutePath,
    [string]$Label
  )

  if ([string]::IsNullOrWhiteSpace($RelativeOrAbsolutePath)) {
    Write-Error "$Label path is empty"
    exit 2
  }

  $candidatePath = $RelativeOrAbsolutePath
  if (-not [System.IO.Path]::IsPathRooted($candidatePath)) {
    $normalizedRelative = $candidatePath.Replace('\', '/')
    foreach ($segment in $normalizedRelative.Split('/')) {
      if ($segment -eq "..") {
        Write-Error "$Label path must not contain '..' relative segments: $RelativeOrAbsolutePath"
        exit 2
      }
    }
    $candidatePath = Join-Path $RepoRoot $candidatePath
  }

  $resolvedRoot = [System.IO.Path]::GetFullPath($RepoRoot).TrimEnd('\', '/')
  $resolvedCandidate = [System.IO.Path]::GetFullPath($candidatePath)
  $rootPrefix = $resolvedRoot + [System.IO.Path]::DirectorySeparatorChar
  if (($resolvedCandidate -ne $resolvedRoot) -and
      (-not $resolvedCandidate.StartsWith($rootPrefix, [System.StringComparison]::OrdinalIgnoreCase))) {
    Write-Error "$Label path escapes repository root: $RelativeOrAbsolutePath"
    exit 2
  }

  return $resolvedCandidate
}

function Get-Objc3cNativeCompileInputPath {
  param([string[]]$ArgsWithoutOutDir)

  $argsWithoutOutDir = @($ArgsWithoutOutDir)
  if ($argsWithoutOutDir.Count -le 0) {
    return $null
  }

  $inputCandidate = $argsWithoutOutDir[0]
  if ([string]::IsNullOrWhiteSpace($inputCandidate)) {
    return $null
  }

  return [System.IO.Path]::GetFullPath($inputCandidate)
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeCompileInputPath",
  "Resolve-RepoBoundPath"
)
