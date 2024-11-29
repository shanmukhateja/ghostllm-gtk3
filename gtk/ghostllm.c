#include "config.h"

#include <gtk/gtk.h>

#include <gio/gio.h>

char *ghostllm_rewrite_text(char *input_str) {
  // Connect to DBus Session Bus
  GDBusConnection *conn = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, NULL);

  // Define the remote service and object path
  char *service_name = "in.suryatejak.ghostllm";
  char *object_path = "/in/suryatejak/ghostllm";

  // This is required for communicating with DBus.
  GDBusMessage *call_message = NULL;
  GDBusMessage *reply_message = NULL;
  GError **error = NULL;

  // Define the DBus message to be sent.
  call_message = g_dbus_message_new_method_call(service_name, object_path,
                                                service_name, "RewriteText");

  // Set arguments
  g_dbus_message_set_body(call_message, g_variant_new("(s)", input_str));

  // Call the RewriteText method on the remote service
  reply_message = g_dbus_connection_send_message_with_reply_sync(
      conn, call_message, G_DBUS_SEND_MESSAGE_FLAGS_NONE, -1, NULL, NULL,
      error);

  // Retrieve the response data from DBus
  GDBusMessageType reply_type = g_dbus_message_get_message_type(reply_message);

  if (reply_type == G_DBUS_MESSAGE_TYPE_ERROR) {
    // TODO: show error to user
    g_dbus_message_to_gerror(reply_message, error);
    return NULL;
  }

  if (reply_type == G_DBUS_MESSAGE_TYPE_METHOD_RETURN) {
    // We got something!
    GVariant *response = g_dbus_message_get_body(reply_message);

    /**
     * We receive a tuple which contains required string.
     * Now, we fetch the first item and extract the string.
     */
    gsize index = {0};
    response = g_variant_get_child_value(response, index);

    if (response == NULL) {
      return NULL;
    }

    char *msg = g_variant_get_string(response, NULL);

    return msg;
  }

  return NULL;
}
