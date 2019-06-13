#Motifs in Biological Networks

PROG = MotifFinder
SRC_DIR = src
CXX = g++
CXXFLAGS += -std=c++11 -Wall -pedantic
RM = rm -f


SRCS=$(wildcard $(SRC_DIR)/*.cpp)
OBJS=$(subst .cpp,.o, $(SRCS))



all: $(OBJS)
	$(CXX) -o ./$(PROG) $(OBJS) $(LDFLAGS)
	@echo Build successful


depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend