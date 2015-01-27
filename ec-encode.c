/*
 * Mini Object Storage, (C) 2014 Minio, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdio.h>

#include "ec-code.h"
#include "ec-common.h"

int32_t minio_init_encoder (int technique, int k, int m,
                            uint8_t **encode_matrix,
                            uint8_t **encode_tbls)
{
        size_t encode_matrix_size;
        size_t encode_tbls_size;
        uint8_t *tmp_matrix;
        uint8_t *tmp_tbls;

        tmp_matrix = (uint8_t *) malloc (k * (k + m));
        tmp_tbls = (uint8_t *) malloc (k * (k + m) * 32);

	if (technique == 0) {
                /*
                  Commonly used method for choosing coefficients in erasure
                  encoding but does not guarantee invertable for every sub
                  matrix.  For large k it is possible to find cases where the
                  decode matrix chosen from sources and parity not in erasure
                  are not invertable. Users may want to adjust for k > 5.
                  -- Intel
                */
		gf_gen_rs_matrix (tmp_matrix, k + m, k);
	} else if (technique == 1) {
		gf_gen_cauchy1_matrix (tmp_matrix, k + m, k);
        }

	ec_init_tables(k, m, &tmp_matrix[k * k], tmp_tbls);

        *encode_matrix = tmp_matrix;
        *encode_tbls = tmp_tbls;

        return 0;
}

uint32_t minio_calc_chunk_size (int k, uint32_t split_len)
{
        int alignment;
        int remainder;
        int padded_len;

        alignment = k * SIMD_ALIGN;
        remainder = split_len % alignment;

        padded_len = split_len;
        if (remainder) {
                padded_len = split_len + (alignment - remainder);
        }
        return padded_len / k;
}
