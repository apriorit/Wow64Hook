::BUILD x32 Debug
cmake -Hsrc -B build -G"Visual Studio 14 2015" || goto :eof
cmake --build build --target install --config RelWithDebInfo || echo build failed && goto :eof