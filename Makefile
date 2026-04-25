SRCS = \
	main.cpp \
	\
	src/core/client.cpp \
	src/core/server.cpp \
	\
	src/http/request/Request.cpp \
	src/http/response/Response.cpp \
	src/http/buffer/CircularBuffer.cpp \
	\
	src/cgi/CGI.cpp \
	src/cgi/CgiContext.cpp \
	src/cgi/cgi_exec.cpp \
	src/cgi/cgi_env.cpp \
	src/cgi/cgi_parse.cpp \
	\
	src/config/parser/parser.cpp \
	src/config/parser/parse_server.cpp \
	src/config/parser/parse_location.cpp \
	src/config/parser/parse_cgi.cpp \
	src/config/parser/parse_mime.cpp \
	src/config/parser/parse_error.cpp \
	src/config/parser/Location_setters.cpp \
	src/config/parser/parsing_utils.cpp \
	\
	src/utils/filesystem/file_utils.cpp \
	src/utils/filesystem/path_utils.cpp \
	src/utils/http/mime.cpp \
	src/utils/routing/autoindex.cpp \
	src/utils/routing/file_handler.cpp \
	src/utils/routing/Router.cpp \
	src/utils/signals/signals.cpp \
	src/utils/validation/validation_utils.cpp
	
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