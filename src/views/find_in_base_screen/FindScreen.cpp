#include <cassert>
#include <utility>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QWidget>
#include <QProgressDialog>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QBoxLayout>
#include <QTextDocument>
#include <QLineEdit>
#include <QByteArray>
#include <QtWidgets/QStackedWidget>

#include "main.h"
#include "views/main_window/MainWindow.h"
#include "FindScreen.h"
//#include "FindTableWidget.h"
#include "models/tree/TreeKnowModel.h"
#include "models/app_config/AppConfig.h"
#include "models/tree/TreeItem.h"
#include "views/record/MetaEditor.h"
#include "libraries/GlobalParameters.h"
#include "views/tree/TreeScreen.h"
#include "libraries/FlatControl.h"
#include "views/tree/TreeKnowView.h"
#include "views/browser/entrance.h"
#include "views/browser/toolbarsearch.h"
#include "views/browser/chasewidget.h"
#include "libraries/FlatControl.h"
#include "models/record_table/ItemsFlat.h"

extern AppConfig appconfig;
extern GlobalParameters globalparameters;
extern const char *global_root_id;

FindScreen::FindScreen(QString object_name, boost::intrusive_ptr<TreeItem> _selected_branch_root, QWidget *parent)
    : QWidget(parent)
    , back_ground{_selected_branch_root}
    , _navigater(new QToolBar(this))
    , _chasewidget(new browser::ChaseWidget(QSize(17, 17), this))
    , _progress(new QProgressDialog(this))
      //    , _findtable(new FindTableWidget(this)) // deprecated
    , _is_search_global(_selected_branch_root->id() == global_root_id)
    , _selected_branch_as_pages([ & ]
{
    std::shared_ptr<ItemsFlat> result = std::make_shared<ItemsFlat>();

    for(int i = 0; i < _selected_branch_root->current_count(); i++) {
        auto it = _selected_branch_root->child(i);

        if(it->is_lite())it->to_fat();

        result->insert_new_item(result->current_count() - 1, it);
    }

    return result;
}())    // static_cast<ItemsFlat *>(_selected_branch_root.get()) // never transfer this data directly, it is come from _treeknowmodel!   // _resultset_data(std::make_shared<RecordTable>(QDomElement()))
, _toolbarsearch(new browser::ToolbarSearch(this))
{
    setObjectName(object_name);
    _navigater->setMaximumWidth(130);
    _chasewidget->setMaximumWidth(20);
    _progress->reset();
    _toolbarsearch->setMinimumWidth(100);

    _toolbarsearch->sizePolicy().setHorizontalStretch(10);
    //_toolbarsearch->setContentsMargins(0, 0, 0, 0);

    //    if(appconfig.getInterfaceMode() == "desktop") {
    //        _navigater->setMinimumWidth(400);
    //    }

    setup_navigate();
    assembly_navigate();

    setup_findtext_and_button();
    assembly_findtext_and_button();

    setup_closebutton();
    assembly_closebutton();

    setup_combooption();
    //assemblyComboOption();

    setup_wherefind_line();
    assembly_wherefind_line();

    setup_ui();
    assembly();

    setup_signals();
}


FindScreen::~FindScreen(void)
{
    //    delete _selected_branch_as_pages;
}

void FindScreen::setup_navigate(void)
{
    //    assert(globalparameters.entrance());
    //    assert(globalparameters.entrance()->activebrowser());
    //    this->addAction(globalparameters.entrance()->activebrowser()->historyback());

    //    historyback_ = new FlatToolButton(this);
    //    historyback_->setStatusTip(tr("Back"));
    //    historyback_->setIcon(style()->standardIcon(QStyle::SP_ArrowBack, 0, this));    //QIcon(":/resource/pic/pentalpha.svg");
    //    historyback_->setObjectName("Back");

    //_history_home = new FlatToolButton(this);
    _historyhome = new QAction(tr("Home"), _navigater);
    _historyhome->setIcon(
        //        QIcon(
        //            ":/resource/pic/up-arrow-circle.svg"    // ":/resource/pic/streamline_home.svg"
        //        )  //
        style()->standardIcon(QStyle::SP_ArrowUp, 0, this)
    );

    _historyhome->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H));

    insert_action_as_button<QToolButton>(_navigater, _historyhome); // _navigater->addAction(_historyhome);

    //_history_back = new FlatToolButton(this);
    _historyback = new QAction(tr("Back"), _navigater);
    _historyback->setShortcuts(QKeySequence::Back);
    //    _historyback->setIconVisibleInMenu(false);
    _historyback->setIcon(  //QIcon(":/resource/pic/walk_history_next.svg")//
        style()->standardIcon(QStyle::SP_ArrowBack, 0, this)
    );
    //    QMenu *_historybackmenu = new QMenu(this);
    //    _historyback->setMenu(_historybackmenu);
    //    connect(_historybackmenu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowBackMenu()));
    //    connect(_historybackmenu, SIGNAL(triggered(QAction *)), this, SLOT(slotOpenActionUrl(QAction *)));
    insert_action_as_button<QToolButton>(_navigater, _historyback); // _navigater->addAction(_historyback);
    //insertActionAsButton(_container, _historyback);

    //_history_forward = new FlatToolButton(this);
    _historyforward = new QAction(tr("Forward"), _navigater);
    _historyforward->setShortcuts(QKeySequence::Forward);
    _historyforward->setIcon(style()->standardIcon(QStyle::SP_ArrowForward, 0, this));
    //    _historyforwardmenu = new QMenu(this);
    //    connect(_historyforwardmenu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowForwardMenu()));
    //    connect(_historyforwardmenu, SIGNAL(triggered(QAction *)), this, SLOT(slotOpenActionUrl(QAction *)));
    //    _historyforward->setMenu(_historyforwardmenu);
    insert_action_as_button<QToolButton>(_navigater, _historyforward); // _navigater->addAction(_historyforward);


    //_stop_reload = new FlatToolButton(this);
    _stopreload = new QAction(_navigater);
    //_reloadicon = style()->standardIcon(QStyle::SP_BrowserReload);
    _stopreload->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    insert_action_as_button<QToolButton>(_navigater, _stopreload); // _navigater->addAction(_stopreload);


}

void FindScreen::assembly_navigate(void)
{
    //    _navigater = new QHBoxLayout();
    //    _navigater->addWidget(_container);
    //    _navigater->addStretch();

}

// Текст поиска и кнопка "Поиск"
void FindScreen::setup_findtext_and_button(void)
{
    // Поле текста для поиска
    //_findtext = new QLineEdit();

    // Кнопка "Поиск"
    _find_start_button = new FlatToolButton(this);    // QPushButton
    _find_start_button->setText(tr("Find"));
    //    _findstartbutton->setDefault(true);
    _find_start_button->setEnabled(false);
    _find_start_button->setFixedWidth(50);
    //    _findstartbutton->setFlat(true);
    _find_start_button->setAutoRaise(true);

    // Кнопка разворачивания инструментов
    _tools_expand = new FlatToolButton(this);
    _tools_expand->setIcon(QIcon(":/resource/pic/find_in_base_expand_tools.svg"));
    _tools_expand->setEnabled(true);
    _tools_expand->setAutoRaise(true);
}


// Текст поиска и кнопка "Поиск"
void FindScreen::assembly_findtext_and_button(void)
{
    _find_text_and_button_tools_area = new QHBoxLayout();
    //    QWidget *con = new QWidget(this);
    //    QHBoxLayout *addressbar = new QHBoxLayout();
    //    addressbar->addWidget(_toolbarsearch);
    //    addressbar->addWidget(_chasewidget);

    //    addressbar->setContentsMargins(0, 0, 0, 0);
    //    con->setLayout(addressbar);
    _find_text_and_button_tools_area->addWidget(_navigater);
    //    toolsAreaFindTextAndButton->addWidget(_history_home);
    //    toolsAreaFindTextAndButton->addWidget(_history_forward);
    //    toolsAreaFindTextAndButton->addWidget(_history_back);
    //    toolsAreaFindTextAndButton->addWidget(_stop_reload);
    //    toolsAreaFindTextAndButton->addLayout(addressbar);
    _find_text_and_button_tools_area->addWidget(_toolbarsearch);  //_findtext
    //    toolsAreaFindTextAndButton->addWidget(con);
    _find_text_and_button_tools_area->addWidget(_chasewidget);
    _find_text_and_button_tools_area->addWidget(_find_start_button);
    _find_text_and_button_tools_area->addWidget(_tools_expand);
    //toolsAreaFindTextAndButton->addStretch();
    //toolsAreaFindTextAndButton->setContentsMargins(0, 0, 0, 0);
}


// Набор опций поиска в виде выпадающих списков
void FindScreen::setup_combooption(void)
{
    // Выбор "Любое слово" - "Все слова"
    _word_regard = new FlatComboBox();
    _word_regard->addItem(QIcon(":/resource/pic/find_in_base_any.svg"), tr("Any word"));
    _word_regard->addItem(QIcon(":/resource/pic/find_in_base_all.svg"), tr("All words"));
    _word_regard->setCurrentIndex(appconfig.get_findscreen_wordregard());

    // Выбор "Только целые слова" - "Подстрока"
    _how_extract = new FlatComboBox();
    _how_extract->addItem(QIcon(":/resource/pic/find_in_base_separate.svg"), tr("Whole words"));
    _how_extract->addItem(QIcon(":/resource/pic/find_in_base_substring.svg"), tr("Substring"));
    _how_extract->setCurrentIndex(appconfig.get_findscreen_howextract());

    // Выбор "Во всей базе" - "В текущей ветке"
    _tree_search_area = new FlatComboBox();
    _tree_search_area->addItem(QIcon(":/resource/pic/find_in_base_search_all.svg"), tr("Entire base")); // Вся база
    _tree_search_area->addItem(QIcon(":/resource/pic/find_in_base_search_branch.svg"), tr("In current branch")); // Текущая ветка
    _tree_search_area->setCurrentIndex(appconfig.getFindScreenTreeSearchArea());

    if(appconfig.getInterfaceMode() == "desktop") {
        _tree_search_area->hide();
    }

    if(appconfig.getInterfaceMode() == "mobile") {
        // wordRegard->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        // howExtract->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        // treeSearchArea->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

        // wordRegard->showMinimized();
        // howExtract->showMinimized();
        // treeSearchArea->showMinimized();

        _word_regard->setMinimumContentsLength(1);
        _word_regard->setMaximumWidth(calculate_iconsize_px() * 2);
        _word_regard->setMinimumWidth(calculate_iconsize_px() * 2);
        _word_regard->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

        _how_extract->setMinimumContentsLength(1);
        _how_extract->setMaximumWidth(calculate_iconsize_px() * 2);
        _how_extract->setMinimumWidth(calculate_iconsize_px() * 2);
        _how_extract->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

        _tree_search_area->setMinimumContentsLength(1);
        _tree_search_area->setMaximumWidth(calculate_iconsize_px() * 2);
        _tree_search_area->setMinimumWidth(calculate_iconsize_px() * 2);
        _tree_search_area->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    }
}


// Набор опций поиска в виде выпадающих списков
void FindScreen::assembly_combooption(void)
{
    _combo_option_tools_area = new QHBoxLayout();
    //    toolsAreaComboOption->addWidget(wordRegard);
    //    toolsAreaComboOption->addWidget(howExtract);
    //    toolsAreaComboOption->addWidget(treeSearchArea);
    //toolsAreaComboOption->addStretch();
}


void FindScreen::setup_closebutton(void)
{
    // Кнопка закрытия виджета
    _close_button = new FlatToolButton(this);
    _close_button->setVisible(true);
    _close_button->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton)); // SP_TitleBarCloseButton SP_DialogCloseButton
    _close_button->setAutoRaise(true);

    if(appconfig.getInterfaceMode() == "desktop") {
        int w = _close_button->geometry().width();
        int h = _close_button->geometry().height();
        int x = min(w, h) / 2; // imin(w, h) / 2;
        _close_button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::ToolButton));
        _close_button->setMinimumSize(x, x);
        _close_button->setMaximumSize(x, x);
        _close_button->resize(x, x);
    }
}


void FindScreen::assembly_closebutton(void)
{
    // Вертикальная область с кнопкой закрытия и распоркой
    // чтобы кнопка была вверху
    _close_button_tools_area = new QVBoxLayout();
    _close_button_tools_area->setContentsMargins(0, 0, 0, 0);
    _close_button_tools_area->addWidget(_close_button);
    _close_button_tools_area->addStretch();
}


void FindScreen::setup_wherefind_line(void)
{
    _where_find_label = new QLabel(tr("Find in: "));

    _find_in_pin = new QCheckBox(tr("Pin"));
    _find_in_pin->setChecked(appconfig.get_findscreen_find_in_field("pin"));

    _find_in_name = new QCheckBox(tr("Title"));
    _find_in_name->setChecked(appconfig.get_findscreen_find_in_field("name"));

    _find_in_author = new QCheckBox(tr("Author(s)"));
    _find_in_author->setChecked(appconfig.get_findscreen_find_in_field("author"));

    _find_in_author = new QCheckBox(tr("Author(s)"));
    _find_in_author->setChecked(appconfig.get_findscreen_find_in_field("author"));

    _find_in_home = new QCheckBox(tr("Home"));
    _find_in_home->setChecked(appconfig.get_findscreen_find_in_field("home"));

    _find_in_url = new QCheckBox(tr("Url"));
    _find_in_url->setChecked(appconfig.get_findscreen_find_in_field("url"));

    _find_in_tags = new QCheckBox(tr("Tags"));
    _find_in_tags->setChecked(appconfig.get_findscreen_find_in_field("tags"));

    _find_in_text = new QCheckBox(tr("Text"));
    _find_in_text->setChecked(appconfig.get_findscreen_find_in_field("text"));
}


void FindScreen::assembly_wherefind_line(void)
{
    _where_find_line = new QHBoxLayout();
    _where_find_line->setEnabled(false);

    if(appconfig.getInterfaceMode() == "desktop")
        _where_find_line->addWidget(_where_find_label);

    if(appconfig.getInterfaceMode() == "mobile")
        _where_find_label->hide();

    _where_find_line->addWidget(_word_regard);
    _where_find_line->addWidget(_how_extract);

    if(appconfig.getInterfaceMode() == "mobile") _where_find_line->addWidget(_tree_search_area);

    _where_find_line->addWidget(_find_in_pin);
    _where_find_line->addWidget(_find_in_name);
    _where_find_line->addWidget(_find_in_author);
    _where_find_line->addWidget(_find_in_home);
    _where_find_line->addWidget(_find_in_url);
    _where_find_line->addWidget(_find_in_tags);
    _where_find_line->addWidget(_find_in_text);

    _where_find_line->addStretch();

    _where_find_line->setContentsMargins(3, 0, 0, 0); // Устанавливаются границы
}


void FindScreen::setup_signals(void)
{
    // При каждом изменении текста в строке запроса
    connect(_toolbarsearch, &browser::ToolbarSearch::textChanged, this, &FindScreen::enable_findbutton);

    // При каждом изменении текста извне может вырабатыватся этот сигнал
    // Он вырабатывается в слоте setFindText()
    connect(this, &FindScreen::text_changed_from_another, this, &FindScreen::enable_findbutton);


    // При нажатии Enter в строке запроса
    connect(_toolbarsearch, &browser::ToolbarSearch::returnPressed, this
    , [this]() {FindScreen::find_clicked();}
           );

    // При нажатии кнопки Find
    connect(_find_start_button, &QPushButton::clicked, this
    , [this]() {FindScreen::find_clicked();}
           );

    // При нажатии кнопки разворачивания/сворачивания инструментов поиска
    connect(_tools_expand, &FlatToolButton::clicked, this, &FindScreen::tools_expand_clicked);

    // После установки текста извне, вырабатывается этот сигнал
    connect(this, &FindScreen::find_clicked_after_another_text_changed, this
    , [this]() {FindScreen::find_clicked();}
           );


    // При нажатии кнопки закрытия
    connect(_close_button, &FlatToolButton::clicked, this, &FindScreen::widget_hide);
    connect(_close_button, &FlatToolButton::clicked, [] {
        auto win = globalparameters.entrance()->activiated_registered().first;

        if(win)win->updateToolbarActionText(false);
    });


    // Сигналы для запоминания состояния интерфейса
    connect(_word_regard, _word_regard->currentindexchanged, this, &FindScreen::word_regard);

    connect(_how_extract, _how_extract->currentindexchanged, this, &FindScreen::how_extract);

    connect(_tree_search_area, _tree_search_area->currentindexchanged, this, &FindScreen::tree_search_area);

    //    assert(globalparameters.vtab());
    //    connect(globalparameters.vtab(), &QTabWidget::currentChanged
    //            , this, &FindScreen::changedTreeSearchArea    // , &appconfig, &AppConfig::setFindScreenTreeSearchArea
    //           );

    connect(_find_in_pin, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_pin);

    connect(_find_in_name, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_name);

    connect(_find_in_author, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_author);

    connect(_find_in_home, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_home);

    connect(_find_in_url, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_url);

    connect(_find_in_tags, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_tags);

    connect(_find_in_text, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_text);
}


void FindScreen::setup_ui(void)
{
    //_findtable = new FindTableWidget();
    //_findtable->hide();
    //_progress = new QProgressDialog(this);
    _progress->hide();
}


void FindScreen::assembly(void)
{
    _central_desktop_layout = new QVBoxLayout(this);

    if(appconfig.getInterfaceMode() == "desktop") {
        _tools_line = new QHBoxLayout();
        _tools_line->setEnabled(true);
        _tools_line->addLayout(_find_text_and_button_tools_area);
        //toolsLine->addLayout(toolsAreaComboOption);
        _tools_line->addLayout(_close_button_tools_area);

        _central_desktop_layout->addLayout(_tools_line);
    }

    if(appconfig.getInterfaceMode() == "mobile") {
        _tools_grid = new QGridLayout();
        _tools_grid->setEnabled(true);
        _tools_grid->addLayout(_find_text_and_button_tools_area, 0, 0);
        _tools_grid->addLayout(_close_button_tools_area,       0, 1);
        //toolsGrid->addLayout(toolsAreaComboOption,       1, 0);

        _central_desktop_layout->addLayout(_tools_grid);
    }

    _central_desktop_layout->addLayout(_where_find_line);
    //    centralDesktopLayout->addWidget(_findtable, 10);
    _central_desktop_layout->setContentsMargins(0, 0, 0, 0); // Boundaries removed // Границы убираются
    _central_desktop_layout->setSizeConstraint(QLayout::   // SetFixedSize // this setting will lead TableScreen can not resize!!!
                                               SetNoConstraint
                                              );
    _central_desktop_layout->setMargin(0);
    _central_desktop_layout->setSpacing(0);

    // whereFindLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    // _findtable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    this->setLayout(_central_desktop_layout);

    switch_tools_expand(appconfig.getFindInBaseExpand());
}

void FindScreen::adjust_size()
{
    //    int height = toolsLine->isEnabled() ? toolsLine->sizeHint().height() : 0 + whereFindLine->isEnabled() ? whereFindLine->sizeHint().height() : 0;
    //    setMinimumHeight(height);
    //    setMaximumHeight(height);
    //    setMinimumWidth(width());
    //    setMaximumWidth(width());
    QWidget::adjustSize();
}

void FindScreen::resizeEvent(QResizeEvent *e)
{
    //    adjustSize();
    //    auto hint = whereFindLine->sizeHint();
    //    int height = whereFindLine->sizeHint().height();
    setMinimumHeight(_tools_line->sizeHint().height() + _where_find_line->sizeHint().height());
    setMaximumHeight(_tools_line->sizeHint().height() + _where_find_line->sizeHint().height());
    QWidget::resizeEvent(e);
}

void FindScreen::enable_findbutton(const QString &text)
{
    _find_start_button->setEnabled(!text.isEmpty());
}


// Слот, с помощью которого другие виджеты могут устанавливать
// текст для поиска
void FindScreen::find_text(QString text)
{
    //_findtext
    _toolbarsearch->text(text);

    emit text_changed_from_another(text);
    emit find_clicked_after_another_text_changed();
}


// Слот, срабатывающий при нажатии на кнопку начала поиска
std::shared_ptr<ItemsFlat> FindScreen::find_clicked(void)
{
    // Поля, где нужно искать (Заголовок, текст, теги...)
    _search_area["pin"]       = _find_in_pin->isChecked();
    _search_area["name"]      = _find_in_name->isChecked();
    _search_area["author"]    = _find_in_author->isChecked();
    _search_area["home"]      = _find_in_home->isChecked();
    _search_area["url"]       = _find_in_url->isChecked();
    _search_area["tags"]      = _find_in_tags->isChecked();
    _search_area["text"]      = _find_in_text->isChecked();

    // Проверяется, установлено ли хоть одно поле для поиска
    int findEnableFlag = 0;

    foreach(bool value, _search_area)
        if(value == true)findEnableFlag = 1;

    // Если не отмечены поля для поиска
    if(findEnableFlag == 0) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Cannot start find process"));
        messageBox.setText(tr("Verify that you selected fields for search for starting find process."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return nullptr;
    }

    // Выясняется список слов, которые нужно искать
    _search_word_list = text_delimiter_decompose(_toolbarsearch->text());

    if(_search_word_list.size() == 0) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Can not start find process"));
        messageBox.setText(tr("The search request is too short. Enter at least one word."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return nullptr;
    }

    return find_start();
}


std::shared_ptr<ItemsFlat> FindScreen::find_start(void)
{

    if(globalparameters.vtab()->currentWidget()->objectName() == table_screen_singleton_name
       && ! // find_object<TreeScreen>(tree_screen_singleton_name)
       globalparameters.tree_screen()->current_index().isValid()
      ) {
        appconfig.setFindScreenTreeSearchArea(2);
    }

    // Сохраняется текущая редактируемая запись, чтобы и в ней
    // были найдены введенные перед нажатием Find данные, если они есть
    //    find_object<MainWindow>("mainwindow")
    globalparameters.mainwindow()->save_text_area();

    //    // Очищается таблица результата поиска
    //    _findtable->re_initialize();

    // Выясняется ссылка на модель дерева данных
    TreeKnowModel *_search_model = static_cast<TreeKnowModel *>(    // find_object<TreeKnowView>(knowtreeview_singleton_name)
                                       globalparameters.tree_screen()->tree_view()->model());


    // Выясняется стартовый элемент в дереве, с которого будет начат поиск
    // Выясняется сколько всего конечных записей
    boost::intrusive_ptr<TreeItem> _search_start_item;

    //    auto _candidate_root = find_object<TreeScreen>(tree_screen_singleton_name)->_shadow_candidate_model->_root_item;
    back_ground::_selected_branch_root->field("name", _toolbarsearch->text());
    //    TreeScreen *tree_screen = find_object<TreeScreen>(tree_screen_singleton_name);
    (*reocrd_controller)()->record_screen()->setObjectName(_toolbarsearch->text());

    // globalparameters.tree_screen()->insert_branch_process(globalparameters.tree_screen()->last_index(), "buffer", true);
    //    std::shared_ptr<RecordTable> _resultset_data = _candidate_root->tabledata();    // std::make_shared<RecordTable>(_resultset_item);
    int _candidate_records = 0;


    //__________________________________________________________________________________________________


    auto global_search_prepare = [&](boost::intrusive_ptr<TreeItem>     &search_start_item
                                     , int                              &candidate_records
                                     , std::shared_ptr<ItemsFlat>       &resultset_item
                                     // , std::shared_ptr<RecordTable>     &resultset_data
    ) {
        // Корневой элемент дерева
        search_start_item = _search_model->root_item();    // this change the value of local smart pointer, which can't be return to outer start_item, so function parameter type must be a reference.
        // Количество элементов (веток) во всем дереве
        candidate_records = _search_model->all_records_count();
        resultset_item = resultset_item->active_subset();
    };

    auto branch_search_prepare = [&](boost::intrusive_ptr<TreeItem>     &search_start_item
                                     , int                              &candidate_records
                                     , std::shared_ptr<ItemsFlat>       &resultset_item
    ) {

        // Индекс текущей выбранной ветки
        QModelIndex current_item_index =    // find_object<TreeScreen>(tree_screen_singleton_name)
            globalparameters.tree_screen()->current_index();

        // Текущая ветка
        search_start_item = _search_model->item(current_item_index);

        // Количество элементов (веток) в текущей ветке и всех подветках
        candidate_records = _search_model->size_of(search_start_item);
        resultset_item = resultset_item->active_subset();
    };

    Q_UNUSED(branch_search_prepare)

    auto resultset_search_prepare = [&](boost::intrusive_ptr<TreeItem>      &search_start_item
                                        , int                               &candidate_records
                                        , std::shared_ptr<ItemsFlat>        &resultset_item
    ) {
        // to be done
        //        QMap<QString, QString> data;


        //        resultset_item.swap(_candidate_root);   // resultset_item == _candidate_root
        //        std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
        //        auto dommodel = resultset_item->export_to_dom();    // source->init(startItem, QDomElement());
        //        search_start_item->tabledata(dommodel);
        //        QMap<QString, QString> field_data;
        search_start_item = boost::intrusive_ptr<TreeItem>(new TreeItem(
                                                               QMap<QString, QString>() // field_data // QMap<QString, QString> &_field_data
                                                               , nullptr                // boost::intrusive_ptr<TreeItem> _parent_item
                                                               // , std::make_shared<RecordTable>(dommodel)    // std::shared_ptr<RecordTable> _table_data
                                                           ));   // resultset_item;     // std::make_shared<TreeItem>(data, search_model->_root_item);

        resultset_item = resultset_item->active_subset();
        // resultset_item
        // resultset_record_source->active_subset(globalparameters.tree_screen()->insert_branch_process(globalparameters.tree_screen()->last_index(), "buffer", true));  //
        //            std::make_shared<TableData>();      // assert(_result->size() == 0); //_result->empty();
        candidate_records = search_start_item->current_count();

    };


    auto prepare_progressbar = [&]() {
        // Показывается виджет линейки наполняемости
        _progress->reset();
        _progress->setLabelText(tr("Search..."));
        _progress->setRange(0, _candidate_records);
        _progress->setModal(true);
        _progress->setMinimumDuration(0);
        _progress->show();

        // Обнуляется счетчик обработанных конечных записей
        _total_progress_counter = 0;
        _cancel_flag = 0;

    };


    auto assert_start_item = [&]() {
        // Если стартовый элемент не был установлен
        //        if(!startItem)
        //        {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Cannot start find process"));
        messageBox.setText(tr("Can\'t set start search element in tree."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        //        return nullptr;
        //        } else
        //            return startItem;
    };


    auto close_progressbar = [&]() {
        // Виджет линейки наполняемости скрывается
        _progress->reset();
        _progress->hide();
        _progress->close();
    };

    auto final_search = [&](boost::intrusive_ptr<TreeItem>  &search_start_item
                            , std::shared_ptr<ItemsFlat>    &candidate_root  // std::shared_ptr<RecordTable> &resultset_data
    ) {
        qDebug() << "Start finding in " << _candidate_records << " records";
        prepare_progressbar();
        candidate_root->clear();
        //Вызывается рекурсивный поиск в дереве
        //        find_recursive(search_start_item, candidate_root);

        //        if(result->size() == 0 && appconfig.getFindScreenTreeSearchArea() == 2) {
        //            global_search_prepare(start_item, total_records, result);
        //        }
        return find_recursive(search_start_item, candidate_root);   // candidate_root->tabledata();
    };

    auto output = [&](std::shared_ptr<ItemsFlat> &resultset_item) { // , std::shared_ptr<RecordTable> &resultset_data

        // После вставки всех данных подгоняется ширина колонок
        //        _findtable->updateColumnsWidth();
        globalparameters.tree_screen()->candidate_from_search_result(resultset_item);   // dump to table screen
    };


    //__________________________________________________________________________________________________



    // stage 1
    if( // appconfig.getFindScreenTreeSearchArea() == 2
        //        globalparameters.vtab()->currentWidget()->objectName() == table_screen_singleton_name
        //        && !find_object<TreeScreen>(tree_screen_singleton_name)->getCurrentItemIndex().isValid()
        _selected_branch_as_pages->current_count() > 0
    ) { // search in last search result
        resultset_search_prepare(_search_start_item, _candidate_records, _selected_branch_as_pages);  // , _resultset_data, _resultset_data

        if(!_search_start_item) {assert_start_item(); return nullptr;}

        if(0 != _candidate_records) {
            _selected_branch_as_pages = final_search(_search_start_item, _selected_branch_as_pages);
            globalparameters.tree_screen()->enable_up_action(!_is_search_global  // _selected_branch_as_pages != globalparameters.tree_screen()->know_root()->root_item()
                                                            );
        }
    }

    // stage 2
    if(0 == _selected_branch_as_pages->current_count()) {
        //        auto tree_screen = find_object<TreeScreen>(tree_screen_singleton_name);
        //        tree_screen->delete_one_branch(_search_model->index_item(_search_model->findChild<boost::intrusive_ptr<TreeItem>>(QString("buffer"))));

        //    if( // appconfig.getFindScreenTreeSearchArea() == 1
        //        // globalparameters.vtab()->currentWidget()->objectName() == tree_screen_singleton_name
        //        find_object<TreeScreen>(tree_screen_singleton_name)->getCurrentItemIndex().isValid()
        //    ) { // If you want to search the current branch // Если нужен поиск в текущей ветке
        //        branch_search_prepare(start_item, total_records, _result);
        //    }

        //    if( // appconfig.getFindScreenTreeSearchArea() == 0
        //        globalparameters.vtab()->currentWidget()->objectName() == tree_screen_singleton_name
        //        && !find_object<TreeScreen>(tree_screen_singleton_name)->getCurrentItemIndex().isValid()
        //    ) { // Если нужен поиск во всем дереве

        global_search_prepare(_search_start_item, _candidate_records, _selected_branch_as_pages);   // , _resultset_data

        if(!_search_start_item) {assert_start_item(); return nullptr;}

        if(0 != _candidate_records) {
            _selected_branch_as_pages = final_search(_search_start_item, _selected_branch_as_pages);
            globalparameters.tree_screen()->enable_up_action(!_is_search_global  // _selected_branch_as_pages != globalparameters.tree_screen()->know_root()->root_item()
                                                            );
        }

        //    }

    }





    //        close_progressbar();
    //    } else {

    close_progressbar();



    // create a new note and open in browser
    // if it is a web address, open it
    // else, open from search engine
    //    }

    output(_selected_branch_as_pages);


    return _selected_branch_as_pages; // ->record_table();
}


std::shared_ptr<ItemsFlat> FindScreen::find_recursive(
    boost::intrusive_ptr<TreeItem>  curritem
    , std::shared_ptr<ItemsFlat>    _candidate_pages
)
{
    auto result_pages = _candidate_pages;  // ->record_table();

    // Если была нажата отмена поиска
    if(_cancel_flag == 1)return result_pages;

    // Если ветка зашифрована, и пароль не был введен
    if(curritem->field("crypt") == "1" &&
       globalparameters.crypt_key().length() == 0)
        return result_pages;

    // Если в ветке присутсвует таблица конечных записей
    if(curritem->current_count() > 0) {
        // Обработка таблицы конечных записей

        // Выясняется ссылка на таблицу конечных записей
        auto child_items_root = curritem;   // ->record_table();

        // Перебираются записи таблицы
        for(int i = 0; i < child_items_root->current_count(); i++) {
            // Обновляется линейка наполняемости
            _progress->setValue(++ _total_progress_counter);
            qApp->processEvents();

            if(_progress->wasCanceled()) {
                _cancel_flag = 1;
                return result_pages;
            }

            // Результаты поиска в полях
            QMap<QString, bool> iteration_search_result;

            iteration_search_result["pin"]      = false;
            iteration_search_result["name"]     = false;
            iteration_search_result["author"]   = false;
            iteration_search_result["home"]     = false;
            iteration_search_result["url"]      = false;
            iteration_search_result["tags"]     = false;
            iteration_search_result["text"]     = false;

            // Текст в котором будет проводиться поиск
            QString inspect_text;

            // Цикл поиска в отмеченных пользователем полях
            QMapIterator<QString, bool> j(iteration_search_result);

            while(j.hasNext()) {
                j.next();
                QString key = j.key();

                // Если в данном поле нужно проводить поиск
                if(_search_area[key] == true) {
                    if(key != "text") {
                        // Поиск в обычном поле
                        inspect_text = child_items_root->child(i)->field(key);
                        iteration_search_result[key] = find_in_text_process(inspect_text);
                    } else {
                        // Поиск в тексте записи
                        if(child_items_root->item(i)->file_exists()) inspect_text = child_items_root->text(i);
                        else inspect_text = QString();

                        QTextDocument textdoc;
                        textdoc.setHtml(inspect_text);
                        iteration_search_result[key] = find_in_text_process(textdoc.toPlainText());
                    }
                }
            } // Закрылся цикл поиска в полях


            // Проверяется, есть ли поле, в котором поиск был успешен
            int found_flag = 0;

            foreach(bool value, iteration_search_result)
                if(value == true)found_flag = 1;

            // Если запись найдена
            if(found_flag == 1) {
                qDebug() << "Find succesfull in " << child_items_root->child(i)->field("name");

                // QString pin0 = curritem->getField("pin");
                // QString pin1 = searchRecordTable->getField("pin", i);   // work

                //                // В таблицу результатов поиска добавляются данные
                //                // Имя записи
                //                // Имя ветки
                //                // Теги
                //                // Путь к ветке
                //                // ID записи в таблице конечных записей
                //                _findtable->addRow(searchRecordTable->field("name", i)
                //                                   , searchRecordTable->field("pin", i)  // curritem->getField("pin")
                //                                   , curritem->getField("name")
                //                                   , searchRecordTable->field("tags", i)
                //                                   , curritem->getPath()
                //                                   , searchRecordTable->field("id", i)
                //                                  );

                if(child_items_root->item(i)->is_lite())child_items_root->item(i)->to_fat();

                result_pages->insert_new_item(result_pages->current_count() - 1, child_items_root->item(i)); // result->import_from_dom(_recordtable->record(i)->export_to_dom());

                //                assert(_recordtable->record(i)->is_lite());
                //                result->shadow_record_lite(result->size(), _recordtable->record(i));

                //                if(_recordtable->record(i)->isLite()) {
                //                    result->shadow_record(result->work_pos(), _recordtable->record_lite(i), ADD_NEW_RECORD_TO_END);
                //                } else {
                //                    result->shadow_record(result->work_pos(), _recordtable->record_fat(i), ADD_NEW_RECORD_TO_END);
                //                }
            }

        } // Закрылся цикл перебора записей в таблице конечных записей
    } // Закрылось условие что в ветке есть таблица конечных записей


    // Рекурсивная обработка каждой подчиненной ветки
    for(int i = 0; i < curritem->current_count(); i++) find_recursive(curritem->child(i), _candidate_pages);

    return result_pages;
}


// Поиск в переданном тексте
// Учитываются состояния переключателей wordregard и howextract
bool FindScreen::find_in_text_process(const QString &text)
{
    int findWordCount = 0;
    int findFlag = 0;

    // Перебираются искомые слова
    for(int i = 0; i < _search_word_list.size(); ++i) {
        findFlag = 0;

        // Если надо найти совпадение целого слова
        if(_how_extract->currentIndex() == 0) {
            // Текст разбивается на слова с очисткой от лишних знаков
            // и проверяется, есть ли в полученном списке текущее слово
            if(text_delimiter_decompose(text).contains(_search_word_list.at(i), Qt::CaseInsensitive))
                findFlag = 1;
        } else {
            // Если надо найти слово как подстроку
            if(text.contains(_search_word_list.at(i), Qt::CaseInsensitive))
                findFlag = 1;
        }

        // Если слово было найдено, количество найденных слов увеличивается
        if(findFlag == 1)findWordCount++;

        // Если ищется хотя бы одно совпадение
        if(findFlag == 1 && _word_regard->currentIndex() == 0)
            return true; // То при первом же совпадении цикл прекращается
    }

    // Искалось хотябы одно совпадение, но небыло найдено
    if(_word_regard->currentIndex() == 0) return false;
    else {
        // Иначе требовалось найти все слова в запросе
        if(findWordCount == _search_word_list.size())
            return true;
        else
            return false;
    }

}


void FindScreen::word_regard(int pos)
{
    appconfig.set_findscreen_wordregard(pos);
}


void FindScreen::how_extract(int pos)
{
    appconfig.set_findscreen_howextract(pos);
}


void FindScreen::tree_search_area(int pos)
{
    appconfig.setFindScreenTreeSearchArea(pos);
}

void FindScreen::if_find_in_pin(int state)
{
    if_find_in_field("pin", state);
}

void FindScreen::if_find_in_name(int state)
{
    if_find_in_field("name", state);
}


void FindScreen::if_find_in_author(int state)
{
    if_find_in_field("author", state);
}

void FindScreen::if_find_in_home(int state)
{
    if_find_in_field("home", state);
}

void FindScreen::if_find_in_url(int state)
{
    if_find_in_field("url", state);
}


void FindScreen::if_find_in_tags(int state)
{
    if_find_in_field("tags", state);
}


void FindScreen::if_find_in_text(int state)
{
    if_find_in_field("text", state);
}


void FindScreen::if_find_in_field(QString fieldname, int state)
{
    bool i;

    if(state == Qt::Checked) i = true;
    else i = false;

    appconfig.set_findscreen_find_in_field(fieldname, i);
}


void FindScreen::widget_show(void)
{
    appconfig.set_findscreen_show(true);
    this->show();

    // При появлении виджета курсор должен сразу стоять на поле ввода
    //_findtext
    _toolbarsearch->setFocus();
}


// Полное сокрытие виджета
void FindScreen::widget_hide(void)
{
    // Запоминается размер сплиттера перед скрытием виджета
    QSplitter *findSplitterRel = globalparameters.find_splitter();  // find_object<QSplitter>("find_splitter");
    appconfig.findsplitter_sizelist(findSplitterRel->sizes());

    // Виджет скрывается
    appconfig.set_findscreen_show(false);
    this->close();
}


// Слот, срабатывающий при клике на кнопку expand
void FindScreen::tools_expand_clicked(void)
{
    // Если нужно сомкнуть инструменты
    if(_find_in_name->isVisible()) {
        switch_tools_expand(false);
        appconfig.setFindInBaseExpand(false);
    } else {
        switch_tools_expand(true);
        appconfig.setFindInBaseExpand(true);
    }
}


void FindScreen::switch_tools_expand(bool flag)
{
    // toolsAreaComboOption->setVisible(flag);
    // whereFindLine->setVisible(flag);

    // Выпадающие списки скрываются в мобильном интерфейсе, так как они на отдельной строке
    if(appconfig.getInterfaceMode() == "mobile") {
        _find_in_pin->setVisible(flag);
        _word_regard->setVisible(flag);
        _how_extract->setVisible(flag);
        _tree_search_area->setVisible(flag);
    }

    // Надпись Find in видна и управляется только в desktop режиме интерфейса
    if(appconfig.getInterfaceMode() == "desktop") {
        _where_find_label->setVisible(flag);
    }

    // Флаги поиска скрываются для любого интерфейса, так как они всегда находятся на отдельной строке
    _word_regard->setVisible(flag);
    _how_extract->setVisible(flag);
    //    treeSearchArea->setVisible(flag);
    _find_in_pin->setVisible(flag);
    _find_in_name->setVisible(flag);
    _find_in_author->setVisible(flag);
    _find_in_home->setVisible(flag);
    _find_in_url->setVisible(flag);
    _find_in_tags->setVisible(flag);
    _find_in_text->setVisible(flag);

    _where_find_line->setEnabled(flag);
    this->adjust_size();
}

// dangerous!
void FindScreen::remove_child(const QString &id)
{
    _selected_branch_as_pages->remove_child(id);    // _findtable->remove_id(id);
}

// dangerous!
void FindScreen::remove_child(const int row)
{
    _selected_branch_as_pages->remove_child(row);    // _findtable->remove_row(row);
}