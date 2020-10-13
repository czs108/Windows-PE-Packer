New-Variable -Name 'AppName' -Value 'PE-Packer' -Option Constant

New-Item -Name 'bin' -ItemType 'Directory' -Force
New-Item -Name 'build' -ItemType 'Directory' -Force
Set-Location -Path '.\build'

Start-Process -FilePath 'cmake' -ArgumentList '..', '-D CMAKE_C_COMPILER=gcc', '-G "MinGW Makefiles"' -NoNewWindow -Wait
Start-Process -FilePath 'cmake' -ArgumentList '--build', '.' -NoNewWindow -Wait

Move-Item -Path ".\bin\$AppName.exe" -Destination '..\bin' -Force