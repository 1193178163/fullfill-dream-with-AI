TAG=./bin/Myphoto   # 编译规则中的最终生成的可执行文件
SRC=$(wildcard ./src/*.c)# 需要编译的源文件 wildcard 该函数可以在指定路径 ./src/ 搜索 所有的 .c 文件作为变量的值
OBJ=$(SRC:%.c=%.o)
CC=arm-linux-gcc  # 使用的编译器工具  gcc   arm-linux-gcc 
override CCONFIG+=-L./lib -ljpeg  -I./inc  -pthread # 库文件的路径 以及 库的名字  + 头文件的路径

HOST=root@192.168.41.158# scp 拷贝到的目标地址以及登录名
FILE_PATH=/Johnathan/Myphoto/code # 目标板的路径


#=======================================================

$(TAG):$(OBJ)
	$(CC) $(^) -o $(@) $(CCONFIG) 
	scp $(@) $(HOST):$(FILE_PATH)

%.o:%.c
	$(CC) $(^) -o $(@)  $(CCONFIG)  -c

scp:$(TAG)
	$(@) $(TAG) $(HOST):$(FILE_PATH)


clean:
	$(RM) $(TAG) $(OBJ)


.PHONY:clean