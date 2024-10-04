param 
(
    [Parameter(Mandatory = $True)] [ValidateSet('Win64vs2022', 'Uwp64vs2022')][string] $Target,
    [switch]$NoUnityBuild,
    [switch]$NoVulkan,
    [string]$SolutionName
)

Set-Location $PSScriptRoot

$CMAKE_ARGS = @("-S", "$PSScriptRoot")

if ($NoUnityBuild) {
    $CMAKE_ARGS += "-DPL_ENABLE_FOLDER_UNITY_FILES:BOOL=OFF"
}
else {
    $CMAKE_ARGS += "-DPL_ENABLE_FOLDER_UNITY_FILES:BOOL=ON"
}

if ($NoVulkan) {
    $CMAKE_ARGS += "-DPL_BUILD_EXPERIMENTAL_VULKAN:BOOL=OFF"
}
else {
    $CMAKE_ARGS += "-DPL_BUILD_EXPERIMENTAL_VULKAN:BOOL=ON"
}

if ($SolutionName -ne "") {
    $CMAKE_ARGS += "-DPL_SOLUTION_NAME:STRING='${SolutionName}'"
}

$CMAKE_ARGS += "-G"

Write-Host ""

if ($Target -eq "Win64vs2022") {

    Write-Host "=== Generating Solution for Visual Studio 2022 x64 ==="

    $CMAKE_ARGS += "Visual Studio 17 2022"
    $CMAKE_ARGS += "-A"
    $CMAKE_ARGS += "x64"
    $CMAKE_ARGS += "-B"
    $CMAKE_ARGS += "$PSScriptRoot\Workspace\vs2022x64"
}
elseif ($Target -eq "Uwp64vs2022") {

    Write-Host "=== Generating Solution for Visual Studio 2022 x64 UWP ==="

    $CMAKE_ARGS += "Visual Studio 17 2022"
    $CMAKE_ARGS += "-A"
    $CMAKE_ARGS += "x64"
    $CMAKE_ARGS += "-B"
    $CMAKE_ARGS += "$PSScriptRoot\Workspace\vs2022x64uwp"
    $CMAKE_ARGS += "-DCMAKE_TOOLCHAIN_FILE=$PSScriptRoot\Code\BuildSystem\CMake\toolchain-winstore.cmake"

    $CMAKE_ARGS += "-DPL_BUILD_FILTER='UwpProjects'"
}
else {
    throw "Unknown target '$Target'."
}

Write-Host ""
Write-Host "Running cmake.exe $CMAKE_ARGS"
Write-Host ""
&Data\Tools\Precompiled\cmake\bin\cmake.exe $CMAKE_ARGS

if (!$?) {
    throw "CMake failed with exit code '$LASTEXITCODE'."
}
