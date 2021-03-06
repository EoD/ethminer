#ifndef _ETHASH_CUDA_MINER_KERNEL_H_
#define _ETHASH_CUDA_MINER_KERNEL_H_

#include <stdexcept>
#include <stdio.h>
#include <stdint.h>
#include <cuda_runtime.h>

// It is virtually impossible to get more than
// one solution per stream hash calculation
// Leave room for up to 4 results. A power
// of 2 here will yield better CUDA optimization
#define SEARCH_RESULTS 4

typedef struct {
	uint32_t count;
	struct {
		// One word for gid and 8 for mix hash
		uint32_t gid;
		uint32_t mix[8];
		uint32_t pad[7]; // pad to size power of 2
	} result[SEARCH_RESULTS];
} search_results;

#define ACCESSES 64
#define THREADS_PER_HASH (128 / 16)

typedef struct
{
	uint4 uint4s[32 / sizeof(uint4)];
} hash32_t;

typedef struct
{
	uint4	 uint4s[128 / sizeof(uint4)];
} hash128_t;

typedef union {
	uint32_t words[64 / sizeof(uint32_t)];
	uint2	 uint2s[64 / sizeof(uint2)];
	uint4	 uint4s[64 / sizeof(uint4)];
} hash64_t;

typedef union {
	uint32_t words[200 / sizeof(uint32_t)];
	uint2	 uint2s[200 / sizeof(uint2)];
	uint4	 uint4s[200 / sizeof(uint4)];
} hash200_t;

void set_constants(
	hash128_t* _dag,
	uint32_t _dag_size,
	hash64_t * _light,
	uint32_t _light_size
	);

void set_header(
	hash32_t _header
	);

void set_target(
	uint64_t _target
	);

void run_ethash_search(
	uint32_t search_batch_size,
	uint32_t workgroup_size,
	cudaStream_t stream,
	volatile search_results* g_output,
	uint64_t start_nonce,
	uint32_t parallelHash
	);

void ethash_generate_dag(
	uint64_t dag_size,
	uint32_t blocks,
	uint32_t threads,
	cudaStream_t stream,
	int device
	);

#define CUDA_SAFE_CALL(call)						\
do {									\
	cudaError_t err = call;						\
	if (cudaSuccess != err) {					\
		const char * errorString = cudaGetErrorString(err);	\
		fprintf(stderr,						\
			"CUDA error in func '%s' at line %i : %s.\n",	\
			__FUNCTION__, __LINE__, errorString);		\
		throw std::runtime_error(errorString);			\
	}								\
} while (0)

#endif
