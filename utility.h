#ifndef _UTILITY_H_
#define _UTILITY_H_
/// Error codes for Thingspeak ervice

String msgFeedBack(int httpCode) { /// diagnostica: codici d'errore ThingSpeak

  switch (httpCode) {
    case  200: return String("(200) OK / Success");
    case  400: return String("(400) The request cannot be fulfilled due to bad syntax");
    case  404: return String("(404) Incorrect API key (or invalid ThingSpeak server address)");
    case -101: return String("(-101) Value is out of range or string is too long (> 255 characters)");
    case -201: return String("(-201) Invalid field number specified");
    case -210: return String("(-210) setField() was not called before writeFields()");
    case -301: return String("(-301) Failed to connect to ThingSpeak");
    case -302: return String("(-302) Unexpected failure during write to ThingSpeak");
    case -303: return String("(-303) Unable to parse response");
    case -304: return String("(-304) Server Response Timeout");
    case -401: return String("(-401) Point was not inserted (most probable cause is the rate limit of once every 15 seconds)");
    case    0: return String("(0) Generic error!");
    default  : return String("("+String(httpCode)+") Uncoded error!");
  }
}

#endif
