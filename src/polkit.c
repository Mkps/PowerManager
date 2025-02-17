#include "power_manager.h"

void
auth_check_callback(GObject *source_object,
                   GAsyncResult *res,
                   gpointer user_data)
{
    t_AuthCallbackData *callback_data = (t_AuthCallbackData *)user_data;
    PolkitAuthority *authority = POLKIT_AUTHORITY(source_object);
    GError *error = NULL;
    PolkitAuthorizationResult *result;
    
    result = polkit_authority_check_authorization_finish(authority, res, &error);
    
    if (error != NULL) {
        g_error("Error checking authorization: %s", error->message);
        callback_data->result->success = FALSE;
        goto cleanup;
    }

    if (polkit_authorization_result_get_is_authorized(result)) {
        // Execute the privileged operation and store result
        printf("polkit authorized. Doing operation %d\n", callback_data->operation);
        switch (callback_data->operation) {
            case ACPI_OP_GET_RC_STATE: {
                // Original is_rc_on logic
                printf("Trying to read is rc on?\n");
                int fd = open("/proc/acpi/call", O_RDWR);
                if (fd == -1)
                {
                  perror("error opening /proc/acpi/call prout");
                  exit(EXIT_FAILURE);
                } else {
                  printf("Success opened fd is %d\n", fd);
                  exit(EXIT_SUCCESS);
                }
                const char *acpi_mode = query_acpi_info("\\_SB.PCI0.LPC0.EC0.QCHO");
                if (!acpi_mode) {
                    callback_data->result->success = FALSE;
                } else {
                    callback_data->result->success = TRUE;
                    callback_data->result->data.bool_result = strncmp(acpi_mode, "0x0", 4) != 0;
                    free((char *)acpi_mode);
                }
                break;
            }
            case ACPI_OP_GET_BC_STATE: {
                // Original is_bc_on logic
                const char *acpi_mode = query_acpi_info("\\_SB.PCI0.LPC0.EC0.QCHO");
                if (!acpi_mode) {
                    callback_data->result->success = FALSE;
                } else {
                    callback_data->result->success = TRUE;
                    callback_data->result->data.bool_result = strncmp(acpi_mode, "0x0", 4) != 0;
                    free((char *)acpi_mode);
                }
                break;
            default:
              callback_data->result->success = FALSE;
              return;
            }
        }
    } else {
        callback_data->result->success = FALSE;
    }

cleanup:
    // Call the completion callback if one was provided
    if (callback_data->completion_callback) {
        callback_data->completion_callback(callback_data->result, 
                                         callback_data->completion_data);
    }

    g_object_unref(result);
    g_free(callback_data);
}

// Function to initiate a privileged query
void
query_privileged_acpi(t_AppData *app_data,
                     e_AcpiOperation operation,
                     t_AcpiQueryResult *result,
                     void (*completion_callback)(t_AcpiQueryResult*, gpointer),
                     gpointer completion_data)
{
    t_AuthCallbackData *callback_data = g_new0(t_AuthCallbackData, 1);
    callback_data->operation = operation;
    callback_data->app_data = app_data;
    callback_data->result = result;
    callback_data->completion_callback = completion_callback;
    callback_data->completion_data = completion_data;

    PolkitSubject *subject = polkit_unix_process_new_for_owner(getpid(), 0, -1);
    
    polkit_authority_check_authorization(app_data->authority,
                                       subject,
                                       "com.mkps.powermanager.admin",
                                       NULL,
                                       POLKIT_CHECK_AUTHORIZATION_FLAGS_ALLOW_USER_INTERACTION,
                                       NULL,
                                       auth_check_callback,
                                       callback_data);
    g_object_unref(subject);
}

