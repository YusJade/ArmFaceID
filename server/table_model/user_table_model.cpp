#include "user_table_model.h"

#include <QPixmap>

using namespace arm_face_id;

QVariant table_view::UserTableModel::data(const QModelIndex &index,
                                          int role) const {
  if (index.column() == 2) {
    auto img_data = QSqlTableModel::data(index, Qt::DisplayRole).toByteArray();
    QPixmap pixmap;
    pixmap.loadFromData(img_data);
    return pixmap.scaled(50, 50, Qt::KeepAspectRatio);  // 调整图片大小
  }
  return QSqlTableModel::data(index, role);
}