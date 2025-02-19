NAME = PowerManager

SRCDIR = ./src
INCDIR = ./inc
OBJ_DIR = ./build

SRC_NAME =	main.c acpi_access.c battery_management.c pwr_management.c
HELPER_SRC_NAME = helper.c acpi_access.c battery_management.c pwr_management.c

SRC = $(addprefix $(SRCDIR), $(SRC_NAME))
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC_NAME:c=o))

HELPER_SRC = $(addprefix $(SRCDIR), $(HELPER_SRC_NAME))
HELPER_OBJ = $(addprefix $(OBJ_DIR)/, $(HELPER_SRC_NAME:c=o))

CC = cc

debug: CFLAGS = -g3 -pipe -Wall -Wextra -Werror -pedantic `pkg-config --cflags gtk4` -I$(INCDIR)
helper: CFLAGS = -g3 -pipe -Wall -Wextra -Werror -pedantic `pkg-config --cflags gtk4` -I$(INCDIR)
$(NAME): CFLAGS = -O2 `pkg-config --cflags gtk4` -I$(INCDIR)

LDFLAGS = `pkg-config --libs gtk4`

all: $(NAME)

helper: $(HELPER_OBJ)
	@echo "Compiling helper binary..."
	@$(CC) -o acpi_helper $(HELPER_OBJ) $(LDFLAGS)

$(NAME): release $(OBJ)
	@$(CC) -o $(NAME) $(OBJ) $(LDFLAGS) 
	@echo "Done!"

release:
	@echo "Compiling $(NAME) in release mode..." 

$(OBJ_DIR)/%.o:	$(SRCDIR)/%.c
	@mkdir -p '$(@D)'
	@$(CC) -c $(CFLAGS) $< -o $@

debug: fclean $(OBJ)
	@echo "Compiling $(NAME) in debug mode..."
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS) 

clean:
	@echo "Cleaning object files..."
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Cleaning binaries..."
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
