CXX = g++
CXXFLAGS = -g $(INCDIR)
SRC = main.cpp 
OBJ = $(SRC:.cpp=.o)
SRCDIR = src
BUILDDIR = build
LIBDIR = libs
INCDIR = -Iinc/GLFW -Iinc/glad 
LDLIBS = -L$(LIBDIR)/glfw/ -L$(LIBDIR)/glad/ -lglfw3 -lopengl32 -lgdi32

all: $(BUILDDIR)/game

$(BUILDDIR)/game: $(addprefix $(BUILDDIR)/, $(OBJ)) $(LIBDIR)/glad/glad.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)
