Set-StrictMode -Version Latest

function Get-ManifestProvenanceCoreToolchainFiles {
  return @(
    "package.json",
    "artifacts/bin/objc3c-native.exe",
    "artifacts/bin/objc3c-frontend-c-api-runner.exe",
    "artifacts/lib/objc3_runtime.lib",
    "scripts/build_objc3c_native.ps1",
    "scripts/objc3c_native_compile.ps1",
    "scripts/objc3c_workflow/__init__.py",
    "scripts/objc3c_workflow/__main__.py"
  )
}

Export-ModuleMember -Function @("Get-ManifestProvenanceCoreToolchainFiles")
