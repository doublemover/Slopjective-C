function Assert-SemaPassManagerPositiveReplayExitCodes {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$Run1Log,
    [Parameter(Mandatory = $true)][string]$Run2Log,
    [Parameter(Mandatory = $true)][int]$Run1Exit,
    [Parameter(Mandatory = $true)][int]$Run2Exit,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage
  )

  Assert-Contract `
    -Condition ($Run1Exit -eq 0 -and $Run2Exit -eq 0) `
    -Id $Id `
    -FailureMessage ($FailureMessage -f $Run1Exit, $Run2Exit) `
    -PassMessage $PassMessage `
    -Evidence @{
      run1_exit = $Run1Exit
      run2_exit = $Run2Exit
      run1_log = Get-RepoRelativePath -Path $Run1Log -Root $RepoRoot
      run2_log = Get-RepoRelativePath -Path $Run2Log -Root $RepoRoot
    }
}

function Assert-SemaPassManagerPositiveMatrixExitCodes {
  param(
    [Parameter(Mandatory = $true)][int]$Run1Exit,
    [Parameter(Mandatory = $true)][int]$Run2Exit,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage
  )

  Assert-Contract `
    -Condition ($Run1Exit -eq 0 -and $Run2Exit -eq 0) `
    -Id $Id `
    -FailureMessage ($FailureMessage -f $Run1Exit, $Run2Exit) `
    -PassMessage $PassMessage
}
