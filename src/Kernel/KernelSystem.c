#include "Kanso.h"
#include "Platform.h"

SystemInformation SystemGetInformation()
{
    auto platformInformation = PlatformGetInformation();
    return platformInformation.SystemInformation;
}
