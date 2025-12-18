java -jar tools\thirdparty\ugt\UGT-0.17.jar -imagefile assets\source\testmap003.png -name testmap003 -destination assets\final
assets2banks assets\final --compile
move /y bank*.* assets\banks 
tools\bin\spritemaster\spritemaster.exe gamedata\animations source\exported\animations -updateonly -streamed
tools\bin\spritemaster\spritemaster.exe gamedata\palettes source\exported\palettes -palette -updateonly

