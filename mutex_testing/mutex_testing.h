#ifndef __MUTEX_TESTING_H__
#define __MUTEX_TESTING_H__

typedef int mutex;

// lock a global/shared mutex located at lock
__device__ int mutex_lock(void);

// unlock a global/shared mutex located at lock, returns nonzero on fail.
__device__ int mutex_unlock(void);

__device__ int lock(mutex *lock);

__device__ int unlock(mutex *unlock);

// this initializes the mutex to be unlocked. this must be a separate call from the cpu to ensure 
// no race conditions from initialization and first lock of other threads.
__global__ void mutex_init(mutex *lock);


#endif //__MUTEX_TESTING_H__
