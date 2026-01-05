param (
        [ValidateSet('2022', '2026')] [string]$vsVersion
      )

$cmake = "cmake"
if ($vsVersion -eq '2022')
{
    $args = @("-S", ".", "-B", "build-vs2022", "-G", "Visual Studio 17 2022")
}
elseif ($vsVersion -eq '2026')
{
    $args = @("-S", ".", "-B", "build-vs2026", "-G", "Visual Studio 18 2026")
}
else
{
    $args = @(".")
}
write-host "$cmake $args"
& $cmake $args
