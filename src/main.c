#include "power_manager.h"

static void activate (GtkApplication *app, gpointer user_data)
{
  (void)app;
  t_AppData *app_data = (t_AppData *)user_data;
  GObject *window;
  GObject *button;
  GError *error = NULL;

  // Initialize PolicyKit authority
  app_data->authority = polkit_authority_get_sync(NULL, &error);
  if (error != NULL) {
      g_error("Error getting authority: %s", error->message);
      g_error_free(error);
      return;
  }


  /* Construct a GtkBuilder instance and load our UI description */
  GtkBuilder *builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "src/builder.ui", NULL);

  /* Connect signal handlers to the constructed widgets. */
  window = gtk_builder_get_object (builder, "window");
  // GObject *label = gtk_builder_get_object (builder, "currentPwrMode");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
  gtk_window_set_application (GTK_WINDOW (window), app_data->app);
  app_data->main_window = GTK_WINDOW(window);
  (void)button;

  // button = gtk_builder_get_object (builder, "btnIntelligentCooling");
  // g_signal_connect (button, "clicked", G_CALLBACK (set_PwrMode_IC), label);
  // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), (get_power_state() == PS_IC_MODE));
  //
  // button = gtk_builder_get_object (builder, "btnExtremePerformance");
  // g_signal_connect (button, "clicked", G_CALLBACK (set_PwrMode_EP), label);
  // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), (get_power_state() == PS_EP_MODE));
  //
  // button = gtk_builder_get_object (builder, "btnBatterySaving");
  // g_signal_connect (button, "clicked", G_CALLBACK (set_PwrMode_BS), label);
  // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), (get_power_state() == PS_BC_MODE));

  // update_PwrMode_text(GTK_LABEL(label));

  GObject *bc_button = gtk_builder_get_object (builder, "btnBatteryBC");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(bc_button), is_bc_on());
  GObject *rc_button = gtk_builder_get_object (builder, "btnBatteryRC");
  check_rc_status(app_data, handle_rc_status, rc_button);
  //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttonRC), is_rc_on());

  g_signal_connect (bc_button, "clicked", G_CALLBACK (switch_battery_bc), rc_button);
  g_signal_connect (rc_button, "clicked", G_CALLBACK (switch_battery_rc), bc_button);
  gtk_widget_set_visible (GTK_WIDGET (window), TRUE);
  g_object_unref (builder);
}

int main (int   argc, char *argv[])
{
#ifdef GTK_SRCDIR
  g_chdir (GTK_SRCDIR);
#endif
  GtkApplication *app;
  t_AppData *app_data;

  app_data = g_new0(t_AppData, 1);
  app = gtk_application_new ("com.mkps.powermanager", G_APPLICATION_DEFAULT_FLAGS);
  app_data->app = app;
  
  g_signal_connect (app, "activate", G_CALLBACK (activate), app_data);
  int status = g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);
  if (app_data->authority)
    g_object_unref (app_data->authority);
  g_free(app_data);

  return status;
}

