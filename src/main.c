#include "power_manager.h"

static void activate (GtkApplication *app, gpointer user_data)
{
  (void)user_data;
  /* Construct a GtkBuilder instance and load our UI description */
  GtkBuilder *builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "/home/alx/Code/Powermanager/src/builder.ui", NULL);

  /* Connect signal handlers to the constructed widgets. */
  GObject *window = gtk_builder_get_object (builder, "window");
  GObject *label = gtk_builder_get_object (builder, "currentPwrMode");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
  gtk_window_set_application (GTK_WINDOW (window), app);

  GObject *button = gtk_builder_get_object (builder, "btnIntelligentCooling");
  g_signal_connect (button, "clicked", G_CALLBACK (set_PwrMode_IC), label);

  button = gtk_builder_get_object (builder, "btnExtremePerformance");
  g_signal_connect (button, "clicked", G_CALLBACK (set_PwrMode_EP), label);

  button = gtk_builder_get_object (builder, "btnBatterySaving");
  g_signal_connect (button, "clicked", G_CALLBACK (set_PwrMode_BS), label);

  update_PwrMode_text(GTK_LABEL(label));
  GObject *buttonBC = gtk_builder_get_object (builder, "btnBatteryBC");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttonBC), is_bc_on());
  GObject *buttonRC = gtk_builder_get_object (builder, "btnBatteryRC");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttonRC), is_rc_on());
  g_signal_connect (buttonBC, "clicked", G_CALLBACK (switch_battery_bc), buttonRC);
  g_signal_connect (buttonRC, "clicked", G_CALLBACK (switch_battery_rc), buttonBC);
  gtk_widget_set_visible (GTK_WIDGET (window), TRUE);
  g_object_unref (builder);
}

int main (int   argc, char *argv[], char **env)
{
#ifdef GTK_SRCDIR
  g_chdir (GTK_SRCDIR);
#endif
  if (geteuid() != 0) {  // If not root, relaunch with pkexec
    char *exec_args[256];
    bzero(exec_args, sizeof(exec_args));
    exec_args[0] = "pkexec";
    exec_args[1] = "env";
    int i = 0;
    for (; env[i] && i < 250; ++i) {
      exec_args[i + 2] = env[i];
    }
    exec_args[i + 2] =  "/home/alx/Code/Powermanager/PowerManager";
    execvp("pkexec", exec_args);
    perror("Failed to execute pkexec");
    return EXIT_FAILURE;
  }
  GtkApplication *app = gtk_application_new ("com.mkps.powermanager", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

  int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}

