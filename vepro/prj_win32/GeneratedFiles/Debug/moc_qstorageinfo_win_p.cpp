/****************************************************************************
** Meta object code from reading C++ file 'qstorageinfo_win_p.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../systeminfo/windows/qstorageinfo_win_p.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qstorageinfo_win_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QStorageInfoPrivate_t {
    QByteArrayData data[6];
    char stringdata[74];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_QStorageInfoPrivate_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_QStorageInfoPrivate_t qt_meta_stringdata_QStorageInfoPrivate = {
    {
QT_MOC_LITERAL(0, 0, 19),
QT_MOC_LITERAL(1, 20, 19),
QT_MOC_LITERAL(2, 40, 0),
QT_MOC_LITERAL(3, 41, 5),
QT_MOC_LITERAL(4, 47, 5),
QT_MOC_LITERAL(5, 53, 19)
    },
    "QStorageInfoPrivate\0logicalDriveChanged\0"
    "\0drive\0added\0notificationArrived\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QStorageInfoPrivate[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       5,    0,   29,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void QStorageInfoPrivate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QStorageInfoPrivate *_t = static_cast<QStorageInfoPrivate *>(_o);
        switch (_id) {
        case 0: _t->logicalDriveChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->notificationArrived(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QStorageInfoPrivate::*_t)(const QString & , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QStorageInfoPrivate::logicalDriveChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject QStorageInfoPrivate::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QStorageInfoPrivate.data,
      qt_meta_data_QStorageInfoPrivate,  qt_static_metacall, 0, 0}
};


const QMetaObject *QStorageInfoPrivate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QStorageInfoPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QStorageInfoPrivate.stringdata))
        return static_cast<void*>(const_cast< QStorageInfoPrivate*>(this));
    return QObject::qt_metacast(_clname);
}

int QStorageInfoPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QStorageInfoPrivate::logicalDriveChanged(const QString & _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
