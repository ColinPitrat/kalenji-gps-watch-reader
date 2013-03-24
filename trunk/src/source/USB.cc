#include "USB.h"

#include <iostream>
#include <cerrno>

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

		libusb_init(&_USBContext);
		ssize_t nbDevices = libusb_get_device_list(_USBContext, &listOfDevices);
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
				_kernelDriver0 = true;
			}

			rc = libusb_claim_interface(_device, 0);
			found = found && checkUSBOperation(rc);

			// TODO: Cleaner way to handle this: when to use which interfaces numbers ?
			if(vendorId == 0x0483 && productId == 0x5740)
			{
				_hasInterface1 = true;
				rc = libusb_kernel_driver_active(_device, 1);
				found = found && checkUSBOperation(rc);

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
		// TODO: Throw an exception if found == false
		if(!found)
		{
			std::cerr << "USB device not found !" << std::endl;
			throw std::exception();
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

	bool USB::read_data(unsigned char iEndPoint, unsigned char **oData, size_t *oLength)
	{
		int transferred;
		int rc = libusb_bulk_transfer(_device, iEndPoint /*0x81*/, _responseData, RESPONSE_BUFFER_SIZE, &transferred, _timeout);
		*oLength = (size_t) transferred;
		*oData = _responseData;
		checkUSBOperation(rc);
		return true;
	}

	void USB::write_data(unsigned char iEndPoint, unsigned char *iData, size_t iLength)
	{
		int transferred;
		int rc = libusb_bulk_transfer(_device, iEndPoint /*0x03*/, iData, iLength, &transferred, _timeout);
		bool ok = checkUSBOperation(rc);
		if(transferred != iLength)
		{
			std::cerr << "Error: transferred (" << transferred << ") != length (" << iLength << ")" << std::endl;
			ok = false;
		}
		// TODO: Throw an exception if ok == false
	}

	void USB::control_transfer(unsigned char iRequestType, unsigned char iRequest, unsigned short iValue, unsigned short iIndex, unsigned char *iData, unsigned short iLength)
	{
		int rc = libusb_control_transfer(_device, iRequestType, iRequest, iValue, iIndex, iData, iLength, _timeout);
		bool ok = checkUSBOperation(rc);
		// TODO: Throw an exception if ok == false
	}
}
