#ifndef _DEVICE_DEVICE_HPP_
#define _DEVICE_DEVICE_HPP_

#include "../Registry.h"
#include "../source/Source.h"
#include "../bom/Session.h"

#define DECLARE_DEVICE(DeviceClass) static LayerRegistrer<Device, DeviceClass> _registrer;
#define REGISTER_DEVICE(DeviceClass) LayerRegistrer<Device, DeviceClass> DeviceClass::_registrer;

namespace device
{
	/**
	  An interface for a GPS device. 
	 */
	class Device
	{
		public:
			Device() : _dataSource(NULL) {};

			virtual void setSource(source::Source *dataSource) { _dataSource = dataSource; };
			/** 
			  Initialize the device. Any action that needs to be done before discussing with it.
			 */ 
			virtual void init() = 0;

			/** 
			  Release the device. Any action that needs to be done once the work is over.
			 */ 
			virtual void release() = 0;

			/** 
			  Read list of sessions from the device.
			  @param oSessions The list of sessions to fill
			 */ 
			virtual void getSessionsList(SessionsMap *oSessions) = 0;

			/** 
			  Send data to the source. 
			  @param iData A pointer to data to be sent
			 */ 
			virtual void getSessionsDetails(SessionsMap *oSessions) = 0;

			virtual std::string getName() = 0;

		protected:
			source::Source *_dataSource;
	};
}

#endif
