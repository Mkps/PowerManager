NAME = PowerManager

SRCDIR = ./src
INCDIR = ./inc
OBJ_DIR = ./build

SRC_NAME =	main.c powermanager-dbus.c acpi_access.c battery_management.c pwr_management.c
HELPER_SRC_NAME = helper.c acpi_access.c battery_management.c pwr_management.c
DAEMON_SRC_NAME = daemon.c acpi_access.c battery_management.c pwr_management.c
DBUS_SRC_NAME = dbus-server.c acpi_access.c battery_management.c pwr_management.c

SRC = $(addprefix $(SRCDIR), $(SRC_NAME))
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC_NAME:c=o))

HELPER_SRC = $(addprefix $(SRCDIR), $(HELPER_SRC_NAME))
HELPER_OBJ = $(addprefix $(OBJ_DIR)/, $(HELPER_SRC_NAME:c=o))

DAEMON_SRC = $(addprefix $(SRCDIR), $(DAEMON_SRC_NAME))
DAEMON_OBJ = $(addprefix $(OBJ_DIR)/, $(DAEMON_SRC_NAME:c=o))

DBUS_SRC = $(addprefix $(SRCDIR), $(DBUS_SRC_NAME))
DBUS_OBJ = $(addprefix $(OBJ_DIR)/, $(DBUS_SRC_NAME:c=o))
CC = cc

debug: CFLAGS = -g3 -pipe -Wall -Wextra -Werror `pkg-config --cflags gtk4 polkit-gobject-1` -I$(INCDIR)
helper: CFLAGS = -g3 -pipe -Wall -Wextra -Werror -pedantic `pkg-config --cflags gtk4 polkit-gobject-1` -I$(INCDIR)
daemon: CFLAGS = -g3 -pipe -Wall -Wextra -Werror -pedantic `pkg-config --cflags gtk4 polkit-gobject-1` -I$(INCDIR)
dbus: CFLAGS = -g3 -pipe -Wall -Wextra -Werror  `pkg-config --cflags gtk4 polkit-gobject-1` -I$(INCDIR)
$(NAME): CFLAGS = -O2 `pkg-config --cflags gtk4 polkit-gobject-1` -I$(INCDIR)

LDFLAGS = `pkg-config --libs gtk4 polkit-gobject-1`

all: $(NAME)

helper: $(HELPER_OBJ)
	@echo "Compiling helper binary..."
	@$(CC) -o acpi_helper $(HELPER_OBJ) $(LDFLAGS)

daemon: $(DAEMON_OBJ)
	@echo "Compiling daemon..."
	@$(CC) -o PowerManagerDaemon $(DAEMON_OBJ) $(LDFLAGS)
dbus: $(DBUS_OBJ)
	@echo "Compiling daemon..."
	@$(CC) -o PowerManagerDbus $(DBUS_OBJ) $(LDFLAGS)

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
