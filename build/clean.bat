pushd ..
call build\clean_assets_helper.bat
C:\Windows\System32\wsl.exe make clean -f build/Makefile
popd