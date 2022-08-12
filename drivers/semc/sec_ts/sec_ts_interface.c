/*
 * sec_ts_interface.c - samsung ts driver custom interface
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include "include/sec_ts.h"
#include "linux/device.h"

int sec_ts_lpmode_enable(void) {
	int rc = 0;
	struct sec_ts_data *ts;

	ts = get_sec_ts_data();
	rc = sec_ts_set_lowpowermode(ts, TO_LOWPOWER_MODE);
	ts->lowpower_mode = TO_LOWPOWER_MODE;
	input_info(true, &ts->client->dev,
			"Setting lowpower mode by external interface: %d\n", rc);

	return rc;
}
EXPORT_SYMBOL(sec_ts_lpmode_enable);

int sec_ts_lpmode_disable(void) {
	int rc = 0;
	struct sec_ts_data *ts;

	ts = get_sec_ts_data();
	rc = sec_ts_set_lowpowermode(ts, TO_TOUCH_MODE);
	ts->lowpower_mode = TO_TOUCH_MODE;
	input_info(true, &ts->client->dev,
			"Setting lowpower mode by external interface: %d\n", rc);

	return rc;
}
EXPORT_SYMBOL(sec_ts_lpmode_disable);
