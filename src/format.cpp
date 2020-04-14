#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    constexpr long secondsInMinute = 60;
    constexpr long secondsInHour = secondsInMinute * 60;

    const long hours = seconds/secondsInHour;
    seconds = seconds % secondsInHour;
    const long minutes = seconds/secondsInMinute;
    seconds = seconds % secondsInMinute;

    return std::to_string(hours).append(":")
    .append(std::to_string(minutes)).append(":")
    .append(std::to_string(seconds));
}