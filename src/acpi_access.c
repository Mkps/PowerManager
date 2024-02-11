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