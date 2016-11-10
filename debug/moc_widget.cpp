/****************************************************************************
** Meta object code from reading C++ file 'widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Widget_t {
    QByteArrayData data[55];
    char stringdata0[1037];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widget_t qt_meta_stringdata_Widget = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Widget"
QT_MOC_LITERAL(1, 7, 15), // "OnBtnMinClicked"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 17), // "OnBtnCloseClicked"
QT_MOC_LITERAL(4, 42, 14), // "mouseMoveEvent"
QT_MOC_LITERAL(5, 57, 12), // "QMouseEvent*"
QT_MOC_LITERAL(6, 70, 5), // "event"
QT_MOC_LITERAL(7, 76, 15), // "mousePressEvent"
QT_MOC_LITERAL(8, 92, 17), // "mouseReleaseEvent"
QT_MOC_LITERAL(9, 110, 15), // "OnLoginAccepted"
QT_MOC_LITERAL(10, 126, 2), // "id"
QT_MOC_LITERAL(11, 129, 22), // "OnBtnOpenCameraClicked"
QT_MOC_LITERAL(12, 152, 17), // "OnBtnComOpClicked"
QT_MOC_LITERAL(13, 170, 19), // "OnBtnWaterInClicked"
QT_MOC_LITERAL(14, 190, 21), // "OnBtnStartTestClicked"
QT_MOC_LITERAL(15, 212, 21), // "OnBtnPauseTestClicked"
QT_MOC_LITERAL(16, 234, 20), // "OnBtnWaterOffClicked"
QT_MOC_LITERAL(17, 255, 16), // "OnRxDataReceived"
QT_MOC_LITERAL(18, 272, 5), // "rxBuf"
QT_MOC_LITERAL(19, 278, 24), // "OnHandShakeStateReceived"
QT_MOC_LITERAL(20, 303, 17), // "STRUCT_HandShake*"
QT_MOC_LITERAL(21, 321, 9), // "handshake"
QT_MOC_LITERAL(22, 331, 21), // "OnBtnSaveCurveClicked"
QT_MOC_LITERAL(23, 353, 25), // "OnBtnTestInterfaceClicked"
QT_MOC_LITERAL(24, 379, 22), // "OnBtnTestMethodClicked"
QT_MOC_LITERAL(25, 402, 23), // "OnBtnReportQueryClicked"
QT_MOC_LITERAL(26, 426, 19), // "OnBtnAdvanceClicked"
QT_MOC_LITERAL(27, 446, 16), // "OnBtnHelpClicked"
QT_MOC_LITERAL(28, 463, 22), // "OnBtnChartVideoClicked"
QT_MOC_LITERAL(29, 486, 22), // "OnBtnChartCurveClicked"
QT_MOC_LITERAL(30, 509, 23), // "OnBtnChartReportClicked"
QT_MOC_LITERAL(31, 533, 22), // "OnCombSelMethodChanged"
QT_MOC_LITERAL(32, 556, 5), // "index"
QT_MOC_LITERAL(33, 562, 23), // "OnBtnPrintReportClicked"
QT_MOC_LITERAL(34, 586, 23), // "OnMethodListItemClicked"
QT_MOC_LITERAL(35, 610, 23), // "OnCombMethodPlanChanged"
QT_MOC_LITERAL(36, 634, 25), // "OnCombPressureUnitChanged"
QT_MOC_LITERAL(37, 660, 21), // "OnBtnNewMethodClicked"
QT_MOC_LITERAL(38, 682, 22), // "OnBtnSaveMethodClicked"
QT_MOC_LITERAL(39, 705, 24), // "OnBtnDeleteMethodClicked"
QT_MOC_LITERAL(40, 730, 24), // "OnBtnModifyMethodClicked"
QT_MOC_LITERAL(41, 755, 17), // "OnBtnQueryClicked"
QT_MOC_LITERAL(42, 773, 28), // "OnBtnDeleteReportListClicked"
QT_MOC_LITERAL(43, 802, 24), // "OnAccountListItemClicked"
QT_MOC_LITERAL(44, 827, 22), // "OnBtnNewAccountClicked"
QT_MOC_LITERAL(45, 850, 23), // "OnBtnSaveAccountClicked"
QT_MOC_LITERAL(46, 874, 25), // "OnBtnDeleteAccountClicked"
QT_MOC_LITERAL(47, 900, 25), // "OnBtnModifyAccountClicked"
QT_MOC_LITERAL(48, 926, 15), // "OnImagePrepared"
QT_MOC_LITERAL(49, 942, 7), // "OnTimer"
QT_MOC_LITERAL(50, 950, 19), // "OnUpdateDeviceState"
QT_MOC_LITERAL(51, 970, 19), // "STRUCT_DeviceState&"
QT_MOC_LITERAL(52, 990, 11), // "deviceState"
QT_MOC_LITERAL(53, 1002, 25), // "OnCurrentPressureReceived"
QT_MOC_LITERAL(54, 1028, 8) // "pressure"

    },
    "Widget\0OnBtnMinClicked\0\0OnBtnCloseClicked\0"
    "mouseMoveEvent\0QMouseEvent*\0event\0"
    "mousePressEvent\0mouseReleaseEvent\0"
    "OnLoginAccepted\0id\0OnBtnOpenCameraClicked\0"
    "OnBtnComOpClicked\0OnBtnWaterInClicked\0"
    "OnBtnStartTestClicked\0OnBtnPauseTestClicked\0"
    "OnBtnWaterOffClicked\0OnRxDataReceived\0"
    "rxBuf\0OnHandShakeStateReceived\0"
    "STRUCT_HandShake*\0handshake\0"
    "OnBtnSaveCurveClicked\0OnBtnTestInterfaceClicked\0"
    "OnBtnTestMethodClicked\0OnBtnReportQueryClicked\0"
    "OnBtnAdvanceClicked\0OnBtnHelpClicked\0"
    "OnBtnChartVideoClicked\0OnBtnChartCurveClicked\0"
    "OnBtnChartReportClicked\0OnCombSelMethodChanged\0"
    "index\0OnBtnPrintReportClicked\0"
    "OnMethodListItemClicked\0OnCombMethodPlanChanged\0"
    "OnCombPressureUnitChanged\0"
    "OnBtnNewMethodClicked\0OnBtnSaveMethodClicked\0"
    "OnBtnDeleteMethodClicked\0"
    "OnBtnModifyMethodClicked\0OnBtnQueryClicked\0"
    "OnBtnDeleteReportListClicked\0"
    "OnAccountListItemClicked\0"
    "OnBtnNewAccountClicked\0OnBtnSaveAccountClicked\0"
    "OnBtnDeleteAccountClicked\0"
    "OnBtnModifyAccountClicked\0OnImagePrepared\0"
    "OnTimer\0OnUpdateDeviceState\0"
    "STRUCT_DeviceState&\0deviceState\0"
    "OnCurrentPressureReceived\0pressure"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      43,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  229,    2, 0x09 /* Protected */,
       3,    0,  230,    2, 0x09 /* Protected */,
       4,    1,  231,    2, 0x09 /* Protected */,
       7,    1,  234,    2, 0x09 /* Protected */,
       8,    1,  237,    2, 0x09 /* Protected */,
       9,    1,  240,    2, 0x0a /* Public */,
      11,    0,  243,    2, 0x0a /* Public */,
      12,    0,  244,    2, 0x0a /* Public */,
      13,    0,  245,    2, 0x0a /* Public */,
      14,    0,  246,    2, 0x0a /* Public */,
      15,    0,  247,    2, 0x0a /* Public */,
      16,    0,  248,    2, 0x0a /* Public */,
      17,    1,  249,    2, 0x0a /* Public */,
      19,    1,  252,    2, 0x0a /* Public */,
      22,    0,  255,    2, 0x0a /* Public */,
      23,    0,  256,    2, 0x0a /* Public */,
      24,    0,  257,    2, 0x0a /* Public */,
      25,    0,  258,    2, 0x0a /* Public */,
      26,    0,  259,    2, 0x0a /* Public */,
      27,    0,  260,    2, 0x0a /* Public */,
      28,    0,  261,    2, 0x0a /* Public */,
      29,    0,  262,    2, 0x0a /* Public */,
      30,    0,  263,    2, 0x0a /* Public */,
      31,    1,  264,    2, 0x0a /* Public */,
      33,    0,  267,    2, 0x0a /* Public */,
      34,    1,  268,    2, 0x0a /* Public */,
      35,    1,  271,    2, 0x0a /* Public */,
      36,    1,  274,    2, 0x0a /* Public */,
      37,    0,  277,    2, 0x0a /* Public */,
      38,    0,  278,    2, 0x0a /* Public */,
      39,    0,  279,    2, 0x0a /* Public */,
      40,    0,  280,    2, 0x0a /* Public */,
      41,    0,  281,    2, 0x0a /* Public */,
      42,    0,  282,    2, 0x0a /* Public */,
      43,    1,  283,    2, 0x0a /* Public */,
      44,    0,  286,    2, 0x0a /* Public */,
      45,    0,  287,    2, 0x0a /* Public */,
      46,    0,  288,    2, 0x0a /* Public */,
      47,    0,  289,    2, 0x0a /* Public */,
      48,    0,  290,    2, 0x0a /* Public */,
      49,    0,  291,    2, 0x0a /* Public */,
      50,    1,  292,    2, 0x0a /* Public */,
      53,    1,  295,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,   18,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   32,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,   32,
    QMetaType::Void, QMetaType::Int,   32,
    QMetaType::Void, QMetaType::Int,   32,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,   32,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 51,   52,
    QMetaType::Void, QMetaType::Int,   54,

       0        // eod
};

void Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Widget *_t = static_cast<Widget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnBtnMinClicked(); break;
        case 1: _t->OnBtnCloseClicked(); break;
        case 2: _t->mouseMoveEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 3: _t->mousePressEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 4: _t->mouseReleaseEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 5: _t->OnLoginAccepted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->OnBtnOpenCameraClicked(); break;
        case 7: _t->OnBtnComOpClicked(); break;
        case 8: _t->OnBtnWaterInClicked(); break;
        case 9: _t->OnBtnStartTestClicked(); break;
        case 10: _t->OnBtnPauseTestClicked(); break;
        case 11: _t->OnBtnWaterOffClicked(); break;
        case 12: _t->OnRxDataReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 13: _t->OnHandShakeStateReceived((*reinterpret_cast< STRUCT_HandShake*(*)>(_a[1]))); break;
        case 14: _t->OnBtnSaveCurveClicked(); break;
        case 15: _t->OnBtnTestInterfaceClicked(); break;
        case 16: _t->OnBtnTestMethodClicked(); break;
        case 17: _t->OnBtnReportQueryClicked(); break;
        case 18: _t->OnBtnAdvanceClicked(); break;
        case 19: _t->OnBtnHelpClicked(); break;
        case 20: _t->OnBtnChartVideoClicked(); break;
        case 21: _t->OnBtnChartCurveClicked(); break;
        case 22: _t->OnBtnChartReportClicked(); break;
        case 23: _t->OnCombSelMethodChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: _t->OnBtnPrintReportClicked(); break;
        case 25: _t->OnMethodListItemClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 26: _t->OnCombMethodPlanChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: _t->OnCombPressureUnitChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: _t->OnBtnNewMethodClicked(); break;
        case 29: _t->OnBtnSaveMethodClicked(); break;
        case 30: _t->OnBtnDeleteMethodClicked(); break;
        case 31: _t->OnBtnModifyMethodClicked(); break;
        case 32: _t->OnBtnQueryClicked(); break;
        case 33: _t->OnBtnDeleteReportListClicked(); break;
        case 34: _t->OnAccountListItemClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 35: _t->OnBtnNewAccountClicked(); break;
        case 36: _t->OnBtnSaveAccountClicked(); break;
        case 37: _t->OnBtnDeleteAccountClicked(); break;
        case 38: _t->OnBtnModifyAccountClicked(); break;
        case 39: _t->OnImagePrepared(); break;
        case 40: _t->OnTimer(); break;
        case 41: _t->OnUpdateDeviceState((*reinterpret_cast< STRUCT_DeviceState(*)>(_a[1]))); break;
        case 42: _t->OnCurrentPressureReceived((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< STRUCT_HandShake* >(); break;
            }
            break;
        }
    }
}

const QMetaObject Widget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Widget.data,
      qt_meta_data_Widget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Widget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Widget.stringdata0))
        return static_cast<void*>(const_cast< Widget*>(this));
    return QWidget::qt_metacast(_clname);
}

int Widget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 43)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 43;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 43)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 43;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
