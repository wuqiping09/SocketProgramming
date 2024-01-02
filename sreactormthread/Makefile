CC = g++
COMPILERFLAGS = -g

.PHONY: all clean obj debug

#CPP_FILES := $(notdir $(shell find -name "*.cpp"))
CPP_FILES := \
	main.cpp \
	inetaddress.cpp \
	socket.cpp \
	epoll.cpp \
	channel.cpp \
	eventloop.cpp \
	tcpserver.cpp \
	acceptor.cpp \
	connection.cpp \
	processor.cpp
OBJ_FILES := $(patsubst %.cpp,obj/%.o,$(CPP_FILES))
#EXE_FILES := $(patsubst %,bin/%,$(basename $(CPP_FILES)))
EXE_FILES := bin/main

all : $(EXE_FILES)

clean :
	$(RM) \
		$(OBJ_FILES) \
		$(EXE_FILES)

obj/%.o : %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(COMPILERFLAGS) -o $@ -c $<

obj : $(OBJ_FILES)

# bin/% : obj/%.o
# 	@mkdir -p $(dir $@)
# 	$(CC) $(COMPILERFLAGS) $< -o $@

bin/main : $(OBJ_FILES)
	@mkdir -p $(dir $@)
	$(CC) $(COMPILERFLAGS) $^ -o $@

debug :
	@echo $(CPP_FILES)
	@echo $(OBJ_FILES)
	@echo $(EXE_FILES)