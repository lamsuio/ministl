# Powershell building script for windows

$build_path = 'build'
$log_file = 'Build.log'

function CheckEnvironment()
{
    if(!(Test-Path $build_path))
    {
        echo -- Build target not existed. Created. - OK
        New-Item -name $build_path -ItemType Directory
    }
    
    cd $build_path
    
    Try {
        cmake ..
    }
    Catch {
        echo -- CMAKE cound not be found. - ERROR
        exit 1
    }
    
    Write-Verbose "-- Compile and logging file to $log_file"
    
    MSBuild .\MiniSTL.sln /flp1:logfile="$log_file"
}

CheckEnvironment