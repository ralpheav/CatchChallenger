/****************************************************************************
** Meta object code from reading C++ file 'PlayerUpdater.h'
**
** Created: Fri Oct 5 13:40:08 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../server/base/PlayerUpdater.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlayerUpdater.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Pokecraft__PlayerUpdater[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      44,   26,   25,   25, 0x05,
      71,   25,   25,   25, 0x05,
      97,   25,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
     126,   25,   25,   25, 0x0a,
     147,   25,   25,   25, 0x0a,
     171,   25,   25,   25, 0x08,
     201,   25,   25,   25, 0x08,
     234,   25,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Pokecraft__PlayerUpdater[] = {
    "Pokecraft::PlayerUpdater\0\0connected_players\0"
    "newConnectedPlayer(qint32)\0"
    "send_addConnectedPlayer()\0"
    "send_removeConnectedPlayer()\0"
    "addConnectedPlayer()\0removeConnectedPlayer()\0"
    "internal_addConnectedPlayer()\0"
    "internal_removeConnectedPlayer()\0"
    "send_timer()\0"
};

void Pokecraft::PlayerUpdater::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PlayerUpdater *_t = static_cast<PlayerUpdater *>(_o);
        switch (_id) {
        case 0: _t->newConnectedPlayer((*reinterpret_cast< qint32(*)>(_a[1]))); break;
        case 1: _t->send_addConnectedPlayer(); break;
        case 2: _t->send_removeConnectedPlayer(); break;
        case 3: _t->addConnectedPlayer(); break;
        case 4: _t->removeConnectedPlayer(); break;
        case 5: _t->internal_addConnectedPlayer(); break;
        case 6: _t->internal_removeConnectedPlayer(); break;
        case 7: _t->send_timer(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Pokecraft::PlayerUpdater::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pokecraft::PlayerUpdater::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Pokecraft__PlayerUpdater,
      qt_meta_data_Pokecraft__PlayerUpdater, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pokecraft::PlayerUpdater::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pokecraft::PlayerUpdater::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pokecraft::PlayerUpdater::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pokecraft__PlayerUpdater))
        return static_cast<void*>(const_cast< PlayerUpdater*>(this));
    return QObject::qt_metacast(_clname);
}

int Pokecraft::PlayerUpdater::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Pokecraft::PlayerUpdater::newConnectedPlayer(qint32 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Pokecraft::PlayerUpdater::send_addConnectedPlayer()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Pokecraft::PlayerUpdater::send_removeConnectedPlayer()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE