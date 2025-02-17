#include "power_manager.h"

int is_bc_on(void)
{
    const char *acpi_mode;
    int         ret;

    ret = 0;
    acpi_mode = query_acpi_info("\\_SB.PCI0.LPC0.EC0.BTSM");
    if (!acpi_mode)
    {
        printf("Could not query battery protection status\n");
        return (ret);
    }
    if (strncmp(acpi_mode, "0x0", 4))
        ret = 1;
    free((char *)acpi_mode);
    return (ret);
}

void switch_battery_bc(GtkWidget *widget, gpointer data)
{
  g_print ("Battery Conservation toggled\n");

  if (is_bc_on())
  {
      write_acpi(ACPI_SET_BMODE_BC_OFF);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), is_bc_on());
      return ;
  }
  else if (is_rc_on())
  {
      write_acpi(ACPI_SET_BMODE_RC_OFF);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), is_rc_on());
  }
  write_acpi(ACPI_SET_BMODE_BC_ON);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), is_bc_on());
}

int is_rc_on(void)
{
    const char *acpi_mode;
    int         ret;

    ret = 0;
    acpi_mode = query_acpi_info("\\_SB.PCI0.LPC0.EC0.QCHO");
    if (!acpi_mode)
    {
        printf("Could not query rapid charge status\n");
        return (ret);
    }
    if (strncmp(acpi_mode, "0x0", 4))
        ret = 1;
    free((char *)acpi_mode);
    return (ret);
}

void switch_battery_rc(GtkWidget *widget, gpointer data)
{
  g_print ("Rapid Charge toggled\n");

  if (is_rc_on())
  {
      write_acpi(ACPI_SET_BMODE_RC_OFF);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), is_rc_on());
      return ;
  }
  else if (is_bc_on())
  {
      write_acpi(ACPI_SET_BMODE_BC_OFF);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), is_bc_on());
  }
  write_acpi(ACPI_SET_BMODE_RC_ON);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), is_rc_on());
}

void
handle_rc_status(gboolean is_on, gboolean success, gpointer user_data)
{
    GtkToggleButton *button = GTK_TOGGLE_BUTTON(user_data);
    
    if (success) {
        // Update button state
        gtk_toggle_button_set_active(button, is_on);
    } else {
        // Handle error (e.g., show error message)
        // GtkWidget *dialog = gtk_message_dialog_new(NULL,
        //     GTK_DIALOG_MODAL,
        //     GTK_MESSAGE_ERROR,
        //     GTK_BUTTONS_OK,
        //     "Failed to query rapid charge status");
        // gtk_window_present(GTK_WINDOW(dialog));
        // g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
      fprintf(stderr, "Failed to get rapid charge status\n");
      return;
    }
}

void handle_rc_result(t_AcpiQueryResult *result, gpointer data) {
    RcStatusCallback user_callback = ((gpointer *)data)[0];
    gpointer user_data = ((gpointer *)data)[1];
    
    user_callback(result->data.bool_result,  // is_on
                  result->success,            // success
                  user_data);                 // original user_data
    
    g_free(result);
    g_free(data);
}

void check_rc_status(t_AppData *app_data, RcStatusCallback callback, gpointer user_data)
{
    // Prepare result storage
    t_AcpiQueryResult *result = g_new0(t_AcpiQueryResult, 1);
    
    // Completion callback that will be called after authorization
    
    // Pack callback and user_data together
    gpointer *callback_data = g_new0(gpointer, 2);
    callback_data[0] = callback;
    callback_data[1] = user_data;
    
    // Initiate the privileged query
    query_privileged_acpi(app_data,
                         ACPI_OP_GET_RC_STATE,
                         result,
                         handle_rc_result,
                         callback_data);
}
