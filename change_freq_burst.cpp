#include <iostream>
#include <complex>
#include <signal.h>

#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/utils/msg.hpp>

char dummy_buffer[1024*1024];
static bool running = true;

void intHandler(int dummy=0) {
  running = false;
}

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

int cuhd_start_rx_burst(uhd::usrp::multi_usrp::sptr usrp, int nsamps)
{
  uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
  cmd.num_samps = nsamps;
  cmd.stream_now = true;
  usrp->issue_stream_cmd(cmd);
  return 0;
}

int main(int argc, char **argv) {

  signal(SIGINT, intHandler);
  
  std::string args = std::string("master_clock_rate=30720000");
  uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(args);

  usrp->set_clock_source("internal");

  std::string otw, cpu;
  otw = "sc16";
  cpu = "fc32";
  uhd::stream_args_t stream_args(cpu, otw);
  uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);

  double srate = 15360000;
  double freq1 = 1000000000;
  double freq2 = 2000000000;

  usrp->set_rx_rate(srate);
  
  uint32_t nof_changes=0;
  while(running) {
    usrp->set_rx_freq(freq1);
    cuhd_start_rx_burst(usrp, 10*15360);
    int n1 = recv(rx_stream, 15360, 10);

    usrp->set_rx_freq(freq2);
    cuhd_start_rx_burst(usrp, 10*15360);
    int n2 = recv(rx_stream, 15360, 10);

    nof_changes++;    
    std::cout << "Done " << nof_changes << " Rate changes. Read: " 
      << n1 << "," << n2 << " packets. " << std::endl;    
  }
  
  exit(0);
}
