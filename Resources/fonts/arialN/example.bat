for /l %%i in (32,1,127) do msdfgen.exe -font arialn.ttf %%i -size 32 32 -pxrange 4 -autoframe -o %%i.png -testrender test%%i.png 32 32


