#ifndef POWER_MANAGER_H
# define POWER_MANAGER_H

# include <gtk/gtk.h>
# include <glib/gstdio.h>
# include <fcntl.h>
# include <unistd.h>
# include <string.h>

# define ACPI_INFO "\\_SB.PCI0.LPC0.EC0.SPMO"
# define ACPI_SET_PWRMODE_IC "\\_SB.PCI0.LPC0.EC0.VPC0.DYTC 0x000FB001"
# define ACPI_SET_PWRMODE_EP "\\_SB.PCI0.LPC0.EC0.VPC0.DYTC 0x0012B001"
# define ACPI_SET_PWRMODE_BS "\\_SB.PCI0.LPC0.EC0.VPC0.DYTC 0x0013B001"
# define ACPI_SET_BMODE_BC_OFF "\\_SB.PCI0.LPC0.EC0.VPC0.SBMC 0x05"
# define ACPI_SET_BMODE_BC_ON "\\_SB.PCI0.LPC0.EC0.VPC0.SBMC 0x03"
# define ACPI_SET_BMODE_RC_OFF "\\_SB.PCI0.LPC0.EC0.VPC0.SBMC 0x08"
# define ACPI_SET_BMODE_RC_ON "\\_SB.PCI0.LPC0.EC0.VPC0.SBMC 0x07"

enum e_powerstate {IC = 0, EP, BS};

// acpi_access.c
void        write_acpi(const char *value);
const char  *query_acpi_info(const char *r_value);

// pwr_management.c
const char  *query_PwrMode_info(void);
void        update_PwrMode_text(GtkLabel *label);
void        set_PwrMode_IC(GtkWidget *widget, gpointer data);
void        set_PwrMode_EP(GtkWidget *widget, gpointer data);
void        set_PwrMode_BS(GtkWidget *widget, gpointer data);

// battery_management.c
int         is_rc_on(void);
int         is_bc_on(void);
void        switch_battery_bc(GtkWidget *widget, gpointer data);
void        switch_battery_rc(GtkWidget *widget, gpointer data);
#endif
