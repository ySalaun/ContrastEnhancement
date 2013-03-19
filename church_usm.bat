SET pathfile=C:\Users\Yohann\Documents\GitHub\ContrastEnhancement

SET execfile=%pathfile%\Build\bin\Release\main.exe

SET name=church
SET img=%pathfile%\Images\%name%\%name%.png

SET method=0
SET sd=20
SET s=2

SET result=%pathfile%\Images\%name%\result_method=%method%_sd=%sd%_s=%s%.png

SET cmd=%execfile% %img% %method% %result% %sd% %s%

start %cmd%