function Import-VsDevEnvironment {
    param (
        [string]$VsDevCmd = "H:\VisualStudio\2026\Common7\Tools\VsDevCmd.bat"
    )

    $envDump = cmd /c "`"$VsDevCmd`" -arch=x64 && set"

    foreach ($line in $envDump) {
        if ($line -match "^(.*?)=(.*)$") {
            Set-Item -Path "Env:$($matches[1])" -Value $matches[2]
        }
    }
}

Import-VsDevEnvironment

# MSVC - GLFW backend
# cl /O2 /EHs /GA /Zi /I "./dep/raylib/build/raylib/include" "src/main.c" /Fe:"build/game.exe" /link /LIBPATH:"./dep/raylib/build/raylib/RelWithDebInfo" raylib.lib opengl32.lib gdi32.lib winmm.lib user32.lib shell32.lib kernel32.lib

# MSVC - RGFW backend
cl /O2 /EHs /GA /Zi /MD /I "./dep/raylib/build_rgfw/raylib/include" "src/main.c" /Fe:"build/game.exe" /link /LIBPATH:"./dep/raylib/build_rgfw/raylib/RelWithDebInfo" raylib.lib opengl32.lib gdi32.lib winmm.lib user32.lib shell32.lib kernel32.lib

# MSVC - precompiled
# cl /O2 /EHs /GA /Zi /MD /I "./dep/raylib_dist/include" "src/main.c" /Fe:"build/game.exe" /link /LIBPATH:"./dep/raylib_dist/lib" raylib.lib opengl32.lib gdi32.lib winmm.lib user32.lib shell32.lib kernel32.lib ucrt.lib 
