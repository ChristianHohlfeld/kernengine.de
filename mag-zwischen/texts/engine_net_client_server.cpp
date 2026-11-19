struct engine_net_client_server : virtual kern::Library, virtual engine_net_connection {
  // ---- types ----
  KERN_ENUM(net_ClientState, denied, timed_out, error, disconnected, connecting, challenged, connected);
  KERN_STRUCT(net_ClientServerConfig) { Int64 protocol_id = Int64{0x4b45524e30303031}; Int max_clients = 16; Float timeout = 6.0; Float request_interval = 0.1; Float connect_timeout = 6.0; Float keepalive_interval = 0.1; net_ConnectionConfig connection = net_ConnectionConfig{List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}}, 8192, 256, net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28}}; };
};
