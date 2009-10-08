all : index.exe list.exe

index.exe : index.mkf libasptstring.a libparser.a libmemindex.a
	make -f index.mkf

index.mkf : index.occ libasptstring.a
	omakef index -21


list.exe : list.mkf libasptstring.a
	make -f list.mkf

list.mkf : list.occ libasptstring.a
	omakef list -21


libasptstring.a : asptstring.a
	copy asptstring.a libasptstring.a

asptstring.a : asptstring.mkf
	make -f asptstring.mkf

asptstring.mkf : asptstring.occ
	omakef asptstring.lib -21


libparser.a : parser.a
	copy parser.a libparser.a

parser.a : parser.mkf
	make -f parser.mkf

parser.mkf : parser.occ
	omakef parser.lib -21


libmemindex.a : memindex.a
	copy memindex.a libmemindex.a

memindex.a : memindex.mkf
	make -f memindex.mkf

memindex.mkf : memindex.occ
	omakef memindex.lib -21


clean :
	del *.obj *.exe *.c *.t *.hdr *.ilk *.mkf *.lib *.pdb *.liu *.a *.bak

