SRCS =  src/http/Request.cpp \
		src/http/Response.cpp \
		src/http/CircularBuffer.cpp \
		src/http/router/Router.cpp \
		src/http/router/autoindex.cpp \
		src/http/router/file_handler.cpp \
		src/http/utils/utils.cpp \
		src/http/utils/mime.cpp \
		main.cpp \
		src/server/server.cpp \
		src/client/client.cpp \
		src/config/parser/parser.cpp \
		src/config/parser/parse_server.cpp \
		src/config/parser/parse_location.cpp \
		src/config/parser/parse_mime.cpp \
		src/config/parser/parse_error.cpp \
		src/config/parsing/Location_setters.cpp \
		src/http/cgi/CGI.cpp \
		src/http/cgi/cgi_exec.cpp \
		src/http/cgi/cgi_env.cpp \
		src/http/cgi/cgi_parse.cpp \
		src/config/parsing/parsing_utils.cpp \

NAME = Webserv

COMPRESS = ar rcs

RM = rm -f

CC = c++

CFLAGS = -Wall -Wextra -Werror -g -std=c++98 -I./includes

OBJ_DIR = obj

# Lista de arquivos .o correspondentes aos .cpp
OBJS = $(SRCS:src/%.cpp=$(OBJ_DIR)/%.o)

RESET   = \033[0m
GREEN   = \033[32m
YELLOW  = \033[33m
BLUE    = \033[34m
RED     = \033[31m

all: $(NAME)

# Regra para criar o executável a partir dos arquivos objeto e bibliotecas
$(NAME): $(OBJS)
	@echo "$(YELLOW)compiling $(NAME) ...$(RESET)"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
	@echo "$(GREEN)$(NAME) ready! $(RESET)"

# Regra para compilar arquivos objeto a partir de arquivos fonte, cria a pasta para o arq obj se nao existir
$(OBJ_DIR)/%.o: src/%.cpp | $(CREATE_DIR)
	@mkdir -p $(@D)
	@echo "$(BLUE)making objs directorys!$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(CREATE_DIR):
	@mkdir -p $(OBJ_DIR)/src

v: re
	valgrind --leak-check=full --track-fds=yes --show-leak-kinds=all --track-origins=yes --show-reachable=yes ./$(NAME)

clean:
	@echo "$(RED)cleaning objs ...$(RESET)"
	@$(RM) -r $(OBJ_DIR)    # Remove o diretório obj e seu conteúdo
	@echo "$(RED)objs && libs cleaned! $(RESET)"

fclean: clean
	@$(RM) $(NAME)  # Remove o executável
	@echo "$(RED)$(NAME) removed! $(RESET)"

re: fclean all

.PHONY: all clean fclean re