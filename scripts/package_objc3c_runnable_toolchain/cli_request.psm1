Set-StrictMode -Version Latest

function Get-DefaultRunnableToolchainManifestRelativePath {
  return "artifacts/package/objc3c-runnable-toolchain-package.json"
}

function New-RunnableToolchainPackageRequest {
  param(
    [string]$PackageRoot = "",
    [string]$ManifestRelativePath = (Get-DefaultRunnableToolchainManifestRelativePath)
  )

  return [ordered]@{
    PackageRoot = $PackageRoot
    ManifestRelativePath = $ManifestRelativePath
  }
}

Export-ModuleMember -Function @(
  "Get-DefaultRunnableToolchainManifestRelativePath",
  "New-RunnableToolchainPackageRequest"
)
