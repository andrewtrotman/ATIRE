// omp_sections.cpp
// compile with: /openmp 
#include <stdio.h>
#include <omp.h>

int main()
{
#pragma omp parallel sections num_threads(4)
	{
	#pragma omp section
	printf_s("Hello from thread %d\n", omp_get_thread_num());
	#pragma omp section
	printf_s("Hello from thread %d\n", omp_get_thread_num());
	#pragma omp section
	printf_s("Hello from thread %d\n", omp_get_thread_num());
	#pragma omp section
	printf_s("Hello from thread %d\n", omp_get_thread_num());
	}
printf_s("End from thread %d\n", omp_get_thread_num());
}
