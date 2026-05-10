Set-StrictMode -Version Latest

function Write-RunnableToolchainPackageStatus {
  param([Parameter(Mandatory = $true)]$ManifestPayload)

  Write-Output "status: PASS"
  Write-Output ("package_root: " + $ManifestPayload.package_root)
  Write-Output ("manifest: " + $ManifestPayload.manifest_artifact)
}

Export-ModuleMember -Function @("Write-RunnableToolchainPackageStatus")
