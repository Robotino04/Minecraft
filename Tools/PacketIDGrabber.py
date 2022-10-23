"""
This script generates the "PacketID.hpp" file
from "PacketID.tpp" by downloading the wikipage
andmextracting the ids. It is automatically
invoked when running cmake.
"""

import requests
import sys
import copy

from html.parser import HTMLParser

# the url from which to extract the packets
url = 'https://wiki.vg/Protocol'

# helper functions
def bit(n): return 1<<n

quiet = False
def printq(*args, **kwargs):
    if not quiet: print(*args, **kwargs)

class Packet:
    """A internal representation for a single packet."""
    BOUND_TO_CLIENT = "Client"
    BOUND_TO_SERVER = "Server"
    BIND_TARGETS = [BOUND_TO_SERVER, BOUND_TO_CLIENT]

    STATE_HANDSHAKING = "Handshaking"
    STATE_STATUS = "Status"
    STATE_LOGIN = "Login"
    STATE_PLAY = "Play"
    STATES = [STATE_HANDSHAKING, STATE_LOGIN, STATE_STATUS, STATE_PLAY]

    def __init__(self):
        self.original_name = ""
        self.name = ""
        self.id = 0
        self.bound_to = ""
        self.state = ""

    def __eq__(self, other):
        return self.name == other.name \
            and self.id == other.id \
            and self.bound_to == other.bound_to \
            and self.state == other.state

class MyHTMLParser(HTMLParser):
    """The parser for the wikipage
    
    This html parser extracts h4 headings followed by tables
    and extracts the packets from them.
    """
    EXPECT_H4_BEGIN             = bit(0)
    EXPECT_H4_END               = bit(1)
    EXPECT_H4_DATA              = bit(2)
    EXPECT_TABLE_BEGIN          = bit(3)
    EXPECT_TABLE_ROW_BEGIN      = bit(4)
    EXPECT_TABLE_COLUMN_BEGIN   = bit(5)
    EXPECT_TABLE_COLUMN_DATA    = bit(6)
    EXPECT_TABLE_COLUMN_END     = bit(7)
    EXPECT_TABLE_ROW_END        = bit(8)
    EXPECT_TABLE_END            = bit(9)

    def __init__(self, *, convert_charrefs: bool = ...) -> None:
        super().__init__(convert_charrefs=convert_charrefs)

        self.expected = MyHTMLParser.EXPECT_H4_BEGIN
        self.col_count = 0
        self.row_count = 0

        self.is_packet = False
        self.packet_list = list()

        self.packet = Packet()
    

    def handle_starttag(self, tag, attrs):
        if (self.expected & MyHTMLParser.EXPECT_H4_BEGIN and tag == "h4"):
            self.expected = MyHTMLParser.EXPECT_H4_DATA
        elif (self.expected & MyHTMLParser.EXPECT_TABLE_BEGIN and tag == "table"):
            self.expected = MyHTMLParser.EXPECT_TABLE_ROW_BEGIN
            self.row_count = -1
        elif (self.expected & MyHTMLParser.EXPECT_TABLE_ROW_BEGIN and tag == "tr"):
            self.expected = MyHTMLParser.EXPECT_TABLE_COLUMN_BEGIN
            self.col_count = -1
            self.row_count += 1
        elif (self.expected & MyHTMLParser.EXPECT_TABLE_COLUMN_BEGIN and (tag == "th" or tag == "td")):
            self.expected = MyHTMLParser.EXPECT_TABLE_COLUMN_DATA
            self.col_count += 1


    def handle_data(self, data):
        if (self.expected & MyHTMLParser.EXPECT_H4_DATA):
            printq(f"Packet: {data}")
            self.packet.name = data
            self.expected = MyHTMLParser.EXPECT_H4_END
        elif (self.expected & MyHTMLParser.EXPECT_TABLE_COLUMN_DATA):
            printq(f"Col {self.col_count}, Row: {self.row_count}: \"{data.strip()}\"")
            if (self.is_packet and self.col_count == 0 and self.row_count == 1):
                self.packet.id = int(data, base = 16)
            if (self.is_packet and self.col_count == 1 and self.row_count == 1):
                if (data.capitalize().strip() in Packet.STATES):
                    self.packet.state = data.capitalize().strip()
            if (self.is_packet and self.col_count == 2 and self.row_count == 1):
                if (data.capitalize().strip() in Packet.BIND_TARGETS):
                    self.packet.bound_to = data.capitalize().strip()
            elif (self.col_count == 0 and self.row_count == 0):
                self.is_packet = data.strip().lower() == "Packet ID".lower()
            self.expected = MyHTMLParser.EXPECT_TABLE_COLUMN_END
        

    def handle_endtag(self, tag):
        if (self.expected & MyHTMLParser.EXPECT_H4_END and tag == "h4"):
            self.expected = MyHTMLParser.EXPECT_TABLE_BEGIN
        elif (self.expected & MyHTMLParser.EXPECT_TABLE_COLUMN_END and (tag == "th" or tag == "td")):
            self.expected = MyHTMLParser.EXPECT_TABLE_COLUMN_BEGIN | MyHTMLParser.EXPECT_TABLE_ROW_END
        elif (self.expected & MyHTMLParser.EXPECT_TABLE_ROW_END and tag == "tr"):
            self.expected = MyHTMLParser.EXPECT_TABLE_ROW_BEGIN | MyHTMLParser.EXPECT_TABLE_END
        elif (self.expected & MyHTMLParser.EXPECT_TABLE_END and tag == "table"):
            self.expected = MyHTMLParser.EXPECT_H4_BEGIN

            if (self.is_packet):
                self.packet.original_name = self.packet.name.split("(")[0].strip()
                self.packet.name = ''.join(list(map(lambda x: x.capitalize(), self.packet.name.split()))).split("(")[0]
                self.packet.id = '0x' + hex(self.packet.id).split('x')[1].rjust(2, '0').upper()

                self.packet_list.append(copy.deepcopy(self.packet))

# handle arguments
infile_name = "/home/roman/source-build/Minecraft/Minecraft/src/include/Minecraft/PacketID.tpp"
if len(sys.argv) >= 2:
    infile_name = sys.argv[1]
else:
    print(f"No input filename given!")
    # exit(1)

if len(sys.argv) >= 3 and sys.argv[2] == "-q":
    quiet = True

outfile_name = infile_name.replace(".tpp", ".hpp")


# read in the template file
with open(infile_name, "r") as infile:
    template = infile.read()

# convert the template to a formatable string
replacements = [
    ("{", "{{"),
    ("}", "}}"),
    ("{{packets}}", "{packets}"),
    ("{{cases}}", "{cases}"),
]

for old, new in replacements:
    template = template.replace(old, new)

# remove the include warning from the first line
template = "\n".join(template.split("\n")[1:])



# get the file
try:
    r = requests.get(url)
except (requests.ConnectionError):
    print(f"Unable to reach \"{url}\"!")
    exit(1)

if (r.ok):
    html = r.content.decode("utf-8")
else:
    print(f"Unable to reach \"{url}\"!")
    exit(1)

# parse the website
parser = MyHTMLParser()
parser.feed(html)


# make all packet names unique
for packet in parser.packet_list:
    same_name = list(filter(lambda other_: packet != other_ and packet.name == other_.name, parser.packet_list))

    if len(same_name):
        packet.name += f"_{packet.state}"
    for other in same_name:
        other.name += f"_{other.state}"
    

    same_name = list(filter(lambda other_: packet != other_ and packet.name == other_.name, same_name))

    if len(same_name):
        packet.name += f"_{packet.bound_to}"
    for other in same_name:
        other.name += f"_{other.bound_to}"

# generate the PacketID definitions
packet_ids = []
for packet in parser.packet_list:
    packet_ids.append(f"\n    {packet.name} = {packet.id},")

# generate the switch-case cases
switch_cases = []
for packet in parser.packet_list:
    switch_cases.append("\n\n" +
        f"        case static_cast<uint32_t>(PacketID::{packet.name}) | static_cast<uint32_t>(ConnectionState::{packet.state}) << 8 | static_cast<uint32_t>(BindTarget::{packet.bound_to}) << 16:{{\n" +
         "            std::stringstream ss;\n" +
        f"            ss << \"{packet.original_name} (ID: {packet.id}, state: {packet.state}, length: 0x\" << std::hex << std::uppercase << length << \")\";\n" +
         "            return ss.str();\n" +
         "        }"
    )


# save the generated file
with open(outfile_name, "w") as outfile:
    outfile.write(template.format(
        packets = "".join(packet_ids),
        cases = "".join(switch_cases)
    ))

printq(f"Wrote {len(parser.packet_list)} PacketIDs to \"{outfile_name}\".")