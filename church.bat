SET pathfile=C:\Users\Yohann\Documents\GitHub\ContrastEnhancement

SET execfile=%pathfile%\Build\bin\Release\main.exe

SET name=church
SET img=%pathfile%\Images\%name%\%name%.png

SET method=0

SET result=%pathfile%\Images\%name%\result_method=%method%.png

SET cmd=%execfile% %img% %method% %result%

start %cmd%