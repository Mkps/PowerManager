#include "power_manager.h"

int acpi_query_pwrstatus(void) {
  const char *value = query_acpi_info(ACPI_INFO);
  if (!value)
	  return (-1);
  if (!strncmp("0x0", value, 4))
  {
	  free((char *)value);
	  return (PWR_MODE_IC);
  }
  else if (!strncmp("0x1", value, 4))
  {
	  free((char *)value);
	  return (PWR_MODE_EP);
  }
  else if (!strncmp("0x2", value, 4))
  {
	  free((char *)value);
	  return (PWR_MODE_BS);
  }
  else
	  return (-1);
}

const char	*query_PwrMode_info(Powermanager *proxy)
{
  int mode = access_acpi(proxy, ACPI_CHK_PWR);
  switch (mode) {
    case PWR_MODE_IC:
	    return ("Intelligent Cooling");
      break;
    case PWR_MODE_EP:
	    return ("Extreme Performance");
      break;
    case PWR_MODE_BS:
	    return ("Battery Saving");
      break;
    default:
	    return ("Cannot determine Power Mode");
      break;
  }
}


int acpi_setpwr(const char *powerMode) {
  FILE *file;

  file = fopen("/proc/acpi/call", "w");
  if (file == NULL) {
      perror("Error opening /proc/acpi/call");
      return (-1);
  }
  if (fprintf(file, "%s", powerMode) < 0) {
      perror("Error writing to /proc/acpi/call");
      fclose(file);
      return (-1);
  }
  fclose(file);
  return (0);
}

void update_PwrMode_text(t_app_data *data)
{
    gtk_label_set_text(GTK_LABEL(data->label), query_PwrMode_info(data->proxy));
}

void set_PwrMode_IC(GtkWidget *widget, gpointer data)
{
  (void)widget;
  t_app_data *app_data = (t_app_data *)data;
  int status = access_acpi(app_data->proxy, ACPI_SET_IC);
  if (!status)
    g_print ("Intelligent Cooling Mode Activated\n");
  update_PwrMode_text(app_data);
}

void set_PwrMode_EP(GtkWidget *widget, gpointer   data)
{
  (void)widget;
  t_app_data *app_data = (t_app_data *)data;
  int status = access_acpi(app_data->proxy, ACPI_SET_EP);
  if (!status)
    g_print ("Extreme Performance Mode Activated\n");
  update_PwrMode_text(app_data);
}

void set_PwrMode_BS(GtkWidget *widget, gpointer   data)
{
  (void)widget;
  t_app_data *app_data = (t_app_data *)data;
  int status = access_acpi(app_data->proxy, ACPI_SET_BS);
  if (!status)
    g_print ("Battery Saving mode activated\n");
  update_PwrMode_text(app_data);
}
