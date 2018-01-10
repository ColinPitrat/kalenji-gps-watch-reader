#include "OnMove200.h"
#include <cstring>
#include <iomanip>
#include <set>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
//#define _BSD_SOURCE // already declared
#include <endian.h>


#include <dirent.h>
#include <sys/stat.h>

#ifdef DEBUG
#define DEBUG_CMD(x) x;
#else
#define DEBUG_CMD(x) ;
#endif

namespace device
{
  REGISTER_DEVICE(OnMove200);

  int OnMove200::bytesToInt2(unsigned char b0, unsigned char b1)
  {
    int Int = b0 | ( (int)b1 << 8 );
    return Int;
  }

  int OnMove200::bytesToInt4(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3)
  {
    int Int = b0 | ( (int)b1 << 8 ) | ( (int)b2 << 16 ) | ( (int)b3 << 24 );
    return Int;
  }

  unsigned char* OnMove200::readAllBytes(const std::string& filename, size_t& size)
  {
    std::ifstream fl(filename.c_str());
    fl.seekg( 0, std::ios::end );
    size_t len = fl.tellg();
    auto buffer = new char[len];
    fl.seekg(0, std::ios::beg);
    fl.read(buffer, len);
    fl.close();
    size = len;
    return (unsigned char*)buffer;
  }

  bool OnMove200::fileExists(const std::string& filename)
  {
    struct stat fileInfo;
    return stat(filename.c_str(), &fileInfo) == 0;
  }

  void OnMove200::dump(unsigned char *data, int length)
  {
    DEBUG_CMD(std::cout << std::hex);
    for(int i = 0; i < length; ++i)
    {
      DEBUG_CMD(std::cout << std::setw(2) << std::setfill('0') << (int) data[i] << " ");
    }
    DEBUG_CMD(std::cout << std::endl);
  }

  void OnMove200::init(const DeviceId& deviceId)
  {
    //check if getPath() is a valid path
    DIR* folder = opendir(getPath().c_str());
    if (folder == nullptr)
    {
      std::cout<< "Error: path '" << getPath() << "' does not exist (check option -p <path> on command line or line path=<path> in configuration file)." << std::endl;
      throw std::exception();
    }
    closedir(folder);
  }

  void OnMove200::release()
  {
    DEBUG_CMD(std::cout << "OnMove200: Release (nothing to do)" << std::endl);
  }

  std::string OnMove200::getPath()
  {
    return _configuration["path"];
  }

  void OnMove200::getSessionsList(SessionsMap *oSessions)
  {
    DEBUG_CMD(std::cout << "OnMove200: Get sessions list !" << std::endl);

    DIR* folder = nullptr;
    struct dirent* file = nullptr;
    folder = opendir(getPath().c_str());
    if (folder == nullptr)
    {
      std::cerr << "Couldn't open dir " << getPath() << std::endl;
      throw std::exception();
    }

    std::set<std::string> filenamesPrefix;

    while ((file = readdir(folder)) != nullptr)
    {
      std::string fn = std::string(file->d_name);
      if(strstr(file->d_name,".OMD") || strstr(file->d_name,".OMH"))
      {
        std::string fileprefix = fn.substr(0,fn.length() - 4);
        std::string filepathprefix = getPath() + std::string("/") + fileprefix;
        //check if both files (header and data) exists
        if(fileExists(filepathprefix + ".OMD") && fileExists(filepathprefix + ".OMH"))
        {
          filenamesPrefix.insert(fileprefix);
        }
        else
        {
          std::cout << "Discarding " << fileprefix << std::endl;
        }
      }
    }

    closedir(folder);

    int i = 0;
    std::set<std::string>::iterator iter;
    for (iter=filenamesPrefix.begin(); iter!=filenamesPrefix.end(); ++iter)
    {
      std::string fileprefix = *iter;

      DEBUG_CMD(std::cout << "Decode summary of session " << fileprefix << std::endl);
      // Decoding of basic info about the session
      std::vector<char> _sessionId(fileprefix.begin(), fileprefix.end());
      SessionId id = SessionId(_sessionId);
      uint32_t num = i++; //Just increment by one each time

      tm time;
      memset(&time, 0, sizeof(time));

      double duration = 0;
      uint32_t distance = 0;
      uint32_t nbLaps = 999;

      Session mySession(id, num, time, 0, duration, distance, nbLaps);

      // Properly fill necessary session info (duration, distance, nbLaps)
      std::string ghtFilename = getPath() + std::string("/") + fileprefix + std::string(".OMH");
      size_t size = -1;
      unsigned char* buffer = readAllBytes(ghtFilename, size);
      // TODO: if(size != 60) error !
      parseOMHFile(buffer, &mySession);
      delete buffer;

      oSessions->insert(SessionsMapElement(id, mySession));
    }
  }

  void OnMove200::getSessionsDetails(SessionsMap *oSessions)
  {
    for(auto& sessionPair : *oSessions)
    {
      Session* session = &(sessionPair.second);
      SessionId sessionId = sessionPair.second.getId();
      std::string filenamePrefix(sessionId.begin(),sessionId.end());
      std::cout << "Retrieve session " << filenamePrefix << std::endl;

      filenamePrefix = getPath() + std::string("/") + filenamePrefix;

      unsigned char* buffer;
      size_t size = -1;

      std::string omdFilename = filenamePrefix + std::string(".OMD");
      buffer = readAllBytes(omdFilename,size);
      parseOMDFile(buffer, size, session);
      delete buffer;
    }
  }

  void OnMove200::dumpInt2(std::ostream &oStream, unsigned int iInt)
  {
    oStream << (char)(iInt & 0xFF) << (char)((iInt & 0xFF00) >> 8);
  }

  void OnMove200::dumpInt4(std::ostream &oStream, unsigned int iInt)
  {
    oStream << (char)(iInt & 0xFF) << (char)((iInt & 0xFF00) >> 8) << (char)((iInt & 0xFF0000) >> 16) << (char)((iInt & 0xFF000000) >> 24);
  }

  void OnMove200::dumpString(std::ostream &oStream, const std::string &iString, size_t iLength)
  {
    size_t toCopy = iString.length();
    if(iLength <= toCopy) toCopy = iLength - 1;
    oStream.write(iString.c_str(), toCopy);
    for(size_t i = toCopy; i < iLength; ++i)
    {
      oStream.put('\0');
    }
  }

  void OnMove200::exportSession(const Session *iSession)
  {
    std::cerr << "Unsupported export session for OnMove200" << std::endl;
  }

  void OnMove200::parseOMHFile(const unsigned char* bytes, Session* session)
  {
    uint32_t distance = bytesToInt4(bytes[0], bytes[1], bytes[2], bytes[3]);
    uint32_t duration = bytesToInt2(bytes[4], bytes[5]);
    uint32_t avgSpeed = bytesToInt2(bytes[6], bytes[7]);
    uint32_t maxSpeed = bytesToInt2(bytes[8], bytes[9]);
    uint32_t energy = bytesToInt2(bytes[10], bytes[11]);
    uint32_t avgHeartRate = static_cast<uint32_t>(bytes[12]);
    uint32_t maxHeartRate = static_cast<uint32_t>(bytes[13]);

    uint32_t year = static_cast<uint32_t>(bytes[14]);
    uint32_t month = static_cast<uint32_t>(bytes[15]);
    uint32_t day = static_cast<uint32_t>(bytes[16]);
    uint32_t hour = static_cast<uint32_t>(bytes[17]);
    uint32_t minute = static_cast<uint32_t>(bytes[18]);
    //uint32_t fileNum = static_cast<uint32_t>(bytes[19]);

    tm time;
    time.tm_year  = 100 + year;// In tm, year is year since 1900. GPS returns year since 2000
    time.tm_mon   = month - 1;// In tm, month is between 0 and 11.
    time.tm_mday  = day;
    time.tm_hour  = hour;
    time.tm_min   = minute;
    time.tm_sec   = 0;
    time.tm_isdst = -1;
    session->setTime(time);
    session->setDistance(distance);
    session->setDuration(duration);
    session->setAvgSpeed(avgSpeed / 100.0);
    session->setMaxSpeed(maxSpeed / 100.0);
    session->setCalories(energy);
    session->setAvgHr(avgHeartRate);
    session->setMaxHr(maxHeartRate);
  }

  enum OmdType
  {
    GPS_POINT = 0xf1,
    GPS_META  = 0xf2
  };

  struct __attribute__((__packed__)) OmdGpsPoint
  {
    uint32_t lat;
    uint32_t lon;
    uint32_t dist;

    uint16_t time;
    uint8_t gpsStatus;
    uint16_t alt; // not sure the altitude is on 16 or 8bits. Let's assume 16bit and wait for a montain trip to confirm

    uint8_t unknown_17;
    uint8_t unknown_18;
    uint8_t id;
    // size: 20 bytes
  };

  struct __attribute__((__packed__)) OmdGpsMetaSingle
  {
    uint16_t time;
    uint16_t speed;
    uint16_t kcal;
    uint8_t hr;
    uint8_t lap;
    uint8_t cad;
  };

  struct __attribute__((__packed__)) OmdGpsMeta
  {
    struct OmdGpsMetaSingle p0;
    uint8_t pad;

    struct OmdGpsMetaSingle p1;
    uint8_t id;
  };


  // make sure structure size are equal to 20
  // by defining a negative bitfield
  struct size_checker {
    char t : sizeof(OmdGpsPoint) == 20 && sizeof(OmdGpsMeta) ? 1 : -1;
  };

  static void recordOneMeta(Point *p, struct OmdGpsMetaSingle *s, time_t startTime, uint8_t *maxHr, double *maxSpeed)
  {
    // normally, we can't have a META before we have seen 2 points,
    // so, previous and current points are already created
    // but, we can do the check anyway

    // also when a lap is recorded, we can have duplicate entries,
    // it is better to match the time of the meta data and the points
    double speed = ((double) le16toh(s->speed)) / 100.0;
    if (p != NULL &&
	p->getTime() == le16toh(s->time) + startTime) {
      p->setSpeed(speed);
      p->setHeartRate(s->hr);
    }

    if (*maxHr < s->hr)
      *maxHr = s->hr;
    if (*maxSpeed < speed)
      *maxSpeed = speed;
  }

  void OnMove200::parseOMDFile(const unsigned char* bytes, int length, Session *session)
  {
    time_t startTime = session->getTime();

    struct OmdGpsPoint *omdPoint;
    struct OmdGpsMeta *omdMeta;

    uint32_t prev_time = 0xffffffff;
    Point *currPoint = NULL, *prevPoint = NULL;

    // for lap creation
    Point *lapStart = NULL;
    uint32_t lapStartIdx = 0;
    uint32_t lapEndIdx = 0;
    uint32_t lapCount = 0;
    uint8_t maxHr = 0;
    double maxSpeed = 0;

    for(int i = 0; i < length; i += 20)
    {
      omdPoint = (struct OmdGpsPoint* ) &bytes[i];
      omdMeta = (struct OmdGpsMeta* ) &bytes[i];
      if (omdPoint->id == GPS_POINT)
      {
        double latitude         = ((double) le32toh(omdPoint->lat)) / 1000000.;
        double longitude        = ((double) le32toh(omdPoint->lon)) / 1000000.;
        uint32_t distance       = le32toh(omdPoint->dist);
        uint32_t time           = le16toh(omdPoint->time);
        uint32_t alt            = le16toh(omdPoint->alt);

        if (time == prev_time) // a lap records multiple identical points, let's skip them
          continue;
        prev_time = time;

	// don't record altitude if it is 0
	Field<int16_t> alt_def(FieldUndef);
	if (alt != 0)
	  alt_def = alt;
	else
	  alt_def = FieldUndef;

        auto p = new Point(latitude, longitude, alt_def, FieldUndef, startTime + time, 0, FieldUndef, 3);
        p->setDistance(distance);
        session->addPoint(p);

        // keep record of the last 2 points to update them with meta data
        prevPoint = currPoint;
        currPoint = p;

        lapEndIdx++;
        if (lapStart == NULL) // first point starts the first lap
          lapStart = p;

      }
      else if (omdMeta->id == GPS_META)
      {
        // first entry is for the older point (previous), second entry for the
        // more recent (current)
        recordOneMeta(prevPoint, &omdMeta->p0, startTime, &maxHr, &maxSpeed);
        recordOneMeta(currPoint, &omdMeta->p1, startTime, &maxHr, &maxSpeed);
      }

      // handle new lap and end of parsing here
      if (omdMeta->id == GPS_META &&
          (
            // laps are always on 2 meta data, so it is ok to check both
            (omdMeta->p0.lap != 0 && omdMeta->p1.lap != 0 && lapEndIdx > 0) ||
          // create a lap for the last point, but only if a lap has already been
          // created
          (i+20 == length && lapCount > 0)
	 ))
      {
        uint32_t totalDistance = currPoint->getDistance() - lapStart->getDistance();
        uint32_t totalTime     = currPoint->getTime() - lapStart->getTime();
        double avgSpeed;
        double avgHr;
        if (totalTime != 0)
          avgSpeed = 3.6 * totalDistance / totalTime;
        else
          avgSpeed = 0;

        std::list<Point*> points = session->getPoints();
        int hr = 0;
        int cnt=0;
        // having a vector for points would help here
        for(auto it = points.begin(); it != points.end(); ++it)
        {
          if (cnt == 0 && (*it) != lapStart)
            continue;
          hr += (*it)->getHeartRate();
          cnt++;
          if ((*it) == currPoint)
            break;
        }
        if (cnt > 0)
          avgHr = hr / cnt;
        else
          avgHr = 0;

        auto l = new Lap(lapStartIdx, lapEndIdx-1, totalTime, totalDistance, maxSpeed, avgSpeed, maxHr, avgHr, 0, 0, 0, 0);
        l->setLapNum(lapCount++);
        l->setStartPoint(lapStart);
        l->setEndPoint(currPoint);

        session->addLap(l);

        lapStartIdx = lapEndIdx-1;
        lapStart = currPoint;
        maxSpeed = 0;
        maxHr = 0;
      }
    }
  }
}
