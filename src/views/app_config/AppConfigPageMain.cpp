#include <QString>
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>
#include <QAbstractSpinBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QCommonStyle>

#include "AppConfigPageMain.h"
#include "models/app_config/AppConfig.h"
#include "libraries/FlatControl.h"

extern AppConfig appconfig;


AppConfigPageMain::AppConfigPageMain(QWidget *parent) : ConfigPage(parent)
{
    setup_ui();
    setup_signals();
    assembly();
}


void AppConfigPageMain::setup_ui(void)
{
    qDebug() << "Create main config page";

    // Блок работы с путем до каталога данных
    tetradirLabel = new QLabel(this);
    tetradirLabel->setText(tr("Data directory"));

    tetradirInput = new QLineEdit(this);
    tetradirInput->setMinimumWidth(50);
    tetradirInput->setText(appconfig.get_tetradir());

    tetradirButton = new FlatToolButton(this);
    tetradirButton->setText(tr("..."));


    // Блок работы с путем до корзины
    trashdirLabel = new QLabel(this);
    trashdirLabel->setText(tr("Trash directory"));

    trashdirInput = new QLineEdit(this);
    trashdirInput->setMinimumWidth(50);
    trashdirInput->setText(appconfig.get_trashdir());

    trashdirButton = new FlatToolButton(this);
    trashdirButton->setText(tr("..."));


    // Блок работы с размером корзины
    trashsizeLabel = new QLabel(this);
    trashsizeLabel->setText(tr("Trash size"));

    trashsizeInput = new QSpinBox(this);
    trashsizeInput->setRange(1, 1000);
    trashsizeInput->setValue(appconfig.get_trashsize());

    trashsizeFlexion = new QLabel(this);
    trashsizeFlexion->setText(tr("Mb"));


    // Блок работы с максимальным количеством файлов в корзине
    trashmaxfilecountLabel = new QLabel(this);
    trashmaxfilecountLabel->setText(tr("Maximum files in trash"));

    trashmaxfilecountInput = new QSpinBox(this);
    trashmaxfilecountInput->setRange(1, 3000);
    trashmaxfilecountInput->setValue(appconfig.get_trashmaxfilecount());

    trashmaxfilecountFlexion = new QLabel(this);
    trashmaxfilecountFlexion->setText(tr("files"));


    // Блок работы с выбором языка интерфейса
    interfaceLanguageLabel = new QLabel(this);
    interfaceLanguageLabel->setText(tr("Language"));

    interfaceLanguage = new MtComboBox(this);
    interfaceLanguage->setMinimumContentsLength(2);
    interfaceLanguage->addItem("en");
    interfaceLanguage->addItem("ru");
    interfaceLanguage->setCurrentIndex(interfaceLanguage->findText(appconfig.get_interfacelanguage(), Qt::MatchCaseSensitive));


    // Блок работы с отображением даты и времени
    dateTimeFormatBox = new QGroupBox(this);
    dateTimeFormatBox->setTitle(tr("Date and time show settings"));

    disableCustomDateTimeFormat = new QRadioButton(tr("Show date and time by OS locale settings"));
    enableCustomDateTimeFormat = new QRadioButton(tr("Custom date and time show format"));
    customDateTimeFormat = new QLineEdit();
    dateTimeFormatHelpButton = new FlatToolButton();
    QCommonStyle styleHelp;
    dateTimeFormatHelpButton->setIcon(styleHelp.standardIcon(QStyle::SP_MessageBoxQuestion));

    // Точка устанавливается возле того пункта, который настроен в конфиге
    if(appconfig.getEnableCustomDateTimeFormat() == false) {
        disableCustomDateTimeFormat->setChecked(true);
        enableCustomDateTimeFormat->setChecked(false);
        customDateTimeFormat->setEnabled(false);
    } else {
        // Если разрешен пользовательский формат даты и времени
        disableCustomDateTimeFormat->setChecked(false);
        enableCustomDateTimeFormat->setChecked(true);
        customDateTimeFormat->setEnabled(true);
        customDateTimeFormat->setText(appconfig.getCustomDateTimeFormat());
    }

}


AppConfigPageMain::~AppConfigPageMain(void)
{
    delete tetradirInput;
    delete trashdirInput;
    delete trashsizeInput;
    delete trashmaxfilecountInput;
    delete interfaceLanguage;

    delete dateTimeFormatBox;
    delete disableCustomDateTimeFormat;
    delete enableCustomDateTimeFormat;
    delete customDateTimeFormat;
}


void AppConfigPageMain::setup_signals(void)
{
    connect(tetradirButton, &FlatToolButton::clicked, this, &AppConfigPageMain::open_tetradir_select_dialog);
    connect(trashdirButton, &FlatToolButton::clicked, this, &AppConfigPageMain::open_trashdir_select_dialog);

    connect(disableCustomDateTimeFormat, &QRadioButton::toggled, this, &AppConfigPageMain::onDisableCustomDateTimeFormatToggle);
    connect(enableCustomDateTimeFormat, &QRadioButton::toggled, this, &AppConfigPageMain::onEnableCustomDateTimeFormatToggle);

    connect(dateTimeFormatHelpButton, &FlatToolButton::clicked, this, &AppConfigPageMain::onDateTimeFormatHelpButton);
}


void AppConfigPageMain::assembly(void)
{
    // Блок работы с путем до каталога данных
    QHBoxLayout *tetradirLayout = new QHBoxLayout();
    tetradirLayout->addWidget(tetradirInput);
    tetradirLayout->addWidget(tetradirButton);

    // Блок работы с путем до корзины
    QHBoxLayout *trashdirLayout = new QHBoxLayout();
    trashdirLayout->addWidget(trashdirInput);
    trashdirLayout->addWidget(trashdirButton);

    // Блок работы с размером корзины
    QHBoxLayout *trashsizeLayout = new QHBoxLayout();
    trashsizeLayout->addWidget(trashsizeInput);
    trashsizeLayout->addWidget(trashsizeFlexion);

    // Блок работы с максимальным количеством файлов в корзине
    QHBoxLayout *trashmaxfilecountLayout = new QHBoxLayout();
    trashmaxfilecountLayout->addWidget(trashmaxfilecountInput);
    trashmaxfilecountLayout->addWidget(trashmaxfilecountFlexion);

    // Блок работы с выбором языка интерфейса
    QHBoxLayout *interfaceLanguageLayout = new QHBoxLayout();
    interfaceLanguageLayout->addWidget(interfaceLanguage);


    // Блок работы с отображением даты и времени
    QVBoxLayout *dateTimeFormatVBox = new QVBoxLayout(this);
    QHBoxLayout *dateTimeFormatHBox = new QHBoxLayout(this);
    dateTimeFormatVBox->addWidget(disableCustomDateTimeFormat);
    dateTimeFormatVBox->addWidget(enableCustomDateTimeFormat);

    dateTimeFormatHBox->addWidget(customDateTimeFormat);
    dateTimeFormatHBox->addWidget(dateTimeFormatHelpButton);

    dateTimeFormatVBox->addLayout(dateTimeFormatHBox);
    dateTimeFormatBox->setLayout(dateTimeFormatVBox);


    // Сборка всех блоков
    QGridLayout *dirLayout = new QGridLayout();
    dirLayout->addWidget(tetradirLabel, 0, 0);
    dirLayout->addLayout(tetradirLayout, 0, 1);
    dirLayout->addWidget(trashdirLabel, 1, 0);
    dirLayout->addLayout(trashdirLayout, 1, 1);


    QGridLayout *otherSettingLayout = new QGridLayout();

    otherSettingLayout->addWidget(trashsizeLabel, 0, 0);
    otherSettingLayout->addLayout(trashsizeLayout, 0, 1);

    otherSettingLayout->addWidget(trashmaxfilecountLabel, 1, 0);
    otherSettingLayout->addLayout(trashmaxfilecountLayout, 1, 1);

    otherSettingLayout->addWidget(interfaceLanguageLabel, 2, 0);
    otherSettingLayout->addLayout(interfaceLanguageLayout, 2, 1);

    otherSettingLayout->setColumnStretch(1, 100);


    QVBoxLayout *centralLayout = new QVBoxLayout();
    centralLayout->addLayout(dirLayout);
    centralLayout->addLayout(otherSettingLayout);
    centralLayout->addWidget(dateTimeFormatBox);
    centralLayout->addStretch();

    setLayout(centralLayout);
}


// Действия при нажатии кнопки выбора директории данных
void AppConfigPageMain::open_tetradir_select_dialog(void)
{
    QFileDialog tetradirSelectDialog(this);
    tetradirSelectDialog.setFileMode(QFileDialog::Directory);
    tetradirSelectDialog.setWindowTitle(tr("Select data directory"));
    tetradirSelectDialog.setDirectory(tetradirInput->text());

    tetradirSelectDialog.exec();

    tetradirInput->setText(tetradirSelectDialog.directory().absolutePath());
}


// Действия при нажатии кнопки выбора директории корзины
void AppConfigPageMain::open_trashdir_select_dialog(void)
{
    QFileDialog trashdirSelectDialog(this);
    trashdirSelectDialog.setFileMode(QFileDialog::Directory);
    trashdirSelectDialog.setWindowTitle(tr("Select trash directory"));
    trashdirSelectDialog.setDirectory(trashdirInput->text());

    trashdirSelectDialog.exec();

    trashdirInput->setText(trashdirSelectDialog.directory().absolutePath());
}


// Действия при нажатии на радиокнопку системных установок даты и времени
void AppConfigPageMain::onDisableCustomDateTimeFormatToggle(bool checked)
{
    qDebug() << "In onDisableCustomDateTimeFormat(): " << checked;

    if(checked == true) {
        customDateTimeFormat->setEnabled(false);
        customDateTimeFormat->setText("");
    }

}


// Действия при нажатии на радиокнопку пользовательских установок даты и времени
void AppConfigPageMain::onEnableCustomDateTimeFormatToggle(bool checked)
{
    qDebug() << "In onEnableCustomDateTimeFormat(): " << checked;

    if(checked == true)
        customDateTimeFormat->setEnabled(true);
}


void AppConfigPageMain::onDateTimeFormatHelpButton(void)
{
    QString text = "Format symbols:\n\
\n\
d: day 1-31, dd: day 01-31,\n\
ddd: day short name, dddd: day long name,\n\
M: month 1-12, MM: month 01-12,\n\
MMM: month short name, MMMM: month long name,\n\
yy: year 00-99, yyyy: year 0000-9999,\n\
h: hour 0-23, hh: hour 00-23,\n\
H: hour 0-23 AM/PM, HH: hour 00-23 AM/PM,\n\
n: minute 0-59, mm: minute 00-59,\n\
s: second 0-59, ss: second 00-59,\n\
AP: use AM/PM display, ap: use am/pm display,\n\
t: timezone\n\
\n\
Sample: dd.MM.yyyy - hh:mm:ss";

    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPageMain::apply_changes(void)
{
    qDebug() << "Apply changes main";

    int difficultChanges = 0;

    // Если был изменен путь к базе, он запоминается в конфигфайл
    if(appconfig.get_tetradir() != tetradirInput->text()) {
        QDir dir(tetradirInput->text());

        // Проверяется, допустимо ли имя директории
        if(dir.isReadable() == false || dir.exists() == false)
            QMessageBox::warning(this, tr("Warning"),
                                 tr("The data directory does not exists or unavailable for reading."),
                                 QMessageBox::Ok);
        else {
            // Новое имя запоминается в конфиг
            appconfig.set_tetradir(tetradirInput->text());
            difficultChanges = 1;
        }
    }


    // Если был изменен путь к корзине, он запоминается в конфигфайл
    if(appconfig.get_trashdir() != trashdirInput->text()) {
        QDir dir(trashdirInput->text());

        // Проверяется, допустимо ли имя директории
        if(dir.isReadable() == false || dir.exists() == false)
            QMessageBox::warning(this, tr("Warning"),
                                 tr("The trash directory does not exists or unavailable for reading."),
                                 QMessageBox::Ok);
        else {
            // Новое имя запоминается в конфиг
            appconfig.set_trashdir(trashdirInput->text());
        }
    }


    // Если был изменен размер корзины
    if((int)appconfig.get_trashsize() != (int)trashsizeInput->text().toInt()) {
        appconfig.set_trashsize(trashsizeInput->text().toInt());
    }


    // Если было изменено максимально возможное количество файлов в корзине
    if(appconfig.get_trashmaxfilecount() != trashmaxfilecountInput->text().toInt()) {
        appconfig.set_trashmaxfilecount(trashmaxfilecountInput->text().toInt());
    }


    // Если было изменено использование пользовательского формата даты и времени
    if(appconfig.getEnableCustomDateTimeFormat() != enableCustomDateTimeFormat->isChecked()) {
        appconfig.setEnableCustomDateTimeFormat(enableCustomDateTimeFormat->isChecked());
    }


    // Если было изменен пользовательский формат даты и времени
    if(appconfig.getCustomDateTimeFormat() != customDateTimeFormat->text()) {
        appconfig.setCustomDateTimeFormat(customDateTimeFormat->text());
    }


    // Если был изменен язык
    if(appconfig.get_interfacelanguage() != interfaceLanguage->currentText()) {
        appconfig.set_interfacelanguage(interfaceLanguage->currentText());
        difficultChanges = 1;
    }

    return difficultChanges;
}