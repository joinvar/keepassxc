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

#include "EditEntryDialog.h"

#include "gui/entry/EditEntryWidget.h"

#include <QKeyEvent>
#include <QScreen>
#include <QStackedWidget>
#include <QVBoxLayout>

EditEntryDialog::EditEntryDialog(EditEntryWidget* editWidget, EditEntryWidget* historyWidget, QWidget* parent)
    : QDialog(parent)
    , m_editWidget(editWidget)
    , m_historyWidget(historyWidget)
{
    setModal(false);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    setAttribute(Qt::WA_DeleteOnClose, false);

    m_stack = new QStackedWidget(this);
    m_editWidget->setParent(m_stack);
    m_historyWidget->setParent(m_stack);
    m_stack->addWidget(m_editWidget);
    m_stack->addWidget(m_historyWidget);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_stack);
}

void EditEntryDialog::showEditor(const QString& title)
{
    m_stack->setCurrentWidget(m_editWidget);
    if (!title.isEmpty()) {
        setWindowTitle(title);
    }
    reveal();
}

void EditEntryDialog::showHistory(const QString& title)
{
    m_stack->setCurrentWidget(m_historyWidget);
    if (!title.isEmpty()) {
        setWindowTitle(title);
    }
    reveal();
}

bool EditEntryDialog::requestClose()
{
    if (!isVisible()) {
        return true;
    }

    // History is a page inside this same window. Step back to the entry, then close that.
    if (m_stack->currentWidget() == m_historyWidget && m_historyWidget) {
        m_historyWidget->cancel();
    }
    if (isVisible() && m_editWidget && m_editWidget->currentEntry()) {
        m_editWidget->cancel();
    }
    return !isVisible();
}

void EditEntryDialog::discard()
{
    if (m_editWidget) {
        m_editWidget->clear();
    }
    if (m_historyWidget) {
        m_historyWidget->clear();
    }
    hide();
}

void EditEntryDialog::closeEvent(QCloseEvent* event)
{
    event->ignore();
    requestClose();
}

void EditEntryDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && event->modifiers() == Qt::NoModifier) {
        requestClose();
        return;
    }
    QDialog::keyPressEvent(event);
}

void EditEntryDialog::reject()
{
    requestClose();
}

void EditEntryDialog::reveal()
{
    if (!m_placed) {
        placeBesideParent();
        m_placed = true;
    }
    show();
    raise();
    activateWindow();
}

void EditEntryDialog::placeBesideParent()
{
    QWidget* owner = parentWidget() ? parentWidget()->window() : nullptr;
    if (!owner) {
        resize(920, 720);
        return;
    }

    const QRect mainGeo = owner->geometry();
    const QRect avail = owner->screen() ? owner->screen()->availableGeometry() : QRect();
    const int leftVisible = qMin(320, qMax(220, mainGeo.width() / 3));
    const int width = avail.isNull() ? 920 : qBound(640, avail.width() - leftVisible - 48, 980);
    const int height = qBound(520, mainGeo.height() - 80, 800);
    resize(width, height);

    int x = mainGeo.x() + leftVisible;
    int y = mainGeo.y() + 40;
    if (!avail.isNull()) {
        x = qBound(avail.left(), x, qMax(avail.left(), avail.right() - width + 1));
        y = qBound(avail.top(), y, qMax(avail.top(), avail.bottom() - height + 1));
    }
    move(x, y);
}
