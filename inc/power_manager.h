#ifndef POWER_MANAGER_H
# define POWER_MANAGER_H

# include <gtk/gtk.h>
# include <glib/gstdio.h>
# include <fcntl.h>
# include <unistd.h>
# include <string.h>
# include <sys/socket.h>
# include <sys/un.h>
# include <stdio.h>
# include <stdlib.h>
# include "powermanager-dbus.h"

# define ACPI_INFO "\\_SB.PCI0.LPC0.EC0.SPMO"
# define ACPI_SET_PWRMODE_IC "\\_SB.PCI0.LPC0.EC0.VPC0.DYTC 0x000FB001"
# define ACPI_SET_PWRMODE_EP "\\_SB.PCI0.LPC0.EC0.VPC0.DYTC 0x0012B001"
# define ACPI_SET_PWRMODE_BS "\\_SB.PCI0.LPC0.EC0.VPC0.DYTC 0x0013B001"
# define ACPI_SET_BMODE_BC_OFF "\\_SB.PCI0.LPC0.EC0.VPC0.SBMC 0x05"
# define ACPI_SET_BMODE_BC_ON "\\_SB.PCI0.LPC0.EC0.VPC0.SBMC 0x03"
# define ACPI_SET_BMODE_RC_OFF "\\_SB.PCI0.LPC0.EC0.VPC0.SBMC 0x08"
# define ACPI_SET_BMODE_RC_ON "\\_SB.PCI0.LPC0.EC0.VPC0.SBMC 0x07"

# define ACPI_SHUTDOWN 0
# define ACPI_CHK_BC 1
# define ACPI_CHK_RC 2
# define ACPI_SET_BC_ON 31
# define ACPI_SET_BC_OFF 32
# define ACPI_SET_RC_ON 41
# define ACPI_SET_RC_OFF 42
# define ACPI_CHK_PWR 5
# define ACPI_SET_IC 6
# define ACPI_SET_EP 7
# define ACPI_SET_BS 8

#ifndef DEBUG
# define DEBUG 0
#endif

# define PWR_MODE_IC 1
# define PWR_MODE_EP 2
# define PWR_MODE_BS 3

enum e_powerstate {IC = 0, EP, BS};

typedef struct e_app_data {
    Powermanager *proxy;
    GObject      *buttonBC;
    GObject      *buttonRC;
    GObject      *label;
} t_app_data;

// acpi_access.c
void        write_acpi(const char *value);
const char  *query_acpi_info(const char *r_value);
int         access_acpi(Powermanager* proxy, int opcode);

// pwr_management.c
const char  *query_PwrMode_info(Powermanager *proxy);
void        update_PwrMode_text(t_app_data *data);
void        set_PwrMode_IC(GtkWidget *widget, gpointer data);
void        set_PwrMode_EP(GtkWidget *widget, gpointer data);
void        set_PwrMode_BS(GtkWidget *widget, gpointer data);
int         acpi_query_pwrstatus(void); 
int         acpi_setpwr(const char *powerMode);

// battery_management.c
int         is_rc_on(void);
int         is_bc_on(void);
void        switch_battery_bc(GtkWidget *widget, gpointer data);
void        switch_battery_rc(GtkWidget *widget, gpointer data);
#endif
