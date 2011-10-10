
OBJECTS = main.c canvas.c pallette.c ../source/disk.c ../source/file_internals.c


edit.exe : $(OBJECTS)
	rc resources/resources.rc
	$(CXX) $(CXXFLAGS) /Zi -Femain.exe  /TP $(OBJECTS) /link /fixed:no user32.lib gdi32.lib winmm.lib comdlg32.lib resources/resources.res
