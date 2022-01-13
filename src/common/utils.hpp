#include <time.h>

#include <glog/logging.h>
#include <brpc/server.h>
#include <brpc/channel.h>


bool DownloadFile(const std::string &url, std::string &outbuf, std::string &err_msg) {
  brpc::Controller cntl;
  cntl.http_request().set_method(brpc::HTTP_METHOD_GET);
  cntl.http_request().uri() = url;


  brpc::Channel channel;
  brpc::ChannelOptions options;
  options.connect_timeout_ms = 20000;
  options.timeout_ms = 120000;
  options.protocol = "http";
  
  brpc::URI uri;
  uri.SetHttpURL(url);
  int ret = channel.Init(uri.host().c_str(), &options);
  if (0 != ret) {
    err_msg = "Download failed to init channel. url: " + url + ", ret " + std::to_string(ret);
    return false;
  }

  channel.CallMethod(NULL, &cntl, NULL, NULL, NULL /*done*/);

  int status = cntl.http_response().status_code();
  if (status < 200 || status >= 300) {
    err_msg = "Get " + url + " return failed, code: " + std::to_string(status);
    return false;
  }

  outbuf = cntl.response_attachment().to_string();
  return true;
}


std::string GenRandomString(const int len) {
  srand((unsigned)time(NULL));
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  std::string tmp_s;
  tmp_s.reserve(len);

  for (int i = 0; i < len; ++i) {
      tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
  }
  
  return tmp_s;
}

double rad2angle(double rad) {
  return rad / PI * 180.0;
}
