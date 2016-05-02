#include "Navbike400.h"
#include <cstring>
#include <iostream>
#include <iomanip>

#ifdef DEBUG
#define DEBUG_CMD(x) x;
#else
#define DEBUG_CMD(x) ;
#endif

namespace device
{
  REGISTER_DEVICE(Navbike400);

  const int     Navbike400::lengthDeviceInit = 32;
  unsigned char Navbike400::deviceInit[lengthDeviceInit] = { 0xee, 0xee, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
  const int     Navbike400::lengthGetData = 32;
  unsigned char Navbike400::getData[lengthGetData] = { 0x08, 0x08, 0xaa, 0xaa, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

  void Navbike400::dump(unsigned char *data, int length)
  {
    DEBUG_CMD(std::cout << std::hex);
    for(int i = 0; i < length; ++i)
    {
      DEBUG_CMD(std::cout << std::setw(2) << std::setfill('0') << (int) data[i] << " ");
    }
    DEBUG_CMD(std::cout << std::endl);
  }

  void Navbike400::init()
  {
    LOG_VERBOSE("Navbike400::init() - init device");
    _dataSource->init(getVendorId(), getProductId());
    // Step 1 - USB to UART initialization ?
    {
      unsigned char data[256] = { 0, 0xC2, 1, 0 };
      unsigned char dataIn[256];

      LOG_VERBOSE("Navbike400::init() - step 0 (frame 18 (empty))");
      _dataSource->control_transfer(0x00, 0x9, 0x1, 0x0, data, 0);

      LOG_VERBOSE("Navbike400::init() - step 1 (frame 2)");
      _dataSource->control_transfer(0x40, 0x0, 0xffff, 0x0, data, 0);

      LOG_VERBOSE("Navbike400::init() - step 2 (frame 4)");
      _dataSource->control_transfer(0x40, 0x1, 0x2000, 0x0, data, 0);

      LOG_VERBOSE("Navbike400::init() - step 3 (frame 6)");
      _dataSource->control_transfer(0xc0, 0xff, 0x370b, 0x0, dataIn, 1);

      LOG_VERBOSE("Navbike400::init() - step 4 (frame 9)");
      _dataSource->control_transfer(0xc0, 0xff, 0x370b, 0x0, dataIn, 1);

      LOG_VERBOSE("Navbike400::init() - step 5 (frame 12)");
      _dataSource->control_transfer(0x40, 0x1e, 0x0, 0x0, data, 4);

      LOG_VERBOSE("Navbike400::init() - step 6 (frame 15)");
      _dataSource->control_transfer(0x40, 0x12, 0xc, 0x0, data, 0);
    }
    // Step 2 - Device initialization ?
    {
      //unsigned char initResponse[sizeInitResponse] = { 0x45, 0x0c, 0x0c, 0x18, 0x01, 0x65 };
      // In issue 23, a dump attached as 45 0c 04 18 00 as answer, considered correct by OnConnect software
      // So let's only check 45 0c as it's the common part to all dumps until now
            // In issue 48, Navbike 40 answers 45 0d ... so only check 0x45 ...
      const size_t sizeInitResponse = 1;
      unsigned char initResponse[sizeInitResponse] = { 0x45 };
      size_t received = 0;
      unsigned char *responseData;
      int attempts = 0;
      do
      {
        try
        {
          attempts++;
          LOG_VERBOSE("Navbike400::init() - device init attempt " << attempts);
          _dataSource->write_data(0x01, deviceInit, lengthDeviceInit);
          _dataSource->read_data(0x81, &responseData, &received);
        }
        catch(...)
        {
          unsigned char data[256] = { 0 };
          LOG_VERBOSE("Navbike400::init() - step 13 (retry device init)");
          _dataSource->control_transfer(0x40, 0x12, 0xc, 0x0, data, 0);
        }
      } while(memcmp(responseData, initResponse, sizeInitResponse) && attempts < 10);
    }
  }

  void Navbike400::release()
  {
    unsigned char data[256] = { 0 };
    LOG_VERBOSE("Navbike400::release() - step 1 (frame 32)");
    _dataSource->control_transfer(0x40, 0x0, 0x0, 0x0, data, 0);
  }

  double Navbike400::decodeCoordinate(unsigned char *buffer)
  {
    uint64_t deg = 0;
    deg += 10*((buffer[0] & 0xF0) >> 4) + (buffer[0] & 0xF);
    deg *= 100;
    deg += 10*((buffer[1] & 0xF0) >> 4) + (buffer[1] & 0xF);
    uint64_t min = 0;
    for(int i = 0; i < 3; ++i)
    {
      min *= 100;
      min += 10*((buffer[i+2] & 0xF0) >> 4) + (buffer[i+2] & 0xF);
    }
    double pos_neg = 1.0;
    // W (west) and S (south) correspond to negative coordinates
    if(buffer[5] == 'W' || buffer[5] == 'S')
    {
      pos_neg = -1.0;
    }
    return pos_neg * (deg + (min/600000.0));
  }

#define READ_MORE_DATA if(received > 36) { responseData += 36; received -= 36; } else { _dataSource->read_data(0x81, &responseData, &received); }

  void Navbike400::getSessionsList(SessionsMap *oSessions)
  {
    LOG_VERBOSE("Navbike400: Get sessions list !");
    unsigned char data[256] = { 0 };
    _dataSource->control_transfer(0x40, 0x12, 0xc, 0x0, data, 0);
    LOG_VERBOSE("Navbike400: Read sessions list !");
    unsigned char *responseData;
    size_t received;
    _dataSource->write_data(0x01, getData, lengthGetData);
    _dataSource->read_data(0x81, &responseData, &received);
    if(responseData[35] != 0xff)
    {
      std::cout << "Unexpected line termination " << (int)responseData[35] << " instead of 0xFF." << std::endl;
    }
    READ_MORE_DATA;
    // First lines to be reverse engineered (sessions global infos ?)
    while(responseData[35] == 0xfd)
    {
      try
      {
        tm time;
        memset(&time, 0, sizeof(time));
        time.tm_sec = responseData[0];
        time.tm_min = responseData[1];
        time.tm_hour = responseData[2];
        time.tm_mday = responseData[3];
        // In tm, month is between 0 and 11.
        time.tm_mon = responseData[4] - 1;
        // In tm, year is year since 1900. GPS returns year since 2000
        time.tm_year = 100 + responseData[5];
        time.tm_isdst = -1;
        LOG_VERBOSE("Session from: " << time.tm_year + 1900 << "-" << time.tm_mon + 1 << "-" << time.tm_mday << " " << time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec);
        int nb_points = (responseData[14] << 8) + responseData[15];
        int num_session = responseData[34];
        SessionId id = SessionId(responseData[34], responseData[34]+1);
        // TODO: Find duration, distance and # laps (watch doesn't support laps ?)
        Session mySession(id, num_session, time, nb_points, 0, 0, 0);
        oSessions->insert(SessionsMapElement(id, mySession));
        //Session *session = &(oSessions->find(id)->second);
        // Ignore second line (for now ?)
        READ_MORE_DATA;
      }
      catch(std::runtime_error e)
      {
        LOG_VERBOSE("Navbike400: read_data failed: " << e.what() << " - Retrying a get data.");
        _dataSource->write_data(0x01, getData, lengthGetData);
      }
      READ_MORE_DATA;
    }
    while(responseData[35] == 0xfa || responseData[0] != 0xee)
    {
      // If some data was added or lost in the point, just ignore it and try to find back the start of the next one
      if(responseData[35] != 0xfa)
      {
        LOG_VERBOSE("Point not ending with 0xfa - skipping");
        dump(responseData, 36);
        while(responseData[0] != 0xfa && received > 0)
        {
          responseData++;
          received--;
        }
        if(received > 0)
        {
          LOG_VERBOSE("Skipping");
          responseData++;
          received--;
        }
        READ_MORE_DATA;
        continue;
      }
      LOG_VERBOSE("Navbike400: Read session !");
      // First line ending with fa is the header of the session
      // It's exactly the same as the first line ending with fd
      /*
      tm time;
      time.tm_sec = responseData[0];
      time.tm_min = responseData[1];
      time.tm_hour = responseData[2];
      time.tm_mday = responseData[3];
      // In tm, month is between 0 and 11.
      time.tm_mon = responseData[4] - 1;
      // In tm, year is year since 1900. GPS returns year since 2000
      time.tm_year = 100 + responseData[5];
      time.tm_isdst = -1;
      LOG_VERBOSE("Session from: " << time.tm_year + 1900 << "-" << time.tm_mon + 1 << "-" << time.tm_mday << " " << time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec);
      int nb_points = (responseData[14] << 8) + responseData[15];
      int num_session = responseData[34];
      SessionId id = SessionId(responseData[34], responseData[34]+1);
      // TODO: Find duration, distance and # laps (watch doesn't support laps ?)
      Session mySession(id, num_session, time, nb_points, 0, 0, 0);
      oSessions->insert(SessionsMapElement(id, mySession));
      Session *session = &(oSessions->find(id)->second);
      time_t current_time = session->getTime();
      */
      SessionId id = SessionId(responseData[34], responseData[34]+1);
      Session *session = &(oSessions->find(id)->second);
      time_t start_time = session->getTime();
      READ_MORE_DATA;
      // Can a session have more than 65536 points ? (bytes 32 & 33 contains the point number)
      double prevDistance = 0;
      double prevElapsed = 0;
      while(responseData[32] != 0 || responseData[33] != 0)
      {
        // If some data was added or lost in the point, just ignore it and try to find back the start of the next one
        if(responseData[35] != 0xfa)
        {
          LOG_VERBOSE("Point not ending with 0xfa - skipping");
          dump(responseData, 36);
          while(responseData[0] != 0xfa && received > 0)
          {
            responseData++;
            received--;
          }
          if(received > 0)
          {
            LOG_VERBOSE("Skipping");
            responseData++;
            received--;
          }
          READ_MORE_DATA;
          continue;
        }
        //dump(responseData, 36);
        double distance = (responseData[0] + (responseData[1] << 8) + (responseData[2] << 16) + (responseData[3] << 24)) / 100.0;
        double latitude = decodeCoordinate(&responseData[14]);
        double longitude = decodeCoordinate(&responseData[20]);
        if(latitude == 0 and longitude == 0)
        {
          READ_MORE_DATA;
          continue;
        }
        int16_t elevation = (responseData[10] << 8) + responseData[11];

        double elapsed = (responseData[32] << 8) + responseData[33];
        //int pointID = (responseData[32] << 8) + responseData[33];
        //int sessionNum = responseData[34];
        double speed = 0;
        if(elapsed != prevElapsed)
        {
          speed = ((distance - prevDistance) * 3.6) / (elapsed - prevElapsed);
        }
        prevElapsed = elapsed;
        prevDistance = distance;
        // TODO: Find out altitude, speed
        Point *myPoint = new Point(latitude, longitude, elevation, speed, start_time+elapsed, 0, FieldUndef, 3);
        session->addPoint(myPoint);
        READ_MORE_DATA;
      }
    }
    LOG_VERBOSE("Navbike400: Finished !");
  }

#undef READ_MORE_DATA

  void Navbike400::getSessionsDetails(SessionsMap *oSessions)
  {
  }

  void Navbike400::exportSession(Session *iSession)
  {
    std::cerr << "Unsupported export of session with Navbike 400" << std::endl;
  }
}
