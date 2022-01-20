#ifndef UTILITIES_H  /* Include guard */
#define UTILITIES_H
#include "HttpRequest.h"
#include <Arduino_JSON.h>



int get_time(String* region , int *utc_offset,int* _year , int* day_of_year);
String get_wheather(String region , String* lon , String* lat);
int get_sunrise_sunset(String lon , String lat , int*sunrise , int*sunset , int utc_offset );

#endif // UTILITIES_H
