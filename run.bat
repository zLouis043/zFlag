@echo off

cd bin
echo [INFO] Executing the program: 
call zflag.exe -bool -str "check" -char d -uint64_t 2048 -size 2K
cd ..
