/****************************************************************************
** Meta object code from reading C++ file 'QFakeSocket.h'
**
** Created: Fri Oct 5 13:40:08 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../general/base/QFakeSocket.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QFakeSocket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Pokecraft__QFakeSocket[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x05,
      36,   23,   23,   23, 0x05,
      63,   51,   23,   23, 0x05,
     111,   99,   23,   23, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Pokecraft__QFakeSocket[] = {
    "Pokecraft::QFakeSocket\0\0connected()\0"
    "disconnected()\0socketError\0"
    "error(QAbstractSocket::SocketError)\0"
    "socketState\0stateChanged(QAbstractSocket::SocketState)\0"
};

void Pokecraft::QFakeSocket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QFakeSocket *_t = static_cast<QFakeSocket *>(_o);
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 3: _t->stateChanged((*reinterpret_cast< QAbstractSocket::SocketState(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Pokecraft::QFakeSocket::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pokecraft::QFakeSocket::staticMetaObject = {
    { &QIODevice::staticMetaObject, qt_meta_stringdata_Pokecraft__QFakeSocket,
      qt_meta_data_Pokecraft__QFakeSocket, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pokecraft::QFakeSocket::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pokecraft::QFakeSocket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pokecraft::QFakeSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pokecraft__QFakeSocket))
        return static_cast<void*>(const_cast< QFakeSocket*>(this));
    return QIODevice::qt_metacast(_clname);
}

int Pokecraft::QFakeSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QIODevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Pokecraft::QFakeSocket::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Pokecraft::QFakeSocket::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Pokecraft::QFakeSocket::error(QAbstractSocket::SocketError _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Pokecraft::QFakeSocket::stateChanged(QAbstractSocket::SocketState _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE