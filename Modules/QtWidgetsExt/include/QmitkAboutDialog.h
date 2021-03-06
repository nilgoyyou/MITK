/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef QmitkAboutDialog_h
#define QmitkAboutDialog_h

#include "MitkQtWidgetsExtExports.h"
#include <ui_QmitkAboutDialogGUI.h>

class MITKQTWIDGETSEXT_EXPORT QmitkAboutDialog : public QDialog
{
  Q_OBJECT

public:
  QmitkAboutDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
  ~QmitkAboutDialog() override;

  QString GetAboutText() const;
  QString GetCaptionText() const;
  QString GetRevisionText() const;

  void SetAboutText(const QString &text);
  void SetCaptionText(const QString &text);
  void SetRevisionText(const QString &text);

protected slots:
  void ShowModules();

private:
  Ui::QmitkAboutDialog m_GUI;
};

#endif
