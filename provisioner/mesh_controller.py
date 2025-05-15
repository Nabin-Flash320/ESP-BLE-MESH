
import serial
import threading
import sys
import time

class BLEMeshProvisionerController():
    def __init__(self):
        self.serial = serial.Serial(port=f'/dev/ttyUSB1', baudrate=115200)
        self.exit_read = threading.Event()
        self.command_lists = {
            'system': ('ping', 'restart'),
            'ble': ('start', 'prov_start'),
            'prov': ('begin', 'scan', 'list_dev', 'add_dev_uuid', 'add_dev_idx'),
            'exit': None,
        }

    def handle_command(self, operation):
        """Process a given command string."""
        self.operation_found = False
        for key in self.command_lists.keys():
            try:
                if operation in self.command_lists[key]:
                    self.call_attribute(command=key, operation=operation)
                    self.operation_found = True
                    break
            except TypeError:
                if operation in self.command_lists.keys() and key == operation:
                    self.call_attribute(command=operation)
                    self.operation_found = True
                    break
                else:
                    continue
        
        if self.operation_found is False:
            print(f'Operation {operation} not found')
        return True

    def call_attribute(self, command, operation=None):
        if not isinstance(command, str) and len(command) == 0:
            ValueError(f"Invalid command provided")
        self.callable_attribute = str()
        if operation is None:
            self.callable_attribute = command
        else:
            if not isinstance(operation, str) and len(operation) == 0:
                ValueError(f'Invlaid operation provided')
            self.callable_attribute = f'{command}_{operation}'
        if hasattr(self, self.callable_attribute):
            attribute = getattr(self, self.callable_attribute)
            if callable(attribute):
                attribute()
            else:
                print(f'Unable to find the attribute {self.callable_attribute}')
        else:
            print(f'Cannot find attribute {self.callable_attribute}')

    def read_serial(self):
        while not self.exit_read.is_set or self.serial.is_open:
            self.bytes_read = self.serial.read(self.serial.in_waiting)
            if len(self.bytes_read) > 0:
                print(f'Here, {self.bytes_read}')
        self.serial.close()

    def run(self):
        threading.Thread(target=self.read_serial, args=()).start()
        while True:
            try:
                user_input = input(">>> ")
                if not self.handle_command(user_input):
                    break
            except (KeyboardInterrupt, EOFError):
                print("\nConsole interrupted. Exiting.")
                break
    
    def send_command(self, command_array):
        if not isinstance(command_array, list):
            ValueError(f'command_array required list got {type(command_array)}')
        self.checksum = 0
        for command in command_array:
            self.checksum ^= command
        command_array.append(self.checksum)
        print(bytearray(self.command_array))
        self.serial.write(bytearray(command_array))

    def system_ping(self):
        self.command_array = [0xAA, 0x01, 0x00, 0x00, 0x00]
        self.send_command(self.command_array)

    def system_restart(self):
        self.command_array = [0xAA, 0x01, 0x00, 0x01, 0x00]
        self.send_command(self.command_array)

    def exit(self):
        self.exit_read.set()
        sys.exit(0)

if __name__ == '__main__':
    controller = BLEMeshProvisionerController()
    controller.run()


