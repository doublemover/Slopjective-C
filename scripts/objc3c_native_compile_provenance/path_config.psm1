$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-RepoRelativeDisplayPath {
  param(
    [string]$RepoRoot,
    [string]$Path
  )

  if ([string]::IsNullOrWhiteSpace($Path)) {
    return ""
  }

  $resolvedRoot = [System.IO.Path]::GetFullPath($RepoRoot).TrimEnd('\', '/')
  $resolvedPath = [System.IO.Path]::GetFullPath($Path)
  $rootPrefix = $resolvedRoot + [System.IO.Path]::DirectorySeparatorChar
  if ($resolvedPath -eq $resolvedRoot) {
    return "."
  }
  if ($resolvedPath.StartsWith($rootPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
    return [System.IO.Path]::GetRelativePath($resolvedRoot, $resolvedPath).Replace('\', '/')
  }
  return $resolvedPath.Replace('\', '/')
}

Export-ModuleMember -Function "Get-RepoRelativeDisplayPath"
