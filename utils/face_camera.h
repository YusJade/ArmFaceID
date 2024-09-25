#include <memory>
#include <thread>

#include <opencv2/videoio.hpp>

#include "interface.h"

namespace arm_face_id {
/**
  使用 OpenCV 实现摄像头模块，支持调用网络摄像头
 */
class FaceCamera : public interface::Camera {
 public:
  // 摄像头相关配置
  struct Settings {
    // 网络摄像头 url
    std::string cam_url = "";
    // 本地摄像头设备 index
    int cam_index = 0;
    // 是否启用网络摄像头
    bool enable_net_cam = true;
  };

  FaceCamera() = delete;
  FaceCamera(Settings&&) = delete;
  FaceCamera(const Settings&);

  // 打开摄像头并抓取画面，将会创建一个子线程
  bool OpenAndStart();

 private:
  Settings settings_;
  bool is_running_ = false;
  std::unique_ptr<std::thread> cam_thread_;
  cv::VideoCapture cap_;
};
}  // namespace arm_face_id