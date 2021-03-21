/*
 *  Copyright (C) 2021 Team KeePassXC <team@keepassxc.org>
 *  Copyright (C) 2012 Felix Geyer <debfx@fobos.de>
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

#include "AutoTypeAction.h"

#include "core/Tools.h"

AutoTypeKey::AutoTypeKey(Qt::Key key, Qt::KeyboardModifiers modifiers)
    : key(key)
    , modifiers(modifiers)
{
}

AutoTypeKey::AutoTypeKey(const QChar& character, Qt::KeyboardModifiers modifiers)
    : character(character)
    , modifiers(modifiers)
{
}

void AutoTypeKey::exec(AutoTypeExecutor* executor) const
{
    executor->execType(this);
}

void AutoTypeKey::exec(TargetedAutoTypeExecutor* executor, QSharedPointer<AutoTypeTarget> target)
{
    executor->execType(this, target);
}

AutoTypeDelay::AutoTypeDelay(int delayMs, bool setExecDelay)
    : delayMs(delayMs)
    , setExecDelay(setExecDelay)
{
}

void AutoTypeDelay::exec(AutoTypeExecutor* executor) const
{
    if (setExecDelay) {
        // Change the delay between actions
        executor->execDelayMs = delayMs;
    } else {
        // Pause execution
        Tools::wait(delayMs);
    }
}

void AutoTypeDelay::exec(TargetedAutoTypeExecutor* executor, QSharedPointer<AutoTypeTarget> target)
{
    Q_UNUSED(target);
    if (setExecDelay) {
        // Change the delay between actions
        executor->execDelayMs = delayMs;
    } else {
        // Pause execution
        Tools::wait(delayMs);
    }
}

void AutoTypeClearField::exec(AutoTypeExecutor* executor) const
{
    executor->execClearField(this);
}

void AutoTypeClearField::exec(TargetedAutoTypeExecutor* executor, QSharedPointer<AutoTypeTarget> target)
{
    executor->execClearField(this, target);
}
