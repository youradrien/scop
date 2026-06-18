NAME = scop

CXX = c++

CXXFLAGS = -Wall -Wextra -std=c++11 -O2 \
-I/opt/homebrew/include -Iincludes

LIBS = -L/opt/homebrew/lib -lglfw -lvulkan
LIBS += -lSDL2

# Vulkan + GLFW + X11 (Linux)
INCLUDES = -Iinclude -Ilib/SDL2/include -Ilib/vulkan/include
SRC_DIR = src
OBJ_DIR = obj

SRCS = \
$(SRC_DIR)/main.cpp \
$(SRC_DIR)/scop.cpp \
$(SRC_DIR)/window.cpp \
$(SRC_DIR)/vulkan_context.cpp \
$(SRC_DIR)/mesh.cpp \
$(SRC_DIR)/obj_loader.cpp \
$(SRC_DIR)/camera.cpp

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# ---------- RULES ----------

all: check $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

UNAME_S := $(shell uname -s)
check:
	@command -v pkg-config >/dev/null 2>&1 || ( \
		echo "\033[1;33mpkg-config is required\033[0m" && exit 1 )

	@pkg-config --exists vulkan || ( \
		echo "\033[1;33m[VULKAN] not installed\033[0m"; \
		if [ "$(UNAME_S)" = "Darwin" ]; then \
			echo "\033[1;32m👉 try: brew install vulkan-volk (macOS)\033[0m"; \
		else \
			echo "\033[1;32m👉 try: sudo apt install -y vulkan-tools libvulkan-dev \033[0m"; \
		fi; \
		exit 1 )

	@pkg-config --exists sdl2 || ( \
		echo "\033[1;33m[SDL2] not installed\033[0m"; \
		if [ "$(UNAME_S)" = "Darwin" ]; then \
			echo "\033[1;32minstall with: brew install sdl2 \033[0m"; \
		else \
			echo "\033[1;32minstall with: sudo apt install -y libsdl2-dev \033[0m"; \
		fi; \
		exit 1 )

	@echo "\033[1;32mall dependencies found ✔\033[0m"

.PHONY: all clean fclean re
