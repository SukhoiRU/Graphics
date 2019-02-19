rem Английские буквы
for /l %%i in (0x0020, 1, 0x007F) do (
msdfgen.exe -font arialn.ttf %%i -size 64 64 -pxrange 12 -autoframe -o %%i.png
)

rem Русские буквы
for /l %%i in (0x0410, 1, 0x0451) do (
msdfgen.exe -font arialn.ttf %%i -size 64 64 -pxrange 12 -autoframe -o %%i.png
)

rem Дополнительные буквы
msdfgen.exe -font arialn.ttf 169 -size 64 64 -pxrange 12 -autoframe -o 169.png
msdfgen.exe -font arialn.ttf 171 -size 64 64 -pxrange 12 -autoframe -o 171.png
msdfgen.exe -font arialn.ttf 174 -size 64 64 -pxrange 12 -autoframe -o 174.png
msdfgen.exe -font arialn.ttf 187 -size 64 64 -pxrange 12 -autoframe -o 187.png
msdfgen.exe -font arialn.ttf 1025 -size 64 64 -pxrange 12 -autoframe -o 1025.png
msdfgen.exe -font arialn.ttf 8211 -size 64 64 -pxrange 12 -autoframe -o 8211.png
msdfgen.exe -font arialn.ttf 8212 -size 64 64 -pxrange 12 -autoframe -o 8212.png
msdfgen.exe -font arialn.ttf 8220 -size 64 64 -pxrange 12 -autoframe -o 8220.png
msdfgen.exe -font arialn.ttf 8221 -size 64 64 -pxrange 12 -autoframe -o 8221.png