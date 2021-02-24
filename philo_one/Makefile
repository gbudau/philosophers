NAME = philo_one
CC = gcc
CFLAGS = -Wall -Werror -Wextra -O3
LFLAGS = -lpthread
RMF = rm -rf

INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj

_OBJ = main.o atou_error.o print_output.o ft_utoa.o check_args.o \
	   utils.o monitor_utils.o dine_philo_utils.o ft_usleep.o
OBJ := $(patsubst %, $(OBJ_DIR)/%, $(_OBJ))

_DEPS = philo_one.h
DEPS := $(patsubst %, $(INC_DIR)/%, $(_DEPS))

.PHONY: all
all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ -I/$(INC_DIR) $(LFLAGS)

.PHONY: fsanitizethread
fsanitizethread: CFLAGS += -g3 -fsanitize=thread
fsanitizethread: fclean all

.PHONY: memoryleaks
memoryleaks: CFLAGS += -g3 -fsanitize=leak
memoryleaks: fclean all

.PHONY: fsanitizeaddress
fsanitizeaddress: CFLAGS += -g3 -fsanitize=address
fsanitizeaddress: re

.PHONY: clean
clean:
	$(RMF) $(OBJ_DIR)

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: re
re: fclean all
