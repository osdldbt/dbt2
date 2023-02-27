/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#ifndef _INPUT_DATA_GENERATOR_H_
#define _INPUT_DATA_GENERATOR_H_

int generate_input_data(pcg64f_random_t *, int, void *, int);
int generate_input_data2(pcg64f_random_t *, int, void *, int, int);

#endif /* _INPUT_DATA_GENERATOR_H_ */
