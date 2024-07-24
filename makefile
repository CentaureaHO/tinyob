CC = g++
FLAGS = -g -pedantic -Wall -Wextra -std=c++20 -pthread

SRCDIR = ./
BUILDDIR = build

TEST_TARGET = $(BUILDDIR)/test
SERVER_TARGET = $(BUILDDIR)/server
CLIENT_TARGET = $(BUILDDIR)/client

TEST_SRC = $(SRCDIR)/test.cpp \
           $(shell find $(SRCDIR)/utils -name '*.cpp' -or -name '*.tpp')

SERVER_SRC = $(SRCDIR)/db/server/db_server.cpp \
             $(shell find $(SRCDIR)/utils -name '*.cpp' -or -name '*.tpp')

CLIENT_SRC = $(SRCDIR)/db/client/db_client.cpp \
             $(shell find $(SRCDIR)/utils -name '*.cpp' -or -name '*.tpp')

TEST_OBJ = $(TEST_SRC:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
TEST_OBJ := $(TEST_OBJ:$(SRCDIR)/%.tpp=$(BUILDDIR)/%.o)

SERVER_OBJ = $(SERVER_SRC:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
SERVER_OBJ := $(SERVER_OBJ:$(SRCDIR)/%.tpp=$(BUILDDIR)/%.o)

CLIENT_OBJ = $(CLIENT_SRC:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
CLIENT_OBJ := $(CLIENT_OBJ:$(SRCDIR)/%.tpp=$(BUILDDIR)/%.o)

INCLUDES = -I$(SRCDIR)/utils -I$(SRCDIR)/db/server

DIRS = $(sort $(dir $(TEST_OBJ) $(SERVER_OBJ) $(CLIENT_OBJ)))
$(shell mkdir -p $(DIRS))

all: $(TEST_TARGET) $(SERVER_TARGET) $(CLIENT_TARGET)

test: $(TEST_TARGET)

server: $(SERVER_TARGET)

client: $(CLIENT_TARGET)

$(TEST_TARGET): $(TEST_OBJ)
	$(CC) $(FLAGS) $(INCLUDES) -o $@ $^

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(FLAGS) $(INCLUDES) -o $@ $^

$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CC) $(FLAGS) $(INCLUDES) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)  # Ensure directory exists
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

.PHONY: clean all test server client
