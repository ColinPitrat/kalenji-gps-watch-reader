#include "USB.h"

#include <iostream>
#include <cerrno>

namespace source
{
	void USB::init(uint32_t vendorId, uint32_t productId)
	{
		libusb_context *myUSBContext;
		libusb_device **listOfDevices;
		bool found = false;
		int rc = 0;

		libusb_init(&myUSBContext);
		ssize_t nbDevices = libusb_get_device_list(myUSBContext, &listOfDevices);
		if (nbDevices < 0)
		{
			std::cerr << "Error retrieving USB devices list: " << nbDevices << std::endl;
			// TODO: Throw an exception
		}
		for(int i = 0; i < nbDevices; ++i)
		{
			libusb_device_descriptor deviceDescriptor;
			libusb_get_device_descriptor(listOfDevices[i], &deviceDescriptor);
			if(deviceDescriptor.idVendor == vendorId && deviceDescriptor.idProduct == productId)
			{
				libusb_open(listOfDevices[i], &_device);
				found = true;
			}
		}

		if(found)
		{
			rc = libusb_kernel_driver_active(_device, 0);
			found = found && checkUSBOperation(rc);

			if (rc > 0)
			{
				rc = libusb_detach_kernel_driver(_device, 0); 
				found = found && checkUSBOperation(rc);
			}

			rc = libusb_claim_interface(_device, 0);
			found = found && checkUSBOperation(rc);

			rc = libusb_claim_interface(_device, 1);
			found = found && checkUSBOperation(rc);
		}

		libusb_free_device_list(listOfDevices, 1);
		// TODO: Throw an exception if found == false
		if(!found)
		{
			std::cerr << "USB device not found !" << std::endl;
		}
	}

	void USB::release()
	{
		int rc;
		if (_device) 
		{
			rc = libusb_release_interface(_device, 0);
			checkUSBOperation(rc);
			rc = libusb_release_interface(_device, 1);
			checkUSBOperation(rc);

			libusb_close(_device);
		}
		libusb_exit(NULL); 
	}

	bool USB::checkUSBOperation(int rc)
	{
		if(rc < 0)
		{
			std::cerr << "Transmission error: " << rc << " - " << errno;
			switch(rc)
			{
				case LIBUSB_ERROR_IO:
					std::cerr << " LIBUSB_ERROR_IO";
					break;
				case LIBUSB_ERROR_ACCESS:
					std::cerr << " LIBUSB_ERROR_ACCESS";
					break;
				case LIBUSB_ERROR_NO_DEVICE:
					std::cerr << " LIBUSB_ERROR_NO_DEVICE";
					break;
				case LIBUSB_ERROR_TIMEOUT:
					std::cerr << " LIBUSB_ERROR_TIMEOUT";
					break;
				case LIBUSB_ERROR_BUSY:
					std::cerr << " LIBUSB_ERROR_BUSY";
					break;
				default:
					std::cerr << " Unknown error";
					break;
			}
			std::cerr << std::endl;
			return false;
		}
		return true;
	}

	bool USB::read_data(unsigned char **oData, size_t *oLength)
	{
		int transferred;
		int rc = libusb_bulk_transfer(_device, 0x81, _responseData, RESPONSE_BUFFER_SIZE, &transferred, _timeout);
		*oLength = (size_t) transferred;
		*oData = _responseData;
		checkUSBOperation(rc);
		return true;
	}

	void USB::write_data(unsigned char *iData, size_t iLength)
	{
		int transferred;
		int rc = libusb_bulk_transfer(_device, 0x03, iData, iLength, &transferred, _timeout);
		bool ok = checkUSBOperation(rc);
		if(transferred != iLength)
		{
			std::cerr << "Error: transferred (" << transferred << ") != length (" << iLength << ")" << std::endl;
			ok = false;
		}
		// TODO: Throw an exception if ok == false
	}
}
