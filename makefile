all:
	g++ -std=c++0x -o run API.cc bufferManager.cpp indexManager.cc CatalogManager.cc filter.cc interpreter.cc RecordManager.cc global.cc main.cc
	rm -f *.db
	rm -f *.table
