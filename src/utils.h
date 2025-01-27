
// Needs to be a time zone string from: https://raw.githubusercontent.com/nayarsystems/posix_tz_db/master/zones.csv
time_t getEpoch() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec;
}

// Display local time
static void showLocalTime(const char* timeSrc) {
  time_t currEpoch = getEpoch();
  char timeFormat[20];
  strftime(timeFormat, sizeof(timeFormat), "%d/%m/%Y %H:%M:%S", localtime(&currEpoch));
  LOG_I("Got current time from %s: %s \n", timeSrc, timeFormat);
  timeSynchronized = true;
}
// Get mac id
String getChipID(){
  String mac = WiFi.macAddress();
  mac.replace(":","");
  mac.toLowerCase();
  return mac; //.substring(12);
}

// Convert a string to time_t
time_t convertDateTimeString(String sDateTime){
  tmElements_t tm;
  int Year, Month, Day, Hour, Minute, Second;
  sscanf(sDateTime.c_str(), "%d-%d-%d %d:%d:%d", &Year, &Month, &Day, &Hour, &Minute, &Second);
  tm.Year = CalendarYrToTm(Year);
  tm.Month = Month;
  tm.Day = Day;
  tm.Hour = Hour;
  tm.Minute = Minute;
  tm.Second = Second;
  return makeTime(tm);
}

// Get a string of current date/time
String getCurDateTimeString(bool isFolder = false, bool isFile=false) {
  // construct timestamp from date/time
  if(!timeSynchronized) return "";
  size_t buff_Len = 64;
  char buff[buff_Len];
  time_t currEpoch = getEpoch();
  if(isFolder) strftime(buff, buff_Len, "/%Y-%m/", localtime(&currEpoch));
  else if (isFile) strftime(buff, buff_Len, "%Y-%m-%d", localtime(&currEpoch));
  else strftime(buff, buff_Len, "%Y-%m-%d %H:%M:%S", localtime(&currEpoch));
  return String(buff);
}

// Get an advice on salt
String getSaltAdvice(uint32_t salt){
  String advice;
  if (salt < 201)        advice = "needed";
  else if (salt < 251)   advice = "low";
  else if (salt < 351)   advice = "optimal";
  else if (salt > 350)   advice = "too high";
  return advice;
}

float truncateFloat(float v, int prec=1){  return atof(String(v, prec).c_str());   }

bool isNumeric(String s){ //1.0, -.232, .233, -32.32
  unsigned int l = s.length();
  if(l==0) return false;
  bool dec=false, sign=false;
  for(unsigned int i = 0; i < l; ++i) {
    if (s.charAt(i) == '.'){
      if(dec) return false;
      else dec = true;
    }else if(s.charAt(i) == '+' || s.charAt(i) == '-' ){
      if(sign) return false;
      else sign = true;
    }else if (!isDigit(s.charAt(i))){
      LOG_I("%c\n", s.charAt(i));
      return false;
    }
  }
  return true;
}
bool isFloat(const std::string& str)
{
    if (str.empty())
        return false;

    char* ptr;
    strtof(str.c_str(), &ptr);
    return (*ptr) == '\0';
}
bool isInt(const std::string& str)
{
    if (str.empty())
        return false;

    char* ptr;
    strtol(str.c_str(), &ptr,10);
    return (*ptr) == '\0';
}

// Reset timer used for sleep
void ResetCountdownTimer(const char *reason, const unsigned long sleepInterval = SLEEP_DELAY_INTERVAL){
  //LOG_D("Reset countdown:  %s.\n",reason);
  sleepTimerCountdown = sleepInterval;
}

// Remove all ini files
void reset(){
  LOG_W("Removing all ini files.");
  //listDir("/", 1);
  lastBoot.deleteConfig(LAST_BOOT_INI);
  STORAGE.remove(LAST_BAT_INI);
  conf.deleteConfig();
}

// Read analog average value (delete min max)
uint32_t analogReadAvg(const uint8_t pin, const uint8_t samples = 8){
  uint32_t val = 0;
  uint16_t array[samples];
  for (int i = 0; i < samples; i++){
    array[i] = analogRead(pin);
    delay(2);
  }
  std::sort(array, array + samples);
  for (int i = 0; i < samples; i++){
    if (i == 0 || i == samples - 1)
      continue;
    val += array[i];
  }
  val /= samples - 2;
  return val;
}