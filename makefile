CXX = g++
CXXFLAGS = -g $(INCDIR)
SRC = main.cpp shader.cpp utill.cpp mesh.cpp texture.cpp camera.cpp block.cpp chunk.cpp
OBJ = $(SRC:.cpp=.o)
SRCDIR = src
BUILDDIR = build
LIBDIR = libs
INCDIR = -Iinc/GLFW -Iinc/glad -Iinc/stb -Iinc/glm 
LDLIBS = -L$(LIBDIR)/glfw/ -L$(LIBDIR)/glad/ -lglfw3 -lopengl32 -lgdi32

all: $(BUILDDIR)/game

$(BUILDDIR)/game: $(addprefix $(BUILDDIR)/, $(OBJ)) $(LIBDIR)/glad/glad.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(addprefix $(BUILDDIR)/, $(OBJ)) $(BUILDDIR)/game
	$(BUILDDIR)/game

clean:
	rm -rf $(BUILDDIR)
