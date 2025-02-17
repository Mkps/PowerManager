#include "power_manager.h"

const char	*query_PwrMode_info(void)
{
  const char *value = query_acpi_info(ACPI_INFO);
  if (!value)
	  return ("Cannot determine Power Mode");
  if (!strncmp("0x0", value, 4))
  {
	  free((char *)value);
	  return ("Intelligent Cooling");
  }
  else if (!strncmp("0x1", value, 4))
  {
	  free((char *)value);
	  return ("Extreme Performance");
  }
  else if (!strncmp("0x2", value, 4))
  {
	  free((char *)value);
	  return ("Battery Saving");
  }
  else {
	  free((char *)value);
	  return ("Cannot determine Power Mode");
  }
}

void update_PwrMode_text(GtkLabel *label)
{
    gtk_label_set_text(label, query_PwrMode_info());
}

void set_PwrMode_IC(GtkWidget *widget, gpointer data)
{
  GObject *label = (GObject *)data;
  g_print ("Intelligent Cooling Mode Activated\n");
  FILE *file;
  
  // Open /proc/acpi/call for writing with elevated privileges
  file = fopen("/proc/acpi/call", "w");
  if (file == NULL) {
      perror("Error opening /proc/acpi/call");
      return;
  }

  // Write the value to the file
  if (fprintf(file, "%s", ACPI_SET_PWRMODE_IC) < 0) {
      perror("Error writing to /proc/acpi/call");
      fclose(file);
      return;
  }

  // Close the file
  fclose(file);
  update_PwrMode_text(GTK_LABEL(label));
  (void)widget;
}

void set_PwrMode_EP(GtkWidget *widget, gpointer   data)
{
  GObject *label = (GObject *)data;
  g_print ("Extreme Performance Mode Activated\n");
  FILE *file;
  
  // Open /proc/acpi/call for writing with elevated privileges
  file = fopen("/proc/acpi/call", "w");
  if (file == NULL) {
      perror("Error opening /proc/acpi/call");
      return;
  }

  // Write the value to the file
  if (fprintf(file, "%s", ACPI_SET_PWRMODE_EP) < 0) {
      perror("Error writing to /proc/acpi/call");
      fclose(file);
      return;
  }

  // Close the file
  fclose(file);
  update_PwrMode_text(GTK_LABEL(label));
  (void)widget;
}

void set_PwrMode_BS(GtkWidget *widget, gpointer   data)
{
  GObject *label = (GObject *)data;
  g_print ("Battery Saving mode activated\n");
  FILE *file;

  // Open /proc/acpi/call for writing with elevated privileges
  file = fopen("/proc/acpi/call", "w");
  if (file == NULL) {
      perror("Error opening /proc/acpi/call");
      return;
  }

  // Write the value to the file
  if (fprintf(file, "%s", ACPI_SET_PWRMODE_BS) < 0) {
      perror("Error writing to /proc/acpi/call");
      fclose(file);
      return;
  }

  // Close the file
  fclose(file);
  update_PwrMode_text(GTK_LABEL(label));
  (void)widget;
}

int get_power_state(void)
{
  const char *value = query_acpi_info(ACPI_INFO);
  if (!value)
	  return (0);
  if (!strncmp("0x0", value, 4))
  {
	  free((char *)value);
	  return (PS_IC_MODE);
  }
  else if (!strncmp("0x1", value, 4))
  {
	  free((char *)value);
	  return (PS_EP_MODE);
  }
  else if (!strncmp("0x2", value, 4))
  {
	  free((char *)value);
	  return (PS_BC_MODE);
  }
  else 
  {
	  free((char *)value);
	  return (0);
  }
}
