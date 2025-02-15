/*
 *  Copyright (C) 2012 Felix Geyer <debfx@fobos.de>
 *  Copyright (C) 2017 KeePassXC Team <team@keepassxc.org>
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

#ifndef KEEPASSX_AUTOTYPE_H
#define KEEPASSX_AUTOTYPE_H

#include <QMutex>
#include <QObject>
#include <QStringList>
#include <QWidget>

#include "autotype/AutoTypeExternalPlugin.h"
#include "autotype/AutoTypeMatch.h"

class AutoTypeAction;
class AutoTypeExecutor;
class TargetedAutoTypeExecutor;
class AutoTypePlatformInterface;
class AutoTypeExternalInterface;
class Database;
class Entry;
class QPluginLoader;

class AutoType : public QObject
{
    Q_OBJECT

public:
    QStringList windowTitles();
    bool registerGlobalShortcut(Qt::Key key, Qt::KeyboardModifiers modifiers, QString* error = nullptr);
    void unregisterGlobalShortcut();
    void performAutoType(const Entry* entry, QWidget* hideWindow = nullptr);
    void performAutoTypeWithSequence(const Entry* entry, const QString& sequence, QWidget* hideWindow = nullptr);
    void performAutoTypeOnExternalPlugin(const Entry* entry,
                                         const QString& pluginName,
                                         QSharedPointer<AutoTypeTarget> target);

    bool isExternalPluginTargetSelectionRequired(const QString& pluginName);
    AutoTypeTargetMap getExternalPluginTargets(const QString& pluginName);

    static bool verifyAutoTypeSyntax(const QString& sequence, const Entry* entry, QString& error);

    inline bool isAvailable()
    {
        return m_plugin;
    }

    inline bool isAnyExternalPluginAvailable()
    {
        return !m_external_plugins.isEmpty();
    }

    inline bool isExternalPluginAvailable(const QString& name)
    {
        return m_external_plugins.contains(name);
    }

    static AutoType* instance();
    static void createTestInstance();

public slots:
    void performGlobalAutoType(const QList<QSharedPointer<Database>>& dbList);
    void raiseWindow();

signals:
    void globalAutoTypeTriggered();
    void autotypePerformed();
    void autotypeRejected();

private slots:
    void startGlobalAutoType();
    void unloadPlugin();
    void unloadExternalPlugin(const QString& name);

private:
    enum WindowState
    {
        Normal,
        Minimized,
        Hidden
    };

    explicit AutoType(QObject* parent = nullptr, bool test = false);
    ~AutoType() override;
    void loadPlugin(const QString& pluginPath);
    void loadExternalPlugin(const QString& name, const QString& pluginPath);
    void executeAutoTypeActions(const Entry* entry,
                                QWidget* hideWindow = nullptr,
                                const QString& customSequence = QString(),
                                WId window = 0);
    void executeAutoTypeActionsOnExternalTarget(const Entry* entry,
                                                const QString& pluginName,
                                                QSharedPointer<AutoTypeTarget> target,
                                                const QString& customSequence = QString());
    void restoreWindowState();
    void resetAutoTypeState();

    static QList<QSharedPointer<AutoTypeAction>>
    parseSequence(const QString& entrySequence, const Entry* entry, QString& error, bool syntaxOnly = false);

    QMutex m_inAutoType;
    QMutex m_inGlobalAutoTypeDialog;
    QPluginLoader* m_pluginLoader;
    AutoTypePlatformInterface* m_plugin;
    AutoTypeExecutor* m_executor;
    QHash<QString, AutoTypeExternalInterface*> m_external_plugins;
    QHash<QString, TargetedAutoTypeExecutor*> m_external_executors;
    static AutoType* m_instance;

    QString m_windowTitleForGlobal;
    WindowState m_windowState;
    WId m_windowForGlobal;

    Q_DISABLE_COPY(AutoType)
};

inline AutoType* autoType()
{
    return AutoType::instance();
}

#endif // KEEPASSX_AUTOTYPE_H
