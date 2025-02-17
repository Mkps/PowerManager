#ifndef POWER_MANAGER_H
# define POWER_MANAGER_H

# include <gtk/gtk.h>
# include <polkit/polkit.h>
# include <glib/gstdio.h>
# include <fcntl.h>
# include <unistd.h>

# define ACPI_INFO "\\_SB.PCI0.LPC0.EC0.SPMO"
# define ACPI_SET_PWRMODE_IC "\\_SB.PCI0.LPC0.EC0.VPC0.DYTC 0x000FB001"
# define ACPI_SET_PWRMODE_EP "\\_SB.PCI0.LPC0.EC0.VPC0.DYTC 0x0012B001"
# define ACPI_SET_PWRMODE_BS "\\_SB.PCI0.LPC0.EC0.VPC0.DYTC 0x0013B001"
# define ACPI_SET_BMODE_BC_OFF "\\_SB.PCI0.LPC0.EC0.VPC0.SBMC 0x05"
# define ACPI_SET_BMODE_BC_ON "\\_SB.PCI0.LPC0.EC0.VPC0.SBMC 0x03"
# define ACPI_SET_BMODE_RC_OFF "\\_SB.PCI0.LPC0.EC0.VPC0.SBMC 0x08"
# define ACPI_SET_BMODE_RC_ON "\\_SB.PCI0.LPC0.EC0.VPC0.SBMC 0x07"
# define PS_IC_MODE 3
# define PS_EP_MODE 2
# define PS_BC_MODE 1
# define APPNAME "com.mkps.powermanager"
# ifndef DEBUG
#  define DEBUG 0
# endif

enum e_powerstate {IC = 0, EP, BS};
typedef enum {
    ACPI_OP_GET_POWER_STATE,
    ACPI_OP_SET_IC_MODE,
    ACPI_OP_SET_EP_MODE,
    ACPI_OP_SET_BS_MODE,
    ACPI_OP_GET_BC_STATE,
    ACPI_OP_GET_RC_STATE,
    ACPI_OP_SWITCH_BC,
    ACPI_OP_SWITCH_RC
} e_AcpiOperation;


typedef struct s_AppData {
    GtkApplication *app;
    PolkitAuthority *authority;
    GtkWindow *main_window;
} t_AppData;

typedef struct s_AcpiQueryResult {
    gboolean success;
    union {
        gboolean bool_result;
        gint     int_result;
        gchar*   string_result;
    } data;
} t_AcpiQueryResult;

typedef struct s_AuthCallbackData {
    e_AcpiOperation operation;
    GtkWidget *widget;
    t_AppData *app_data;
    t_AcpiQueryResult *result;
    void (*completion_callback)(t_AcpiQueryResult*, gpointer);
    gpointer completion_data;
} t_AuthCallbackData;

// acpi_access.c
void        write_acpi(const char *value);
const char  *query_acpi_info(const char *r_value);

// pwr_management.c
const char  *query_PwrMode_info(void);
void        update_PwrMode_text(GtkLabel *label);
void        set_PwrMode_IC(GtkWidget *widget, gpointer data);
void        set_PwrMode_EP(GtkWidget *widget, gpointer data);
void        set_PwrMode_BS(GtkWidget *widget, gpointer data);
int         get_power_state(void);

// battery_management.c

typedef void (*RcStatusCallback)(gboolean is_on, gboolean success, gpointer user_data);

int         is_bc_on(void);
void        switch_battery_bc(GtkWidget *widget, gpointer data);
int         is_rc_on(void);
void        switch_battery_rc(GtkWidget *widget, gpointer data);
void        handle_rc_status(gboolean is_on, gboolean success, gpointer user_data);
void        check_rc_status(t_AppData*, RcStatusCallback, gpointer);

// polkit.c
void
query_privileged_acpi(t_AppData *app_data,
                     e_AcpiOperation operation,
                     t_AcpiQueryResult *result,
                     void (*completion_callback)(t_AcpiQueryResult*, gpointer),
                     gpointer completion_data);
void
auth_check_callback(GObject *source_object,
                   GAsyncResult *res,
                   gpointer user_data);

#endif
