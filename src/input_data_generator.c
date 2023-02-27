/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 *
 * 24 june 2002
 * Based on TPC-C Standard Specification Revision 5.0.
 */

#include <string.h>
#include <strings.h>

#include "common.h"
#include "transaction_data.h"

int generate_delivery_data(pcg64f_random_t *, int, struct delivery_t *);
int generate_new_order_data(pcg64f_random_t *, int, struct new_order_t *);
int generate_order_status_data(pcg64f_random_t *, int, struct order_status_t *);
int generate_payment_data(pcg64f_random_t *, int, struct payment_t *);
int generate_stock_level_data(pcg64f_random_t *, int, int, struct stock_level_t *);
int generate_integrity_data(int w_id, struct integrity_t *data);

/* Does it make sense to include driver.h for this? */
extern int mode_altered;

/* This function generates data for all transactions except Stock-Level. */
int generate_input_data(pcg64f_random_t *rng, int type, void *data, int w_id)
{
	switch (type) {
	case DELIVERY:
		generate_delivery_data(rng, w_id, (struct delivery_t *) data);
		break;
	case NEW_ORDER:
		generate_new_order_data(rng, w_id, (struct new_order_t *) data);
		break;
	case ORDER_STATUS:
		generate_order_status_data(rng, w_id, (struct order_status_t *) data);
		break;
	case PAYMENT:
		generate_payment_data(rng, w_id, (struct payment_t *) data);
		break;
        case INTEGRITY:
		generate_integrity_data(w_id, (struct integrity_t *) data);
	default:
		return ERROR;
	}
	return OK;
}

/* This function generates data only for the Stock-Level transaction. */
int generate_input_data2(pcg64f_random_t *rng, int type, void *data, int w_id,
		int d_id)
{
	generate_stock_level_data(rng, w_id, d_id, (struct stock_level_t *) data);
	return OK;
}

/* Clause 2.7.1 */
int generate_delivery_data(pcg64f_random_t *rng, int w_id,
		struct delivery_t *data)
{
	memset(data, 0, sizeof(struct delivery_t));
	data->w_id = w_id;
	data->o_carrier_id = (int) get_random(rng, O_CARRIER_ID_MAX) + 1;

	return OK;
}

/* Clause 2.4.1 */
int generate_new_order_data(pcg64f_random_t *rng, int w_id, struct new_order_t *data)
{
	int i;

	memset(data, 0, sizeof(struct new_order_t));
	data->w_id = w_id;
	data->d_id = get_random(rng, D_ID_MAX) + 1;
	data->c_id = get_nurand(rng, 1023, 1, 3000);
	data->o_ol_cnt = (int) get_random(rng, 10) + 6;
	for (i = 0; i < data->o_ol_cnt; i++) {
		data->order_line[i].ol_i_id = get_nurand(rng, 8191, 1, 100000);
		if (table_cardinality.warehouses > 1) {
			if (mode_altered == 1 || get_random(rng, 100) > 0) {
				data->order_line[i].ol_supply_w_id = w_id;
			} else {
				data->order_line[i].ol_supply_w_id =
						get_random(rng, table_cardinality.warehouses - 1) + 1;
				if (data->order_line[i].ol_supply_w_id >= w_id)
					data->order_line[i].ol_supply_w_id =
							(data->order_line[i].ol_supply_w_id + 1) %
							table_cardinality.warehouses;
			}
		} else {
			data->order_line[i].ol_supply_w_id = 1;
		}
		data->order_line[i].ol_quantity = (int) get_random(rng, 10) + 1;
	}

	/* Use an invalid i_id 1% of the time. */
	if (get_random(rng, 100) == 0) {
		data->order_line[data->o_ol_cnt - 1].ol_i_id = 0;
	}

	return OK;
}

/* Clause 2.6.1 */
int generate_order_status_data(pcg64f_random_t *rng, int w_id,
		struct order_status_t *data)
{
	memset(data, 0, sizeof(struct order_status_t));
	data->c_w_id = w_id;
	data->c_d_id = (int) get_random(rng, D_ID_MAX) + 1;

	/* Select a customer by last name 60%, byt c_id 40% of the time. */
	if (get_random(rng, 100) < 60) {
		data->c_id = C_ID_UNKNOWN;
		get_c_last(data->c_last, get_nurand(rng, 255, 0, 999));
	} else {
		data->c_id = get_nurand(rng, 1023, 1, 3000);
	}

	return OK;
}

/* Clause 2.5.1 */
int generate_payment_data(pcg64f_random_t *rng, int w_id,
		struct payment_t *data)
{
	memset(data, 0, sizeof(struct payment_t));
	data->w_id = w_id;
	data->d_id = (int) get_random(rng, D_ID_MAX) + 1;

	/* Select a customer by last name 60%, by c_id 40% of the time. */
	if (get_random(rng, 100) < 60) {
		data->c_id = C_ID_UNKNOWN;
		get_c_last(data->c_last, get_nurand(rng, 255, 0, 999));
	} else {
		data->c_id = get_nurand(rng, 1023, 1, 3000);
	}

	if (mode_altered == 1 || get_random(rng, 100) < 85) {
		data->c_w_id = w_id;
		data->c_d_id = data->d_id;
	} else {
		data->c_d_id = get_random(rng, D_ID_MAX) + 1;
		if (table_cardinality.warehouses > 1) {
			/*
			 * Select a random warehouse that is not the same
			 * as this user's home warehouse by shifting the
			 * numbers slightly.
			 */
			data->c_w_id = (int) get_random(rng,
					table_cardinality.warehouses - 1) + 1;
			if (data->c_w_id >= w_id) {
				data->c_w_id = (data->c_w_id + 1) %
						table_cardinality.warehouses;
			}
			if (!data->c_w_id)
			{
			  data->c_w_id = 1;
			}
		} else {
			data->c_w_id = 1;
		}
	}
	data->h_amount = (double) (get_random(rng, 500000) + 101) / 100.0;

	return OK;
}

/* Clause 2.8.1 */
int generate_stock_level_data(pcg64f_random_t *rng, int w_id, int d_id,
		struct stock_level_t *data)
{
	memset(data, 0, sizeof(struct stock_level_t));
	data->w_id = w_id;
	data->d_id = d_id;
	data->threshold = (int) get_random(rng, 11) + 10;

	return OK;
}

int generate_integrity_data(int w_id, struct integrity_t *data)
{
        memset(data, 0, sizeof(struct integrity_t));
        data->w_id = w_id;

        return OK;
}
