#include "power_manager.h"
#include <signal.h>

int g_server_fd;
int g_continue = 1;

void cleanup (int sig) {
  (void)sig;
  close(g_server_fd);
  unlink(SOCKET_PATH);
  exit(0);
}

int execute_action(int opcode) {
  int status = 0;
  switch (opcode) {
    case ACPI_SHUTDOWN:
      g_continue = 0;
      status = 0;
      break;
    case ACPI_CHK_BC:
      status = is_bc_on();
      break;
    case ACPI_CHK_RC:
      status = is_rc_on();
      break;
    case ACPI_SET_BC_OFF:
      write_acpi(ACPI_SET_BMODE_BC_OFF);
      break;
    case ACPI_SET_BC_ON:
      write_acpi(ACPI_SET_BMODE_BC_ON);
      break;
    case ACPI_SET_RC_OFF:
      write_acpi(ACPI_SET_BMODE_RC_OFF);
      break;
    case ACPI_SET_RC_ON:
      write_acpi(ACPI_SET_BMODE_RC_ON);
      break;
    case ACPI_CHK_PWR:
      status = acpi_query_pwrstatus();
      break;
    case ACPI_SET_IC:
      status = acpi_setpwr(ACPI_SET_PWRMODE_IC);
      break;
    case ACPI_SET_EP:
      status = acpi_setpwr(ACPI_SET_PWRMODE_EP);
      break;
    case ACPI_SET_BS:
      status = acpi_setpwr(ACPI_SET_PWRMODE_BS);
      break;
    default:
      status = -1;
      break;
  }
  return status;
}

int handle_acpi_request(int client_sock) {
  char buffer[256];
  int ret;
  int bytes_read = read(client_sock, buffer, sizeof(buffer) - 1);
  if (bytes_read > 0) {
    buffer[bytes_read] = '\0';
    ret = execute_action(atoi(buffer));
    if (ret >= 0) {
      char acpi_data[64];
      bzero(acpi_data, sizeof(acpi_data));
      sprintf(acpi_data, "%d", ret);
      write(client_sock, acpi_data, strlen(acpi_data));
    } else {
      write(client_sock, "ERROR", 5);
    }
  }
  close(client_sock);
  return 0;
}

int server_daemon(void) {
  int g_server_fd, client_sock;
  struct sockaddr_un server_addr;

  unlink(SOCKET_PATH);

  g_server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (g_server_fd < 0) {
      perror("error: failed to initialize socket:");
      return(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

  if (bind(g_server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
      perror("error: failed to bind socket:");
      return(EXIT_FAILURE);
  }

  chmod(SOCKET_PATH, 0666);
  listen(g_server_fd, 5);

  printf("Daemon running, waiting for connections...\n");

  while (g_continue) {
      client_sock = accept(g_server_fd, NULL, NULL);
      if (client_sock >= 0) {
          handle_acpi_request(client_sock);
      }
  }
  printf("Shutting down daemon...\n");

  close(g_server_fd);
  unlink(SOCKET_PATH);
  return(EXIT_SUCCESS);
}

int main (int ac, char **av) {
  int status;
  (void)av;

  signal(SIGINT, cleanup);
  signal(SIGTERM, cleanup);
  signal(SIGUSR1, cleanup);

  if (geteuid() != 0) {
    fprintf(stderr, "Fatal: Not SuperUser\n");
    return (EXIT_FAILURE);
  }
  if (ac != 1) {
    fprintf(stderr, "Fatal: Bad usage: Incorrect argument count\n");
    return (EXIT_FAILURE);
  }
  status = server_daemon();
  if (status) {
    fprintf(stderr, "error: daemon exited with code %d\n", status);
  }
  return status;
}
