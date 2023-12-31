CC = g++
COMPILERFLAGS = -g

.PHONY: all clean obj debug

#CPP_FILES := $(notdir $(shell find -name "*.cpp"))
CPP_FILES := tcpepollnonblock.cpp inetaddress.cpp socket.cpp epoll.cpp
OBJ_FILES := $(patsubst %.cpp,obj/%.o,$(CPP_FILES))
#EXE_FILES := $(patsubst %,bin/%,$(basename $(CPP_FILES)))
EXE_FILES := bin/tcpepollnonblock

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

bin/tcpepollnonblock : $(OBJ_FILES)
	@mkdir -p $(dir $@)
	$(CC) $(COMPILERFLAGS) $^ -o $@

debug :
	@echo $(CPP_FILES)
	@echo $(OBJ_FILES)
	@echo $(EXE_FILES)