$homeDir = "$env:APPDATA\ROOTMAP\working"
$firstDir = (dir $homeDir -Directory | Select-Object -First 1)
dir $firstDir.FullName -Filter *.txt | 
Foreach-Object {
    $path1 = $_.FullName
    $name1 = $_.Name
    $content1 = Get-Content $_.FullName
    dir $homeDir -Directory |
    Foreach-Object {
        dir $_.FullName -Filter "*.txt" |
        Where-Object { ($_.FullName -ne $path1) -and ($_.Name -eq $name1) } |
        Foreach-Object {
            $path2 = $_.FullName
            echo ("Comparing $path1 to $path2")
            $name2 = $_.Name
            $content2 = Get-Content $_.FullName
            $diff = Compare-Object $content1 $content2
            if ($diff -ne $null) {
                echo "Encountered a mismatch: $path1 differs from $path2"
                exit 1
            }
        }
    }
}
echo "All files matched."
