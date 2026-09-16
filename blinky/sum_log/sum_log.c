/*
 * SPDX-FileCopyrightText: Copyright The Zephyr Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>

#include "sum_log.h"

LOG_MODULE_REGISTER(sum_log);

int sum_log(int first, int second)
{
	int inputs[2] = {first, second};
	int result = first + second;

	LOG_DBG("Debug level: received two integers");
	LOG_INF("Info level: %d + %d = %d", first, second, result);
	LOG_WRN("Warning level example: result is %d", result);
	LOG_ERR("Error level example: result is %d", result);
	LOG_HEXDUMP_INF(inputs, sizeof(inputs), "Input values");

	return result;
}
