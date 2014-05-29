#ifndef _DEVICE_DEVICE_HPP_
#define _DEVICE_DEVICE_HPP_

#include "../Registry.h"
#include "../source/Source.h"
#include "../bom/Session.h"

#include <string>
#include <map>

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
	                virtual void setConfiguration(std::map<std::string, std::string> configuration) {_configuration = configuration;}
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
			  Export a session from the computer to the watch
			  @param iSession The session to export
			 */ 
			virtual void exportSession(Session *iSession) = 0;

			/** 
			  Retrieve details of sessions passed in input/output map
			  @param ioSessions The sessions to import, the function enrich it with details
			 */ 
			virtual void getSessionsDetails(SessionsMap *ioSessions) = 0;

			/**
			  Deleting sessions is not the primary function of this software. The way it's handled is not very user friendly.
			  This function should only be implemented for devices that do not offer a way to delete sessions from the watch. (e.g: OnMove100)
			*/
			virtual void deleteSessions() { std::cerr << "deleteSessions() not implemented for this device" << std::endl; };

			virtual std::string getName() = 0;

			virtual unsigned int getVendorId() = 0;
			virtual unsigned int getProductId() = 0;

		protected:
			std::map<std::string, std::string> _configuration;
			source::Source *_dataSource;
	};
}

#endif
