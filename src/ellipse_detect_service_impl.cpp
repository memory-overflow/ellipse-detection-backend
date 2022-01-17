#include <stdlib.h>

#include <vector>
#include <exception>

#include <glog/logging.h>
#include <brpc/server.h>
#include <brpc/channel.h>
#include <json2pb/json_to_pb.h>
#include <json2pb/pb_to_json.h>
#include <ellipse_detection/detect.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include "ellipse_detect_service_impl.h"
#include "common/utils.hpp"

DEFINE_string(image_save_dir, "./images", "文件保存路径");


EllipseDetectServiceImpl::~EllipseDetectServiceImpl() {

}


void Process(EllipseDetectRequest *request, EllipseDetectResponse *response) {
  if (request == nullptr) {
    return;
  }
  LOG(INFO) << "[" << request->requestid() << "]Receive EllipseDetect image_url: " << request->imageurl()
            << ", image binary length: " << request->image().length();
  response->set_requestid(request->requestid());
  cv::Mat origin_image, image;
  if (request->has_image()) {
    std::vector<uchar> buf(request->image().c_str(), request->image().c_str() + request->image().size());
    origin_image = cv::imdecode(buf, cv::IMREAD_COLOR);
    if (origin_image.empty()) {
      response -> set_errorcode(InvalidParameter);
      response -> set_errormessage("解析图片失败");
      LOG(ERROR) << "[" << request->requestid() << "]" << response->errormessage();
      return;
    }
  } else if (request->has_imageurl()) {
    std::string file_buf, err_msg;
    bool download_success = DownloadFile(request->imageurl(), file_buf, err_msg);
    if (!download_success) {
      response -> set_errorcode(InvalidParameter);
      response -> set_errormessage(err_msg);
      LOG(ERROR) << "[" << request->requestid() << "]" << response->errormessage();
      return;
    }
    std::vector<uchar> buf(file_buf.c_str(), file_buf.c_str() + file_buf.size());
    origin_image = cv::imdecode(buf, cv::IMREAD_COLOR);
    if (origin_image.empty()) {
      response -> set_errorcode(InvalidParameter);
      response -> set_errormessage("解析图片失败");
      LOG(ERROR) << "[" << request->requestid() << "]" << response->errormessage();
      return;
    }
  } else {
      response -> set_errorcode(InvalidParameter);
      response -> set_errormessage("未传图片或者图片下载地址");
      return;
  }
  std::string filename = FLAGS_image_save_dir + "/" + GenRandomString(16) + "-" + request->requestid() + ".jpg";
  // mkdir 
  system(("mkdir -p " + FLAGS_image_save_dir).c_str());
  cv::imwrite(filename, origin_image);
  cv::cvtColor(origin_image, image, CV_RGB2GRAY);

  std::vector<std::shared_ptr<zgh::Ellipse> > ells;
  zgh::detectEllipse(image.data, image.rows, image.cols, ells);
  for (auto &ell : ells) {
    auto ellipse = response->add_ellipseset();
    ellipse->mutable_o()->set_x(ell->o.x);
    ellipse->mutable_o()->set_y(ell->o.y);
    ellipse->set_a(ell->a);
    ellipse->set_b(ell->b);
    ellipse->set_angle(ell->phi);
    ellipse->set_goodness(ell->goodness);
    ellipse->set_coverangle(ell->coverangle);
    ellipse->set_polarity(ell->polarity);
    for (auto c : ell->coefficients) {
      ellipse->add_coefficients(c);
    }
    cv::ellipse(origin_image,
              cv::Point(ell->o.y, ell->o.x),
              cv::Size(ell->a, ell->b),
              rad2angle(PI_2 - ell->phi),
              0,
              360,
              cv::Scalar(0, 255, 0),
              2.0,
              8,
              0);
  }
  int max_size = std::max(origin_image.cols, origin_image.rows);
  if (max_size > 1280) {
    double scale = 1280.0 / max_size;
    cv::resize(origin_image, origin_image, cv::Size(int(origin_image.cols * scale), int(origin_image.rows * scale)));
  }
  std::vector<uchar> ret_buf;
  cv::imencode(".jpg", origin_image, ret_buf);
  response->set_showimage((const char *)(ret_buf.data()), ret_buf.size());
}


void EllipseDetectServiceImpl::EllipseDetect(
  ::google::protobuf::RpcController* controller,
  const ::HttpRequest*,
  ::HttpResponse*,
  ::google::protobuf::Closure* done) {

  
  brpc::ClosureGuard done_guard(done);
  brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
  cntl->http_response().set_content_type("application/json");

  EllipseDetectRequest request;
  EllipseDetectResponse response;
  std::string body = cntl->request_attachment().to_string();
  std::string error_msg;
  // Get Content-Type
  if (cntl->http_request().content_type() == "application/json") {
    bool ok = json2pb::JsonToProtoMessage(body, &request, &error_msg);
    if (!ok) {
      response.set_errorcode(InvalidParameter);
      response.set_errormessage("Parser request body failed. error: " + error_msg);
      LOG(ERROR) << response.errormessage();
      return;
    }
  } else {
    response.set_errorcode(InvalidParameter);
    response.set_errormessage("接口不支持'" + cntl->http_request().content_type() + "'协议");
    return;
  }
  try {
    Process(&request, &response);
  } catch(std::exception e) {
    response.set_errorcode(InternalError);
    response.set_errormessage("内部错误" + std::string(e.what()));
    LOG(ERROR) << response.errormessage();
    return;
  }
  std::string rsp = "";
  if (!json2pb::ProtoMessageToJson(response, &rsp, &error_msg)) {
    LOG(ERROR) << error_msg;
  }
  
  cntl->response_attachment() = rsp;
  LOG(INFO) << "[" << request.requestid() << "]" << "Response data length: " << rsp.length();
}

