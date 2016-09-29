comp		=gcc
compflags	=-std=c99 -Wall
libs		=-lpthread
path		=bin
obj			=	\
			$(path)/main.o	\
			$(path)/network.o	\
			$(path)/packets.o	\

default : check_bin $(path)/TFTPServer

check_bin :
	@ if [ ! -d $(path) ]; then mkdir $(path); fi

$(path)/TFTPServer : $(obj)
	$(comp) $(compflags) -o $(path)/TFTPServer $(obj) $(libs)

$(path)/%.o : %.c
	$(comp) $(compflags) -c $< -o $@

clean :
	rm -rf $(path)
