::BUILD x32 Debug
cmake -Hsrc -B build -G "Visual Studio 16 2019" -A Win32 || goto :eof
cmake --build build --target install --config RelWithDebInfo || echo build failed && goto :eof