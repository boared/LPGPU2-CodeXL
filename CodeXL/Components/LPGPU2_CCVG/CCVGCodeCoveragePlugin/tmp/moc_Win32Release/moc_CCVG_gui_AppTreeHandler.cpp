/****************************************************************************
** Meta object code from reading C++ file 'CCVG_gui_AppTreeHandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Include/CCVG_gui_AppTreeHandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CCVG_gui_AppTreeHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ccvg__gui__CCVGGUIAppTreeHandler_t {
    QByteArrayData data[8];
    char stringdata0[223];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ccvg__gui__CCVGGUIAppTreeHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ccvg__gui__CCVGGUIAppTreeHandler_t qt_meta_stringdata_ccvg__gui__CCVGGUIAppTreeHandler = {
    {
QT_MOC_LITERAL(0, 0, 32), // "ccvg::gui::CCVGGUIAppTreeHandler"
QT_MOC_LITERAL(1, 33, 24), // "OnApplicationTreeCreated"
QT_MOC_LITERAL(2, 58, 0), // ""
QT_MOC_LITERAL(3, 59, 33), // "ActionDoRefreshCCVGItemsReadF..."
QT_MOC_LITERAL(4, 93, 35), // "ActionDoCCVGItemViewCoverageR..."
QT_MOC_LITERAL(5, 129, 33), // "ActionDoCCVGItemDriverSrcCode..."
QT_MOC_LITERAL(6, 163, 30), // "ActionDoCCVGItemResultFileView"
QT_MOC_LITERAL(7, 194, 28) // "ActionDoCCVGItemExecFileView"

    },
    "ccvg::gui::CCVGGUIAppTreeHandler\0"
    "OnApplicationTreeCreated\0\0"
    "ActionDoRefreshCCVGItemsReadFiles\0"
    "ActionDoCCVGItemViewCoverageResults\0"
    "ActionDoCCVGItemDriverSrcCodeView\0"
    "ActionDoCCVGItemResultFileView\0"
    "ActionDoCCVGItemExecFileView"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ccvg__gui__CCVGGUIAppTreeHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x08 /* Private */,
       3,    0,   45,    2, 0x08 /* Private */,
       4,    0,   46,    2, 0x08 /* Private */,
       5,    0,   47,    2, 0x08 /* Private */,
       6,    0,   48,    2, 0x08 /* Private */,
       7,    0,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ccvg::gui::CCVGGUIAppTreeHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CCVGGUIAppTreeHandler *_t = static_cast<CCVGGUIAppTreeHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnApplicationTreeCreated(); break;
        case 1: _t->ActionDoRefreshCCVGItemsReadFiles(); break;
        case 2: _t->ActionDoCCVGItemViewCoverageResults(); break;
        case 3: _t->ActionDoCCVGItemDriverSrcCodeView(); break;
        case 4: _t->ActionDoCCVGItemResultFileView(); break;
        case 5: _t->ActionDoCCVGItemExecFileView(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject ccvg::gui::CCVGGUIAppTreeHandler::staticMetaObject = {
    { &afApplicationTreeHandler::staticMetaObject, qt_meta_stringdata_ccvg__gui__CCVGGUIAppTreeHandler.data,
      qt_meta_data_ccvg__gui__CCVGGUIAppTreeHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ccvg::gui::CCVGGUIAppTreeHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ccvg::gui::CCVGGUIAppTreeHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ccvg__gui__CCVGGUIAppTreeHandler.stringdata0))
        return static_cast<void*>(const_cast< CCVGGUIAppTreeHandler*>(this));
    if (!strcmp(_clname, "apIEventsObserver"))
        return static_cast< apIEventsObserver*>(const_cast< CCVGGUIAppTreeHandler*>(this));
    if (!strcmp(_clname, "CCVGClassCmnBase"))
        return static_cast< CCVGClassCmnBase*>(const_cast< CCVGGUIAppTreeHandler*>(this));
    if (!strcmp(_clname, "fw::CCVGFWTheCurrentStateProjectData::NotifyOnProjectDataChanged"))
        return static_cast< fw::CCVGFWTheCurrentStateProjectData::NotifyOnProjectDataChanged*>(const_cast< CCVGGUIAppTreeHandler*>(this));
    if (!strcmp(_clname, "fw::CCVGFWTheCurrentState::NotifyOnSettingChangedCcvgAllProjectsEnabledFlag"))
        return static_cast< fw::CCVGFWTheCurrentState::NotifyOnSettingChangedCcvgAllProjectsEnabledFlag*>(const_cast< CCVGGUIAppTreeHandler*>(this));
    if (!strcmp(_clname, "CCVGGUIProjectSettings::NotifyOnProjectClose"))
        return static_cast< CCVGGUIProjectSettings::NotifyOnProjectClose*>(const_cast< CCVGGUIAppTreeHandler*>(this));
    if (!strcmp(_clname, "ISingleton<CCVGGUIAppTreeHandler>"))
        return static_cast< ISingleton<CCVGGUIAppTreeHandler>*>(const_cast< CCVGGUIAppTreeHandler*>(this));
    return afApplicationTreeHandler::qt_metacast(_clname);
}

int ccvg::gui::CCVGGUIAppTreeHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = afApplicationTreeHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
