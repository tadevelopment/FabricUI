/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef _FABRICUI_UTIL_QT_SIGNALS_SLOTS_H
#define _FABRICUI_UTIL_QT_SIGNALS_SLOTS_H

#include <QObject>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#define QOBJECT_CONNECT( objA, ClassA, methodA, ArgsA, objB, ClassB, methodB, ArgsB ) \
    QObject::connect( objA, & ClassA :: methodA, objB, & ClassB :: methodB );
#else
#define QOBJECT_CONNECT( objA, ClassA, methodA, ArgsA, objB, ClassB, methodB, ArgsB ) \
    QObject::connect( objA, SIGNAL( methodA( Args A ) ), objB, SLOT( methodB( ArgsB ) ) );
#endif

#endif //_FABRICUI_UTIL_QT_SIGNALS_SLOTS_H
