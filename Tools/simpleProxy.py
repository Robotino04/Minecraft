from twisted.internet import reactor
from quarry.net.proxy import DownstreamFactory, Bridge
import quarry.net

numPackets = 0
class ExampleBridge(Bridge):
    def packet_unhandled(self, buff, direction, name):
        global numPackets
        numPackets += 1

        if direction == "downstream":
            if (numPackets < 10): print(f"[S->C] {name}")
            self.downstream.send_packet(name, buff.read())
        elif direction == "upstream":
            if (numPackets < 10): print(f"[C->S] {name}")
            self.upstream.send_packet(name, buff.read())


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