# include "WiFi.h"

int connect(const char* ssid ,const char* password){
	WiFi.begin(ssid, password);
	//Serial.print("Connecting to ");
 	//Serial.print(ssid); Serial.println(" ...");

  	int i = 0;
  	while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    	delay(1000);
    //	Serial.print(++i); Serial.print(' ');
  	}

	//Serial.println('\n');
	//Serial.println("Connection established!");  
	//Serial.print("IP address:\t");
	//Serial.println(WiFi.localIP());
	return 1 ;
}
