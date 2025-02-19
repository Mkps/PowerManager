#include "power_manager.h"


int main (int ac, char **av) {
  int status;

  if (geteuid() != 0) {
    fprintf(stdout, "Fatal: Not SuperUser\n");
    return (EXIT_FAILURE);
  }
  if (ac != 2) {
    fprintf(stdout, "Fatal: Bad usage: Incorrect argument count\n");
    return (EXIT_FAILURE);
  }
  status = 0;
  int opcode = atoi(av[1]);
  switch (opcode) {
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
    default:
      status = -1;
      break;
  }
  if (status < 0) {
    fprintf(stderr, "error: acpi_helper exited with code %d\n", status);
  } else {
    printf("%d", status);
  }
  return status;
}
