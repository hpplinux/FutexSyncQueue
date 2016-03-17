CPPFLAGS+=-pthread
SOURCES_FUTEX=futex.cpp futex.hpp
SOURCES_TEST_FUTEX=$(SOURCES_FUTEX) testFutex.cpp
SOURCES_QUEUE=queue.hpp $(SOURCES_FUTEX)
SOURCES_TEST_QUEUE=$(SOURCES_QUEUE) testQueue.cpp

futex: $(SOURCES_TEST_FUTEX)
	$(CXX) $(CPPFLAGS) futex.cpp testFutex.cpp -o testFutex

testFutex: $(SOURCES_TEST_FUTEX)
	make futex

queue: $(SOURCES_QUEUE)
	$(CXX) $(CPPFLAGS) -lgtest futex.cpp testQueue.cpp -o testQueue

testQueue: $(SOURCES_TEST_QUEUE)
	make queue
