param (
  [string]$quality = 9 # POV-Ray's default quality setting is 9. https://www.povray.org/documentation/view/3.6.1/223/
)

# Step 1: Prepare the scene setup data file, using the current working directory as the input dir.
# Note that $inputDir (and only $inputDir) needs double backslashes as path separators.
$inputDir = ((Get-Location).Path + "\").Replace("\", "\\")
# Optional: directly specify the input directory.
$inputDir = "C:\\Users\\MikeAirey\\AppData\\Roaming\\ROOTMAP\\v0.2.2\\Output\\Raytracer\\"
(Get-Content .\Rootmap_Scene_Setup_Data) -replace "HOME_DIRECTORY_PLACEHOLDER", $inputDir | Out-File -encoding ASCII Rootmap_Scene_Setup_Data.inc

# Step 2: Define configuration settings.
$povRay = Get-ChildItem HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall | ForEach-Object { Get-ItemProperty $_.PsPath } | Select-Object DisplayName, InstallLocation | Where-Object { $_.DisplayName -Match "POV-Ray" }
if (!$povRay) {
  throw "Could not find POV-Ray installation directory. Please insure POV-Ray 3.6 is installed."
}
$povrayDir = $povRay.InstallLocation + "bin"
$frameCount = 720
$height = 3600
$width = 2700
$timestamp = Get-Date -Format o | ForEach-Object { $_ -replace ":", "." }
$outputDir = ($inputDir + "\$timestamp\")
# Allowable values for $showPreview: +D = Show graphical preview while rendering. -D = Do not show preview while rendering.
#$showPreview = '-D'
$showPreview = '+D'

mkdir $outputDir | Out-Null

# Step 3: Run the render.
Write-Host "---"
Write-Host 'Now raytracing the scene referenced in "Rootmap_Scene_Setup_Data.inc".'
Write-Host "Width: $width pixels. Height: $height pixels. Number of frames: $frameCount."
Write-Host "Saving results in $outputDir."
# Running a separate instance of POV-Ray for each frame results in *dramatically* improved performance.
# Unsure why this is. Perhaps ROOTMAP's organisation of include files results in pathological behaviour of POV-Ray's parser.
For ($i = 0; $i -lt $frameCount; $i++) {
  & ($povrayDir + "\pvengine64.exe") Rootmap_Scene.pov +SF$i +EF$i +KFF$frameCount +FN +H$height +W$width +O$outputDir +Q$quality $showPreview /EXIT | Out-Null
}
