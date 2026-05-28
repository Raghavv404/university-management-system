#ifndef UNIVERSITY_MANAGEMENT_SYSTEM_COUNTRY_H
#define UNIVERSITY_MANAGEMENT_SYSTEM_COUNTRY_H
#pragma once

// Country is used by Person (nationality), Building (location) and Faculty (location).
// Add new entries at the end so existing static_cast<Country>(N) values stay stable.
enum Country {
    POLAND,
    GERMANY,
    FRANCE,
    SPAIN,
    ITALY,
    UNITED_KINGDOM,
    USA,
    INDIA,
    CHINA,
    OTHER_COUNTRY
};

#endif
