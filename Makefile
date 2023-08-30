CC ?= clang

#concord specific
CONCORD_DIR	= ..
CC_CORE		= $(CONCORD_DIR)/core
CC_INCLUDE	= $(CONCORD_DIR)/include
CC_GENCODECS	= $(CONCORD_DIR)/gencodecs
CC_LIB		= $(CONCORD_DIR)/lib
CONCORD_CFLAG	= -I$(CC_CORE) -I$(CC_INCLUDE) -I$(CC_GENCODECS) -L$(CC_LIB)


SRC		= main.c command.c response.c

#normal stuff
CFLAGS  += -O0 -g -pthread -Wall $(CONCORD_CFLAG) \
	-I/usr/local/include `mysql_config --cflags` -DCCORD_SIGINTCATCH

LDFLAGS  = -L/usr/local/lib `mysql_config --libs`
LDLIBS   = -ldiscord -lcurl

make: 

	$(CC) $(SRC) -o nic3 $(CFLAGS) $(LDFLAGS) $(LDLIBS) 
