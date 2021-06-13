# TODO
# * Pass in config directory and optionally seed as args
$rootmapDir = "C:\dev\code\ROOTMAP\build\Win32_d"
$rootmapCli = $rootmapDir + "\RootmapCLI.exe"

# Run the simulation with the given seed
& $rootmapCli --seed=42 --config-dir=$rootmapDir\..\..\Configurations\default

.\moveOutput.ps1
.\diffOutput.ps1