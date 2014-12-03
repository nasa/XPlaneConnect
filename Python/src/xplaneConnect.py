import socket

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
        timeout_ms = 500
        self.socket.settimeout(timeout_ms / 1000)

    def __del__(self):
        self.close()

    def close():
        '''Closes the underlying UDP socket'''
        if self.socket is not None:
            self.socket.close()
            self.socket = None

    def send_udp(msg):
        pass

    def read_udp():
        pass

    # Configuration
    def set_conn(rec_port):
        pass

    def pause_sim(state):
        pass

    # UDP Data
    def parse_data(msg, dataRef):
        pass

    def read_data(msg, dataRef):
        pass

    def send_data(msg, dataRef, rows):
        pass

    # Position
    def parse_pos(msg, resultArray, gear):
        pass

    def read_pos(resultArray, gear):
        pass

    def send_pos(ACNum, numArgs, valueArray):
        pass

    # Controls
    def parse_ctrl(msg, resultArray, gear):
        pass

    def read_ctrol(resultArray, gear):
        pass

    def send_pos(numArgs, valueArray):
        pass
        
    # DREF Manipulation
    def read_dref(resultArray):
        pass

    def parse_dref(msg, dataRef, length_of_dref, values):
        pass

    def send_dref(dataRef, length_of_dref, values):
        pass

    def request_dref(dref_array, dref_sizes, list_len, resultArray):
        pass

    def parse_getd(msg, dref_array, dref_sizes):
        pass

    def parse_request(msg, result_array, array_sizes):
        pass

    def read_request(data_ref, array_sizes, recv_addr):
        pass