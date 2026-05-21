$ErrorActionPreference = "Stop"

function Invoke-Objc3cNativeCMakeBuild {
  param(
    [Parameter(Mandatory = $true)]
    [string]$CmakeTool,
    [Parameter(Mandatory = $true)]
    [string]$BuildDir,
    [int]$Parallelism = 4
  )

  Write-Objc3cNativeBuildStep "cmake_build_start=native-binaries"
  if ($Parallelism -lt 1) {
    throw "native CMake build parallelism must be a positive integer"
  }
  Write-Objc3cNativeBuildStep ("cmake_build_parallelism=" + $Parallelism)
  & $CmakeTool --build $BuildDir --parallel $Parallelism --target objc3c-native objc3c_tools_frontend_c_api_runner objc3_runtime
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
  Write-Objc3cNativeBuildStep "cmake_build_done=native-binaries"
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cNativeCMakeBuild"
)
