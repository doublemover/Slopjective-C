function Get-Objc3cNativeFrontendRepoRoot {
  return (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
}

function Get-Objc3cNativeFrontendModuleSources {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string[]]$ModuleRoots
  )

  $sourceRoot = Join-Path $RepoRoot "native/objc3c/src"
  $sources = New-Object System.Collections.Generic.List[string]
  foreach ($moduleRoot in $ModuleRoots) {
    $modulePath = Join-Path $sourceRoot $moduleRoot
    if (!(Test-Path -LiteralPath $modulePath -PathType Container)) {
      throw "frontend source module root missing: $modulePath"
    }

    $moduleSources = @(
      Get-ChildItem -LiteralPath $modulePath -Filter "*.cpp" -File -Recurse |
        Sort-Object FullName
    )
    foreach ($source in $moduleSources) {
      $sources.Add(([System.IO.Path]::GetRelativePath($RepoRoot, $source.FullName)).Replace("\", "/"))
    }
  }

  return $sources.ToArray()
}

function New-Objc3cNativeFrontendModuleEntry {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$Name,
    [Parameter(Mandatory = $true)]
    [string[]]$ModuleRoots
  )

  return [ordered]@{
    name = $Name
    sources = @(Get-Objc3cNativeFrontendModuleSources -RepoRoot $RepoRoot -ModuleRoots $ModuleRoots)
  }
}

function Get-Objc3cNativeFrontendModules {
  $repoRoot = Get-Objc3cNativeFrontendRepoRoot
  return @(
    (New-Objc3cNativeFrontendModuleEntry `
      -RepoRoot $repoRoot `
      -Name "config-support" `
      -ModuleRoots @("config", "support", "token", "ast", "cli")),
    (New-Objc3cNativeFrontendModuleEntry `
      -RepoRoot $repoRoot `
      -Name "driver" `
      -ModuleRoots @("driver")),
    (New-Objc3cNativeFrontendModuleEntry `
      -RepoRoot $repoRoot `
      -Name "diagnostics-io" `
      -ModuleRoots @("diag", "diagnostics", "io")),
    (New-Objc3cNativeFrontendModuleEntry `
      -RepoRoot $repoRoot `
      -Name "ir" `
      -ModuleRoots @("ir")),
    (New-Objc3cNativeFrontendModuleEntry `
      -RepoRoot $repoRoot `
      -Name "lex-parse" `
      -ModuleRoots @("lex", "parse")),
    (New-Objc3cNativeFrontendModuleEntry `
      -RepoRoot $repoRoot `
      -Name "frontend-api" `
      -ModuleRoots @("libobjc3c_frontend")),
    (New-Objc3cNativeFrontendModuleEntry `
      -RepoRoot $repoRoot `
      -Name "lowering" `
      -ModuleRoots @("lower")),
    (New-Objc3cNativeFrontendModuleEntry `
      -RepoRoot $repoRoot `
      -Name "pipeline" `
      -ModuleRoots @("artifacts", "pipeline")),
    (New-Objc3cNativeFrontendModuleEntry `
      -RepoRoot $repoRoot `
      -Name "sema" `
      -ModuleRoots @("sema"))
  )
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeFrontendModules"
)
