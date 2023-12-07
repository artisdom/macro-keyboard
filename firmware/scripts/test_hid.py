import hid
import sys

vid = 0x303a
pid = 0x4009
via_usage_page = 0xFF60
via_usage_id = 0x61

device = None
GET_VERSION = 0x01

def select_via_device():
	device = None
	for keyboard in hid.enumerate(vid, pid):
		# print("{}".format(keyboard))
		if keyboard["usage_page"] == via_usage_page:
			device = hid.Device(path=keyboard["path"])
			print("Selected Via device at {}".format(keyboard["path"]))
			break

	return device

# device = hid.Device(vid, pid)
# device = hid.Device(path=b"DevSrvsID:4294972483")
# print(device.manufacturer)

# device.write("hello".encode())
# # device.get_feature_report(0, 8)
# print(device.get_input_report(0, 32))


def send_via_command(device, command_id):
	buffer = [0] * 33
	# buffer[0] = 
	buffer[1] = command_id

	device.write(bytes(buffer))
	data = device.read(32, timeout=1000)
	print(data)



if __name__ == "__main__":
	device = select_via_device()
	if not device:
		print("Unable to find VIA compatible device")
		sys.exit(1)
	
	send_via_command(device, GET_VERSION)