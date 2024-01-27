#include "Power.h"
#include <gtk/gtk.h>
#include <glib/gstdio.h>

enum e_powerstate {IC = 0, EP, BS};

static void set_IC(GtkWidget *widget, gpointer data)
{
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

  (void)widget;
  (void)data;
}

static void
set_EP(GtkWidget *widget,
             gpointer   data)
{
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
  (void)widget;
  (void)data;
}

static void
set_BS(GtkWidget *widget,
             gpointer   data)
{
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
  gtk_window_set_application (GTK_WINDOW (window), app);

  GObject *button = gtk_builder_get_object (builder, "btnIntelligentCooling");
  g_signal_connect (button, "clicked", G_CALLBACK (set_IC), NULL);

  button = gtk_builder_get_object (builder, "btnExtremePerformance");
  g_signal_connect (button, "clicked", G_CALLBACK (set_EP), NULL);

  button = gtk_builder_get_object (builder, "btnBatterySaving");
  g_signal_connect (button, "clicked", G_CALLBACK (set_BS), NULL);

  //button = gtk_builder_get_object (builder, "quit");
  //g_signal_connect_swapped (button, "clicked", G_CALLBACK (quit_cb), window);
  (void)quit_cb;
  gtk_widget_set_visible (GTK_WIDGET (window), TRUE);

  /* We do not need the builder any more */
  g_object_unref (builder);
}

int
main (int   argc,
      char *argv[])
{
#ifdef GTK_SRCDIR
  g_chdir (GTK_SRCDIR);
#endif

  GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

  int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}

