def packet_unhandled(self, buff, direction, name):
    printPacket = True

    if (name == "join_game"):
        print("-- new connection --")
    if ("entity" in name or name == "time_update" or name == "player_position" or name == "player_list_item" or name == "keep_alive"):
        printPacket = False

    if direction == "downstream":
        if (printPacket): print(f"[S->C] {name}")
        self.downstream.send_packet(name, buff.read())
    elif direction == "upstream":
        if (printPacket): print(f"[C->S] {name}")
        self.upstream.send_packet(name, buff.read())