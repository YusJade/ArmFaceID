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
  } else if (index.column() == 5) {
    return "face features";
  }

  return QSqlTableModel::data(index, role);
}

// 重写 flags 函数，设置为只读
Qt::ItemFlags table_view::UserTableModel::flags(
    const QModelIndex &index) const {
  return QAbstractItemModel::flags(index) & ~Qt::ItemIsEditable;
}