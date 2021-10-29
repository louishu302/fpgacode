#File: Makefile
#get variable "src" return value
src:=src/pcie.cpp\
     src/acpitable.cpp\
	 src/test.cpp

#$(shell ls *.cpp)
#declair objs is through "patsubst" to convert from source files
objs:=$(patsubst %.cpp,%.o,$(src))
#$@ is object file name ; $^ is source file name
test: $(objs)
	g++ -m64 -march=x86-64 -Wall -o $@ $^
	mv test ./lib
	mv ./src/*.o ./lib
#match case for everything $< is first source file name
%.o: %.cpp
	g++ -m64 -march=x86-64 -Wall -c -o $@ $<

clean:
	rm -f ./lib/test ./lib/*.o ./lib/*.csv