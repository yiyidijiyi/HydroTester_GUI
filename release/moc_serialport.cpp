/****************************************************************************
** Meta object code from reading C++ file 'serialport.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../serialport.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialport.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SerialPort_t {
    QByteArrayData data[16];
    char stringdata0[195];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialPort_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialPort_t qt_meta_stringdata_SerialPort = {
    {
QT_MOC_LITERAL(0, 0, 10), // "SerialPort"
QT_MOC_LITERAL(1, 11, 10), // "StartTimer"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 15), // "CurrentPressure"
QT_MOC_LITERAL(4, 39, 8), // "pressure"
QT_MOC_LITERAL(5, 48, 12), // "DataReceived"
QT_MOC_LITERAL(6, 61, 5), // "rxBuf"
QT_MOC_LITERAL(7, 67, 14), // "HandshakeState"
QT_MOC_LITERAL(8, 82, 17), // "STRUCT_HandShake*"
QT_MOC_LITERAL(9, 100, 9), // "handshake"
QT_MOC_LITERAL(10, 110, 19), // "DeviceStateReceived"
QT_MOC_LITERAL(11, 130, 19), // "STRUCT_DeviceState&"
QT_MOC_LITERAL(12, 150, 11), // "deviceState"
QT_MOC_LITERAL(13, 162, 11), // "OnReadyRead"
QT_MOC_LITERAL(14, 174, 12), // "OnStartTimer"
QT_MOC_LITERAL(15, 187, 7) // "OnTimer"

    },
    "SerialPort\0StartTimer\0\0CurrentPressure\0"
    "pressure\0DataReceived\0rxBuf\0HandshakeState\0"
    "STRUCT_HandShake*\0handshake\0"
    "DeviceStateReceived\0STRUCT_DeviceState&\0"
    "deviceState\0OnReadyRead\0OnStartTimer\0"
    "OnTimer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SerialPort[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    1,   55,    2, 0x06 /* Public */,
       5,    1,   58,    2, 0x06 /* Public */,
       7,    1,   61,    2, 0x06 /* Public */,
      10,    1,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    0,   67,    2, 0x0a /* Public */,
      14,    0,   68,    2, 0x0a /* Public */,
      15,    0,   69,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::QByteArray,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 11,   12,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SerialPort::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SerialPort *_t = static_cast<SerialPort *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->StartTimer(); break;
        case 1: _t->CurrentPressure((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->DataReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 3: _t->HandshakeState((*reinterpret_cast< STRUCT_HandShake*(*)>(_a[1]))); break;
        case 4: _t->DeviceStateReceived((*reinterpret_cast< STRUCT_DeviceState(*)>(_a[1]))); break;
        case 5: _t->OnReadyRead(); break;
        case 6: _t->OnStartTimer(); break;
        case 7: _t->OnTimer(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< STRUCT_HandShake* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SerialPort::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialPort::StartTimer)) {
                *result = 0;
            }
        }
        {
            typedef void (SerialPort::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialPort::CurrentPressure)) {
                *result = 1;
            }
        }
        {
            typedef void (SerialPort::*_t)(const QByteArray & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialPort::DataReceived)) {
                *result = 2;
            }
        }
        {
            typedef void (SerialPort::*_t)(STRUCT_HandShake * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialPort::HandshakeState)) {
                *result = 3;
            }
        }
        {
            typedef void (SerialPort::*_t)(STRUCT_DeviceState & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialPort::DeviceStateReceived)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject SerialPort::staticMetaObject = {
    { &QSerialPort::staticMetaObject, qt_meta_stringdata_SerialPort.data,
      qt_meta_data_SerialPort,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SerialPort::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialPort::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SerialPort.stringdata0))
        return static_cast<void*>(const_cast< SerialPort*>(this));
    return QSerialPort::qt_metacast(_clname);
}

int SerialPort::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSerialPort::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void SerialPort::StartTimer()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void SerialPort::CurrentPressure(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SerialPort::DataReceived(const QByteArray & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SerialPort::HandshakeState(STRUCT_HandShake * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SerialPort::DeviceStateReceived(STRUCT_DeviceState & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
