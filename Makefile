CXX := clang++
CXXFLAGS := -std=c++11 -stdlib=libc++
LDFLAGS := -framework SDL2
headers := $(wildcard *.hpp)
ofiles := main.o simulation.o particle.o pconfig.o

all: simulation

simulation: $(ofiles) $(headers)
	$(CXX) $(CXXFLAGS) $(ofiles) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -f simulation *.o
