#include "proto/ellipse_detect.pb.h"

class EllipseDetectServiceImpl : public EllipseDetectService {
 public:
  virtual ~EllipseDetectServiceImpl();

  virtual void EllipseDetect(::google::protobuf::RpcController* controller,
                       const ::HttpRequest* request,
                       ::HttpResponse* response,
                       ::google::protobuf::Closure* done);
};
