#include <ElaWindow.h>

#include "interface.h"

namespace arm_face_id {
class ElaGUI : public ElaWindow,
               public interface::FaceDetectorObserver,
               public interface::CameraObserver {
 public:
  ElaGUI();
};

}  // namespace arm_face_id
