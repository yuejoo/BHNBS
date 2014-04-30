
# .cpp files
CPFILES = ./src/main.cpp

# .h files
CHFILES =

# gl flag
GLFLAG =

main:	$(CPFILES)
	nvcc -g $(CPFILES) -lGL -lGLU -lglut -lGLEW -o main
	
 
