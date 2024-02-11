#include "power_manager.h"

int is_rc_on(void)
{
    const char *acpi_mode;
    int         ret;

    ret = 0;
    acpi_mode = query_acpi_info("\\_SB.PCI0.LPC0.EC0.QCHO");
    if (!acpi_mode)
    {
        printf("could not query rapid charge status");
        return (ret);
    }
    if (strncmp(acpi_mode, "0x0", 4))
        ret = 1;
    free((char *)acpi_mode);
    return (ret);
}

int is_bc_on(void)
{
    const char *acpi_mode;
    int         ret;

    ret = 0;
    acpi_mode = query_acpi_info("\\_SB.PCI0.LPC0.EC0.BTSM");
    if (!acpi_mode)
    {
        printf("could not query battery protection status");
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
