struct engine_net_connection : virtual kern::Library, virtual engine_net_reliable {
  // ---- types ----
  KERN_ENUM(net_ChannelType, reliable_ordered, unreliable_unordered);
  KERN_STRUCT(net_ChannelConfig) { net_ChannelType type = net_ChannelType::reliable_ordered; Int send_queue_size = 1024; Int receive_queue_size = 1024; Int max_messages_per_packet = 256; Int packet_budget = 0; Int max_message_size = 65536; Int block_threshold = 512; Int block_fragment_size = 1024; Float resend_time = 0.1; };
  KERN_STRUCT(net_ConnectionConfig) { List<net_ChannelConfig> channels = List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}}; Int max_packet_size = 8192; Int max_message_types = 256; net_EndpointConfig endpoint = net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28}; };
};
