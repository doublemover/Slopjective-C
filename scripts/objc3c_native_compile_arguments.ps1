$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Show-Objc3cNativeCompileUsageAndExit {
  Write-Error "usage: objc3c_native_compile.ps1 <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--use-cache]"
  exit 2
}

function Parse-Objc3cNativeCompileArguments {
  param(
    [string[]]$RawArgs,
    [Parameter(Mandatory = $true)][string]$DefaultOutDir
  )

  if ($RawArgs.Count -lt 1) {
    Show-Objc3cNativeCompileUsageAndExit
  }

  $useCache = $false
  $compileArgs = New-Object System.Collections.Generic.List[string]
  $outDir = $null
  $emitPrefix = "module"
  $wrapperFlagCounts = @{
    "--use-cache" = 0
    "--out-dir" = 0
  }

  for ($i = 0; $i -lt $RawArgs.Count; $i++) {
    $token = $RawArgs[$i]
    if ($token -eq "--use-cache") {
      $wrapperFlagCounts["--use-cache"] = [int]$wrapperFlagCounts["--use-cache"] + 1
      if ([int]$wrapperFlagCounts["--use-cache"] -gt 1) {
        Write-Error "--use-cache can be provided at most once"
        exit 2
      }
      $useCache = $true
      continue
    }
    if ($token.StartsWith("--use-cache=", [System.StringComparison]::OrdinalIgnoreCase)) {
      $wrapperFlagCounts["--use-cache"] = [int]$wrapperFlagCounts["--use-cache"] + 1
      if ([int]$wrapperFlagCounts["--use-cache"] -gt 1) {
        Write-Error "--use-cache can be provided at most once"
        exit 2
      }
      $rawBoolean = $token.Substring("--use-cache=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $rawBoolean) {
        $useCache = $true
        continue
      }
      if (@("0", "false", "no", "off") -contains $rawBoolean) {
        $useCache = $false
        continue
      }
      Write-Error "invalid --use-cache value '$rawBoolean' (expected true/false style token)"
      exit 2
    }

    if ($token -eq "--out-dir") {
      $wrapperFlagCounts["--out-dir"] = [int]$wrapperFlagCounts["--out-dir"] + 1
      if ([int]$wrapperFlagCounts["--out-dir"] -gt 1) {
        Write-Error "--out-dir can be provided at most once"
        exit 2
      }
      if (($i + 1) -ge $RawArgs.Count) {
        Write-Error "missing value for --out-dir"
        exit 2
      }
      $i++
      $value = $RawArgs[$i]
      if ([string]::IsNullOrWhiteSpace($value)) {
        Write-Error "empty value for --out-dir"
        exit 2
      }
      $compileArgs.Add("--out-dir")
      $compileArgs.Add($value)
      $outDir = $value
      continue
    }

    if ($token.StartsWith("--out-dir=", [System.StringComparison]::Ordinal)) {
      $wrapperFlagCounts["--out-dir"] = [int]$wrapperFlagCounts["--out-dir"] + 1
      if ([int]$wrapperFlagCounts["--out-dir"] -gt 1) {
        Write-Error "--out-dir can be provided at most once"
        exit 2
      }
      $value = $token.Substring("--out-dir=".Length)
      if ([string]::IsNullOrWhiteSpace($value)) {
        Write-Error "empty value for --out-dir"
        exit 2
      }
      $compileArgs.Add("--out-dir")
      $compileArgs.Add($value)
      $outDir = $value
      continue
    }

    if ($token.StartsWith("--emit-prefix=", [System.StringComparison]::Ordinal)) {
      $value = $token.Substring("--emit-prefix=".Length)
      if ([string]::IsNullOrWhiteSpace($value)) {
        Write-Error "empty value for --emit-prefix"
        exit 2
      }
      $emitPrefix = $value
      $compileArgs.Add($token)
      continue
    }

    if ($token -eq "--emit-prefix") {
      if (($i + 1) -ge $RawArgs.Count) {
        Write-Error "missing value for --emit-prefix"
        exit 2
      }
      $i++
      $value = $RawArgs[$i]
      if ([string]::IsNullOrWhiteSpace($value)) {
        Write-Error "empty value for --emit-prefix"
        exit 2
      }
      $emitPrefix = $value
      $compileArgs.Add("--emit-prefix")
      $compileArgs.Add($value)
      continue
    }

    $compileArgs.Add($token)
  }

  if ($compileArgs.Count -lt 1) {
    Show-Objc3cNativeCompileUsageAndExit
  }

  if ([string]::IsNullOrWhiteSpace($outDir)) {
    $outDir = $DefaultOutDir
    $compileArgs.Add("--out-dir")
    $compileArgs.Add($outDir)
  }

  return [pscustomobject]@{
    use_cache = $useCache
    compile_args = $compileArgs.ToArray()
    out_dir = $outDir
    emit_prefix = $emitPrefix
  }
}

function Get-Objc3cNativeCompileArgsWithoutOutDir {
  param([string[]]$CompileArgs)

  $result = New-Object System.Collections.Generic.List[string]
  for ($i = 0; $i -lt $CompileArgs.Count; $i++) {
    $token = $CompileArgs[$i]
    if ($token -eq "--out-dir") {
      if (($i + 1) -ge $CompileArgs.Count) {
        break
      }
      $i++
      continue
    }
    $result.Add($token)
  }
  return $result.ToArray()
}
