Set-StrictMode -Version Latest

function Get-DefaultRunnableToolchainManifestRelativePath {
  return "artifacts/package/objc3c-runnable-toolchain-package.json"
}

function New-RunnableToolchainPackageRequest {
  param(
    [string]$PackageRoot = "",
    [string]$ManifestRelativePath = (Get-DefaultRunnableToolchainManifestRelativePath),
    [ValidateSet("release", "address", "undefined")]
    [string]$SanitizerVariant = "release"
  )

  return [ordered]@{
    PackageRoot = $PackageRoot
    ManifestRelativePath = $ManifestRelativePath
    SanitizerVariant = $SanitizerVariant
  }
}

Export-ModuleMember -Function @(
  "Get-DefaultRunnableToolchainManifestRelativePath",
  "New-RunnableToolchainPackageRequest"
)
