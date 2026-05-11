$ErrorActionPreference = "Stop"

function Invoke-Objc3cNativeCMakeBuild {
  param(
    [Parameter(Mandatory = $true)]
    [string]$CmakeTool,
    [Parameter(Mandatory = $true)]
    [string]$BuildDir
  )

  Write-Objc3cNativeBuildStep "cmake_build_start=native-binaries"
  & $CmakeTool --build $BuildDir --parallel --target objc3c-native objc3c_tools_frontend_c_api_runner objc3_runtime
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
  Write-Objc3cNativeBuildStep "cmake_build_done=native-binaries"
}
