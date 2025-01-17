// Project headers
#include <sbgCommon.h>
#include <swap/sbgSwap.h>

// Local headers
#include "sbgNetwork.h"

//----------------------------------------------------------------------//
//- IP manipulation methods                                            -//
//----------------------------------------------------------------------//

/*!
 * Convert an ip to a string of the form A.B.C.D
 * \param[in]	ipAddr						IP address to convert to a string.
 * \param[out]	pBuffer						Pointer on an allocated buffer than can hold ip address as a string.
 * \param[in]	maxSize						Maximum number of chars that can be stored in pBuffer including the NULL
 * char.
 */
SBG_COMMON_LIB_API void sbgNetworkIpToString(sbgIpAddress ipAddr, char *pBuffer, size_t maxSize)
{
    //
    // Check input arguments
    //
    assert(pBuffer);
    assert(maxSize >= 16);

    SBG_UNUSED_PARAMETER(maxSize);

    //
    // Write the IP address
    //
    snprintf(
        pBuffer, maxSize, "%" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8, sbgIpAddrGetA(ipAddr), sbgIpAddrGetB(ipAddr),
        sbgIpAddrGetC(ipAddr), sbgIpAddrGetD(ipAddr));
}

/*!
 * Convert an ip address stored in a string of the form A.B.C.D to an sbgIpAddress object.
 * \param[in]	pBuffer						IP address as a string of the form A.B.C.D
 * \return									IP address parsed from the string or 0.0.0.0 if the IP is invalid.
 */
SBG_COMMON_LIB_API sbgIpAddress sbgNetworkIpFromString(const char *pBuffer)
{
    int          ipAddrA;
    int          ipAddrB;
    int          ipAddrC;
    int          ipAddrD;
    int          numReadParams;
    sbgIpAddress ip;
    sbgIpAddress ret;
    char         checkBuffer[SBG_NETWORK_IPV4_STRING_SIZE];

    assert(pBuffer);

    ret = SBG_IPV4_UNSPECIFIED_ADDR;

    numReadParams = sscanf(pBuffer, "%d.%d.%d.%d", &ipAddrA, &ipAddrB, &ipAddrC, &ipAddrD);

    if ((numReadParams == 4) && (ipAddrA >= 0) && (ipAddrA <= 255) && (ipAddrB >= 0) && (ipAddrB <= 255) &&
        (ipAddrC >= 0) && (ipAddrC <= 255) && (ipAddrD >= 0) && (ipAddrD <= 255))
    {
        ip = sbgIpAddr((uint8_t)ipAddrA, (uint8_t)ipAddrB, (uint8_t)ipAddrC, (uint8_t)ipAddrD);

        sbgNetworkIpToString(ip, checkBuffer, SBG_ARRAY_SIZE(checkBuffer));

        if (strcmp(pBuffer, checkBuffer) == 0)
        {
            ret = ip;
        }
    }

    return ret;
}

//----------------------------------------------------------------------//
//- IP validation methods                                              -//
//----------------------------------------------------------------------//

/*!
 * Check if an IpV4 netmask is valid, the mask should be contiguous (1111 followed by 0)
 * \param[in]	netmask							The netmask stored in an uint32_t (host endianness).
 * \return										true if the netmask is valid ie contiguous.
 */
SBG_COMMON_LIB_API bool sbgIpNetMaskValid(sbgIpAddress netmask)
{
    uint32_t y;
    uint32_t z;

    //
    // First test that the netmask is not zero, if yes, it's valid
    //
    if (netmask != 0)
    {
        //
        // We are doing arithmetic so we have to make sure the netmask is using the platform endianness
        // The IP address is always stored in big endian so we have to swap it for little endian platforms
        //
#if SBG_CONFIG_BIG_ENDIAN == 0
        netmask = sbgSwap32(netmask);
#endif

        //
        // Compute the bitwise inverse
        //
        y = ~netmask;

        //
        // Add one to the inverse so if netmask was a proper one, there will be at most 1 bit set in this.
        //
        z = y + 1;

        //
        // Test that, simply and z with z - 1, which happens to be y. The result will be zero if all is OK, non zero
        // otherwise.
        //
        if ((z & y) == 0)
        {
            //
            // We have a valid netmask
            //
            return true;
        }
        else
        {
            return false;
        }
    }

    return true;
}
