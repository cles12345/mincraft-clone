CXX = g++
CXXFLAGS = -Wall -Werror -Og -g $(INCDIR)
SRC = main.cpp shader.cpp utill.cpp texture.cpp camera.cpp chunk.cpp vao.cpp vbo.cpp ebo.cpp
OBJ = $(SRC:.cpp=.o)
SRCDIR = src
BUILDDIR = build
LIBDIR = libs
INCDIR = -Iinc/GLFW -Iinc/glad -Iinc/stb -Iinc/glm -Iinc/FastNoiseLite/
LDLIBS = -L$(LIBDIR)/glfw/ -lglfw3 -lopengl32 -lgdi32 -static

all: $(BUILDDIR)/game

$(BUILDDIR)/game: $(addprefix $(BUILDDIR)/, $(OBJ)) $(LIBDIR)/glad/glad.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	@mkdir -p save/
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(addprefix $(BUILDDIR)/, $(OBJ)) $(BUILDDIR)/game
	$(BUILDDIR)/game

clean:
	rm -rf $(BUILDDIR)
	rm -rf save/