run: project.cpp 
	g++ project.cpp -lglut -lGLU -lGL -lGLEW -g 
	
clean: 
	rm -f *.out *~ run
	
