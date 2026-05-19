$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Parse-Objc3cNativeCompileArguments {
  param(
    [string[]]$RawArgs,
    [Parameter(Mandatory = $true)][string]$DefaultOutDir
  )

  if ($RawArgs.Count -lt 1) {
    Show-Objc3cNativeCompileUsageAndExit
  }

  $state = [pscustomobject]@{
    use_cache = $false
    compile_args = New-Object System.Collections.Generic.List[string]
    out_dir = $null
    emit_prefix = "module"
    wrapper_flag_counts = New-Objc3cNativeCompileWrapperFlagCounts
  }

  for ($i = 0; $i -lt $RawArgs.Count; $i++) {
    $token = $RawArgs[$i]
    if (Read-Objc3cNativeCompileUseCacheFlag -State $state -Token $token) {
      continue
    }
    if (Read-Objc3cNativeCompileOutDirFlag -State $state -Token $token -RawArgs $RawArgs -Index ([ref]$i)) {
      continue
    }
    if (Read-Objc3cNativeCompileEmitPrefixFlag -State $state -Token $token -RawArgs $RawArgs -Index ([ref]$i)) {
      continue
    }

    $state.compile_args.Add($token)
  }

  if ($state.compile_args.Count -lt 1) {
    Show-Objc3cNativeCompileUsageAndExit
  }

  if ([string]::IsNullOrWhiteSpace($state.out_dir)) {
    $state.out_dir = $DefaultOutDir
    $state.compile_args.Add("--out-dir")
    $state.compile_args.Add($state.out_dir)
  }

  return [pscustomobject]@{
    use_cache = $state.use_cache
    compile_args = $state.compile_args.ToArray()
    out_dir = $state.out_dir
    emit_prefix = $state.emit_prefix
  }
}

function Read-Objc3cNativeCompileUseCacheFlag {
  param(
    [Parameter(Mandatory = $true)]$State,
    [Parameter(Mandatory = $true)][string]$Token
  )

  if ($Token -eq "--use-cache") {
    Add-Objc3cNativeCompileWrapperFlagUse -FlagCounts $State.wrapper_flag_counts -FlagName "--use-cache"
    $State.use_cache = $true
    return $true
  }
  if ($Token.StartsWith("--use-cache=", [System.StringComparison]::OrdinalIgnoreCase)) {
    Add-Objc3cNativeCompileWrapperFlagUse -FlagCounts $State.wrapper_flag_counts -FlagName "--use-cache"
    $State.use_cache = ConvertTo-Objc3cNativeCompileBooleanFlagValue `
      -RawValue ($Token.Substring("--use-cache=".Length)) `
      -FlagName "--use-cache"
    return $true
  }

  return $false
}

function Read-Objc3cNativeCompileOutDirFlag {
  param(
    [Parameter(Mandatory = $true)]$State,
    [Parameter(Mandatory = $true)][string]$Token,
    [Parameter(Mandatory = $true)][string[]]$RawArgs,
    [Parameter(Mandatory = $true)][ref]$Index
  )

  if ($Token -eq "--out-dir") {
    Add-Objc3cNativeCompileWrapperFlagUse -FlagCounts $State.wrapper_flag_counts -FlagName "--out-dir"
    if (($Index.Value + 1) -ge $RawArgs.Count) {
      Stop-Objc3cNativeCompileArgumentError -Message "missing value for --out-dir"
    }
    $Index.Value++
    $value = $RawArgs[$Index.Value]
    if ([string]::IsNullOrWhiteSpace($value)) {
      Stop-Objc3cNativeCompileArgumentError -Message "empty value for --out-dir"
    }
    $State.compile_args.Add("--out-dir")
    $State.compile_args.Add($value)
    $State.out_dir = $value
    return $true
  }

  if ($Token.StartsWith("--out-dir=", [System.StringComparison]::Ordinal)) {
    Add-Objc3cNativeCompileWrapperFlagUse -FlagCounts $State.wrapper_flag_counts -FlagName "--out-dir"
    $value = $Token.Substring("--out-dir=".Length)
    if ([string]::IsNullOrWhiteSpace($value)) {
      Stop-Objc3cNativeCompileArgumentError -Message "empty value for --out-dir"
    }
    $State.compile_args.Add("--out-dir")
    $State.compile_args.Add($value)
    $State.out_dir = $value
    return $true
  }

  return $false
}

function Read-Objc3cNativeCompileEmitPrefixFlag {
  param(
    [Parameter(Mandatory = $true)]$State,
    [Parameter(Mandatory = $true)][string]$Token,
    [Parameter(Mandatory = $true)][string[]]$RawArgs,
    [Parameter(Mandatory = $true)][ref]$Index
  )

  if ($Token.StartsWith("--emit-prefix=", [System.StringComparison]::Ordinal)) {
    $value = $Token.Substring("--emit-prefix=".Length)
    if ([string]::IsNullOrWhiteSpace($value)) {
      Stop-Objc3cNativeCompileArgumentError -Message "empty value for --emit-prefix"
    }
    $State.emit_prefix = $value
    $State.compile_args.Add($Token)
    return $true
  }

  if ($Token -eq "--emit-prefix") {
    if (($Index.Value + 1) -ge $RawArgs.Count) {
      Stop-Objc3cNativeCompileArgumentError -Message "missing value for --emit-prefix"
    }
    $Index.Value++
    $value = $RawArgs[$Index.Value]
    if ([string]::IsNullOrWhiteSpace($value)) {
      Stop-Objc3cNativeCompileArgumentError -Message "empty value for --emit-prefix"
    }
    $State.emit_prefix = $value
    $State.compile_args.Add("--emit-prefix")
    $State.compile_args.Add($value)
    return $true
  }

  return $false
}
