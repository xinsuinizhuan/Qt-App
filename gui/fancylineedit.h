// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include "completinglineedit.h"

#include <QAbstractButton>

#include <functional>

QT_BEGIN_NAMESPACE
class QEvent;
class QKeySequence;
QT_END_NAMESPACE

namespace GUI {

class FancyLineEditPrivate;

class GUI_EXPORT IconButton : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(float iconOpacity READ iconOpacity WRITE setIconOpacity)
    Q_PROPERTY(bool autoHide READ hasAutoHide WRITE setAutoHide)
public:
    explicit IconButton(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;
    float iconOpacity() { return m_iconOpacity; }
    void setIconOpacity(float value)
    {
        m_iconOpacity = value;
        update();
    }
    void animateShow(bool visible);

    void setAutoHide(bool hide) { m_autoHide = hide; }
    bool hasAutoHide() const { return m_autoHide; }

    QSize sizeHint() const override;

protected:
    void keyPressEvent(QKeyEvent *ke) override;
    void keyReleaseEvent(QKeyEvent *ke) override;

private:
    float m_iconOpacity;
    bool m_autoHide;
    QIcon m_icon;
};

class GUI_EXPORT FancyLineEdit : public CompletingLineEdit
{
    Q_OBJECT
public:
    enum Side { Left = 0, Right = 1 };
    Q_ENUM(Side)

    explicit FancyLineEdit(QWidget *parent = nullptr);
    ~FancyLineEdit() override;

    void setTextKeepingActiveCursor(const QString &text);

    QIcon buttonIcon(Side side) const;
    void setButtonIcon(Side side, const QIcon &icon);

    QMenu *buttonMenu(Side side) const;
    void setButtonMenu(Side side, QMenu *menu);

    void setButtonVisible(Side side, bool visible);
    bool isButtonVisible(Side side) const;
    QAbstractButton *button(Side side) const;

    void setButtonToolTip(Side side, const QString &);
    void setButtonFocusPolicy(Side side, Qt::FocusPolicy policy);

    // Set whether tabbing in will trigger the menu.
    void setMenuTabFocusTrigger(Side side, bool v);
    bool hasMenuTabFocusTrigger(Side side) const;

    // Set if icon should be hidden when text is empty
    void setAutoHideButton(Side side, bool h);
    bool hasAutoHideButton(Side side) const;

    // Completion

    // Enable a history completer with a history of entries.
    void setHistoryCompleter(const QString &historyKey, bool restoreLastItemFromHistory = false);
    // Sets a completer that is not a history completer.
    void setSpecialCompleter(QCompleter *completer);

    // Filtering

    // Enables filtering
    void setFiltering(bool on);

    //  Validation

    // line edit, (out)errorMessage -> valid?
    using ValidationFunction = std::function<bool(FancyLineEdit *, QString *)>;
    enum State { Invalid, DisplayingPlaceholderText, Valid };

    State state() const;
    bool isValid() const;
    QString errorMessage() const;

    void setValidationFunction(const ValidationFunction &fn);
    static ValidationFunction defaultValidationFunction();
    void validate();
    void onEditingFinished();

    static void setCamelCaseNavigationEnabled(bool enabled);
    static void setCompletionShortcut(const QKeySequence &shortcut);

protected:
    // Custom behaviour can be added here.
    virtual void handleChanged(const QString &) {}
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void buttonClicked(FancyLineEdit::Side side);
    void leftButtonClicked();
    void rightButtonClicked();

    void filterChanged(const QString &);

    void validChanged(bool validState);
    void validReturnPressed();

protected:
    void resizeEvent(QResizeEvent *e) override;

    virtual QString fixInputString(const QString &string);

private:
    void iconClicked(FancyLineEdit::Side);

    static bool validateWithValidator(FancyLineEdit *edit, QString *errorMessage);
    // Unimplemented, to force the user to make a decision on
    // whether to use setHistoryCompleter() or setSpecialCompleter().
    void setCompleter(QCompleter *);

    void updateMargins();
    void updateButtonPositions();
    bool camelCaseBackward(bool mark);
    bool camelCaseForward(bool mark);
    friend class FancyLineEditPrivate;

    FancyLineEditPrivate *d;
};

} // namespace GUI
