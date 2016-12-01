/********************************************************************************
** Form generated from reading UI file 'reportwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REPORTWINDOW_H
#define UI_REPORTWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_ReportWindow
{
public:
    QPushButton *pushButton_save;
    QPushButton *pushButton_print;
    QTextEdit *textEdit;

    void setupUi(QDialog *ReportWindow)
    {
        if (ReportWindow->objectName().isEmpty())
            ReportWindow->setObjectName(QStringLiteral("ReportWindow"));
        ReportWindow->resize(650, 700);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ReportWindow->sizePolicy().hasHeightForWidth());
        ReportWindow->setSizePolicy(sizePolicy);
        pushButton_save = new QPushButton(ReportWindow);
        pushButton_save->setObjectName(QStringLiteral("pushButton_save"));
        pushButton_save->setGeometry(QRect(510, 5, 60, 30));
        pushButton_print = new QPushButton(ReportWindow);
        pushButton_print->setObjectName(QStringLiteral("pushButton_print"));
        pushButton_print->setGeometry(QRect(580, 5, 60, 30));
        textEdit = new QTextEdit(ReportWindow);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(10, 40, 631, 651));

        retranslateUi(ReportWindow);

        QMetaObject::connectSlotsByName(ReportWindow);
    } // setupUi

    void retranslateUi(QDialog *ReportWindow)
    {
        ReportWindow->setWindowTitle(QApplication::translate("ReportWindow", "ReportWindow", 0));
        pushButton_save->setText(QApplication::translate("ReportWindow", "\344\277\235\345\255\230", 0));
        pushButton_print->setText(QApplication::translate("ReportWindow", "\346\211\223\345\215\260", 0));
    } // retranslateUi

};

namespace Ui {
    class ReportWindow: public Ui_ReportWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REPORTWINDOW_H
