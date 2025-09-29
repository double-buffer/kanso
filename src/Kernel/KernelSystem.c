#include "Platform.h"
#include "System.h"


SystemInformation SystemGetInformation()
{
    auto platformInformation = PlatformGetInformation();
    return platformInformation.SystemInformation;
}
