#Motifs in Biological Networks


BINARY=motifnet
SRC_DIR=src
CXX = g++
CPPFLAGS = -std=c++11 -MD -MP -Wall -pedantic
RM = rm -f


SRCS=$(wildcard $(SRC_DIR)/*.cpp)
OBJS=$(SRCS:%.cpp=%.o)
DPNS=$(SRCS:%.cpp=%.d)



all: main

main: $(OBJS)
	$(CXX) $(CPPFLAGS) -o $(BINARY) $^

clean:
	$(RM) $(OBJS) $(DPNS)


-include $(SRCS:%.cpp=%.d)