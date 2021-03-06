# 定义相关变量
CC=gcc     		
STD=-std=gnu99
LIB= -lm -lpthread
FLAGS=-Wall # -Werror -Wall
OBJ=./Src/cJSON.o ./Src/server.o ./Src/group.o ./Src/init.o ./Src/main.o ./Src/message.o ./Src/private.o \
	./Src/tools.o
BIN=qqbot

# 入口编译目标 合并
all:$(OBJ)
	$(CC) $(OBJ) $(LIB) -o $(BIN)

# 编译单个源文件
%.o:%.c
	$(CC) $(STD) $(FLAGS) -c $< -o $@

# 清理编译临时文件
clean:
	rm -rf $(BIN) 
	rm -rf $(OBJ) 
	rm -rf *.h.gch
