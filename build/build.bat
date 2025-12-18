pushd ..
call build\build_assets_helper.bat
C:\Windows\System32\wsl.exe make -f build/Makefile 
popd