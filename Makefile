CXX = g++
CXXFLAGS = -Wall -g -std=c++17

SFML_LIBS = -lsfml-system -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network

SRC_DIR   = Sources
BUILD_DIR = Build

CLIENT_TARGET = $(BUILD_DIR)/GameClient.exe
SERVER_TARGET = $(BUILD_DIR)/GameServer.exe

SHARED_SRCS := $(wildcard $(SRC_DIR)/Shared/*.cpp)
CLIENT_SRCS := $(wildcard $(SRC_DIR)/Client/*.cpp)
SERVER_SRCS := $(wildcard $(SRC_DIR)/Server/*.cpp)

SHARED_OBJS := $(SHARED_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
CLIENT_OBJS := $(CLIENT_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
SERVER_OBJS := $(SERVER_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

all: $(CLIENT_TARGET) $(SERVER_TARGET)

# --- LINK CLIENT ---
# Client cần: Code Client + Code Shared
$(CLIENT_TARGET): $(CLIENT_OBJS) $(SHARED_OBJS)
	@echo [Linking Client] $@
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIBS) $(INCLUDES)

# --- LINK SERVER ---
# Server cần: Code Server + Code Shared
$(SERVER_TARGET): $(SERVER_OBJS) $(SHARED_OBJS)
	@echo [Linking Server] $@
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIBS) $(INCLUDES)

# --- COMPILE C++ TO OBJECTS ---
# Rule chung cho tất cả file .cpp
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo [Compiling] $<
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@ $(INCLUDES)

client: $(CLIENT_TARGET)
	$(CLIENT_TARGET)

server: $(SERVER_TARGET)
	$(SERVER_TARGET)

clean:
	rm -rf $(BUILD_DIR)

# Include dependencies file (.d) để make biết khi nào cần recompile header
DEPS := $(SHARED_OBJS:.o=.d) $(CLIENT_OBJS:.o=.d) $(SERVER_OBJS:.o=.d)
-include $(DEPS)