#ifndef _DEVICE_TCX_HPP_
#define _DEVICE_TCX_HPP_

#include "Device.h"
#include <libxml/parser.h>

namespace device
{
	/**
	  The interface for reading TCX files.
	 */
	class TCX : public Device
	{
		public:
			TCX() {};

			/**
			  Initialize the device.
			 */
			virtual void init();

			/**
			  Release the device.
			 */
			virtual void release() {};

			/**
			  Read list of sessions from the device.
			  @param oSessions The list of sessions to fill
			 */
			virtual void getSessionsList(SessionsMap *oSessions);

			/**
			  Export a session from the computer to the watch
			  @param iSession The session to export
			 */
			virtual void exportSession(Session *iSession) {};

			/**
			  Send data to the source.
			  @param iData A pointer to data to be sent
			 */
			virtual void getSessionsDetails(SessionsMap *oSessions);

			virtual std::string getName() { return "TCX"; };
			virtual unsigned int getVendorId() { return 0x0; };
			virtual unsigned int getProductId() { return 0x0; };

		private:
			bool openDoc();
			void parseDoc(Session *oSession);
			void parseActivities(Session *oSession, xmlNodePtr rootNode);
			void parseActivity(Session *oSession, xmlNodePtr rootNode);
			void parseLap(Session *oSession, Lap *oLap, xmlNodePtr rootNode);
			void parseTrack(Session *oSession, Lap *oLap, xmlNodePtr rootNode);
			void parseTrackpoint(Point *oPoint, xmlNodePtr rootNode);
			void parsePosition(Point *oPoint, xmlNodePtr rootNode);
			void parsePointExtensions(Point *oPoint, xmlNodePtr rootNode);
			void parsePointTPX(Point *oPoint, xmlNodePtr rootNode);
			void closeDoc();

			xmlChar *_docAsString;
			xmlDocPtr _document;
			xmlNodePtr _rootNode;

			DECLARE_DEVICE(TCX);
	};
}

#endif
