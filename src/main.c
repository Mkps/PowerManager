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
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttonBC), access_acpi(ACPI_CHK_BC));
  GObject *buttonRC = gtk_builder_get_object (builder, "btnBatteryRC");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttonRC), access_acpi(ACPI_CHK_RC));
  g_signal_connect (buttonBC, "clicked", G_CALLBACK (switch_battery_bc), buttonRC);
  g_signal_connect (buttonRC, "clicked", G_CALLBACK (switch_battery_rc), buttonBC);
  gtk_widget_set_visible (GTK_WIDGET (window), TRUE);
  g_object_unref (builder);
}

int filter_env(const char *str) {
  if (strstr(str, "DBUS")) 
    return 1;
  else if (strstr(str, "XDG")) 
    return 1;
  else if (strstr(str, "GTK")) 
    return 1;
  else if (strstr(str, "DISPLAY")) 
    return 1;
  else if (strstr(str, "LANG")) 
    return 1;
  else if (strstr(str, "PATH")) 
    return 1;
  else if (strstr(str, "XAUTHORITY")) 
    return 1;
  else if (strstr(str, "HOME")) 
    return 1;
  else
    return 0;
}

int run_as_superuser(char **env) 
{   
  char *exec_args[256];
  bzero(exec_args, sizeof(exec_args));
  exec_args[0] = "pkexec";
  exec_args[1] = "env";
  int i = -1;
  int j = 2;
  while(env[++i] && j < 250) {
    if (!filter_env(env[i]))
      continue;
    exec_args[j] = env[i];
    j++;
  }
  exec_args[j] = "/home/alx/Code/Powermanager/PowerManager";
  execvp("pkexec", exec_args);
  perror("Failed to execute pkexec");
  return EXIT_FAILURE;
}

int launch_daemon(void) {
  char *exec_args[32];
  bzero(exec_args, sizeof(exec_args));
  exec_args[0] = "pkexec";
  exec_args[1] = "--disable-internal-agent";
  exec_args[2] = "/home/alx/Code/Powermanager/PowerManagerDaemon";
  execvp("pkexec", exec_args);
  perror("Failed to execute pkexec");
  return EXIT_FAILURE;
}

int kill_daemon(int pid) {
  printf("Killing daemon with pid [%d]\n", pid);

  char *exec_args[32];
  char pid_str[32];
  bzero(exec_args, sizeof(exec_args));
  bzero(pid_str, sizeof(pid_str));
  exec_args[0] = "pkexec";
  exec_args[1] = "kill";
  exec_args[2] = "-10";
  sprintf(pid_str, "%d", pid);
  exec_args[3] = pid_str;

  execvp("pkexec", exec_args);
  perror("Failed to execute pkexec");
  return EXIT_FAILURE;
}

int wait_for_server_ready(const char *socket_path, int timeout_sec) {
    int sock;
    struct sockaddr_un addr;
    time_t start = time(NULL);

    while (time(NULL) - start < timeout_sec) {
      printf("Client waiting\n");
      sock = socket(AF_UNIX, SOCK_STREAM, 0);
      if (sock < 0) {
          perror("socket");
          return -1;
      }
      memset(&addr, 0, sizeof(addr));
      addr.sun_family = AF_UNIX;
      strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

      int tmp = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
      if (tmp == 0) {
          close(sock);
          return 0;
      }
      close(sock);
      sleep(1);
    }
    return -1;
}

int main (int   argc, char *argv[])
{
  int status;
  GtkApplication *app;

#ifdef GTK_SRCDIR
  g_chdir (GTK_SRCDIR);
#endif
  int pid = fork();
  if (pid < 0) {
    fprintf(stderr, "error: could not fork to launch daemon\n");
    return(EXIT_FAILURE);
  }
  else if (!pid) {
    printf("Launching daemon...\n");
    launch_daemon();
    return(EXIT_FAILURE);
  }
  else {
    if (wait_for_server_ready(SOCKET_PATH, 15) < 0){
      if (kill_daemon(pid))
        perror("error: daemon did not shutdown");
      return(EXIT_FAILURE);
    }

    app = gtk_application_new ("com.mkps.powermanager", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    kill_daemon(pid);
    return(status);
  }
}
