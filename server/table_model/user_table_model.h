#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqltablemodel.h>

namespace arm_face_id {
namespace table_view {
class UserTableModel : public QSqlTableModel {
 public:
  UserTableModel(QObject* parent, const QSqlDatabase& db)
      : QSqlTableModel(parent, db) {}

  QVariant data(const QModelIndex& index,
                int role = Qt::DisplayRole) const override;
  // 重写 flags 函数，设置为只读
  Qt::ItemFlags flags(const QModelIndex& index) const override;
};
}  // namespace table_view
}  // namespace arm_face_id