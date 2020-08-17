New-Item -Name 'build' -ItemType 'Directory' -Force
Set-Location -Path '.\build'

Start-Process -FilePath 'cmake' -ArgumentList '..', '-D CMAKE_C_COMPILER=gcc', '-G "MinGW Makefiles"' -NoNewWindow -Wait
Start-Process -FilePath 'cmake' -ArgumentList '--build', '.' -NoNewWindow -Wait