#ifndef ATTACHTABLESCREEN_H
#define ATTACHTABLESCREEN_H

#include <QWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QAction>

class AttachTableView;
class AttachTableController;


// Виджет

class AttachTableScreen : public QWidget
{
  Q_OBJECT
public:
  AttachTableScreen(QWidget *parent = 0);
  virtual ~AttachTableScreen();

signals:

public slots:

protected:

  void setupActions(void);
  void setupUI(void);
  void setupSignals(void);
  void assembly(void);


  // Контроллер таблицы приаттаченных файлов
  AttachTableController *attachTableController;

  // Таблица приаттаченных файлов (представление)
  AttachTableView *attachTableView;

  // Экранные элементы
  QToolBar *toolsLine;
  QVBoxLayout *screenLayout;
  QAction *actionAttachFile;
  QAction *actionEditFileName;
  QAction *actionDeleteFile;
  QAction *actionSwitchToEditor;

};

#endif // ATTACHTABLESCREEN_H