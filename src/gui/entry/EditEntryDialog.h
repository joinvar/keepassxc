/*
 *  Copyright (C) 2026 KeePassXC Team <team@keepassxc.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KEEPASSX_EDITENTRYDIALOG_H
#define KEEPASSX_EDITENTRYDIALOG_H

#include <QDialog>
#include <QPointer>

class EditEntryWidget;
class QStackedWidget;

// Non-modal window so the database tree stays usable while an entry is edited.
class EditEntryDialog : public QDialog
{
    Q_OBJECT

public:
    EditEntryDialog(EditEntryWidget* editWidget, EditEntryWidget* historyWidget, QWidget* parent = nullptr);

    void showEditor(const QString& title);
    void showHistory(const QString& title);
    // Ask to save or discard, then hide. Returns true when the window is closed.
    bool requestClose();
    // Hide immediately and drop the form. Caller already decided to discard.
    void discard();

protected:
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void reject() override;

private:
    void reveal();
    void placeBesideParent();

    QPointer<EditEntryWidget> m_editWidget;
    QPointer<EditEntryWidget> m_historyWidget;
    QStackedWidget* m_stack = nullptr;
    bool m_placed = false;
};

#endif // KEEPASSX_EDITENTRYDIALOG_H
