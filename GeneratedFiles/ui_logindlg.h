/********************************************************************************
** Form generated from reading UI file 'logindlg.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDLG_H
#define UI_LOGINDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_loginDlg
{
public:
    QPushButton *pushButton_login;
    QLabel *label_login;
    QLabel *label;
    QLabel *label_2;
    QPushButton *pushButton_cancel;
    QPushButton *pushButton_developer;
    QPushButton *pushButton_admin;
    QPushButton *pushButton_tester;
    QLineEdit *lineEdit_name;
    QLineEdit *lineEdit_password;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_copyright;
    QLabel *label_account;
    QLabel *label_message;

    void setupUi(QDialog *loginDlg)
    {
        if (loginDlg->objectName().isEmpty())
            loginDlg->setObjectName(QStringLiteral("loginDlg"));
        loginDlg->resize(1366, 728);
        pushButton_login = new QPushButton(loginDlg);
        pushButton_login->setObjectName(QStringLiteral("pushButton_login"));
        pushButton_login->setGeometry(QRect(360, 489, 260, 60));
        pushButton_login->setMinimumSize(QSize(260, 60));
        QFont font;
        font.setFamily(QStringLiteral("SimSun-ExtB"));
        font.setPointSize(9);
        font.setBold(false);
        font.setWeight(50);
        pushButton_login->setFont(font);
        label_login = new QLabel(loginDlg);
        label_login->setObjectName(QStringLiteral("label_login"));
        label_login->setGeometry(QRect(280, 159, 812, 487));
        label_login->setPixmap(QPixmap(QString::fromUtf8("resource/login/dlg.png")));
        label = new QLabel(loginDlg);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(290, 179, 448, 56));
        label->setPixmap(QPixmap(QString::fromUtf8("resource/login/logo.png")));
        label_2 = new QLabel(loginDlg);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(360, 319, 110, 109));
        label_2->setPixmap(QPixmap(QString::fromUtf8("resource/login/head.png")));
        pushButton_cancel = new QPushButton(loginDlg);
        pushButton_cancel->setObjectName(QStringLiteral("pushButton_cancel"));
        pushButton_cancel->setGeometry(QRect(750, 489, 260, 60));
        pushButton_cancel->setMinimumSize(QSize(260, 60));
        pushButton_developer = new QPushButton(loginDlg);
        pushButton_developer->setObjectName(QStringLiteral("pushButton_developer"));
        pushButton_developer->setGeometry(QRect(780, 210, 83, 27));
        pushButton_developer->setMinimumSize(QSize(83, 27));
        pushButton_admin = new QPushButton(loginDlg);
        pushButton_admin->setObjectName(QStringLiteral("pushButton_admin"));
        pushButton_admin->setGeometry(QRect(870, 210, 83, 27));
        pushButton_admin->setMinimumSize(QSize(83, 27));
        pushButton_tester = new QPushButton(loginDlg);
        pushButton_tester->setObjectName(QStringLiteral("pushButton_tester"));
        pushButton_tester->setGeometry(QRect(960, 210, 83, 27));
        pushButton_tester->setMinimumSize(QSize(83, 27));
        lineEdit_name = new QLineEdit(loginDlg);
        lineEdit_name->setObjectName(QStringLiteral("lineEdit_name"));
        lineEdit_name->setGeometry(QRect(780, 320, 256, 37));
        lineEdit_name->setMinimumSize(QSize(256, 37));
        lineEdit_name->setInputMethodHints(Qt::ImhNone);
        lineEdit_name->setMaxLength(128);
        lineEdit_password = new QLineEdit(loginDlg);
        lineEdit_password->setObjectName(QStringLiteral("lineEdit_password"));
        lineEdit_password->setGeometry(QRect(780, 390, 256, 37));
        lineEdit_password->setMinimumSize(QSize(256, 37));
        lineEdit_password->setMaxLength(32767);
        label_3 = new QLabel(loginDlg);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(730, 390, 29, 29));
        label_3->setMinimumSize(QSize(29, 29));
        label_3->setMaximumSize(QSize(29, 16777215));
        label_3->setPixmap(QPixmap(QString::fromUtf8("resource/login/icon1.png")));
        label_4 = new QLabel(loginDlg);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(730, 320, 29, 29));
        label_4->setMinimumSize(QSize(29, 29));
        label_4->setMaximumSize(QSize(29, 16777215));
        label_4->setPixmap(QPixmap(QString::fromUtf8("resource/login/icon3.png")));
        label_5 = new QLabel(loginDlg);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(670, 319, 10, 109));
        label_5->setMinimumSize(QSize(10, 109));
        label_5->setMaximumSize(QSize(2, 16777215));
        label_5->setPixmap(QPixmap(QString::fromUtf8("resource/login/split.png")));
        label_copyright = new QLabel(loginDlg);
        label_copyright->setObjectName(QStringLiteral("label_copyright"));
        label_copyright->setGeometry(QRect(510, 700, 421, 16));
        QFont font1;
        font1.setPointSize(9);
        label_copyright->setFont(font1);
        label_account = new QLabel(loginDlg);
        label_account->setObjectName(QStringLiteral("label_account"));
        label_account->setGeometry(QRect(530, 320, 120, 40));
        label_account->setMinimumSize(QSize(120, 40));
        label_message = new QLabel(loginDlg);
        label_message->setObjectName(QStringLiteral("label_message"));
        label_message->setGeometry(QRect(780, 445, 256, 30));
        label_login->raise();
        pushButton_login->raise();
        label->raise();
        label_2->raise();
        pushButton_cancel->raise();
        pushButton_developer->raise();
        pushButton_admin->raise();
        pushButton_tester->raise();
        lineEdit_name->raise();
        lineEdit_password->raise();
        label_3->raise();
        label_4->raise();
        label_5->raise();
        label_copyright->raise();
        label_account->raise();
        label_message->raise();

        retranslateUi(loginDlg);

        QMetaObject::connectSlotsByName(loginDlg);
    } // setupUi

    void retranslateUi(QDialog *loginDlg)
    {
        loginDlg->setWindowTitle(QApplication::translate("loginDlg", "Dialog", 0));
        pushButton_login->setText(QApplication::translate("loginDlg", "\347\231\273\351\231\206", 0));
        label_login->setText(QString());
        label->setText(QString());
        label_2->setText(QString());
        pushButton_cancel->setText(QApplication::translate("loginDlg", "\345\217\226\346\266\210", 0));
        pushButton_developer->setText(QApplication::translate("loginDlg", "\345\274\200\345\217\221\350\200\205", 0));
        pushButton_admin->setText(QApplication::translate("loginDlg", "\347\256\241\347\220\206\345\221\230", 0));
        pushButton_tester->setText(QApplication::translate("loginDlg", "\346\265\213\350\257\225\345\221\230", 0));
        lineEdit_name->setInputMask(QString());
        lineEdit_password->setInputMask(QString());
        label_3->setText(QString());
        label_4->setText(QString());
        label_5->setText(QString());
        label_copyright->setText(QApplication::translate("loginDlg", "Copyright(C)2015 NingBo Textile Instrument Factory", 0));
        label_account->setText(QApplication::translate("loginDlg", "\347\256\241\347\220\206\345\221\230", 0));
        label_message->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class loginDlg: public Ui_loginDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDLG_H
