UNAME := $(shell uname)
CPPC     = g++
OBJS     = AudioAnalyzer.o KeyAnalyzer.o ShaderGL.o TextEditor.o Core.o main.o
TARGET   = LiveCoder
CFLAGS   =  -O2
LIBS     = -framework OpenAL -framework OpenGL `sdl-config --libs`
INCLUDES = `sdl-config --cflags`
ifeq ($(UNAME), Linux)
LIBS     = `pkg-config --libs sdl gl glew openal`
INCLUDES = `pkg-config --cflags sdl gl glew openal`
endif

$(TARGET): $(OBJS)
	$(CPPC) -o $@ $(OBJS) $(LIBS)

.cpp.o:
	$(CPPC) $(CFLAGS) $(INCLUDES) -c $< 

clean:
	rm -f $(TARGET) $(OBJS)