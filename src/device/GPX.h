#ifndef _DEVICE_GPX_HPP_
#define _DEVICE_GPX_HPP_

#include "Device.h"
#include <libxml/parser.h>

namespace device
{
	/**
	  The interface for reading GPX files.
	 */
	class GPX : public Device
	{
		public:
			GPX() {};

			/**
			  Initialize the device.
			 */
			void init(const DeviceId& deviceId) override;

			/**
			  Release the device.
			 */
			void release() override {};

			/**
			  Read list of sessions from the device.
			  @param oSessions The list of sessions to fill
			 */
			void getSessionsList(SessionsMap *oSessions) override;

			/**
			  Export a session from the computer to the watch
			  @param iSession The session to export
			 */
			void exportSession(const Session *iSession) override {};

			/**
			  Send data to the source.
			  @param iData A pointer to data to be sent
			 */
			void getSessionsDetails(SessionsMap *oSessions) override;

			std::string getName() override { return "GPX"; };
      DeviceId getDeviceId() override { return { 0x0, 0x0 }; };

		private:
			bool openDoc();
			void parseDoc(Session *oSession);
			void parseMetadata(Session *oSession, xmlNodePtr rootNode);
			void parseTrk(Session *oSession, xmlNodePtr rootNode);
			void parseTrkSegOrRoute(Session *oSession, xmlNodePtr rootNode);
			void parseWayPoint(Session *oSession, xmlNodePtr rootNode);
			void parseWayPointExtensions(Point *oPoint, xmlNodePtr rootNode);
			void parseExtensions(Session *oSession, xmlNodePtr rootNode);
			void parseLap(Session *oSession, xmlNodePtr rootNode);
			void closeDoc();

			xmlChar *_docAsString;
			xmlDocPtr _document;
			xmlNodePtr _rootNode;

			DECLARE_DEVICE(GPX);
	};
}

#endif
