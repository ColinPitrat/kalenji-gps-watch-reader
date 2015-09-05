#include "USB.h"
#include "../Utils.h"

#include <iostream>
#include <cerrno>
#include <stdexcept>

namespace source
{
	void USB::init(uint32_t vendorId, uint32_t productId)
	{
		libusb_device **listOfDevices;
		bool found = false;
		int rc = 0;
		_kernelDriver0 = false;
		_kernelDriver1 = false;
		_hasInterface1 = false;

		rc = libusb_init(&_USBContext);
        if (rc != 0)
        {
			throw std::runtime_error(Formatter() << "failed to initialize libusb");
        }
		ssize_t nbDevices = libusb_get_device_list(_USBContext, &listOfDevices);
		if (nbDevices < 0)
		{
			throw std::runtime_error(Formatter() << "can't retrieve USB devices list: " << nbDevices);
		}
		for(int i = 0; i < nbDevices; ++i)
		{
			libusb_device_descriptor deviceDescriptor;
			libusb_get_device_descriptor(listOfDevices[i], &deviceDescriptor);
			if(deviceDescriptor.idVendor == vendorId && deviceDescriptor.idProduct == productId)
			{
				int rc = libusb_open(listOfDevices[i], &_device);
				if (rc != 0)
				{
					std::ostringstream oss;
					oss << "can't access usb device: " << rc << libusb_error_name(rc);
					throw std::runtime_error(oss.str());
				}
				found = true;
			}
		}

		if(found)
		{
			#ifndef WINDOWS
			rc = libusb_kernel_driver_active(_device, 0);
			found = found && checkUSBOperation(rc);
			#endif

			if (rc > 0)
			{
				rc = libusb_detach_kernel_driver(_device, 0); 
				found = found && checkUSBOperation(rc);
				_kernelDriver0 = true;
			}

			rc = libusb_claim_interface(_device, 0);
			found = found && checkUSBOperation(rc);

			// TODO: Cleaner way to handle this: when to use which interfaces numbers ?
			if(vendorId == 0x0483 && productId == 0x5740)
			{
				_hasInterface1 = true;
				#ifndef WINDOWS
				rc = libusb_kernel_driver_active(_device, 1);
				found = found && checkUSBOperation(rc);
				#endif

				if (rc > 0)
				{
					rc = libusb_detach_kernel_driver(_device, 1); 
					found = found && checkUSBOperation(rc);
					_kernelDriver1 = true;
				}

				rc = libusb_claim_interface(_device, 1);
				found = found && checkUSBOperation(rc);
			}
		}

		libusb_free_device_list(listOfDevices, 1);
		if(!found)
		{
			throw std::runtime_error("USB device not found !");
		}
	}

	void USB::release()
	{
		int rc;
		if (_device) 
		{
			rc = libusb_release_interface(_device, 0);
			checkUSBOperation(rc);
			if(_hasInterface1 == true)
			{
				rc = libusb_release_interface(_device, 1);
				checkUSBOperation(rc);
			}

			if(_kernelDriver0)
			{
				rc = libusb_attach_kernel_driver(_device, 0);
				checkUSBOperation(rc);
			}
			if(_kernelDriver1)
			{
				rc = libusb_attach_kernel_driver(_device, 1);
				checkUSBOperation(rc);
			}

			libusb_close(_device);
		}
		libusb_exit(_USBContext);
	}

	const char *USB::getUSBErrorMessage(int rc)
	{
		switch(rc)
		{
			case LIBUSB_ERROR_IO:
				return " LIBUSB_ERROR_IO";
			case LIBUSB_ERROR_ACCESS:
				return " LIBUSB_ERROR_ACCESS";
			case LIBUSB_ERROR_NO_DEVICE:
				return " LIBUSB_ERROR_NO_DEVICE";
			case LIBUSB_ERROR_TIMEOUT:
				return " LIBUSB_ERROR_TIMEOUT";
			case LIBUSB_ERROR_BUSY:
				return " LIBUSB_ERROR_BUSY";
			default:
				return " Unknown error";
		}
	}

	bool USB::checkUSBOperation(int rc)
	{
		if(rc < 0)
		{
			std::cerr << "Transmission error: " << rc << " - " << errno << getUSBErrorMessage(rc) << std::endl;
			return false;
		}
		return true;
	}

	void USB::checkAndThrowUSBOperation(int rc)
	{
		if(rc < 0)
		{
			throw std::runtime_error(Formatter() << "on USB operation code " << rc << " - " << errno << getUSBErrorMessage(rc));
		}
	}

	bool USB::read_data(unsigned char iEndPoint, unsigned char **oData, size_t *oLength)
	{
		int transferred;
		int rc = libusb_bulk_transfer(_device, iEndPoint /*0x81*/, _responseData, RESPONSE_BUFFER_SIZE, &transferred, _timeout);
		*oLength = (size_t) transferred;
		*oData = _responseData;
		checkAndThrowUSBOperation(rc);
		return true;
	}

	void USB::write_data(unsigned char iEndPoint, unsigned char *iData, size_t iLength)
	{
		int transferred;
		int rc = libusb_bulk_transfer(_device, iEndPoint /*0x03*/, iData, iLength, &transferred, _timeout);
		checkAndThrowUSBOperation(rc);
		if((size_t)transferred != iLength)
		{
			throw std::runtime_error(Formatter() << "on USB write transferred (" << transferred << ") != length (" << iLength << ")");
		}
	}

	void USB::control_transfer(unsigned char iRequestType, unsigned char iRequest, unsigned short iValue, unsigned short iIndex, unsigned char *iData, unsigned short iLength)
	{
		int rc = libusb_control_transfer(_device, iRequestType, iRequest, iValue, iIndex, iData, iLength, _timeout);
		checkAndThrowUSBOperation(rc);
	}
}
