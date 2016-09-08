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
    QByteArrayData data[19];
    char stringdata0[336];
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
QT_MOC_LITERAL(9, 110, 25), // "OnBtnTestInterfaceClicked"
QT_MOC_LITERAL(10, 136, 22), // "OnBtnTestMethodClicked"
QT_MOC_LITERAL(11, 159, 23), // "OnBtnReportQueryClicked"
QT_MOC_LITERAL(12, 183, 19), // "OnBtnAdvanceClicked"
QT_MOC_LITERAL(13, 203, 16), // "OnBtnHelpClicked"
QT_MOC_LITERAL(14, 220, 22), // "OnBtnChartVideoClicked"
QT_MOC_LITERAL(15, 243, 22), // "OnBtnChartCurveClicked"
QT_MOC_LITERAL(16, 266, 22), // "OnBtnChartChartClicked"
QT_MOC_LITERAL(17, 289, 23), // "OnBtnChartReportClicked"
QT_MOC_LITERAL(18, 313, 22) // "OnBtnChartPrintClicked"

    },
    "Widget\0OnBtnMinClicked\0\0OnBtnCloseClicked\0"
    "mouseMoveEvent\0QMouseEvent*\0event\0"
    "mousePressEvent\0mouseReleaseEvent\0"
    "OnBtnTestInterfaceClicked\0"
    "OnBtnTestMethodClicked\0OnBtnReportQueryClicked\0"
    "OnBtnAdvanceClicked\0OnBtnHelpClicked\0"
    "OnBtnChartVideoClicked\0OnBtnChartCurveClicked\0"
    "OnBtnChartChartClicked\0OnBtnChartReportClicked\0"
    "OnBtnChartPrintClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x09 /* Protected */,
       3,    0,   90,    2, 0x09 /* Protected */,
       4,    1,   91,    2, 0x09 /* Protected */,
       7,    1,   94,    2, 0x09 /* Protected */,
       8,    1,   97,    2, 0x09 /* Protected */,
       9,    0,  100,    2, 0x0a /* Public */,
      10,    0,  101,    2, 0x0a /* Public */,
      11,    0,  102,    2, 0x0a /* Public */,
      12,    0,  103,    2, 0x0a /* Public */,
      13,    0,  104,    2, 0x0a /* Public */,
      14,    0,  105,    2, 0x0a /* Public */,
      15,    0,  106,    2, 0x0a /* Public */,
      16,    0,  107,    2, 0x0a /* Public */,
      17,    0,  108,    2, 0x0a /* Public */,
      18,    0,  109,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

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
        case 5: _t->OnBtnTestInterfaceClicked(); break;
        case 6: _t->OnBtnTestMethodClicked(); break;
        case 7: _t->OnBtnReportQueryClicked(); break;
        case 8: _t->OnBtnAdvanceClicked(); break;
        case 9: _t->OnBtnHelpClicked(); break;
        case 10: _t->OnBtnChartVideoClicked(); break;
        case 11: _t->OnBtnChartCurveClicked(); break;
        case 12: _t->OnBtnChartChartClicked(); break;
        case 13: _t->OnBtnChartReportClicked(); break;
        case 14: _t->OnBtnChartPrintClicked(); break;
        default: ;
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
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
