#include "power_manager.h"
enum e_powerstate {IC = 0, EP, BS};

static const char	*query_acpi_info(const char *r_value)
{
  int fd;
  FILE *file;
  char *value;
  int ret;

  value = calloc(51, sizeof(char));
  // Open /proc/acpi/call for writing with elevated privileges
  file = fopen("/proc/acpi/call", "w");
  if (file == NULL) {
      perror("Error opening /proc/acpi/call");
      return (NULL);
  }

  // Write the value to the file
  if (fprintf(file, "%s", r_value) < 0) {
      perror("Error writing to /proc/acpi/call");
      fclose(file);
      return (NULL);
  }

  // Close the file
  fclose(file);
  fd = open("/proc/acpi/call", 0, O_RDONLY);
  if (fd == -1)
  {
	perror("error opening /proc/acpi/call");
	return (NULL);
  }
  ret = read(fd, value, 50);
  if (!ret)
  {
	  perror ("error reading from /proc/acpi/call");
	  close(fd);
	  return (NULL);
  }
  return (value);
}

static const char	*query_PwrMode_info(void)
{
  const char *value = query_acpi_info("\\_SB.PCI0.LPC0.EC0.SPMO");
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
  else
	  return ("Cannot determine Power Mode");
}

void update_PwrMode_text(GtkLabel *label)
{
    gtk_label_set_text(label, query_PwrMode_info());
}

static void set_IC(GtkWidget *widget, gpointer data)
{
  GObject *label = (GObject *)data;
  g_print ("Intelligent Cooling Mode Activated\n");
  FILE *file;
  const char *value = "\\_SB.PCI0.LPC0.EC0.VPC0.DYTC 0x000FB001";
  
  // Open /proc/acpi/call for writing with elevated privileges
  file = fopen("/proc/acpi/call", "w");
  if (file == NULL) {
      perror("Error opening /proc/acpi/call");
      return;
  }

  // Write the value to the file
  if (fprintf(file, "%s", value) < 0) {
      perror("Error writing to /proc/acpi/call");
      fclose(file);
      return;
  }

  // Close the file
  fclose(file);
  update_PwrMode_text(GTK_LABEL(label));
  (void)widget;
  (void)data;
}

static void set_EP(GtkWidget *widget, gpointer   data)
{
  GObject *label = (GObject *)data;
  g_print ("Extreme Performance Mode Activated\n");
  FILE *file;
  const char *value = "\\_SB.PCI0.LPC0.EC0.VPC0.DYTC 0x0012B001";
  
  // Open /proc/acpi/call for writing with elevated privileges
  file = fopen("/proc/acpi/call", "w");
  if (file == NULL) {
      perror("Error opening /proc/acpi/call");
      return;
  }

  // Write the value to the file
  if (fprintf(file, "%s", value) < 0) {
      perror("Error writing to /proc/acpi/call");
      fclose(file);
      return;
  }

  // Close the file
  fclose(file);
  update_PwrMode_text(GTK_LABEL(label));
  (void)widget;
  (void)data;
}

static void set_BS(GtkWidget *widget, gpointer   data)
{
  GObject *label = (GObject *)data;
  g_print ("Battery Saving mode activated\n");
  FILE *file;
  const char *value = "\\_SB.PCI0.LPC0.EC0.VPC0.DYTC 0x0013B001";

  // Open /proc/acpi/call for writing with elevated privileges
  file = fopen("/proc/acpi/call", "w");
  if (file == NULL) {
      perror("Error opening /proc/acpi/call");
      return;
  }

  // Write the value to the file
  if (fprintf(file, "%s", value) < 0) {
      perror("Error writing to /proc/acpi/call");
      fclose(file);
      return;
  }

  // Close the file
  fclose(file);
  update_PwrMode_text(GTK_LABEL(label));
  (void)widget;
  (void)data;
}

static void switch_battery_mode(GtkWidget *widget, gpointer data)
{
    (void)widget;
    (void)data;
}

static void
quit_cb (GtkWindow *window)
{
  gtk_window_close (window);
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  (void)user_data;
  /* Construct a GtkBuilder instance and load our UI description */
  GtkBuilder *builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "src/builder.ui", NULL);

  /* Connect signal handlers to the constructed widgets. */
  GObject *window = gtk_builder_get_object (builder, "window");
  GObject *label = gtk_builder_get_object (builder, "currentPwrMode");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
  gtk_window_set_application (GTK_WINDOW (window), app);

  GObject *button = gtk_builder_get_object (builder, "btnIntelligentCooling");
  g_signal_connect (button, "clicked", G_CALLBACK (set_IC), label);

  button = gtk_builder_get_object (builder, "btnExtremePerformance");
  g_signal_connect (button, "clicked", G_CALLBACK (set_EP), label);

  button = gtk_builder_get_object (builder, "btnBatterySaving");
  g_signal_connect (button, "clicked", G_CALLBACK (set_BS), label);

  update_PwrMode_text(GTK_LABEL(label));
  button = gtk_builder_get_object (builder, "btnBatteryState");
  g_signal_connect (button, "clicked", G_CALLBACK (switch_battery_mode), NULL);
  //button = gtk_builder_get_object (builder, "quit");
  //g_signal_connect_swapped (button, "clicked", G_CALLBACK (quit_cb), window);
  (void)quit_cb;
  gtk_widget_set_visible (GTK_WIDGET (window), TRUE);

  /* We do not need the builder any more */
  g_object_unref (builder);
}

int
main (int   argc, char *argv[])
{
#ifdef GTK_SRCDIR
  g_chdir (GTK_SRCDIR);
#endif
  GtkApplication *app = gtk_application_new ("org.gtk.PowerManager", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

  int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}

