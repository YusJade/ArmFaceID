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
};
}  // namespace table_view
}  // namespace arm_face_id