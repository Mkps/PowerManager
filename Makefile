NAME = PowerManager

SRCDIR = ./src
INCDIR = ./inc
OBJ_DIR = ./build

SRC_NAME =	main.c powermanager-dbus.c acpi_access.c battery_management.c pwr_management.c
DBUS_SRC_NAME = dbus-server.c powermanager-dbus.c acpi_access.c battery_management.c pwr_management.c

SRC = $(addprefix $(SRCDIR), $(SRC_NAME))
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC_NAME:c=o))

DBUS_SRC = $(addprefix $(SRCDIR), $(DBUS_SRC_NAME))
DBUS_OBJ = $(addprefix $(OBJ_DIR)/, $(DBUS_SRC_NAME:c=o))

CC = cc

debug: CFLAGS = -g3 -pipe -Wall -Wextra -Werror `pkg-config --cflags gtk4 polkit-gobject-1` -I$(INCDIR)
dbus: CFLAGS = -O2 -Wall -Wextra -Werror  `pkg-config --cflags gtk4 polkit-gobject-1` -I$(INCDIR)
$(NAME): CFLAGS = -O2 `pkg-config --cflags gtk4 polkit-gobject-1` -I$(INCDIR)

LDFLAGS = `pkg-config --libs gtk4 polkit-gobject-1`

all: $(NAME)

dbus: $(DBUS_OBJ)
	@echo "Compiling dbus service..."
	@$(CC) -o PowerManagerDbus $(DBUS_OBJ) $(LDFLAGS)

$(NAME): release dbus
	@echo "Done!"

release: $(OBJ)
	@echo "Compiling $(NAME) in release mode..." 
	@$(CC) -o $(NAME) $(OBJ) $(LDFLAGS) 

$(OBJ_DIR)/%.o:	$(SRCDIR)/%.c
	@mkdir -p '$(@D)'
	@$(CC) -c $(CFLAGS) $< -o $@

debug: fclean $(OBJ)
	@echo "Compiling $(NAME) in debug mode..."
	$(CC) -DDEBUG=1 -o $(NAME) $(OBJ) $(LDFLAGS) 

clean:
	@echo "Cleaning object files..."
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Cleaning binaries..."
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
