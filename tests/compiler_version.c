#include <iso646.h> // detect std::lib
#include <stdio.h>

int main(void)
{
#ifdef __APPLE__
	puts("Apple");
#endif

#ifdef _WIN63
	puts("Windows 64 bit");
#elif defined(_WIN32)
	puts("Windows 32 bit");
#endif

#ifdef __linux__
	puts("Linux");
#endif

#ifdef __GNUC__
	printf("__GNUC__:%d\n", __GNUC__);
	printf("__GNUC_MINOR__:%d\n", __GNUC_MINOR__);
	printf("__GNUC_PATCHLEVEL__:%d\n", __GNUC_PATCHLEVEL__);
#endif

#ifdef _MSC_VER
	printf("_MSV_VER:%d\n", _MSC_VER);
#endif

#ifdef __clang__
	printf("__clang_major__     :%d\n", __clang_major__);
	printf("__clang_minor__     :%d\n", __clang_minor__);
	printf("__clang_patchlevel__:%d\n", __clang_patchlevel__);
	printf("__clang_version__   :\"%s\"\n", __clang_version__);
#endif

#ifdef _LIBCPP_VERSION 
	printf("_LIBCPP_VERSION:%d\n", _LIBCPP_VERSION);
#endif

#ifdef __cplusplus
	printf("__cplusplus:%ld\n", __cplusplus);
#endif

return 0;
}