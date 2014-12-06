import socket
import struct

class XPlaneConnect:
    '''XPlaneConnect (XPC) facilitates communication to and from the XPCPlugin.'''

    # Basic Functions
    def __init__(self, port, xp_ip = 'localhost', xp_port = 49009):
        '''Creates a new XPlaneConnect interface, and binds a UDP socket based on the specified parameters.'''

        # Setup server port
        server = (socket.INADDR_ANY, socket.htons(port))

        # Setup XPlane IP and port
        if xp_ip == 'localhost' or xp_ip is None:
            self.xp_ip = '127.0.0.1'
        else:
            self.xp_ip = xp_ip

        self.xp_port = xp_port

        # Create and bind socket
        # TODO: Raise a friendly error if socket creation/binding fails
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.socket.bind(server)
        
        # Set timeout
        timeout_us = 500
        self.socket.settimeout(timeout_ms / 1000000)

    def __del__(self):
        self.close()

    def close(self):
        '''Closes the underlying UDP socket'''
        if self.socket is not None:
            self.socket.close()
            self.socket = None

    def send_udp(self, msg):
        '''Sends a message over the underlying UDP socket.'''
        msg_len = len(msg)
        msg[4] = msg_len

        # Preconditions
        if(msg_len <= 0): # Require message length greater than 0.
            raise RuntimeError("send_udp: message length must be psoitive >0")

        # Code
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1) # TODO: Why?

        self.socket.send(msg)

    def read_udp(self, recv_addr = None):
        if recv_addr is None:
            recv_addr = (self.xp_ip, self.xp_port)

        return self.socket.recv(5000) # TODO: Validate that this matches the behavior of the C version

    # Configuration
    def set_conn(self, rec_port):
        msg = struct.pack("!4sH", ("CONN", rec_port))
        self.send_udp(msg)

    def pause_sim(self, pause):
        '''Pauses or unpauses the X-Plane simulation'''
        pause_val = 0
        if pause:
            pause_val = 1

        msg = struct.pack("4sBB", ("SIMU", pause_val, 0))
        self.send_udp(msg)

    # UDP Data
    def parse_data(self, data):
        pass

    def read_data(self):
        buf = self.read_udp()
        if buf[0] != 0:
            return self.parse_data(buf)
        else:
            return None

    def send_data(self, dataRef):
        '''Sends X-Plane data over the underlying UDP socket.'''
        msg = struct.pack("4s", "DATA")
        for row in dataRef:
            struct.pack_into("!I", msg, len(msg), row[0])
            for i in range(8):
                struct.pack_into("!f", msg, len(msg), row[1][i])
        self.send_udp(msg)

    # Position
    def parse_pos(self, data):
        pass

    def read_pos(self, resultArray, gear):
        buf = self.read_udp()
        if buf[0] != 0:
            return self.parse_pos(buf)
        else:
            return None

    def send_pos(self, ac_num, values):
        # Preconditions
        if len(values) < 1:
            raise RuntimeError("send_pos: Must have at least one argument")

        # Header and Aircraft num
        msg = struct.pack("!4sB", ("POSI", ac_num))

        # States
        for i in range(7):
            val = -998.5 # TODO: Why?
            if i < len(values):
                val = values[i]
            struct.pack_into("!f", msg, len(msg), val)

        # Send
        self.send_udp(msg)
#}
    # Controls
    def parse_ctrl(self, data):
        pass

    def read_ctrol(self, resultArray, gear):
        buf = self.read_udp()
        if buf[0] != 0:
            return self.parse_ctrl(buf)
        else:
            return float("NaN")

    def send_ctrol(self, values):
        # Preconditions
        if len(values) < 1:
            raise RuntimeError("send_ctrl: Must have at least one argument")

        # Header
        msg = struct.pack("!4s", "CTRL")

        # States
        for i in range(6):
            val = -998.5 # TODO: Why?
            if i < len(values):
                val = values[i]
            struct.pack_into("!f", msg, len(msg), val)

        # Send
        self.send_udp(msg)
        
    # DREF Manipulation
    def read_dref(self, resultArray):
        buf = self.read_udp()
        if buf[0] != 0:
            return self.parse_dref(buf)
        else:
            return None

    def parse_dref(self, data):
        pass

    def send_dref(self, data_ref, values):
        '''Sends X-Plane dref over the underlying UDP socket.'''
        msg_len = 7 + len(data_ref) + len(values) * 4
        
        # Header
        msg = struct.pack("4s", "DREF")

        # DRef
        struct.pack_into("B", msg, len(msg), len(data_ref))
        struct.pack_into(str(len(data_ref)) + "B", msg, len(msg), data_ref) # TODO: Verify byte order (should be network order?)

        # Values
        struct.pack_into("B", msg, len(msg), len(values))
        struct.pack_into(str(len(values)) + "f", msg, len(msg), values) # TODO: Verify byte order (should be network order?)

        self.send_udp(msg)

    def request_dref(self, dref_array, resultArray):
        '''Requests drefs and reads the response.'''
        self.send_request(dref_array)
        for i in range(80):
            size = self.read_dref(resultArray)
            if size != -1:
                return size

        return -1

    def parse_getd(self, msg, dref_array, dref_sizes):
        pass

    def send_request(self, dref_array):
        '''Sends a request over the underlying UDP socket.'''
        # Header
        msg = struct.pack("4s", "GETD");

        # Number of values;
        struct.pack_into("B", msg, len(msg), len(dref_array))

        # The Rest
        for dref in dref_array:            
            struct.pack_into("B", msg, len(msg), len(dref_array))
            struct.pack_into("100B", msg, len(msg), dref_array)

        self.send_udp(msg)

    def parse_request(self, data):
        pass

    def read_request(self, data_ref, array_sizes, recv_addr):
        buf = self.read_udp()
        if buf[0] != 0:
            return self.parse_request(buf)
        else:
            return None