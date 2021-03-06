/*
 * input_data_generator.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 24 june 2002
 */

#ifndef _INPUT_DATA_GENERATOR_H_
#define _INPUT_DATA_GENERATOR_H_

int generate_input_data(pcg64f_random_t *, int, void *, int);
int generate_input_data2(pcg64f_random_t *, int, void *, int, int);

#endif /* _INPUT_DATA_GENERATOR_H_ */
