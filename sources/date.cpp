// Created by 김민준 on 2022/12/04.

/**
 * e.g. Sun, 06 Nov 1994 08:49:37 GMT ; IMF-fixdate <br>
 * <p>
 * IMF-fixdate = day-name "," SP date1 SP time-of-day SP GMT <br>
 * ; fixed length/zone/capitalization subset of the format <br>
 * ; see Section 3.3 of [RFC5322] <br>
 * <br>
 * day-name = %s"Mon" / %s"Tue" / %s"Wed"
 * 		 		%s"Thu" / %s"Fri" / %s"Sat" / %s"Sun" <br>
 * <br>
 * date1 = day SP month SP year <br>
 * 			; e.g. 02 Jun 1982 <br>
 * <br>
 * day = 2DIGIT <br>
 * month = %s"Jan" / %s"Feb" / %s"Mar" / %s"Apr" / %s"May" /
 * 			%s"Jun" / %s"Jul" / %s"Aug" / %s"Sep" / %s"Oct" /
 * 			%s"Nov" / %s"Dec" <br>
 * year = 4DIGIT <br>
 * <br>
 * time-of-day = hour ":" minute ":" second <br>
 * 				; 00:00:00 - 23:59:60 (leap second) <br>
 * <br>
 * hour = 2DIGIT <br>
 * minute = 2DIGIT <br>
 * second = 2DIGIT <br>
 * <br>
 * GMT = %s"GMT" <br>
 * </p>
 * HTTP-date is case-sensitive. <br>
 * <br>
 * origin server with a clock MUST generate a Date header field in all 2xx, 3xx, 4xx responses,
 * and MAY generate a Date header field in 1xx and 5xx responses. <br>
 * <br>
 * An origin server without a clock MUST NOT generate a Date header field.
 */

#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>

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

std::string GetTime() {
	time_t raw_time = std::time(NULL);
	struct tm *time_info = std::gmtime(&raw_time);

//	std::cout << "raw time : " << raw_time << std::endl;
//	std::cout << "INF-fixdate : ";
//	std::printf("%s, %02d %s %d %02d:%02d:%02d GMT\n",
//				GetDay(time_info->tm_wday), time_info->tm_mday, GetMonth(time_info->tm_mon),
//				1900 + time_info->tm_year, time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
	std::stringstream ss;
	ss	<< GetDay(time_info->tm_wday)
		<< ", " << std::setw(2) << std::setfill('0') << time_info->tm_mday
		<< " " << GetMonth(time_info->tm_mon)
		<< " " << 1900 + time_info->tm_year
		<< " " << std::setw(2) << std::setfill('0') << time_info->tm_hour
		<< ":" << std::setw(2) << std::setfill('0') << time_info->tm_min
		<< ":" << std::setw(2) << std::setfill('0') << time_info->tm_sec
		<< " GMT" << std::endl;
//	std::cout << ss.str() << std::endl;
	return ss.str();
}