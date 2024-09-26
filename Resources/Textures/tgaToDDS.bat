for /r %%f in (*.tga) do (
  .\texconv.exe -y -f DXT5 -srgb -ft dds "%%~f" -o "%%~dpf
)