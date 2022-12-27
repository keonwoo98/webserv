// Created by 김민준 on 2022/12/04.

/**
 * e.g. Sun, 06 Nov 1994 08:49:37 GMT ; IMF-fixdate
 *
 * IMF-fixdate = day-name "," SP date1 SP time-of-day SP GMT
 * ; fixed length/zone/capitalization subset of the format
 * ; see Section 3.3 of [RFC5322]
 *
 * day-name = %s"Mon" / %s"Tue" / %s"Wed"
 * 		 		%s"Thu" / %s"Fri" / %s"Sat" / %s"Sun"
 *
 * date1 = day SP month SP year
 * 			; e.g. 02 Jun 1982
 *
 * day = 2DIGIT
 * month = %s"Jan" / %s"Feb" / %s"Mar" / %s"Apr" / %s"May" /
 * 			%s"Jun" / %s"Jul" / %s"Aug" / %s"Sep" / %s"Oct" /
 * 			%s"Nov" / %s"Dec"
 * year = 4DIGIT
 *
 * time-of-day = hour ":" minute ":" second
 * 				; 00:00:00 - 23:59:60 (leap second)
 *
 * hour = 2DIGIT
 * minute = 2DIGIT
 * second = 2DIGIT
 *
 * GMT = %s"GMT"
 *
 * HTTP-date is case-sensitive.
 *
 * origin server with a clock MUST generate a Date header field in all 2xx, 3xx, 4xx responses,
 * and MAY generate a Date header field in 1xx and 5xx responses.
 *
 * An origin server without a clock MUST NOT generate a Date header field.
 */

#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "date.hpp"

enum Day {
  Sun, Mon, Tue, Wed, Thu, Fri, Sat
};

enum Month {
  Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
};

const char *GetDay(int day) {
	switch (day) {
		case Sun: return "Sun";
		case Mon: return "Mon";
		case Tue: return "Tue";
		case Wed: return "Wed";
		case Thu: return "Thu";
		case Fri: return "Fri";
		case Sat: return "Sat";
		default: return "";
	}
}

const char *GetMonth(int month) {
	switch (month) {
		case Jan: return "Jan";
		case Feb: return "Feb";
		case Mar: return "Mar";
		case Apr: return "Apr";
		case May: return "May";
		case Jun: return "Jun";
		case Jul: return "Jul";
		case Aug: return "Aug";
		case Sep: return "Sep";
		case Oct: return "Oct";
		case Nov: return "Nov";
		case Dec: return "Dec";
		default: return "";
	}
}

std::string GetTime(time_t raw_time) {
	struct tm *time_info = std::gmtime(&raw_time);
	std::stringstream ss;
	ss	<< GetDay(time_info->tm_wday)
		<< ", " << std::setw(2) << std::setfill('0') << time_info->tm_mday
		<< " " << GetMonth(time_info->tm_mon)
		<< " " << 1900 + time_info->tm_year
		<< " " << std::setw(2) << std::setfill('0') << time_info->tm_hour
		<< ":" << std::setw(2) << std::setfill('0') << time_info->tm_min
		<< ":" << std::setw(2) << std::setfill('0') << time_info->tm_sec
		<< " GMT";
	return ss.str();
}
