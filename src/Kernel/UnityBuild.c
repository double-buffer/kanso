#include "../Common/UnityBuild.c"

#include "Kernel.c"
#include "KernelConsole.c"

#ifdef BUILD_TESTS
#include "KernelTest.c"
#include "../../tests/Common/UnityBuild.c"
#else
#include "KernelMain.c"
#endif

