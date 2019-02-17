for /l %%i in (32,1,127) do (
rem    msdfgen.exe -font arialn.ttf %%i -size 64 64 -scale 2 -translate 2 8 -pxrange 2 -o %%i.png -testrender test%%i.png 64 64
    msdfgen.exe -font arialn.ttf %%i -size 64 64 -pxrange 2 -autoframe -o %%i.png -testrender test%%i.png 64 64
)

