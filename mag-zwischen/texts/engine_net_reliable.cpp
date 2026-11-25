struct engine_net_reliable : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(net_EndpointConfig) { I32 max_packet_size = 16384; I32 fragment_above = 1024; I32 fragment_size = 1024; I32 max_fragments = 16; I32 ack_buffer_size = 256; I32 sent_packets_buffer_size = 256; I32 received_packets_buffer_size = 256; I32 fragment_reassembly_buffer_size = 64; Float rtt_smoothing = 0.1; Float packet_loss_smoothing = 0.1; Float bandwidth_smoothing = 0.1; I32 packet_header_size = 28; };
};
