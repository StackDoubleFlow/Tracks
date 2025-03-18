Param(
    [Parameter(Mandatory=$false)]
    [Switch]$clean
)

# if user specified clean, remove all build files
if ($clean.IsPresent)
{
    if (Test-Path -Path "build")
    {
        remove-item build -R
    }
}

$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

if (($clean.IsPresent) -or (-not (Test-Path -Path "build")))
{
    $out = new-item -Path build -ItemType Directory
}

# build the rust code
cd ./tracks_rs_link
# cargo ndk --bindgen -t arm64-v8a -o build build --release --features ffi 

cd ../build
& cmake -G "Ninja" -DCMAKE_BUILD_TYPE="RelWithDebInfo" ../
& cmake --build .
cd ..