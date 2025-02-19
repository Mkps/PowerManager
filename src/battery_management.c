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
        return (-1);
    }
    if (strncmp(acpi_mode, "0x0", 4))
        ret = 1;
    free((char *)acpi_mode);
    return (ret);
}

void switch_battery_bc(GtkWidget *widget, gpointer data)
{
  g_print ("Battery Conservation toggled\n");

  int bc_on = access_acpi(ACPI_CHK_BC);
  int rc_on = access_acpi(ACPI_CHK_RC);
  if (bc_on)
  {
      //write_acpi(ACPI_SET_BMODE_BC_OFF);
      access_acpi(ACPI_SET_BC_OFF);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), access_acpi(ACPI_CHK_BC));
      return ;
  }
  else if (rc_on)
  {
      //write_acpi(ACPI_SET_BMODE_RC_OFF);
      access_acpi(ACPI_SET_RC_OFF);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), access_acpi(ACPI_CHK_RC));
  }
  //write_acpi(ACPI_SET_BMODE_BC_ON);
  access_acpi(ACPI_SET_BC_ON);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), access_acpi(ACPI_CHK_BC));
}

void switch_battery_rc(GtkWidget *widget, gpointer data)
{
  g_print ("Rapid Charge toggled\n");

  int bc_on = access_acpi(ACPI_CHK_BC);
  int rc_on = access_acpi(ACPI_CHK_RC);
  if (rc_on)
  {
      //write_acpi(ACPI_SET_BMODE_RC_OFF);
      access_acpi(ACPI_SET_RC_OFF);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), access_acpi(ACPI_CHK_RC));
      return ;
  }
  else if (bc_on)
  {
      //write_acpi(ACPI_SET_BMODE_BC_OFF);
      access_acpi(ACPI_SET_BC_OFF);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), access_acpi(ACPI_CHK_BC));
  }
  //write_acpi(ACPI_SET_BMODE_RC_ON);
  access_acpi(ACPI_SET_RC_ON);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), access_acpi(ACPI_CHK_RC));
}
