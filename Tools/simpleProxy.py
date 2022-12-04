from twisted.internet import reactor
from quarry.net.proxy import DownstreamFactory, Bridge
from hotreload import Loader

import sys
import os

os.chdir(os.path.dirname(__file__))

script = Loader("simpleProxyScript.py")

class ExampleBridge(Bridge):
    def packet_unhandled(self, buff, direction, name):
        script.packet_unhandled(self, buff, direction, name)


def main(argv):
    factory = DownstreamFactory()
    factory.bridge_class = ExampleBridge
    factory.connect_host = "127.0.0.1"
    factory.connect_port = 25565
    factory.online_mode = False
    factory.compression_threshold = 0
    factory.listen("127.0.0.1", 25566)
    reactor.run()


if __name__ == "__main__":
    import sys
    main(sys.argv[1:])