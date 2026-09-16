/*
 * SPDX-FileCopyrightText: Copyright The Zephyr Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#include "sum_printk.h"

int sum_printk(int first, int second)
{
	int result = first + second;

	printk("printk sum: %d + %d = %d\n", first, second, result);

	return result;
}
