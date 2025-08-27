NAME = ircserv

CC = c++
CPPFLAGS = -Werror -Wall -Wextra -std=c++98 -g3 -MMD -MP 

SRCS_DIR = src
OBJS_DIR = objs
INCS_DIR = include

SRCS = $(SRCS_DIR)/main.cpp \
		$(SRCS_DIR)/parse.cpp \
		$(SRCS_DIR)/Server.class.cpp \
		$(SRCS_DIR)/Server.class.commands.cpp \
		$(SRCS_DIR)/Client.class.cpp \
		$(SRCS_DIR)/Channel.class.cpp \
		$(SRCS_DIR)/IrcFormatter.class.cpp \
		$(SRCS_DIR)/Bot.class.cpp \

OBJS = $(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $@

$(OBJS_DIR)/%.o:	%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -I$(INCS_DIR) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: all
	./$(NAME) 6667 password

leaks:	all
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./$(NAME) 6667 password

-include $(DEPS)

.PHONY: all clean fclean re run leaks
