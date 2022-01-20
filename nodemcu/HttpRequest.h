
#ifndef HTTPREQUEST_H   /* Include guard */
#define HTTPREQUEST_H


#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


String httpGETRequest(const char* serverName);


#endif // HTTPREQUEST_H