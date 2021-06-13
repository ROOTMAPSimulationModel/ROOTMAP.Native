$homeDir = "$env:APPDATA\ROOTMAP"
$timestampDir = "$homeDir/working/$(get-date -f yyyy-MM-dd_HH_mm_ss)"
mkdir $timestampDir
mv "$homeDir\*.txt" $timestampDir
