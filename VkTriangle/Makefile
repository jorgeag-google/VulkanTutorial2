#CFLAGS := -std=c++17 -O2
CFLAGS := -Wall -std=c++17
LDFLAGS := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
CXX := g++

# Define the object files
OBJS := HelloTriangleApplication.o HelloTriangleDebug.o
# Define the name of the executable
TARGET := HelloTriangle

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(TARGET) main.cpp $(LDFLAGS)

# Create the object files
HelloTriangleApplication.o: HelloTriangleApplication.cpp HelloTriangleApplication.h
	$(CXX) $(CFLAGS) -c HelloTriangleApplication.cpp -o $@ $(LDFLAGS)
	
HelloTriangleDebug.o: HelloTriangleDebug.cpp HelloTriangleApplication.h
	$(CXX) $(CFLAGS) -c HelloTriangleDebug.cpp -o $@ $(LDFLAGS)
# Generic one object per class that has one header and ine implementation file
#%.o: %.cpp %.h
#	$(CXX) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

