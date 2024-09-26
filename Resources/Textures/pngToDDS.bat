for /r %%f in (*.png) do (
  .\texconv.exe -f DXT5 -srgb -ft dds "%%~f" -o "%%~dpf
)