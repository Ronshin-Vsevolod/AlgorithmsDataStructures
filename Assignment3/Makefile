PROJECT = huffman
TESTPROJECT = test-$(PROJECT)

CXX = g++
CXXFLAGS = -I. -std=c++17 -Werror -Wpedantic -Wall -g -fPIC
LDFLAGS = -lstdc++fs
GTEST_FLAGS = -lgtest -lgtest_main -pthread

SRCDIR = .
BUILDDIR = build

SOURCES = main.cpp huffman.cpp
OBJECTS = $(SOURCES:.cpp=.o)

TEST_SOURCES = test.cpp huffman.cpp
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

HEADERS = $(wildcard *.h)

.PHONY: all clean test

all: $(PROJECT) $(TESTPROJECT)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PROJECT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(TESTPROJECT): $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(GTEST_FLAGS)

test: $(TESTPROJECT)
	./$(TESTPROJECT)

clean:
	rm -f *.o $(PROJECT) $(TESTPROJECT) 