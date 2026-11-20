struct engine_net_reliable : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(net_EndpointConfig) { Int max_packet_size = 16384; Int fragment_above = 1024; Int fragment_size = 1024; Int max_fragments = 16; Int ack_buffer_size = 256; Int sent_packets_buffer_size = 256; Int received_packets_buffer_size = 256; Int fragment_reassembly_buffer_size = 64; Float rtt_smoothing = 0.1; Float packet_loss_smoothing = 0.1; Float bandwidth_smoothing = 0.1; Int packet_header_size = 28; };
};
