minisql : flex.l parser.y global.h tree.h read_write.h element.h BufferManger.h Record.h IndexManager.h
	flex flex.l
	yacc -d parser.y
	g++ -o minisql y.tab.c global.cpp tree.cpp read_write.cpp BufferManger.cpp IndexManager.cpp

