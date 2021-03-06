/***************************************************************************
     qgsmultiedittoolbutton.cpp
     --------------------------
    Date                 : March 2016
    Copyright            : (C) 2016 Nyall Dawson
    Email                : nyall dot dawson at gmail.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsmultiedittoolbutton.h"
#include "qgsapplication.h"
#include <QMenu>
QgsMultiEditToolButton::QgsMultiEditToolButton( QWidget *parent )
  : QToolButton( parent )
{
  setFocusPolicy( Qt::StrongFocus );

  // set default tool button icon properties
  setFixedSize( 22, 22 );
  setStyleSheet( QStringLiteral( "QToolButton{ background: none; border: 1px solid rgba(0, 0, 0, 0%);} QToolButton:focus { border: 1px solid palette(highlight); }" ) );
  setIconSize( QSize( 16, 16 ) );
  setPopupMode( QToolButton::InstantPopup );

  mMenu = new QMenu( this );
  connect( mMenu, &QMenu::aboutToShow, this, &QgsMultiEditToolButton::aboutToShowMenu );
  setMenu( mMenu );

  // sets initial appearance
  updateState();
}

void QgsMultiEditToolButton::aboutToShowMenu()
{
  mMenu->clear();

  switch ( mState )
  {
    case Default:
    {
      QAction *noAction = mMenu->addAction( tr( "No Changes to Commit" ) );
      noAction->setEnabled( false );
      break;
    }
    case MixedValues:
    {
      QString title = !mField.name().isEmpty() ? tr( "Set %1 for All Selected Features" ).arg( mField.name() )
                      : tr( "Set field for all selected features" );
      QAction *setFieldAction = mMenu->addAction( title );
      connect( setFieldAction, &QAction::triggered, this, &QgsMultiEditToolButton::setFieldTriggered );
      break;
    }
    case Changed:
    {
      QAction *resetFieldAction = mMenu->addAction( tr( "Reset to Original Values" ) );
      connect( resetFieldAction, &QAction::triggered, this, &QgsMultiEditToolButton::resetFieldTriggered );
      break;
    }
  }
}

void QgsMultiEditToolButton::setFieldTriggered()
{
  mIsChanged = true;
  updateState();
  emit setFieldValueTriggered();
}

void QgsMultiEditToolButton::resetFieldTriggered()
{
  mIsChanged = false;
  updateState();
  emit resetFieldValueTriggered();
}

void QgsMultiEditToolButton::updateState()
{
  //changed state takes priority over mixed values state
  if ( mIsChanged )
    mState = Changed;
  else if ( mIsMixedValues )
    mState = MixedValues;
  else
    mState = Default;

  QIcon icon;
  QString tooltip;
  switch ( mState )
  {
    case Default:
      icon = QgsApplication::getThemeIcon( QStringLiteral( "/multieditSameValues.svg" ) );
      tooltip = tr( "All features in selection have equal value for '%1'" ).arg( mField.name() );
      break;
    case MixedValues:
      icon = QgsApplication::getThemeIcon( QStringLiteral( "/multieditMixedValues.svg" ) );
      tooltip = tr( "Some features in selection have different values for '%1'" ).arg( mField.name() );
      break;
    case Changed:
      icon = QgsApplication::getThemeIcon( QStringLiteral( "/multieditChangedValues.svg" ) );
      tooltip = tr( "Values for '%1' have unsaved changes" ).arg( mField.name() );
      break;
  }

  setIcon( icon );
  setToolTip( tooltip );
}
