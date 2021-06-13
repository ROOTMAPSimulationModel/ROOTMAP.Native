dir *.xsd | Foreach-Object {
    $namespace = "Rootmap.Configuration.Schema." + $_.BaseName
    ..\..\code\ConfigurationApp\xsd.exe $_.FullName /c /eld /o:../../code/ConfigurationApp/model /order /namespace:$namespace
}