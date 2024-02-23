// Copyright (c) 2020 Cesanta Software Limited
// All rights reserved

#include <signal.h>
#include "mongoose.h"

static int s_debug_level = MG_LL_VERBOSE;
static const char *s_listening_address = "http://0.0.0.0:8000";

int main() {
	mg_log_set(s_debug_level);
	mg_hello(s_listening_address);
	return 0;
}
