#include "power_manager.h"

void write_acpi(const char *value)
{
  FILE *file;
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
}

const char	*query_acpi_info(const char *r_value)
{
  int fd;
  char *value;
  int ret;

  value = calloc(51, sizeof(char));
  // Open /proc/acpi/call for writing with elevated privileges
  write_acpi(r_value);
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

int access_acpi(int opcode) {
  int sock;
  struct sockaddr_un server_addr;
  char cmd[128];
  bzero(cmd, sizeof(cmd));
  static char response[256];

  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0) {
      perror("socket");
      return -1;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

  if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
      perror("connect");
      close(sock);
      return -1;
  }
  sprintf(cmd, "%d", opcode);
  write(sock, cmd, strlen(cmd));
  read(sock, response, sizeof(response) - 1);
  close(sock);
  if (response[0] < '0' && response[0] > '9')
    return (-1);
  return atoi(response);
}
