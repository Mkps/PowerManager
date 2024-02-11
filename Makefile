NAME = PowerManager

SRCDIR = ./src/
INCDIR = ./inc/
OBJ_DIR = ./build/

SRC_NAME =	main.c acpi_access.c battery_management.c pwr_management.c

SRC = $(addprefix $(SRCDIR)/, $(SRC_NAME))
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC_NAME:c=o))

CC = cc

CFLAGS = -pipe -Wall -Wextra -Werror -pedantic `pkg-config --cflags gtk4` -I$(INCDIR)

LDFLAGS = `pkg-config --libs gtk4`

all: $(NAME)

$(NAME): $(OBJ)
	@echo "Compiling objects..."
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS) 

$(OBJ_DIR)/%.o:	$(SRCDIR)/%.c
	@mkdir -p '$(@D)'
	@echo $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	@echo "Cleaning object files..."
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Cleaning binaries..."
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
