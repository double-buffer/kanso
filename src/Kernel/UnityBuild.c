#include "../Common/UnityBuild.c"

#include "KernelConsole.c"

#ifdef BUILD_TESTS
#include "KernelTestHost.c"
#include "../../tests/Common/UnityBuild.c"
#else
#include "KernelMain.c"
#endif

