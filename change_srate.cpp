#include <iostream>
#include <complex>
#include <unistd.h>

#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/utils/msg.hpp>

char dummy_buffer[1024*1024];

int recv(uhd::rx_streamer::sptr rx_stream, int packet_len, int nof_packets) {
  uhd::rx_metadata_t md;
  uint32_t recv_packets = 0; 
  int n;
  do {
    n = rx_stream->recv(dummy_buffer, packet_len, md, 1);
    recv_packets++;
  } while (n > 0                                                     && 
           md.error_code == uhd::rx_metadata_t::ERROR_CODE_NONE      && 
           recv_packets < nof_packets);
  return recv_packets;  
}


int cuhd_start_rx_stream(uhd::usrp::multi_usrp::sptr usrp)
{
  uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
  cmd.time_spec = usrp->get_time_now();
  cmd.stream_now = true;
  usrp->issue_stream_cmd(cmd);
  return 0;
}

int cuhd_stop_rx_stream(uhd::usrp::multi_usrp::sptr usrp)
{
  uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
  cmd.time_spec = usrp->get_time_now();
  cmd.stream_now = true;
  usrp->issue_stream_cmd(cmd);
  return 0;
}

int main(int argc, char **argv) {
  
  std::string args = std::string("master_clock_rate=30720000");
  uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(args);

  usrp->set_clock_source("internal");

  std::string otw, cpu;
  otw = "sc16";
  cpu = "fc32";
  uhd::stream_args_t stream_args(cpu, otw);
  uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);

  double freq = 1000000000;
  double srate1 = 1920000;
  double srate2 = 15360000;
  
  usrp->set_rx_freq(freq);
  
  uint32_t nof_changes=0;
  while(1) {
    usrp->set_rx_rate(srate1);  

    cuhd_start_rx_stream(usrp);
    int n1 = recv(rx_stream, 1920, 50);
    cuhd_stop_rx_stream(usrp);

    usrp->set_rx_rate(srate2);  

    cuhd_start_rx_stream(usrp);
    int n2 = recv(rx_stream, 15360, 50);
    cuhd_stop_rx_stream(usrp);

    nof_changes++;    
    std::cout << "Done " << nof_changes << " sampling rate changes. Read: " 
      << n1 << "," << n2 << " packets. " << std::endl;    
  }
  
  exit(0);
}
